// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateVersion.h"
#include "OptionDlg.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "Version.lib")

/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog


COptionDlg::COptionDlg(CCommands* pCommands, CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent), m_pCommands(pCommands)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(COptionDlg)
	m_verBuild = 0;
	m_verMajor = 0;
	m_verMinor = 0;
	m_verRevision = 0;
	m_checkSelfUpdata = FALSE;
	m_checkSvn = FALSE;
	m_svnVerPathSelect = -1;
	m_bAutoCommit = FALSE;
	m_bSetGloble = FALSE;
	//}}AFX_DATA_INIT
}


void COptionDlg::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDlg)
	DDX_Control(pDX, IDC_NEEDCOMMITLIST, m_CommitList);
	DDX_Control(pDX, IDC_PDBSERVER, m_PdbServer);
	DDX_Text(pDX, IDC_VERBUILD, m_verBuild);
	DDX_Text(pDX, IDC_VERMAJOR, m_verMajor);
	DDX_Text(pDX, IDC_VERMINOR, m_verMinor);
	DDX_Text(pDX, IDC_VERREVISION, m_verRevision);
	DDX_Check(pDX, IDC_LOCKVERBUILD, m_verBuildLock);
	DDX_Check(pDX, IDC_LOCKVERMAJOR, m_verMajorLock);
	DDX_Check(pDX, IDC_LOCKVERMINOR, m_verMinorLock);
	DDX_Check(pDX, IDC_LOCKVERREVISION, m_verRevisionLock);
	DDX_Check(pDX, IDC_CHECKSELFUPDATA, m_checkSelfUpdata);
	DDX_Check(pDX, IDC_CHECKSVN, m_checkSvn);
	DDX_Radio(pDX, IDC_CKPROJECTPATH, m_svnVerPathSelect);
	DDX_Check(pDX, IDC_AUTOCOMMIT, m_bAutoCommit);
	DDX_Check(pDX, IDC_SETGLOBLE, m_bSetGloble);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	//{{AFX_MSG_MAP(COptionDlg)
	ON_BN_CLICKED(IDC_CHECKSVN, OnChecksvn)
	ON_BN_CLICKED(IDC_CHECKSELFUPDATA, OnCheckselfupdata)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LOCKVERMAJOR, OnLocksvnmajor)
	ON_BN_CLICKED(IDC_LOCKVERMINOR, OnLocksvnminor)
	ON_BN_CLICKED(IDC_LOCKVERREVISION, OnLocksvnrevision)
	ON_BN_CLICKED(IDC_LOCKVERBUILD, OnLocksvnbuild)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COptionDlg, CDialog)
	//{{AFX_DISPATCH_MAP(COptionDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOptionDlg to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C65698D5-69FF-4882-BE6A-8A35E2F051C8}
static const IID IID_IOptionDlg =
{ 0xc65698d5, 0x69ff, 0x4882, { 0xbe, 0x6a, 0x8a, 0x35, 0xe2, 0xf0, 0x51, 0xc8 } };

BEGIN_INTERFACE_MAP(COptionDlg, CDialog)
	INTERFACE_PART(COptionDlg, IID_IOptionDlg, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionDlg message handlers


void COptionDlg::GetConfig( CONFIG &config, CString strConfigName )
{
	DWORD dwAttr = NULL;
	if ((dwAttr = GetFileAttributes(strConfigName)) == -1 ||
		dwAttr == FILE_ATTRIBUTE_DIRECTORY)
	{
		// Get Globle Setting

		CString strGlobleConfig;
		GetModuleFileName(_Module.m_hInstResource, strGlobleConfig.GetBuffer(MAX_PATH), MAX_PATH);
		strGlobleConfig.ReleaseBuffer();

		TCHAR sDrive[_MAX_DRIVE] = {0};
		TCHAR sDir[_MAX_DIR] = {0};
		TCHAR sFname[_MAX_FNAME] = {0};
		TCHAR sExt[_MAX_EXT] = {0};
		
		// Get Globle Setting Config Path
		_tsplitpath(strGlobleConfig, sDrive, sDir, sFname, sExt);
		
		strGlobleConfig = CString(sDrive) + CString(sDir) + CString(sFname) + CString(_T(".ini"));

		UINT lSelect = GetPrivateProfileInt(UPDATAVERSION, _T("Select"), 1, strGlobleConfig);

		config.bCheckSvn = HIWORD(lSelect);
		config.bCheckSelfUpdata = LOWORD(lSelect);
		
		config.uVerMajor = GetPrivateProfileInt(UPDATAVERSION, _T("Major"), 1, strGlobleConfig);
		config.uVerMinor = GetPrivateProfileInt(UPDATAVERSION, _T("Minor"), 0, strGlobleConfig);
		config.uVerRevision = GetPrivateProfileInt(UPDATAVERSION, _T("Revision"), 0, strGlobleConfig);
		config.uVerBuild = GetPrivateProfileInt(UPDATAVERSION, _T("Build"), 10000, strGlobleConfig);
		
		config.bLockVerMajor = GetPrivateProfileInt(UPDATAVERSION, _T("MajorLock"), 1, strGlobleConfig);
		config.bLockVerMinor = GetPrivateProfileInt(UPDATAVERSION, _T("MinorLock"), 1, strGlobleConfig);
		config.bLockVerRevision = GetPrivateProfileInt(UPDATAVERSION, _T("RevisionLock"), 1, strGlobleConfig);
		config.bLockVerBuild = GetPrivateProfileInt(UPDATAVERSION, _T("BuildLock"), 0, strGlobleConfig);
		
		config.iSvnVersionPath = GetPrivateProfileInt(UPDATAVERSION, _T("Path"), 0, strGlobleConfig);
		
		config.bPdbAutoCommit = GetPrivateProfileInt(COMMITPDB, _T("AutoCommit"), 0, strGlobleConfig);
		
		GetPrivateProfileString(COMMITPDB, _T("ServerIP"), "0.0.0.0", config.strPdbServer.GetBuffer(MAX_PATH), MAX_PATH, strGlobleConfig);
		config.strPdbServer.ReleaseBuffer();
		
		GetPrivateProfileString(COMMITPDB, _T("CommitList"), "", config.strCommitList.GetBuffer(MAX_PATH), MAX_PATH, strGlobleConfig);
		config.strCommitList.ReleaseBuffer();
	}
	else 
	{
		// Load Local Setting
		UINT lSelect = GetPrivateProfileInt(UPDATAVERSION, _T("Select"), 1, strConfigName);
		
		config.bCheckSvn = HIWORD(lSelect);
		config.bCheckSelfUpdata = LOWORD(lSelect);
		
		config.uVerMajor = GetPrivateProfileInt(UPDATAVERSION, _T("Major"), 1, strConfigName);
		config.uVerMinor = GetPrivateProfileInt(UPDATAVERSION, _T("Minor"), 0, strConfigName);
		config.uVerRevision = GetPrivateProfileInt(UPDATAVERSION, _T("Revision"), 0, strConfigName);
		config.uVerBuild = GetPrivateProfileInt(UPDATAVERSION, _T("Build"), 10000, strConfigName);
		
		config.bLockVerMajor = GetPrivateProfileInt(UPDATAVERSION, _T("MajorLock"), 1, strConfigName);
		config.bLockVerMinor = GetPrivateProfileInt(UPDATAVERSION, _T("MinorLock"), 1, strConfigName);
		config.bLockVerRevision = GetPrivateProfileInt(UPDATAVERSION, _T("RevisionLock"), 1, strConfigName);
		config.bLockVerBuild = GetPrivateProfileInt(UPDATAVERSION, _T("BuildLock"), 0, strConfigName);
		
		config.iSvnVersionPath = GetPrivateProfileInt(UPDATAVERSION, _T("Path"), 0, strConfigName);
		
		config.bPdbAutoCommit = GetPrivateProfileInt(COMMITPDB, _T("AutoCommit"), 0, strConfigName);
		
		GetPrivateProfileString(COMMITPDB, _T("ServerIP"), "0.0.0.0", config.strPdbServer.GetBuffer(MAX_PATH), MAX_PATH, strConfigName);
		config.strPdbServer.ReleaseBuffer();
		
		GetPrivateProfileString(COMMITPDB, _T("CommitList"), "", config.strCommitList.GetBuffer(MAX_PATH), MAX_PATH, strConfigName);
		config.strCommitList.ReleaseBuffer();
	}
}

ULARGE_INTEGER COptionDlg::GetFileVersion( HMODULE hModule, UINT nID )
{
	ULARGE_INTEGER uVersion = {0};
	
	try 
	{
		CString strModuleFile;
		GetModuleFileName(hModule, strModuleFile.GetBuffer(MAX_PATH), MAX_PATH);
		strModuleFile.ReleaseBuffer();
		
		uVersion = GetFileVersion(strModuleFile, nID);
	}
	catch (...)
	{
	}
	
	return uVersion;
}

ULARGE_INTEGER COptionDlg::GetFileVersion( CString strModuleFile, UINT nID )
{
	ULARGE_INTEGER uVersion = {0};
	PVOID pData = NULL;
	
	try 
	{
		DWORD dwVerInfoSize = GetFileVersionInfoSize(strModuleFile, NULL);
		if (dwVerInfoSize > 0)
		{
			pData = malloc(dwVerInfoSize);
			if (pData)
			{
				if (GetFileVersionInfo(strModuleFile, NULL, dwVerInfoSize, pData))
				{
					if (pData != NULL)
					{
						VS_FIXEDFILEINFO * pFileInfo = NULL;
						UINT uInfoSize = 0; 
						
						if (VerQueryValue(pData, _T("\\"), (void**)&pFileInfo, &uInfoSize))
						{
							uVersion.LowPart = pFileInfo->dwFileVersionLS;
							uVersion.HighPart = pFileInfo->dwFileVersionMS;
						}
					}
				}
				free(pData);
			}
		}
	}
	catch (...)
	{
		if (pData) free(pData);
	}
	
	return uVersion;
}

BOOL COptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	CString strTitle;
	CString strTitle2;
	ULARGE_INTEGER uVersion;
	
	GetWindowText(strTitle);
	uVersion = GetFileVersion(_Module.m_hInstResource, VS_VERSION_INFO);
	strTitle2.Format(_T("%s   Version:%d.%d.%d.%d"), strTitle,
		HIWORD(uVersion.HighPart), 
		LOWORD(uVersion.HighPart), 
		HIWORD(uVersion.LowPart), 
		LOWORD(uVersion.LowPart));
	SetWindowText(strTitle2);

	CComPtr<IGenericProject> pProjectItem = NULL;
	IApplication* pApplication;
	
	pApplication = m_pCommands->GetApplicationObject();
	pApplication->get_ActiveProject((IDispatch **)&pProjectItem.p);
	
	CComBSTR bStr;
	CString strProjectName;
	
	if (pProjectItem)
	{
		pProjectItem->get_FullName(&bStr);
		strProjectName = bStr;
		
		CComQIPtr<IBuildProject, &IID_IBuildProject> pBuildProj(pProjectItem);
		if (!pBuildProj)
		{
			return S_OK;
		}
		
		CComPtr<IConfigurations> pConfigurations;
		VERIFY_OK(pBuildProj->get_Configurations(&pConfigurations));
		
		long nConfigCount;
		nConfigCount = 0;
		VERIFY_OK(pConfigurations->get_Count(&nConfigCount));
		for (long nConfigIndex = 0; nConfigIndex < nConfigCount; nConfigIndex++)
		{
			CComVariant vtItem;
			BSTR bsConfigName = NULL;
			CString strConfigName;
			CComPtr<IConfiguration> pConfig = NULL;
			vtItem = nConfigIndex + 1;
			VERIFY_OK(pConfigurations->Item(vtItem, &pConfig));
			if (pConfig == NULL)
			{
				return S_OK;
			}
			VERIFY_OK(pConfig->get_Name(&bsConfigName));
			strConfigName = bsConfigName;
			m_CommitList.AddString(strConfigName);
		}
	}
	else
	{
		MessageBox("Not Open Any WorkSpace", "Open Option Error", MB_OK | MB_ICONERROR);
		OnCancel();
		return FALSE;
	}
	
	{
		TCHAR sDrive[_MAX_DRIVE] = {0};
		TCHAR sDir[_MAX_DIR] = {0};
		TCHAR sFname[_MAX_FNAME] = {0};
		TCHAR sExt[_MAX_EXT] = {0};
		
		_tsplitpath(strProjectName, sDrive, sDir, sFname, sExt);
		
		m_strConfigName = CString(sDrive) + CString(sDir) + CString(sFname) + CString(_T(".ini"));
	}
	
	{
		CString strGlobleConfig;
		GetModuleFileName(_Module.m_hInstResource, strGlobleConfig.GetBuffer(MAX_PATH), MAX_PATH);
		strGlobleConfig.ReleaseBuffer();
		
		// Get Globle Setting Config Path
		TCHAR sDrive[_MAX_DRIVE] = {0};
		TCHAR sDir[_MAX_DIR] = {0};
		TCHAR sFname[_MAX_FNAME] = {0};
		TCHAR sExt[_MAX_EXT] = {0};
		
		_tsplitpath(strGlobleConfig, sDrive, sDir, sFname, sExt);
		m_strGlobleConfig = CString(sDrive) + CString(sDir) + CString(sFname) + CString(_T(".ini"));
	}

	GetConfig(config, m_strConfigName);

	m_verMajor = config.uVerMajor;
	m_verMinor = config.uVerMinor;
	m_verRevision = config.uVerRevision;
	m_verBuild = config.uVerBuild;

	m_verMajorLock = config.bLockVerMajor;
	m_verMinorLock = config.bLockVerMinor;
	m_verRevisionLock = config.bLockVerRevision;
	m_verBuildLock = config.bLockVerBuild;
	
	m_svnVerPathSelect = config.iSvnVersionPath;
	
	m_checkSvn = config.bCheckSvn;
	m_checkSelfUpdata = config.bCheckSelfUpdata;

	m_bAutoCommit = config.bPdbAutoCommit;
	
	BYTE bIP[4];
	sscanf(config.strPdbServer, "%d.%d.%d.%d", &bIP[0], &bIP[1], &bIP[2], &bIP[3]);
	m_PdbServer.SetAddress(bIP[0], bIP[1], bIP[2], bIP[3]);

	int iNum = m_CommitList.GetCount();
	int i = 0;
	for (i = 0; i < iNum; i++)
	{
		CString strItemStr;
		m_CommitList.GetText(i, strItemStr);
		if (-1 != config.strCommitList.Find(strItemStr + '#'))
		{
			m_CommitList.SetSel(i, TRUE);
		}
	}
	UpdateData(FALSE);
	
	for (i = 0; i < 4; i++)
	{
		SetSvnLock(i);
	}
	
	if (m_checkSvn)	{
		SwitchType(IDC_CHECKSVN);
	} else if (m_checkSelfUpdata) {
		SwitchType(IDC_CHECKSELFUPDATA);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDlg::SaveOption()
{
	UpdateData(TRUE);
	
	CString strPdbServer;
	BYTE bIP[4];
	m_PdbServer.GetAddress(bIP[0], bIP[1], bIP[2], bIP[3]);
	strPdbServer.Format("%d.%d.%d.%d",
		bIP[0], bIP[1], bIP[2], bIP[3]);
	
	CString strSelect;
	int iSelCount = m_CommitList.GetSelCount();
	if (iSelCount > 0)
	{
		int *iSelIndex = new int[iSelCount];
		CString strTmp;
		m_CommitList.GetSelItems(iSelCount, iSelIndex);
		for (int i = 0; i < iSelCount; i++)
		{
			m_CommitList.GetText(iSelIndex[i], strTmp);
			strSelect += strTmp + '#';
		}
		delete []iSelIndex;
	}
	
	WritePrivateProfileInt(UPDATAVERSION, _T("Major"), m_verMajor, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("Minor"), m_verMinor, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("Revision"), m_verRevision, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("Build"), m_verBuild, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("MajorLock"), m_verMajorLock, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("MinorLock"), m_verMinorLock, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("RevisionLock"), m_verRevisionLock, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("BuildLock"), m_verBuildLock, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("Path"), m_svnVerPathSelect, m_strConfigName);
	WritePrivateProfileInt(UPDATAVERSION, _T("Select"), MAKELONG(m_checkSelfUpdata, m_checkSvn), m_strConfigName);
	WritePrivateProfileString(COMMITPDB, _T("ServerIP"), strPdbServer, m_strConfigName);
	WritePrivateProfileInt(COMMITPDB, _T("AutoCommit"), m_bAutoCommit ? 1 : 0, m_strConfigName);
	WritePrivateProfileString(COMMITPDB, _T("CommitList"), strSelect, m_strConfigName);

	if (m_bSetGloble)
	{
		WritePrivateProfileInt(UPDATAVERSION, _T("Major"), m_verMajor, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("Minor"), m_verMinor, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("Revision"), m_verRevision, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("Build"), m_verBuild, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("MajorLock"), m_verMajorLock, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("MinorLock"), m_verMinorLock, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("RevisionLock"), m_verRevisionLock, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("BuildLock"), m_verBuildLock, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("Path"), m_svnVerPathSelect, m_strGlobleConfig);
		WritePrivateProfileInt(UPDATAVERSION, _T("Select"), MAKELONG(m_checkSelfUpdata, m_checkSvn), m_strGlobleConfig);
		WritePrivateProfileString(COMMITPDB, _T("ServerIP"), strPdbServer, m_strGlobleConfig);
		WritePrivateProfileInt(COMMITPDB, _T("AutoCommit"), m_bAutoCommit ? 1 : 0, m_strGlobleConfig);
		WritePrivateProfileString(COMMITPDB, _T("CommitList"), strSelect, m_strGlobleConfig);
	}

	DWORD dwAttr = NULL;
	if ((dwAttr = GetFileAttributes(m_strConfigName)) != -1
		&& dwAttr != FILE_ATTRIBUTE_DIRECTORY
		&& (dwAttr = GetFileAttributes(m_strGlobleConfig)) != -1
		&& dwAttr != FILE_ATTRIBUTE_DIRECTORY)
	{
		DWORD crc0 = 0;
		DWORD crc1 = 0;
		if ((crc0 = GetFileCrc(m_strConfigName))
			&& (crc1 = GetFileCrc(m_strGlobleConfig))
			&& crc0 == crc1)
		{
			SetFileAttributes(m_strConfigName, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(m_strConfigName);
		}
	}
}

BOOL COptionDlg::WritePrivateProfileInt( LPCTSTR App, LPCTSTR Key, UINT uNum, LPCTSTR strConfigName )
{
	CString strNum;
	strNum.Format(_T("%d"), uNum);

	return WritePrivateProfileString(App, Key,strNum, strConfigName);
}

void COptionDlg::OnChecksvn() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);

	if (m_checkSelfUpdata || m_checkSvn)
	{
		CheckRadioButton(IDC_CHECKSVN, IDC_CHECKSELFUPDATA, IDC_CHECKSVN);
		SwitchType(IDC_CHECKSVN);
	}
	else
	{
		CheckRadioButton(IDC_CHECKSVN, IDC_CHECKSELFUPDATA, IDC_CHECKSELFUPDATA);
		SwitchType(IDC_CHECKSELFUPDATA);
	}
}

void COptionDlg::OnCheckselfupdata() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	
	if (!m_checkSelfUpdata || !m_checkSvn)
	{
		CheckRadioButton(IDC_CHECKSVN, IDC_CHECKSELFUPDATA, IDC_CHECKSVN);
		SwitchType(IDC_CHECKSVN);
	}
	else
	{
		CheckRadioButton(IDC_CHECKSVN, IDC_CHECKSELFUPDATA, IDC_CHECKSELFUPDATA);
		SwitchType(IDC_CHECKSELFUPDATA);
	}
}

void COptionDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
	SaveOption();
}

void COptionDlg::SwitchType( UINT uType )
{
	if (uType == IDC_CHECKSVN) {
		GetDlgItem(IDC_CKPROJECTPATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_CKROOTPATH)->EnableWindow(TRUE);
	} else if (uType == IDC_CHECKSELFUPDATA) {
		GetDlgItem(IDC_CKPROJECTPATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CKROOTPATH)->EnableWindow(FALSE);
	}
}

void COptionDlg::OnLocksvnmajor() 
{
	// TODO: Add your control notification handler code here

	SetSvnLock(0);
}

void COptionDlg::OnLocksvnminor() 
{
	// TODO: Add your control notification handler code here
	
	SetSvnLock(1);
}

void COptionDlg::OnLocksvnrevision() 
{
	// TODO: Add your control notification handler code here
	
	SetSvnLock(2);
}

void COptionDlg::OnLocksvnbuild() 
{
	// TODO: Add your control notification handler code here
	
	SetSvnLock(3);
}

void COptionDlg::SetSvnLock(UINT uLock)
{
	UpdateData(TRUE);
	BOOL bSet[] = {m_verMajorLock, m_verMinorLock, m_verRevisionLock, m_verBuildLock};
	
	if (uLock >= sizeof(bSet)/sizeof(BOOL *))
	{
		return;
	}

	if (bSet[uLock])
	{
		for (UINT i = 0; i < uLock; i++)
		{
			bSet[i] = TRUE;
		}
	}
	else
	{
		for (UINT i = uLock; i < sizeof(bSet)/sizeof(BOOL *); i++)
		{
			bSet[i] = FALSE;
		}
	}
	
	m_verMajorLock = bSet[0];
	m_verMinorLock = bSet[1];
	m_verRevisionLock = bSet[2];
	m_verBuildLock = bSet[3];
	UpdateData(FALSE);
}

UINT COptionDlg::Split(CString strText, CStringArray &strArrResult, char chSymbol)
{
	strArrResult.RemoveAll();
	
	int nLength = strText.GetLength();
	
	if(nLength > 0 && strText.GetAt(0) == chSymbol)
	{
		strText.Delete(0);
		
		nLength -= 1;
	}
	
	while(nLength > 0)
	{
		for(int nIndex = 0; nIndex < nLength; nIndex++)
		{
			if(strText.GetAt(nIndex) == chSymbol)
			{
				strArrResult.Add(strText.Left(nIndex));
				
				strText.Delete(0, nIndex + 1);
				
				break;
			}
		}
		
		if(nIndex == nLength)
		{
			strArrResult.Add(strText);
			
			return strArrResult.GetSize();
		}
		
		nLength = strText.GetLength();
	}
	
	return strArrResult.GetSize();
}

DWORD COptionDlg::GetFileCrc( CString strFileName )
{
	DWORD crcRet = 0;

	CFile file;
	if (file.Open(strFileName, CFile::modeRead))
	{
		DWORD dwFileSize = file.GetLength();
		if (dwFileSize > 0)
		{
			PCHAR pFileData = new char[dwFileSize];
			if (pFileData)
			{
				file.Read(pFileData, dwFileSize);
				crcRet = CRC32(pFileData, dwFileSize);
				delete []pFileData;
			}
		}

		file.Close();
	}

	return crcRet;
}

DWORD COptionDlg::CRC32( PVOID pv, DWORD n )
{
	TRY 
	{
		PCHAR ptr = (PCHAR)pv;
		DWORD crcTable[256], crcTmp1;
		
		//????CRC-32?
		for (int i = 0; i<256; i++)
		{
			crcTmp1 = i;
			for (int j = 8; j>0; j--)
			{
				if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
				else crcTmp1 >>=  1;
			}
			
			crcTable[i] = crcTmp1;
		}
		//??CRC32?
		DWORD crcTmp2 =  0xFFFFFFFF;
		while(n--)
		{
			crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
			ptr++;
		}
		return (crcTmp2^0xFFFFFFFF);
	}
	CATCH (CMemoryException, e)
	{
		return NULL;
	}
	END_CATCH
}
