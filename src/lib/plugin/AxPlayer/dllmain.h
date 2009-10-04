// dllmain.h : Declaration of module class.

class CAxPlayerModule : public CAtlDllModuleT< CAxPlayerModule >
{
public :
	DECLARE_LIBID(LIBID_AxPlayerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AXPLAYER, "{C13A9297-79B1-4BA9-AC24-4820E7656027}")
};

extern class CAxPlayerModule _AtlModule;
