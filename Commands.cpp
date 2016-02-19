// Commands.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateVersion.h"
#include "Commands.h"

#include <fstream>
#include <sstream>
#include "deelx.h"
#include "OptionDlg.h"
#include "BackupFile.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommands


CCommands::CCommands()
	:m_dlgOption(this)
{
	m_pApplication = NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;

	// Create Application event handlers
	XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef();
	m_pApplicationEventsObj->Connect(m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;

	// Create Debugger event handler
	CComPtr<IDispatch> pDebugger;
	if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger))
		&& pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef();
		m_pDebuggerEventsObj->Connect(pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}
}

void CCommands::UnadviseFromEvents()
{
	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect(m_pApplication);
	m_pApplicationEventsObj->Release();
	m_pApplicationEventsObj = NULL;

	if (m_pDebuggerEventsObj != NULL)
	{
		// Since we were able to connect to the Debugger events, we
		//  should be able to access the Debugger object again to
		//  unadvise from its events (thus the VERIFY_OK below--see stdafx.h).
		CComPtr<IDispatch> pDebugger;
		VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect(pDebugger);
		m_pDebuggerEventsObj->Release();
		m_pDebuggerEventsObj = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// TODO: Fill out the implementation for those events you wish handle
//  Use m_pCommands->GetApplicationObject() to access the Developer
//  Studio Application object

// Application events

UINT CCommands::XApplicationEvents::GetWorkSvnVersion( IN CString strCurrentPath, BOOL bRootPath /*= FALSE*/ ) /* TRUE is rootpath, FALSE is current path*/
{
	std::string path;

	path = strCurrentPath;
	if(path.size() > 0 && (path[path.size()-1]!='/' || path[path.size()-1]!='\\'))
		path += '/';

	FILE* EntriesFile = fopen((path+".svn/entries").c_str(), "r");
	if(!EntriesFile)
		EntriesFile = fopen((path+"_svn/entries").c_str(), "r");

	if (!EntriesFile)
	{
		return 0;
	}

	char buf[200];
	int revision = 0;
	char date_str[200];
	char time_str[200];

	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	if (bRootPath) fscanf(EntriesFile,"%i",&revision);
	else fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fgets(buf,200,EntriesFile);
	fscanf(EntriesFile,"%10sT%8s",date_str,time_str);
	fgets(buf,200,EntriesFile);
	if (bRootPath) fgets(buf,200,EntriesFile);
	else fscanf(EntriesFile,"%i",&revision);

	fclose(EntriesFile);


	return revision;
}

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IApplication* pApplication;

	pApplication = m_pCommands->GetApplicationObject();

	CComPtr<IProjects> pIProjects = NULL;
	pApplication->get_Projects((IDispatch **)&pIProjects);

	long					lProjects			= 0;
	CComVariant				varProject			;
	VERIFY_OK( pIProjects->get_Count( &lProjects ) );

	for ( long lProject = 1 ; lProject < lProjects + 1 ; lProject++ )
	{
		varProject = lProject;

		CComPtr< IGenericProject > pIGenericProject;

		VERIFY_OK( pIProjects->Item(varProject, &pIGenericProject));

		if (pIGenericProject)
		{
			CComBSTR bType;
			CString strType;
			pIGenericProject->get_Type(&bType);
			strType = bType;
			if (strType.CompareNoCase(DS_BUILD_PROJECT) != 0) continue;

			CComBSTR bStr;
			pIGenericProject->get_FullName(&bStr);

			CString strProjectName = bStr;
			CString strProjectPath, strRcName, strConfigName;
			UINT uSvnVersion = 0;

			TCHAR sDrive[_MAX_DRIVE] = {0};
			TCHAR sDir[_MAX_DIR] = {0};
			TCHAR sFname[_MAX_FNAME] = {0};
			TCHAR sExt[_MAX_EXT] = {0};

			_tsplitpath(strProjectName, sDrive, sDir, sFname, sExt);

			strProjectPath = CString(sDrive) + CString(sDir);
			strConfigName = CString(sDrive) + CString(sDir) + CString(sFname) + CString(_T(".ini"));

			// 得到配置信息
			CONFIG config;
			COptionDlg::GetConfig(config, strConfigName);

			if (config.bCheckSvn && !config.bCheckSelfUpdata)
			{
				// const svn_version_t *ver = svn_wc_version();
				uSvnVersion = GetWorkSvnVersion(strProjectPath, config.iSvnVersionPath);

				if (uSvnVersion == 0)
				{
					return S_OK;
				}
			}

			CString Data;

			GetFileData(strProjectName, Data);

			if (Data.IsEmpty())
			{
				return S_OK;
			}

			strRcName = GetRegexpData(Data, _T("SOURCE=(?<SCR>.+\\.rc)$|SOURCE=\"(?<SCR>.+\\.rc)\""), MULTILINE | IGNORECASE, "SCR");

			if (strRcName.IsEmpty())
			{
				return S_OK;
			}

			CString strFileVersion, strFileVersionDescribe, strVersion, strVersionDescribe, RcData;
			UINT uFileVer[4], uFileVerDesc[4];
			BOOL bUpdata = FALSE;

			GetFileData(strProjectPath + strRcName, RcData);

			if (RcData.IsEmpty())
			{
				return S_OK;
			}

			// 进行 版本信息 更新
			strVersion = GetRegexpData(RcData, _T(".+VALUE.+\"FileVersion\",.+\"(.+)\""), MULTILINE | IGNORECASE);
			if (!strVersion.IsEmpty())
			{
				sscanf(strVersion, "%d, %d, %d, %d", &uFileVer[0], &uFileVer[1], &uFileVer[2], &uFileVer[3]);
				UpdataVersionInfo(uFileVer, config, uSvnVersion);
				strFileVersion.Format(_T("$1%d, %d, %d, %d\\0$2"),
					uFileVer[0], uFileVer[1], uFileVer[2], uFileVer[3]);

				if (-1 == strFileVersion.Find(strVersion))
				{
					RcData = ReplaceRegexpData(
						RcData,
						_T("(.+VALUE.+\"FileVersion\",.+\").+(\")"),
						MULTILINE | IGNORECASE,
						strFileVersion);
					bUpdata = TRUE;
				}
			}

			strVersion = GetRegexpData(RcData, _T(".+VALUE.+\"ProductVersion\",.+\"(.+)\""), MULTILINE | IGNORECASE);
			if (!strVersion.IsEmpty())
			{
				sscanf(strVersion, "%d, %d, %d, %d", &uFileVer[0], &uFileVer[1], &uFileVer[2], &uFileVer[3]);
				UpdataVersionInfo(uFileVer, config, uSvnVersion);
				strFileVersion.Format(_T("$1%d, %d, %d, %d\\0$2"),
					uFileVer[0], uFileVer[1], uFileVer[2], uFileVer[3]);

				if (-1 == strFileVersion.Find(strVersion))
				{
					RcData = ReplaceRegexpData(
						RcData,
						_T("(.+VALUE.+\"ProductVersion\",.+\").+(\")"),
						MULTILINE | IGNORECASE,
						strFileVersion);
					bUpdata = TRUE;
				}
			}

			strVersionDescribe = GetRegexpData(RcData, _T(".+FILEVERSION[ ]+(.+)"), MULTILINE | IGNORECASE);
			if (!strVersionDescribe.IsEmpty())
			{
				sscanf(strVersionDescribe, "%d,%d,%d,%d", &uFileVerDesc[0], &uFileVerDesc[1], &uFileVerDesc[2], &uFileVerDesc[3]);
				UpdataVersionInfo(uFileVerDesc, config, uSvnVersion);
				strFileVersionDescribe.Format(_T("$1%d,%d,%d,%d"),
					uFileVerDesc[0], uFileVerDesc[1], uFileVerDesc[2], uFileVerDesc[3]);

				if (-1 == strFileVersionDescribe.Find(strVersionDescribe))
				{
					RcData = ReplaceRegexpData(
						RcData,
						_T("(.+FILEVERSION[ ]+).+"),
						MULTILINE | IGNORECASE,
						strFileVersionDescribe);
					bUpdata = TRUE;
				}
			}

			strVersionDescribe = GetRegexpData(RcData, _T(".+ProductVersion[ ]+(.+)"), MULTILINE | IGNORECASE);
			if (!strVersionDescribe.IsEmpty())
			{
				sscanf(strVersionDescribe, "%d,%d,%d,%d", &uFileVerDesc[0], &uFileVerDesc[1], &uFileVerDesc[2], &uFileVerDesc[3]);
				UpdataVersionInfo(uFileVerDesc, config, uSvnVersion);
				strFileVersionDescribe.Format(_T("$1%d,%d,%d,%d"),
					uFileVerDesc[0], uFileVerDesc[1], uFileVerDesc[2], uFileVerDesc[3]);

				if (-1 == strFileVersionDescribe.Find(strVersionDescribe))
				{
					RcData = ReplaceRegexpData(
						RcData,
						_T("(.+ProductVersion[ ]+).+"),
						MULTILINE | IGNORECASE,
						strFileVersionDescribe);
					bUpdata = TRUE;
				}
			}

			if (bUpdata)
			{
				SetFileData(strProjectPath + strRcName, RcData);
			}
		}
	}

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (nNumErrors == 0)
	{
		CComPtr<IGenericProject> pProjectItem = NULL;
		IApplication* pApplication;
		CComPtr<IConfiguration> pConfig = NULL;
		CComBSTR bsBuildName;
		CString strBuildName;

		pApplication = m_pCommands->GetApplicationObject();
		VERIFY_OK(pApplication->get_ActiveProject((IDispatch **)&pProjectItem));
		VERIFY_OK(pApplication->get_ActiveConfiguration((IDispatch **)&pConfig));
		pConfig->get_Name(&bsBuildName);
		strBuildName = bsBuildName;

		if (!pProjectItem)
		{
			return S_OK;
		}

		CComBSTR bsFullName, bsName;
		CString strProjectFullName, strProjectPath,
			strConfigName, strProjectName;
		UINT uSvnVersion = 0;

		pProjectItem->get_FullName(&bsFullName);
		pProjectItem->get_Name(&bsName);

		strProjectFullName = bsFullName;
		strProjectName = bsName;

		TCHAR sDrive[_MAX_DRIVE] = {0};
		TCHAR sDir[_MAX_DIR] = {0};
		TCHAR sFname[_MAX_FNAME] = {0};
		TCHAR sExt[_MAX_EXT] = {0};

		_tsplitpath(strProjectFullName, sDrive, sDir, sFname, sExt);

		strProjectPath = CString(sDrive) + CString(sDir);
		strConfigName = CString(sDrive) + CString(sDir) + CString(sFname) + CString(_T(".ini"));

		CONFIG config;
		COptionDlg::GetConfig(config, strConfigName);

		if (config.bPdbAutoCommit &&
			-1 != config.strCommitList.Find(strBuildName + '#'))
		{
			CString strPdbPath, strOutputDir, strReg, strOutPut, strOutData, strData;
			const TCHAR sReg[] = _T("IF[ ]+\"\\$\\(CFG\\)\" == \"%s\"([^!]+)");
			const TCHAR sRegOutputDir[] = _T("^.+PROP Output_Dir[ ]+\"(.+)\"$");
			const TCHAR sRegPdbPath[] = _T("^# ADD LINK32(?:.+/pdb:\"(?<PDB>[^\"]+)\".+/debug)|(?:.+(?<PDB>/debug))");
			const TCHAR sRegOutPut[] = _T("^# ADD LINK32(?:.+/out:\"(?<OUT>[^\"]+)\")");

			strReg.Format(sReg, strBuildName);

			// Get PDB File Path
			GetFileData(strProjectFullName, strOutData);
			if (strOutData.IsEmpty())
			{
				return S_OK;
			}
			strData = GetRegexpData(strOutData, strReg, MULTILINE | IGNORECASE);
			strOutputDir = GetRegexpData(strData, sRegOutputDir, MULTILINE | IGNORECASE);
			strPdbPath = GetRegexpData(strData, sRegPdbPath, MULTILINE | IGNORECASE, "PDB");
			strOutPut = GetRegexpData(strData, sRegOutPut, MULTILINE | IGNORECASE, "OUT");
			if (strPdbPath.IsEmpty())
			{
				pApplication->PrintToOutputWindow(CComBSTR("Not Find Pdb File Path So Can't Auto Commit PDB\r\n"));
				return S_OK;
			}
			if (!strPdbPath.CompareNoCase(_T("/debug")))
			{
				if (strOutPut.IsEmpty())
				{
					strPdbPath = strOutputDir + '\\' + strProjectName + ".pdb";
				}
				else
				{
					TCHAR ssFname[_MAX_FNAME] = {0};

					_tsplitpath(strOutPut, NULL, NULL, ssFname, NULL);
					strPdbPath = strOutputDir + '\\' + ssFname + ".pdb";
				}
			}

			// Commit PDB File To Server
			MANUALBACKUP ManualBackup = {0};
			ManualBackup.dwMaxSingleFileSize = -1;
			ManualBackup.dwUserData = (DWORD)m_pCommands;
			ManualBackup.pfnCallBackProc = ManualCallBackProc;
			strcpy(ManualBackup.szExtName, "*.pdb");
			ManualBackup.pFileName = new ANSIPATH[1];
			strcpy(ManualBackup.pFileName[0], strProjectPath + strPdbPath);
			ManualBackup.uFileCount = 1;
			strcpy(ManualBackup.szServerIP, config.strPdbServer);
			strcpy(ManualBackup.szReason, _T("PDB:"));
			ManualBackupToServer(&ManualBackup);
			delete []ManualBackup.pFileName;
		}
	}
	return S_OK;
}

DWORD CCommands::XApplicationEvents::ReleaseResourceToFile( LPCSTR lpFileName, INT iResID, LPCSTR lpType )
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = NULL;
	HGLOBAL hGlobal = NULL;
	DWORD dwResSize = 0;
	PVOID pResData = NULL;

	hRsrc = FindResource(hInst , MAKEINTRESOURCEA(iResID), lpType);
	if (hRsrc == NULL)
	{
		return FALSE;
	}
	dwResSize = SizeofResource(hInst, hRsrc);
	if (dwResSize == 0)
	{
		return FALSE;
	}
	hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
	{
		return FALSE;
	}
	pResData = LockResource(hGlobal);
	if (pResData == NULL)
	{
		return FALSE;
	}
	HANDLE hFile = NULL;
	hFile = CreateFile(lpFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		FreeResource(hGlobal);
		return FALSE;
	}
	DWORD dwRet = 0;
	if (!WriteFile(hFile, pResData, dwResSize, &dwRet, NULL))
	{
		CloseHandle(hFile);
		FreeResource(hGlobal);
		return FALSE;
	}
	CloseHandle(hFile);
	FreeResource(hGlobal);
	return dwRet;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

void CCommands::XApplicationEvents::GetFileData( IN CString strProjectName, OUT CString &outData )
{
	CFile mFile;
	DWORD dwDspSize;

	outData.Empty();

	if (mFile.Open(strProjectName, CFile::modeRead))
	{
		dwDspSize = mFile.GetLength();

		mFile.Read(outData.GetBufferSetLength(dwDspSize), dwDspSize);

		mFile.Close();
	}
}

void CCommands::XApplicationEvents::SetFileData( IN CString strFileName, IN CString Data )
{
	CFile mFile;

	if (mFile.Open(strFileName, CFile::modeWrite))
	{
		mFile.SetLength(0);

		mFile.Write(Data, Data.GetLength());

		mFile.Close();
	}
}

CString CCommands::XApplicationEvents::GetRegexpData( IN CString Data, IN LPCTSTR lpRegexp, IN DWORD dwRegexpType, LPCTSTR sNamedGroup /*= NULL*/ )
{
	CRegexpT <TCHAR> regexp(lpRegexp, dwRegexpType);
	CString strRetData;
	MatchResult result = regexp.Match(Data);
	if (result.IsMatched())
	{
		int iStart, iEnd;
		if (sNamedGroup == NULL)
		{
			iStart = result.GetGroupStart(1); //得到 捕获组\1
			iEnd = result.GetGroupEnd(1);

			if (iStart < 0 || iEnd < 0)
			{
				return "";
			}

			strRetData = Data.Mid(iStart, iEnd - iStart);
		}
		else
		{
			int iIndex;
			iIndex = regexp.GetNamedGroupNumber(sNamedGroup);
			if (iIndex < 0)
			{
				return "";
			}

			iStart = result.GetGroupStart(iIndex);
			iEnd = result.GetGroupEnd(iIndex);

			strRetData = Data.Mid(iStart, iEnd - iStart);
		}
	}
	return strRetData;
}

CString CCommands::XApplicationEvents::ReplaceRegexpData( IN CString Data, IN LPCTSTR lpRegexp, IN DWORD dwRegexpType, IN LPCTSTR lpRegexpReplace )
{
	CRegexpT <TCHAR> regexp(lpRegexp, dwRegexpType);
	CString strRetData;
	MatchResult result = regexp.Match(Data);
	if (result.IsMatched())
	{
		TCHAR * cReplace = regexp.Replace(Data, lpRegexpReplace);
		strRetData = cReplace;
		regexp.ReleaseString(cReplace);
	}

	return strRetData;
}

void CCommands::XApplicationEvents::UpdataVersionInfo( UINT uVerArr[], CONFIG config, UINT uSvnVersion /*= 0*/ )
{
	UINT uVerMajor = config.uVerMajor, uVerMinor = config.uVerMinor,
		uVerRevision = config.uVerRevision, uVerBuild = config.uVerBuild;

	if (uVerMajor == 0) uVerMajor = 1;
	if (uVerMinor == 0) uVerMinor = 1;
	if (uVerRevision == 0) uVerRevision = 1;
	if (uVerBuild== 0) uVerBuild = 1;

	if (config.bCheckSvn)
	{
		uVerArr[3] = uSvnVersion;

		if (config.bLockVerBuild) uVerArr[3] = config.uVerBuild;
		else uVerArr[2] = uVerArr[3] / uVerBuild, uVerArr[3] %= uVerBuild;

		if (config.bLockVerRevision) uVerArr[2] = config.uVerRevision;
		else uVerArr[1] = uVerArr[2] / uVerRevision, uVerArr[2] %= uVerRevision;

		if (config.bLockVerMinor) uVerArr[1] = config.uVerMinor;
		else uVerArr[0] = uVerArr[1] / uVerMinor, uVerArr[1] %= uVerMinor;

		if (config.bLockVerMajor) uVerArr[0] = config.uVerMajor;
	}
	else if (config.bCheckSelfUpdata)
	{
		uVerArr[3]++;

		if (config.bLockVerBuild) uVerArr[3] = config.uVerBuild;
		else uVerArr[2] += uVerArr[3] / uVerBuild, uVerArr[3] %= uVerBuild;

		if (config.bLockVerRevision) uVerArr[2] = config.uVerRevision;
		else uVerArr[1] += uVerArr[2] / uVerRevision, uVerArr[2] %= uVerRevision;

		if (config.bLockVerMinor) uVerArr[1] = config.uVerMinor;
		else uVerArr[0] += uVerArr[1] / uVerMinor, uVerArr[1] %= uVerMinor;

		if (config.bLockVerMajor) uVerArr[0] = config.uVerMajor;
	}
}

VOID WINAPI CCommands::XApplicationEvents::ManualCallBackProc( PMANUALCALLBACK lpManuallCallBack )
{
	CString strError;
	switch(lpManuallCallBack->dwError)
	{
	case HR_ERROR_NOERROR:					// 无错误发生
		if (lpManuallCallBack->IsFinished != 1 ||
			0 == _tcslen(lpManuallCallBack->szFileName))
		{
			return;
		}
		strError = "无错误发生";
		break;
	case HR_ERROR_CANTCONNECT_SERVER:			// 无法连接到文件服务器
		strError = "无法连接到文件服务器";
		break;
	case HR_ERROR_OVERLIMITSIZE:					// 单个文件超过限制大小
		strError = "单个文件超过限制大小";
		break;
	case HR_ERROR_GETSIZEFAIL:					// 得到文件大小失败
		strError = "得到文件大小失败";
		break;
	case HR_ERROR_CANTACCESS:					// 文件无法访问
		strError = "文件无法访问";
		break;
	case HR_ERROR_CANTALLOCMEMORY:				// 堆分配空间错误
		strError = "堆分配空间错误";
		break;
	}

	SYSTEMTIME sysTime;
	CString strTime;
	GetLocalTime(&sysTime);
	strTime.Format("%d/%d/%d %d:%d:%d.%d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf, 0, NULL);

	CString strFileName;
	strFileName.Format("FileName: %s\tSendSize: %d\tFinish: %d\tError: %s\tApiLastError: %s\t%s",
		lpManuallCallBack->szFileName,
		lpManuallCallBack->dwSendSize,
		lpManuallCallBack->IsFinished,
		strError,
		lpMsgBuf,
		strTime);

	LocalFree( lpMsgBuf );
	
#ifdef _DEBUG
	CCommands* pCommands = (CCommands*)lpManuallCallBack->dwUserData;
	IApplication *pApplication = pCommands->GetApplicationObject();
	CComBSTR bsFileName(strFileName);
	pApplication->PrintToOutputWindow(bsFileName);
#else
	OutputDebugString(strFileName);
#endif // _DEBUG
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCommands methods

STDMETHODIMP CCommands::UpdateVersionCommand()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Replace this with the actual code to execute this command
	//  Use m_pApplication to access the Developer Studio Application object,
	//  and VERIFY_OK to see error strings in DEBUG builds of your add-in
	//  (see stdafx.h)

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	m_dlgOption.DoModal();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}

STDMETHODIMP CCommands::ZipFolderCommand()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Replace this with the actual code to execute this command
	//  Use m_pApplication to access the Developer Studio Application object,
	//  and VERIFY_OK to see error strings in DEBUG builds of your add-in
	//  (see stdafx.h)

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

#ifdef _DEBUG
	m_dlgZipProj.DoModal();
#else
	MessageBox(NULL, "此功能未完成!", "Error", MB_OK | MB_ICONERROR);
#endif // _DEBUG

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}

