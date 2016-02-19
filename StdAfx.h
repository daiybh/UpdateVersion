// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3B8BD7A0_E6A2_4164_882F_229479849958__INCLUDED_)
#define AFX_STDAFX_H__3B8BD7A0_E6A2_4164_882F_229479849958__INCLUDED_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>
#include <afxdisp.h>
#include <afxcmn.h>
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Developer Studio Object Model
#include <ObjModel\addauto.h>
#include <ObjModel\appdefs.h>
#include <ObjModel\appauto.h>
#include <ObjModel\blddefs.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\textdefs.h>
#include <ObjModel\textauto.h>
#include <ObjModel\dbgdefs.h>
#include <ObjModel\dbgauto.h>

/////////////////////////////////////////////////////////////////////////////
// Debugging support

// Use VERIFY_OK around all calls to the Developer Studio objects which
//  you expect to return S_OK.
// In DEBUG builds of your add-in, VERIFY_OK displays an ASSERT dialog box
//  if the expression returns an HRESULT other than S_OK.  If the HRESULT
//  is a success code, the ASSERT box will display that HRESULT.  If it
//  is a failure code, the ASSERT box will display that HRESULT plus the
//  error description string provided by the object which raised the error.
// In RETAIL builds of your add-in, VERIFY_OK just evaluates the expression
//  and ignores the returned HRESULT.

#ifndef _SafeOutputDebugString
inline void _SafeOutputDebugString(LPCTSTR format, ...)
{
	CString strOutPut;
	
	va_list argList;
	va_start(argList, format);
	strOutPut.FormatV(format, argList);
	va_end(argList);
	
	if (strOutPut.IsEmpty())
	{
		return;
	}
	
	TCHAR ch = strOutPut.GetAt(strOutPut.GetLength() - 1);
	if (ch != '\n' && ch != '\r')
	{
		strOutPut += "\r\n";
	}
	
	OutputDebugString(strOutPut);
}
#endif

#ifdef _DEBUG

void GetLastErrorDescription(CComBSTR& bstr);		// Defined in UpdateVersion.cpp
#define VERIFY_OK(f) \
	{ \
		HRESULT hr = (f); \
		if (hr != S_OK) \
		{ \
			if (FAILED(hr)) \
			{ \
				CComBSTR bstr; \
				GetLastErrorDescription(bstr); \
				_RPTF2(_CRT_ASSERT, "Object call returned %lx\n\n%S", hr, (BSTR) bstr); \
			} \
			else \
				_RPTF1(_CRT_ASSERT, "Object call returned %lx", hr); \
		} \
	}

#else //_DEBUG

#define VERIFY_OK(f) (f);

#endif //_DEBUG

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3B8BD7A0_E6A2_4164_882F_229479849958__INCLUDED)
