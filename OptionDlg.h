#if !defined(AFX_OPTIONDLG_H__52ABF6A7_3F42_4384_90AD_FD6FF876DBDA__INCLUDED_)
#define AFX_OPTIONDLG_H__52ABF6A7_3F42_4384_90AD_FD6FF876DBDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog
class CCommands;

#define UPDATAVERSION _T("UpdataVersion")
#define COMMITPDB _T("CommitPdb")

typedef struct tagConfig 
{
	UINT uVerMajor, uVerMinor, uVerRevision, uVerBuild;
	BOOL bLockVerMajor, bLockVerMinor, bLockVerRevision, bLockVerBuild;
	BOOL bCheckSelfUpdata, bCheckSvn;
	INT iSvnVersionPath;
	BOOL bPdbAutoCommit;
	CString strPdbServer;
	CString strCommitList;
}CONFIG, *PCONFIG;

class COptionDlg : public CDialog
{
// Construction
public:
	COptionDlg(CCommands* pCommands, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionDlg)
	enum { IDD = IDD_OPTION };
	CListBox	m_CommitList;
	CIPAddressCtrl	m_PdbServer;
	UINT	m_verBuild;
	UINT	m_verMajor;
	UINT	m_verMinor;
	UINT	m_verRevision;
	BOOL	m_verBuildLock;
	BOOL	m_verMajorLock;
	BOOL	m_verMinorLock;
	BOOL	m_verRevisionLock;
	BOOL	m_checkSelfUpdata;
	BOOL	m_checkSvn;
	int		m_svnVerPathSelect;
	BOOL	m_bAutoCommit;
	BOOL	m_bSetGloble;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDlg)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDlg)
	afx_msg void OnChecksvn();
	afx_msg void OnCheckselfupdata();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnLocksvnmajor();
	afx_msg void OnLocksvnminor();
	afx_msg void OnLocksvnrevision();
	afx_msg void OnLocksvnbuild();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(COptionDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	void SaveOption();
	BOOL WritePrivateProfileInt( LPCTSTR App, LPCTSTR Key, UINT uNum, LPCTSTR strConfigName );
	void SwitchType( UINT uType );
	UINT Split(CString strText, CStringArray &strArrResult, char chSymbol);
	void SetSvnLock(UINT uLock);

	CCommands* m_pCommands;
	CString m_strConfigName;
	CONFIG config;
	CString m_strGlobleConfig;

public:
	static void GetConfig(CONFIG &config, CString strConfigName);
	static ULARGE_INTEGER GetFileVersion( HMODULE hModule, UINT nID );
	static ULARGE_INTEGER GetFileVersion( CString strModuleFile, UINT nID );
	DWORD GetFileCrc( CString strFileName );
	DWORD CRC32( PVOID pv, DWORD n );
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDLG_H__52ABF6A7_3F42_4384_90AD_FD6FF876DBDA__INCLUDED_)
