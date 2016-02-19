// ZipProj.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateVersion.h"
#include "ZipProj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZipProj dialog


CZipProj::CZipProj(CWnd* pParent /*=NULL*/)
	: CDialog(CZipProj::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(CZipProj)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CZipProj::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CZipProj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZipProj)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZipProj, CDialog)
	//{{AFX_MSG_MAP(CZipProj)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CZipProj, CDialog)
	//{{AFX_DISPATCH_MAP(CZipProj)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IZipProj to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {B518DC84-6431-4A96-9890-218E06EDD741}
static const IID IID_IZipProj =
{ 0xb518dc84, 0x6431, 0x4a96, { 0x98, 0x90, 0x21, 0x8e, 0x6, 0xed, 0xd7, 0x41 } };

BEGIN_INTERFACE_MAP(CZipProj, CDialog)
	INTERFACE_PART(CZipProj, IID_IZipProj, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZipProj message handlers
