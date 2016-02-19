#if !defined(AFX_ZIPPROJ_H__419B7A0A_E937_432E_881D_B15C8FF0533B__INCLUDED_)
#define AFX_ZIPPROJ_H__419B7A0A_E937_432E_881D_B15C8FF0533B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZipProj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZipProj dialog

class CZipProj : public CDialog
{
// Construction
public:
	CZipProj(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CZipProj)
	enum { IDD = IDD_ZIPPRJFOLDER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZipProj)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZipProj)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CZipProj)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZIPPROJ_H__419B7A0A_E937_432E_881D_B15C8FF0533B__INCLUDED_)
