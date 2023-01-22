#ifndef __ASHITA_Defines_H_INCLUDED__
#define __ASHITA_Defines_H_INCLUDED__

#include "C:\Ashita 4\plugins\sdk\Ashita.h"
#include <Windows.h>
#include <atlbase.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
#include <filesystem>
#include <fstream>
#include <map>
#pragma comment(lib, "C:/Ashita 4/plugins/sdk/d3d8/lib/d3dx8.lib")


extern IAshitaCore* g_AshitaCore;

#define SAFE_DELETE(p) \
    if (p)             \
    {                  \
        delete p;      \
        p = nullptr;   \
    }
#define SAFE_RELEASE(p) \
    if (p)              \
    {                   \
        p->Release();   \
        p = nullptr;    \
    }

struct FontInitializer_t
{
    int OffsetX;
    int OffsetY;
    int BoxWidth;
    int BoxHeight;
    int OutlineWidth;
    int FontHeight;
    char FontFamily[256];
    uint32_t FontFlags;
    uint32_t FontAlignment;
    uint32_t FontColor;
    uint32_t OutlineColor;
};

struct GdiplusFontObject_t
{
    Gdiplus::FontFamily* FontFamily;
    Gdiplus::StringFormat* FontFormat;
    Gdiplus::FontStyle FontStyle;
    int FontHeight;
    Gdiplus::Rect Rect;
    Gdiplus::Pen* Pen;
    Gdiplus::Brush* Brush;

    GdiplusFontObject_t()
        : FontFamily(nullptr)
        , FontFormat(nullptr)
        , FontStyle(Gdiplus::FontStyle::FontStyleRegular)
        , FontHeight(0)
        , Rect{0, 0, 0, 0}
        , Pen(nullptr)
        , Brush(nullptr)
    {}

    GdiplusFontObject_t(FontInitializer_t initializer, Gdiplus::Brush** ppBrush, Gdiplus::Pen** ppPen)
    {
        SAFE_DELETE(*ppBrush);
        SAFE_DELETE(*ppPen);

        wchar_t fontBuffer[256];
        swprintf_s(fontBuffer, 256, L"%S", initializer.FontFamily);
        FontFamily = new Gdiplus::FontFamily(fontBuffer);
        FontStyle  = static_cast<Gdiplus::FontStyle>(initializer.FontFlags);
        FontFormat = new Gdiplus::StringFormat();
        FontFormat->SetAlignment((Gdiplus::StringAlignment)initializer.FontAlignment);
        FontHeight = initializer.FontHeight;

        if ((initializer.OutlineWidth > 0) && ((initializer.OutlineColor & 0xFF000000) != 0))
        {
            BYTE alpha = (initializer.OutlineColor & 0xFF000000) >> 24;
            BYTE red   = (initializer.OutlineColor & 0x00FF0000) >> 16;
            BYTE green = (initializer.OutlineColor & 0x0000FF00) >> 8;
            BYTE blue  = (initializer.OutlineColor & 0x000000FF);
            auto color = Gdiplus::Color(alpha, red, green, blue);
            *ppPen = new Gdiplus::Pen(color, initializer.OutlineWidth);
            Pen    = *ppPen;
        }
        else
            Pen = nullptr;

        if ((initializer.FontColor & 0xFF000000) != 0)
        {
            BYTE alpha = (initializer.FontColor & 0xFF000000) >> 24;
            BYTE red   = (initializer.FontColor & 0x00FF0000) >> 16;
            BYTE green = (initializer.FontColor & 0x0000FF00) >> 8;
            BYTE blue  = (initializer.FontColor & 0x000000FF);
            auto color = Gdiplus::Color(alpha, red, green, blue);
            *ppBrush = new Gdiplus::SolidBrush(color);
            Brush    = *ppBrush;
        }
        else
            Brush = nullptr;
    }
};

struct ImageInitializer_t
{
    int OffsetX;
    int OffsetY;
    int Width;
    int Height;
};

struct SquareInitializer_t
{
    int PositionX;
    int PositionY;
};

struct EventInitializer_t
{
    char UniqueIdentifier[256];

    FontInitializer_t Cost;
    FontInitializer_t Macro;
    FontInitializer_t Name;
    FontInitializer_t Recast;

    ImageInitializer_t Frame;
    ImageInitializer_t Icon;
    ImageInitializer_t Overlay;

    float IconFadeAlpha;
    char FramePath[256];

    int PanelHeight;
    int PanelWidth;

    int SquareHeight;
    int SquareWidth;
    int SquareCount;
    SquareInitializer_t Squares[100];
};

struct AbilitySquareConfiguration_t
{
    GdiplusFontObject_t Cost;
    GdiplusFontObject_t Macro;
    GdiplusFontObject_t Name;
    GdiplusFontObject_t Recast;

    Gdiplus::Rect SquareRect;
    Gdiplus::Rect FrameRect;
    Gdiplus::Rect IconRect;
    Gdiplus::Rect OverlayRect;

    Gdiplus::Image* FrameImage;
    Gdiplus::ImageAttributes* IconFadeAttributes;
};

struct AbilitySquareState_t
{
    uint32_t Fade;
    char Cost[32];
    char Macro[32];
    char Name[32];
    char Recast[32];

    char IconImage[256];
    char OverlayImage[3][256];

    AbilitySquareState_t()
        : Cost{0}
        , Macro{0}
        , Name{0}
        , Recast{0}
        , IconImage{0}
    {
        memset(OverlayImage, 0, 3 * 256);
    }
};

struct AbilitySquarePanelState_t
{
    uint32_t Render;
    int32_t PositionX;
    int32_t PositionY;
    AbilitySquareState_t Squares[100];

    AbilitySquarePanelState_t()
        : Render(0)
        , PositionX(0)
        , PositionY(0)
    {}
};
#endif 