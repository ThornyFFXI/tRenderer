#ifndef __ASHITA_ImageCache_H_INCLUDED__
#define __ASHITA_ImageCache_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Defines.h"
#include "GdiResource.h"
#include <unordered_map>

class ImageCache
{
private:
    IAshitaCore* m_AshitaCore;
    std::unordered_map<std::string, Gdiplus::Image*> m_FileCache;
    std::unordered_map<int32_t, GdiResource*> m_ItemCache;
    std::unordered_map<int32_t, GdiResource*> m_StatusCache;

public:
    ImageCache(IAshitaCore* pAshitaCore);
    ~ImageCache();

    Gdiplus::Image* GetImage(const char* path);
    Gdiplus::Image* LoadItem(int32_t itemId);
    Gdiplus::Image* LoadStatus(int32_t statusId);
};
#endif