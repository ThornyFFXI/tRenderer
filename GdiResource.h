#ifndef __ASHITA_GdiResource_H_INCLUDED__
#define __ASHITA_GdiResource_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"

class GdiResource
{
private:
    HBITMAP m_DIB;
    void* pPixels;
    Gdiplus::Bitmap* m_Bitmap;

public:
    GdiResource(uint8_t* bitmap, uint32_t size);
    ~GdiResource();

    Gdiplus::Image* GetImage();
};
#endif