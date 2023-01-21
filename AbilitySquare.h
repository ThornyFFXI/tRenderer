#ifndef __ASHITA_AbilitySquare_H_INCLUDED__
#define __ASHITA_AbilitySquare_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"
#include "GdiLayer.h"
#include "ImageCache.h"

class AbilitySquare
{
private:
    ImageCache* m_Cache;
    AbilitySquareConfiguration_t m_Config;
    GdiLayer* m_Layer;
    AbilitySquareState_t* m_pState;
    AbilitySquareState_t m_State;
    bool m_Dirty;

    Gdiplus::GraphicsPath* m_CostPath;
    Gdiplus::GraphicsPath* m_MacroPath;
    Gdiplus::GraphicsPath* m_NamePath;
    Gdiplus::GraphicsPath* m_RecastPath;
    Gdiplus::Image* m_IconImage;
    Gdiplus::Image* m_OverlayImage[3];

public:
    AbilitySquare(AbilitySquareConfiguration_t config, AbilitySquareState_t* pState, ImageCache* pCache, GdiLayer* pLayer);

    void Clear();
    void CreatePath(Gdiplus::GraphicsPath** ppGraphicsPath, GdiplusFontObject_t Font, const char* text);
    void Render();
    void Update();
};
#endif