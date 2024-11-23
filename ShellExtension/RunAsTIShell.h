// RunAsTIShell.h : Declaration of the CRunAsTIShell

#pragma once
#include "resource.h"       // main symbols

#include "RunAsTIShellExtension_i.h"

#include <ShlObj.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CRunAsTIShell

class ATL_NO_VTABLE CRunAsTIShell :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRunAsTIShell, &CLSID_RunAsTIShell>,
	public IDispatchImpl<IRunAsTIShell, &IID_IRunAsTIShell, &LIBID_RunAsTIShellExtensionLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CRunAsTIShell();
	virtual ~CRunAsTIShell();

	DECLARE_REGISTRY_RESOURCEID(IDR_RUNASTISHELLEXTENSION)

	BEGIN_COM_MAP(CRunAsTIShell)
		COM_INTERFACE_ENTRY(IRunAsTIShell)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// Inherited via IShellExtInit
	HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID);

	// Inherited via IContextMenu
	HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
	HRESULT STDMETHODCALLTYPE InvokeCommand(CMINVOKECOMMANDINFO* pici);
	HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax);

private:
	LPITEMIDLIST m_pIDFolder;
	wchar_t      m_szFile[MAX_PATH];
	IDataObject* m_pDataObj;
	HKEY         m_hRegKey;

	char    m_pszVerb[128];
	wchar_t m_pwszVerb[128];

	HBITMAP m_hShieldBitmap;
	wchar_t m_runAsTIPath[512];

	void CreateIconBitmaps();
	void CreateRunAsTIExecutablePath();
};

OBJECT_ENTRY_AUTO(__uuidof(RunAsTIShell), CRunAsTIShell)
