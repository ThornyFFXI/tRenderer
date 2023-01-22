#ifndef __ASHITA_AbilitySquareManager_H_INCLUDED__
#define __ASHITA_AbilitySquareManager_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"
#include "AbilitySquare.h"

class AbilitySquareManager
{
private:
    EventInitializer_t m_Initializer;
    std::vector<AbilitySquare*> m_Squares;
    AbilitySquarePanelState_t* m_State;
    ImageCache* m_Cache;
    IDirect3DDevice8* m_Device;
    GdiLayer* m_Layer;
    bool m_Rendered;

    Gdiplus::Pen* pCostPen;
    Gdiplus::Pen* pMacroPen;
    Gdiplus::Pen* pNamePen;
    Gdiplus::Pen* pRecastPen;
    Gdiplus::Brush* pCostBrush;
    Gdiplus::Brush* pMacroBrush;
    Gdiplus::Brush* pNameBrush;
    Gdiplus::Brush* pRecastBrush;
    Gdiplus::ImageAttributes* pFadeAttributes;

public:
    AbilitySquareManager(EventInitializer_t initializer, ImageCache* pCache, IDirect3DDevice8* pDevice);
    ~AbilitySquareManager();
    void ReleaseObjects();
    void* GetPointer(EventInitializer_t initializer);
    void Initialize();
    bool Render();
    void Update();
};
#endif