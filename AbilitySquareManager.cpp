#include "AbilitySquareManager.h"


AbilitySquareManager::AbilitySquareManager(EventInitializer_t initializer, ImageCache* pCache, IDirect3DDevice8* pDevice)
    : pCostBrush(nullptr)
    , pCostPen(nullptr)
    , pMacroBrush(nullptr)
    , pMacroPen(nullptr)
    , pNameBrush(nullptr)
    , pNamePen(nullptr)
    , pRecastBrush(nullptr)
    , pRecastPen(nullptr)
    , pFadeAttributes(nullptr)
    , m_Initializer(initializer)
    , m_Cache(pCache)
    , m_Device(pDevice)
    , m_Layer(nullptr)
    , m_State(nullptr)
{
    this->Initialize();
}

AbilitySquareManager::~AbilitySquareManager()
{
    this->ReleaseObjects();
}

void AbilitySquareManager::ReleaseObjects()
{
    SAFE_DELETE(pCostBrush);
    SAFE_DELETE(pCostPen);
    SAFE_DELETE(pMacroBrush);
    SAFE_DELETE(pMacroPen);
    SAFE_DELETE(pNameBrush);
    SAFE_DELETE(pNamePen);
    SAFE_DELETE(pRecastBrush);
    SAFE_DELETE(pRecastPen);
    SAFE_DELETE(pFadeAttributes);

    while (m_Squares.size() > 0)
    {
        delete (m_Squares.back());
        m_Squares.pop_back();
    }

    SAFE_DELETE(m_Layer);
    SAFE_DELETE(m_State);
}

EventInitializer_t AbilitySquareManager::GetInitializer()
{
    return m_Initializer;
}

void* AbilitySquareManager::GetPointer(EventInitializer_t initializer)
{
    if (memcmp(&initializer, &this->m_Initializer, sizeof(EventInitializer_t)) == 0)
    {
        return m_State;
    }

    this->ReleaseObjects();

    m_Initializer = initializer;

    this->Initialize();

    return m_State;
}

void AbilitySquareManager::Initialize()
{
    m_State = new AbilitySquarePanelState_t();
    m_Layer = new GdiLayer(m_Device, m_Initializer.PanelWidth, m_Initializer.PanelHeight);

    AbilitySquareConfiguration_t config;
    config.Cost                = GdiplusFontObject_t(m_Initializer.Cost, &pCostBrush, &pCostPen);
    config.Macro               = GdiplusFontObject_t(m_Initializer.Macro, &pMacroBrush, &pMacroPen);
    config.Name                = GdiplusFontObject_t(m_Initializer.Name, &pNameBrush, &pNamePen);
    config.Recast              = GdiplusFontObject_t(m_Initializer.Recast, &pRecastBrush, &pRecastPen);
    this->pFadeAttributes      = new Gdiplus::ImageAttributes();
    config.IconFadeAttributes  = this->pFadeAttributes;
    auto frameImage            = m_Cache->GetImage(this->m_Initializer.FramePath);
    config.FrameImage          = frameImage;

    Gdiplus::ColorMatrix fadeMatrix{
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, m_Initializer.IconFadeAlpha, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    this->pFadeAttributes->SetColorMatrix(&fadeMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

    for (int x = 0; x < m_Initializer.SquareCount; x++)
    {
        auto square          = m_Initializer.Squares[x];
        auto positionX       = square.PositionX;
        auto positionY       = square.PositionY;
        config.SquareRect    = Gdiplus::Rect(positionX, positionY, this->m_Initializer.SquareWidth, this->m_Initializer.SquareHeight);
        config.FrameRect     = Gdiplus::Rect(positionX + this->m_Initializer.Frame.OffsetX, positionY + this->m_Initializer.Frame.OffsetY, this->m_Initializer.Frame.Width, this->m_Initializer.Frame.Height);
        config.IconRect      = Gdiplus::Rect(positionX + this->m_Initializer.Icon.OffsetX, positionY + this->m_Initializer.Icon.OffsetY, this->m_Initializer.Icon.Width, this->m_Initializer.Icon.Height);
        config.OverlayRect   = Gdiplus::Rect(positionX + this->m_Initializer.Overlay.OffsetX, positionY + this->m_Initializer.Overlay.OffsetY, this->m_Initializer.Overlay.Width, this->m_Initializer.Overlay.Height);
        config.Cost.Rect     = Gdiplus::Rect(positionX + this->m_Initializer.Cost.OffsetX, positionY + this->m_Initializer.Cost.OffsetY, this->m_Initializer.Cost.BoxWidth, this->m_Initializer.Cost.BoxHeight);
        config.Macro.Rect    = Gdiplus::Rect(positionX + this->m_Initializer.Macro.OffsetX, positionY + this->m_Initializer.Macro.OffsetY, this->m_Initializer.Macro.BoxWidth, this->m_Initializer.Macro.BoxHeight);
        config.Name.Rect     = Gdiplus::Rect(positionX + this->m_Initializer.Name.OffsetX, positionY + this->m_Initializer.Name.OffsetY, this->m_Initializer.Name.BoxWidth, this->m_Initializer.Name.BoxHeight);
        config.Recast.Rect   = Gdiplus::Rect(positionX + this->m_Initializer.Recast.OffsetX, positionY + this->m_Initializer.Recast.OffsetY, this->m_Initializer.Recast.BoxWidth, this->m_Initializer.Recast.BoxHeight);

        auto newSquare = new AbilitySquare(config, &(this->m_State->Squares[x]), this->m_Cache, this->m_Layer);
        this->m_Squares.push_back(newSquare);
    }
}

bool AbilitySquareManager::Render()
{
    if ((m_State == nullptr) || (m_State->Render == 0))
    {
        return true;
    }
    m_State->Render = 0;

    for (auto iter = m_Squares.begin(); iter != m_Squares.end(); iter++)
    {
        (*iter)->Render();
    }

    return m_Layer->Render(m_State->PositionX, m_State->PositionY);
}

void AbilitySquareManager::Update()
{
    /*
     * Addon should only be altering shared memory during render event.
     * This plugin has a higher priority than addons.dll, so it will execute afterwards.
     * As a result, every update and render call will execute without the shared memory changing.
     */

    if ((m_State == nullptr) || (m_State->Render == 0))
        return;

    for (auto iter = m_Squares.begin(); iter != m_Squares.end(); iter++)
    {
        (*iter)->Update();
    }
}