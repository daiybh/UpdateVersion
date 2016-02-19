// BindCBHttpCallback.cpp: implementation of the CBindCBHttpCallback class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateVersion.h"
#include "BindCBHttpCallback.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindCBHttpCallback::CBindCBHttpCallback()
{

}

CBindCBHttpCallback::~CBindCBHttpCallback()
{

}
HRESULT  CBindCBHttpCallback ::OnProgress(ULONG ulProgress,ULONG ulProgressMax,ULONG ulStatusCode,LPCWSTR wszStatusText)
{ 
#ifdef _DEBUG
	BINDSTATUS bindStats = (BINDSTATUS)ulStatusCode;
	if (bindStats == BINDSTATUS_BEGINDOWNLOADDATA ||
		bindStats == BINDSTATUS_DOWNLOADINGDATA)
	{
		IApplication* pApplication;
		pApplication = m_pCommands->GetApplicationObject();
		CString strBit;
		strBit.Format("Download %d%%\r\n", ulProgress * 100 / ulProgressMax);
		CComBSTR bsBit(strBit);
		pApplication->PrintToOutputWindow(bsBit);
	}
	else if (bindStats == BINDSTATUS_ENDDOWNLOADDATA)
	{
		IApplication* pApplication;
		pApplication = m_pCommands->GetApplicationObject();
		CString strBit;
		strBit.Format("File:%S Downloaded\r\n", wszStatusText);
		CComBSTR bsBit(strBit);
		pApplication->PrintToOutputWindow(bsBit);
	}
#endif // _DEBUG

	return S_OK; 
}

void CBindCBHttpCallback::SetCommands( CCommands* pCommands )
{
	m_pCommands = pCommands;
}