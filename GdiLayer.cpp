#include "GdiLayer.h"

GdiLayer::GdiLayer(IDirect3DDevice8* pDevice, int width, int height)
    : m_Device(pDevice)
    , m_Height(height)
    , m_Width(width)
    , m_Sprite(nullptr)
    , m_Texture(nullptr)
{
    // Create bitmap in memory..
    m_Size      = width * height * 4;
    m_RawImage  = malloc(m_Size + 108);
    m_RawPixels = (void*)((uint8_t*)m_RawImage + 108);
    memset(m_RawImage, 0, m_Size + 108);
    auto p_Header              = (BITMAPV4HEADER*)m_RawImage;
    p_Header->bV4Size          = sizeof(BITMAPV4HEADER);
    p_Header->bV4Width         = width;
    p_Header->bV4Height        = height;
    p_Header->bV4Planes        = 1;
    p_Header->bV4BitCount      = 32;
    p_Header->bV4V4Compression = BI_BITFIELDS;
    p_Header->bV4RedMask       = 0x00FF0000;
    p_Header->bV4GreenMask     = 0x0000FF00;
    p_Header->bV4BlueMask      = 0x000000FF;
    p_Header->bV4AlphaMask     = 0xFF000000;

    // Create gdiplus objects using bitmap in memory..
    this->m_Bitmap   = new Gdiplus::Bitmap(width, height, width * 4, PixelFormat32bppARGB, (BYTE*)m_RawPixels);
    this->m_Graphics = new Gdiplus::Graphics(this->m_Bitmap);
    this->m_Graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    this->m_Graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    this->m_Graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    this->m_Dirty = true;

    // Create values for sprite rendering..
    m_ScaleV   = D3DXVECTOR2(1.0f, 1.0f);
    m_DrawRect = RECT{0, 0, this->m_Width, this->m_Height};
}
GdiLayer::~GdiLayer()
{
    SAFE_RELEASE(this->m_Texture);
    SAFE_RELEASE(this->m_Sprite);
    delete this->m_Graphics;
    delete this->m_Bitmap;
}

void GdiLayer::Clear()
{
    memset(m_RawPixels, 0, m_Width * m_Height * 4);
}

void GdiLayer::ClearRegion(Gdiplus::Rect region)
{
    auto stride        = m_Width * 4;
    auto pitch         = region.Width * 4;
    auto height        = region.Height;
    auto scanOffset    = (stride * region.Y) + (region.X * 4);
    uint8_t* dstPixels = (uint8_t*)m_RawPixels + scanOffset;

    for (int y = 0; y < height; y++)
    {
        memset(dstPixels, 0, pitch);
        dstPixels += stride;
    }
}

Gdiplus::Graphics* GdiLayer::GetGraphics()
{
    return m_Graphics;
}

bool GdiLayer::Render(int offsetX, int offsetY)
{
    // Create or update texture..
    if (m_Texture == nullptr)
    {
        if (FAILED(::D3DXCreateTexture(m_Device, m_Width, m_Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_Texture)))
        {
            g_AshitaCore->GetChatManager()->Writef(0, false, "%s%s",
                Ashita::Chat::Header("tRenderer").c_str(),
                Ashita::Chat::Error("Failed to create texture.").c_str());
            return false;
        }

        if (FAILED(m_Texture->GetLevelDesc(0, &this->m_SurfaceDesc)))
        {
            g_AshitaCore->GetChatManager()->Writef(0, false, "%s%s",
                Ashita::Chat::Header("tRenderer").c_str(),
                Ashita::Chat::Error("Failed to get texture surface description.").c_str());
            SAFE_RELEASE(m_Texture);
            return false;
        }
        
        // Initial texture creation is coming out upside down and I don't really care if it has to do an extra lock once on initialization..
        this->m_Dirty = true;
    }
    
    if (this->m_Dirty == true)
    {
        D3DLOCKED_RECT rect{};
        m_Texture->LockRect(0, &rect, 0, 0);
        {
            memcpy(rect.pBits, m_RawPixels, m_Size);
        }
        m_Texture->UnlockRect(0);
        this->m_Dirty = false;
    }

    // Create sprite..
    if (m_Sprite == nullptr)
    {
        if (FAILED(::D3DXCreateSprite(this->m_Device, &this->m_Sprite)))
        {
            g_AshitaCore->GetChatManager()->Writef(0, false, "%s%s",
                Ashita::Chat::Header("tRenderer").c_str(),
                Ashita::Chat::Error("Failed to create sprite.").c_str());
            return false;
        }
    }

    // Draw rect..
    auto transV = D3DXVECTOR2(offsetX, offsetY);

    // Save the current state block..
    DWORD stateBlock = 0;
    if (FAILED(this->m_Device->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
    {
        g_AshitaCore->GetChatManager()->Writef(0, false, "%s%s",
            Ashita::Chat::Header("tRenderer").c_str(),
            Ashita::Chat::Error("Failed to create state block.").c_str());
        return false;
    }

    // Render sprite..
    this->m_Sprite->Begin();
    this->m_Device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    this->m_Device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
    this->m_Device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    this->m_Sprite->Draw(this->m_Texture, &m_DrawRect, &m_ScaleV, nullptr, 0.0f, &transV, 0xFFFFFFFF);
    this->m_Sprite->End();

    // Restore state block..
    this->m_Device->ApplyStateBlock(stateBlock);
    this->m_Device->DeleteStateBlock(stateBlock);
    return true;
}

void GdiLayer::SetDirty(bool dirty)
{
    this->m_Dirty = dirty;
}