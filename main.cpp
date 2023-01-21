#include "tRenderer.h"

__declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);

    return (IPlugin*)(new tRenderer());
}

__declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

bool tRenderer::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    m_AshitaCore = core;
    m_LogManager = logger;
    m_PluginId   = id;
    m_Cache      = new ImageCache(core);
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_GDIToken, &gdiplusStartupInput, NULL);

    return true;
}

void tRenderer::Release(void)
{
    for (auto iter = m_Managers.begin(); iter != m_Managers.end(); iter = m_Managers.erase(iter))
    {
        delete iter->second;
    }
    SAFE_DELETE(m_Cache);
    Gdiplus::GdiplusShutdown(m_GDIToken);    
}

void tRenderer::HandleEvent(const char* eventName, const void* eventData, const uint32_t eventSize)
{
	UNREFERENCED_PARAMETER(eventName);
	UNREFERENCED_PARAMETER(eventData);
	UNREFERENCED_PARAMETER(eventSize);

    if (strcmp(eventName, "tRenderer_Initialize") == 0)
    {
        EventInitializer_t event = *((EventInitializer_t*)eventData);
        std::string matchString(event.UniqueIdentifier);
        auto iter = m_Managers.find(matchString);
        if (iter == m_Managers.end())
        {
            AbilitySquareManager* newManager = new AbilitySquareManager(event, m_Cache, m_Direct3DDevice);
            m_Managers[matchString]          = newManager;
            iter                             = m_Managers.find(matchString);
        }
        auto ptr = iter->second->GetPointer(event);

        char responseEventName[512];
        sprintf_s(responseEventName, 512, "tRenderer_Accessor_%s", event.UniqueIdentifier);
        m_AshitaCore->GetPluginManager()->RaiseEvent(responseEventName, &ptr, 4);
    }

    if (strcmp(eventName, "tRenderer_Destroy") == 0)
    {
        std::string matchString((const char*)eventData);
        auto iter = m_Managers.find(matchString);
        if (iter != m_Managers.end())
        {
            delete iter->second;
            m_Managers.erase(iter);
        }
    }
}

bool tRenderer::Direct3DInitialize(IDirect3DDevice8* device)
{
	this->m_Direct3DDevice = device;
	return true;
}

void tRenderer::Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	UNREFERENCED_PARAMETER(pSourceRect);
	UNREFERENCED_PARAMETER(pDestRect);
	UNREFERENCED_PARAMETER(hDestWindowOverride);
    UNREFERENCED_PARAMETER(pDirtyRegion);

    for (auto iter = m_Managers.begin(); iter != m_Managers.end(); iter++)
    {
        iter->second->Update();
    }

    for (auto iter = m_Managers.begin(); iter != m_Managers.end(); iter++)
    {
        iter->second->Render();
    }
}