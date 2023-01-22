#ifndef __ASHITA_tRenderer_H_INCLUDED__
#define __ASHITA_tRenderer_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"
#include "AbilitySquareManager.h"

class tRenderer : IPlugin
{
private:
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    IDirect3DDevice8* m_Direct3DDevice;
    uint32_t m_PluginId;
    ULONG_PTR m_GDIToken;
    GdiLayer* m_GdiLayer;
    ImageCache* m_Cache;
    bool m_Unloading;
    std::map<std::string, AbilitySquareManager*> m_Managers;

public:
    const char* GetName(void) const override
    {
        return "tRenderer";
    }
    const char* GetAuthor(void) const override
    {
        return "Thorny";
    }
    const char* GetDescription(void) const override
    {
        return "Insert description here.";
    }
    const char* GetLink(void) const override
    {
        return "Insert link here.";
    }
    double GetVersion(void) const override
    {
        return 1.02f;
    }
    int32_t GetPriority(void) const override
    {
        return 0;
    }
    uint32_t GetFlags(void) const override
    {
        return (uint32_t)Ashita::PluginFlags::All;
    }
	
    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;

    // Event Callbacks: PluginManager
    void HandleEvent(const char* eventName, const void* eventData, const uint32_t eventSize) override;

    // Event Callbacks: Direct3D
    bool Direct3DInitialize(IDirect3DDevice8* device) override;	
    void Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override;
};
#endif