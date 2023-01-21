#include "AbilitySquare.h"

AbilitySquare::AbilitySquare(AbilitySquareConfiguration_t config, AbilitySquareState_t* pState, ImageCache* pCache, GdiLayer* pLayer)
    : m_Cache(pCache)
    , m_Config(config)
    , m_CostPath(nullptr)
    , m_Dirty(false)
    , m_IconImage(nullptr)
    , m_Layer(pLayer)
    , m_MacroPath(nullptr)
    , m_NamePath(nullptr)
    , m_pState(pState)
    , m_RecastPath(nullptr)
    , m_State(AbilitySquareState_t())
{
    this->m_OverlayImage[0] = nullptr;
    this->m_OverlayImage[1] = nullptr;
    this->m_OverlayImage[2] = nullptr;
}

void AbilitySquare::Clear()
{
    this->m_Layer->ClearRegion(this->m_Config.SquareRect);
}

void AbilitySquare::CreatePath(Gdiplus::GraphicsPath** ppGraphicsPath, GdiplusFontObject_t Font, const char* text)
{
    SAFE_DELETE(*ppGraphicsPath);

    if ((text == nullptr) || (strlen(text) == 0))
        return;

    wchar_t textBuffer[1024];
    swprintf_s(textBuffer, 1024, L"%S", text);
    *ppGraphicsPath = new Gdiplus::GraphicsPath();
    (*ppGraphicsPath)->AddString(textBuffer, wcslen(textBuffer), Font.FontFamily, Font.FontStyle, Font.FontHeight, Font.Rect, Font.FontFormat);
}

void AbilitySquare::Render()
{
    if (!this->m_Dirty)
        return;

    this->m_Layer->SetDirty(true);
    this->Clear();

    auto graphics = m_Layer->GetGraphics();
    graphics->DrawImage(this->m_Config.FrameImage, this->m_Config.FrameRect);

    if (this->m_IconImage)
    {
        if ((this->m_State.Fade) && (m_Config.IconFadeAttributes))
            graphics->DrawImage(this->m_IconImage, this->m_Config.IconRect, 0, 0, this->m_IconImage->GetWidth(), this->m_IconImage->GetHeight(), Gdiplus::Unit::UnitPixel, this->m_Config.IconFadeAttributes);
        else
            graphics->DrawImage(this->m_IconImage, this->m_Config.IconRect);
    }

    for (int x = 0; x < 3; x++)
    {
        if (this->m_OverlayImage[x])
        {
            graphics->DrawImage(this->m_OverlayImage[x], this->m_Config.OverlayRect);
        }
    }

    if (this->m_CostPath)
    {
        if (this->m_Config.Cost.Pen)
            graphics->DrawPath(this->m_Config.Cost.Pen, this->m_CostPath);
        if (this->m_Config.Cost.Brush)
            graphics->FillPath(this->m_Config.Cost.Brush, this->m_CostPath);
    }

    if (this->m_MacroPath)
    {
        if (this->m_Config.Macro.Pen)
            graphics->DrawPath(this->m_Config.Macro.Pen, this->m_MacroPath);
        if (this->m_Config.Macro.Brush)
            graphics->FillPath(this->m_Config.Macro.Brush, this->m_MacroPath);
    }

    if (this->m_NamePath)
    {
        if (this->m_Config.Name.Pen)
            graphics->DrawPath(this->m_Config.Name.Pen, this->m_NamePath);
        if (this->m_Config.Name.Brush)
            graphics->FillPath(this->m_Config.Name.Brush, this->m_NamePath);
    }

    if (this->m_RecastPath)
    {
        if (this->m_Config.Recast.Pen)
            graphics->DrawPath(this->m_Config.Recast.Pen, this->m_RecastPath);
        if (this->m_Config.Recast.Brush)
            graphics->FillPath(this->m_Config.Recast.Brush, this->m_RecastPath);
    }

    this->m_Dirty = false;
}

void AbilitySquare::Update()
{
    auto state = *m_pState;

    if (m_State.Fade != state.Fade)
    {
        this->m_Dirty = true;
    }

    if (strcmp(m_State.Cost, state.Cost))
    {
        this->CreatePath(&this->m_CostPath, this->m_Config.Cost, state.Cost);
        this->m_Dirty = true;
    }

    if (strcmp(m_State.Macro, state.Macro))
    {
        this->CreatePath(&this->m_MacroPath, this->m_Config.Macro, state.Macro);
        this->m_Dirty = true;
    }

    if (strcmp(m_State.Name, state.Name))
    {
        this->CreatePath(&this->m_NamePath, this->m_Config.Name, state.Name);
        this->m_Dirty = true;
    }

    if (strcmp(m_State.Recast, state.Recast))
    {
        this->CreatePath(&this->m_RecastPath, this->m_Config.Recast, state.Recast);
        this->m_Dirty = true;
    }

    if (strcmp(m_State.IconImage, state.IconImage))
    {
        this->m_IconImage = m_Cache->GetImage(state.IconImage);
        this->m_Dirty = true;
    }

    for (int x = 0; x < 3; x++)
    {
        if (strcmp(m_State.OverlayImage[x], state.OverlayImage[x]))
        {
            this->m_OverlayImage[x] = m_Cache->GetImage(state.OverlayImage[x]);
            this->m_Dirty = true;
        }
    }

    this->m_State = state;
}