#ifndef __ASHITA_GdiLayer_H_INCLUDED__
#define __ASHITA_GdiLayer_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"

class GdiLayer
{
private:
    // Rendering components
    IDirect3DDevice8* m_Device;
    IDirect3DTexture8* m_Texture;
    ID3DXSprite* m_Sprite;
    D3DXIMAGE_INFO m_ImageInfo;
    D3DSURFACE_DESC m_SurfaceDesc;
    RECT m_DrawRect;
    D3DXVECTOR2 m_ScaleV;
    bool m_Dirty;

    // Bitmap components
    int m_Width;
    int m_Height;
    int m_Size;
    void* m_RawImage;
    void* m_RawPixels;

    // Gdiplus objects
    Gdiplus::Bitmap* m_Bitmap;
    Gdiplus::Graphics* m_Graphics;

public:
    GdiLayer(IDirect3DDevice8* pDevice, int width, int height);
    ~GdiLayer();

    void Clear();
    void ClearRegion(Gdiplus::Rect region);
    Gdiplus::Graphics* GetGraphics();
    void Render(int offsetX, int offsetY);
    void SetDirty(bool dirty);
    Gdiplus::Bitmap* GetBitmap()
    {
        return m_Bitmap;
    };
};
#endif