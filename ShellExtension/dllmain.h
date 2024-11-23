// dllmain.h : Declaration of module class.

class CRunAsTIShellExtensionModule : public ATL::CAtlDllModuleT< CRunAsTIShellExtensionModule >
{
public :
	DECLARE_LIBID(LIBID_RunAsTIShellExtensionLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_RUNASTISHELLEXTENSION, "{43e7ea1a-d008-4548-8a87-352cf109537c}")
};

extern class CRunAsTIShellExtensionModule _AtlModule;
