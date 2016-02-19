// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__E1256907_A4A1_4A49_A3F4_91A7862A1F27__INCLUDED_)
#define AFX_DSADDIN_H__E1256907_A4A1_4A49_A3F4_91A7862A1F27__INCLUDED_

#include "commands.h"

// {E18FF644-6ACC-4B00-94E3-EB8B4ABDC276}
DEFINE_GUID(CLSID_DSAddIn,
0xe18ff644, 0x6acc, 0x4b00, 0x94, 0xe3, 0xeb, 0x8b, 0x4a, 0xbd, 0xc2, 0x76);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "UpdateVersion.DSAddIn.1",
		"UPDATEVERSION Developer Studio Add-in", IDS_UPDATEVERSION_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);
protected:
	CCommandsObj* m_pCommands;
	CWinThread* m_pUpdateThread;
	HANDLE m_hUpdateEvent;
	DWORD m_dwCookie;
	
	static UINT UpdateThread( LPVOID lParam );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__E1256907_A4A1_4A49_A3F4_91A7862A1F27__INCLUDED)
