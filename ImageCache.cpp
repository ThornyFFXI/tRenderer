#include "ImageCache.h"

std::string normalizePath(const std::string& messyPath)
{
    std::filesystem::path path(messyPath);
    std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
    std::string npath                   = canonicalPath.make_preferred().string();
    return npath;
}

ImageCache::ImageCache(IAshitaCore* pAshitaCore)
    : m_AshitaCore(pAshitaCore)
{
    m_FileCache = std::unordered_map<std::string, Gdiplus::Image*>();
    m_ItemCache = std::unordered_map<int32_t, GdiResource*>();
    m_StatusCache = std::unordered_map<int32_t, GdiResource*>();
}

ImageCache::~ImageCache()
{
    for (auto iter = m_FileCache.begin(); iter != m_FileCache.end(); iter = m_FileCache.erase(iter))
    {
        delete iter->second;
    }
    for (auto iter = m_ItemCache.begin(); iter != m_ItemCache.end(); iter = m_ItemCache.erase(iter))
    {
        delete iter->second;
    }
    for (auto iter = m_StatusCache.begin(); iter != m_StatusCache.end(); iter = m_StatusCache.erase(iter))
    {
        delete iter->second;
    }
}

Gdiplus::Image* ImageCache::GetImage(const char* path)
{
    if (strstr(path, "ITEM:") == path)
    {
        return LoadItem(atoi(path + 5));
    }
    
    /* Need to look into how the resource is structured for this..
    else if (strstr(path, "STATUS:") == path)
    {
        return LoadStatus(atoi(path + 7));
    }
    */

    // This might want to eventually clear old images, since global 10k gdiplus object limit applies and some asshole might allocate every item resource..

    auto nPath = normalizePath(path);
    auto iter = m_FileCache.find(nPath);
    if (iter != m_FileCache.end())
    {
        return iter->second;
    }

    wchar_t textBuffer[1024];
    swprintf_s(textBuffer, 1024, L"%S", nPath.c_str());
    try
    {
        auto image = new Gdiplus::Image(textBuffer);
        m_FileCache[nPath] = image;
        return image;
    }
    catch (...)
    {
        return nullptr;
    }
}

Gdiplus::Image* ImageCache::LoadItem(int32_t itemId)
{
    auto iter     = m_ItemCache.find(itemId);
    if (iter != m_ItemCache.end())
        return iter->second->GetImage();

    auto resource = m_AshitaCore->GetResourceManager()->GetItemById(itemId);
    if (resource)
    {
        auto converted = new GdiResource(resource->Bitmap, resource->ImageSize);
        m_ItemCache[itemId] = converted;
        return converted->GetImage();
    }

    return nullptr;
}
Gdiplus::Image* ImageCache::LoadStatus(int32_t statusId)
{
    auto iter = m_StatusCache.find(statusId);
    if (iter != m_StatusCache.end())
        return iter->second->GetImage();

    auto resource = m_AshitaCore->GetResourceManager()->GetStatusIconById(statusId);    
    if (resource)
    {
        auto converted      = new GdiResource(resource->Bitmap, resource->ImageSize);
        m_StatusCache[statusId] = converted;
        return converted->GetImage();
    }

    return nullptr;
}