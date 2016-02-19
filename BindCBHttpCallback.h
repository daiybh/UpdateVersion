// BindCBHttpCallback.h: interface for the CBindCBHttpCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDCBHTTPCALLBACK_H__77B34036_4843_4927_8E64_9BFE709B32FB__INCLUDED_)
#define AFX_BINDCBHTTPCALLBACK_H__77B34036_4843_4927_8E64_9BFE709B32FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Commands.h"

class CBindCBHttpCallback : public IBindStatusCallback, public IHttpNegotiate
{
public:
	CBindCBHttpCallback();
	virtual ~CBindCBHttpCallback();
	
	// IBindStatusCallback methods
    STDMETHOD(OnStartBinding)(
		DWORD dwReserved,
        IBinding __RPC_FAR *pib)
	{ return E_NOTIMPL; }
	
    STDMETHOD(GetPriority)(
		LONG __RPC_FAR *pnPriority)
	{ return E_NOTIMPL; }
	
    STDMETHOD(OnLowResource)(
        DWORD reserved)
	{ return E_NOTIMPL; }
	
	STDMETHOD(OnProgress)(
        ULONG ulProgress,
        ULONG ulProgressMax,
        ULONG ulStatusCode,
        LPCWSTR wszStatusText);
	
    STDMETHOD(OnStopBinding)(
        HRESULT hresult,
		LPCWSTR szError)
	{ return E_NOTIMPL; }
	
    STDMETHOD(GetBindInfo)(
        DWORD __RPC_FAR *grfBINDF,
		BINDINFO __RPC_FAR *pbindinfo)
	{ return E_NOTIMPL; }
	
    STDMETHOD(OnDataAvailable)(
		DWORD grfBSCF,
		DWORD dwSize,
		FORMATETC __RPC_FAR *pformatetc,
        STGMEDIUM __RPC_FAR *pstgmed)
	{ return E_NOTIMPL; }
	
    STDMETHOD(OnObjectAvailable)(
		REFIID riid,
		IUnknown __RPC_FAR *punk)
	{ return E_NOTIMPL; }
	
	// IHttpNegotiate methods
	
	STDMETHODIMP BeginningTransaction(
		LPCWSTR szURL,
		LPCWSTR szHeaders,
		DWORD dwReserved,
		LPWSTR __RPC_FAR *pszAdditionalHeaders)
	{ return E_NOTIMPL; }
		
    STDMETHODIMP OnResponse(
		DWORD dwResponseCode,
		LPCWSTR szResponseHeaders,
		LPCWSTR szRequestHeaders,
		LPWSTR __RPC_FAR *pszAdditionalRequestHeaders)
	{ return E_NOTIMPL; }
	
    STDMETHOD_(ULONG,AddRef)()
	{ return 0; }
	
    STDMETHOD_(ULONG,Release)()
	{ return 0; }
	
    STDMETHOD(QueryInterface)(
		REFIID riid,
		void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOTIMPL;

		*ppvObject = NULL;
		if (riid==IID_IUnknown || riid==IID_IBindStatusCallback)	
		{
			*ppvObject = (IBindStatusCallback*)this;
			AddRef();
			return S_OK;
		}
		else if (riid==IID_IHttpNegotiate)
		{
			*ppvObject = (IHttpNegotiate*)this;
			AddRef();
			return S_OK;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
public:
	void SetCommands(CCommands* pCommands);
private:
	CCommands* m_pCommands;
};

#endif // !defined(AFX_BINDCBHTTPCALLBACK_H__77B34036_4843_4927_8E64_9BFE709B32FB__INCLUDED_)
