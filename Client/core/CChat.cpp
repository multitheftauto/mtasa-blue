/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CChat.cpp
 *  PURPOSE:     In-game chat box user interface implementation
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <game/CSettings.h>

using std::vector;

extern CCore* g_pCore;

CChat* g_pChat = NULL;

CChat::CChat(CGUI* pManager, const CVector2D& vecPosition)
{
    g_pChat = this;
    m_pManager = pManager;

    // Calculate relative position (assuming a 800x600 native resolution for our defined CCHAT_* values)
    CVector2D vecResolution = m_pManager->GetResolution();
    m_vecScale = CVector2D(vecResolution.fX / 800.0f, vecResolution.fY / 600.0f);
    m_vecBackgroundPosition = vecPosition * vecResolution;

    // Initialize variables
    m_iScrollState = 0;
    m_uiMostRecentLine = 0;
    m_uiScrollOffset = 0;
    m_fSmoothScroll = 0;
    m_fSmoothLastTimeSeconds = 0;
    m_fSmoothAllowAfter = 0;
    m_fSmoothScrollResetTime = 0;
    m_fSmoothRepeatTimer = 0;
    m_TextColor = CHAT_TEXT_COLOR;
    m_bUseCEGUI = false;
    m_iCVarsRevision = -1;
    m_bVisible = false;
    m_bInputBlocked = false;
    m_bInputVisible = false;
    m_pFont = m_pManager->GetClearFont();
    m_pDXFont = NULL;
    SetDxFont(g_pCore->GetGraphics()->GetFont());
    m_fNativeWidth = CHAT_WIDTH;
    m_fRcpUsingDxFontScale = 1;
    m_bCanChangeWidth = true;
    m_iScrollingBack = 0;
    m_fCssStyleOverrideAlpha = 0.0f;
    m_fBackgroundAlpha = 0.0f;
    m_fInputBackgroundAlpha = 0.f;
    m_pCacheTexture = NULL;
    m_iCacheTextureRevision = 0;
    m_iReportCount = 0;
    m_fPositionOffsetX = 0.0125f;
    m_fPositionOffsetY = 0.0150f;
    m_ePositionHorizontal = Chat::Position::Horizontal::LEFT;
    m_ePositionVertical = Chat::Position::Vertical::TOP;
    m_eTextAlign = Chat::Text::Align::LEFT;
    m_iSelectedInputHistoryEntry = -1;
    m_iCharacterLimit = m_iDefaultCharacterLimit;

    // Background area
    m_pBackground = m_pManager->CreateStaticImage();
    m_pBackgroundTexture = m_pManager->CreateTexture();
    m_pBackground->LoadFromTexture(m_pBackgroundTexture);
    m_pBackground->MoveToBack();
    m_pBackground->SetPosition(m_vecBackgroundPosition);
    m_pBackground->SetSize(m_vecBackgroundSize);
    m_pBackground->SetEnabled(false);
    m_pBackground->SetVisible(false);

    // Input area
    m_pInput = m_pManager->CreateStaticImage();
    m_pInputTexture = m_pManager->CreateTexture();
    m_pInput->LoadFromTexture(m_pInputTexture);
    m_pInput->MoveToBack();
    m_pInput->SetPosition(m_vecInputPosition);
    m_pInput->SetSize(m_vecInputSize);
    m_pInput->SetEnabled(false);
    m_pInput->SetVisible(false);
    SetInputPrefix("Say: ");

    // Load cvars and position the GUI
    LoadCVars();
    UpdateGUI();
}

CChat::~CChat()
{
    Clear();
    ClearInput();

    SetDxFont(NULL);
    SAFE_DELETE(m_pBackground);
    SAFE_DELETE(m_pBackgroundTexture);
    SAFE_DELETE(m_pInput);
    SAFE_DELETE(m_pInputTexture);
    SAFE_DELETE(m_pInputHistory);

    if (g_pChat == this)
        g_pChat = NULL;
}

void CChat::OnModLoad()
{
    // Set handlers
    m_pManager->SetCharacterKeyHandler(INPUT_MOD, GUI_CALLBACK_KEY(&CChat::CharacterKeyHandler, this));
}

void CChat::LoadCVars()
{
    unsigned int Font;
    float        fWidth = 1;

    CVARS_GET("chat_color", m_Color);
    if (m_bCanChangeWidth)
        SetColor(m_Color);
    CVARS_GET("chat_input_color", m_InputColor);
    SetInputColor(m_InputColor);
    CVARS_GET("chat_input_text_color", m_InputTextColor);
    CVARS_GET("chat_use_cegui", m_bUseCEGUI);
    CVARS_GET("chat_lines", m_uiNumLines);
    SetNumLines(m_uiNumLines);
    CVARS_GET("chat_text_color", m_TextColor);
    CVARS_GET("chat_scale", m_vecScale);
    CVARS_GET("chat_width", fWidth);
    if (m_bCanChangeWidth)
        m_fNativeWidth = fWidth * CHAT_WIDTH;
    CVARS_GET("chat_text_outline", m_bTextBlackOutline);
    CVARS_GET("chat_css_style_text", m_bCssStyleText);
    CVARS_GET("chat_css_style_background", m_bCssStyleBackground);
    CVARS_GET("chat_line_life", (unsigned int&)m_ulChatLineLife);
    CVARS_GET("chat_line_fade_out", (unsigned int&)m_ulChatLineFadeOut);
    CVARS_GET("chat_font", (unsigned int&)Font);
    SetChatFont((eChatFont)Font);
    CVARS_GET("chat_nickcompletion", m_bNickCompletion);
    CVARS_GET("chat_position_offset_x", m_fPositionOffsetX);
    CVARS_GET("chat_position_offset_y", m_fPositionOffsetY);
    CVARS_GET("chat_position_horizontal", (unsigned int&)m_ePositionHorizontal);
    CVARS_GET("chat_position_vertical", (unsigned int&)m_ePositionVertical);
    CVARS_GET("chat_text_alignment", (unsigned int&)m_eTextAlign);
}

//
// Draw
//
void CChat::Draw(bool bUseCacheTexture, bool bAllowOutline)
{
    // Are we visible and is input blocked?
    if (!m_bVisible && m_bInputBlocked)
        return;

    // Is it time to update all the chat related cvars?
    if (m_iCVarsRevision != CClientVariables::GetSingleton().GetRevision())
    {
        m_iCVarsRevision = CClientVariables::GetSingleton().GetRevision();
        LoadCVars();
        UpdateGUI();
    }

    bool bUsingOutline = m_bTextBlackOutline && bAllowOutline && bUseCacheTexture;
    DrawInputLine(bUsingOutline);

    if (m_bInputVisible)
    {
        // ChrML: Hack so chatbox input always works. It might get unfocused..
        if (!m_pBackground->IsActive())
        {
            m_pBackground->Activate();
        }
    }

    // Are we visible?
    if (!m_bVisible)
        return;

    // Get drawList for the chat box text
    SDrawList drawList;
    GetDrawList(drawList, bUsingOutline);

    // Calc some size info
    CVector2D chatTopLeft(drawList.renderBounds.fX1, drawList.renderBounds.fY1);
    CVector2D chatBotRight(drawList.renderBounds.fX2, drawList.renderBounds.fY2);
    CVector2D chatSize = chatBotRight - chatTopLeft;
    CRect2D   chatBounds(0, 0, chatSize.fX, chatSize.fY);

    // If we are not using a cache texture, just render the text directly to the screen
    if (!bUseCacheTexture)
    {
        DrawDrawList(drawList, chatTopLeft);
        return;
    }

    CGraphics* pGraphics = CGraphics::GetSingletonPtr();

    // Validate rendertarget
    if (m_pCacheTexture && chatSize != m_RenderTargetChatSize)
    {
        SAFE_RELEASE(m_pCacheTexture);
    }

    // Create rendertarget if required
    if (!m_pCacheTexture && (CTickCount::Now() - m_lastRenderTargetCreationFail).ToLongLong() > 60000)
    {
        // Make sure render target size is reasonable
        int iRenderTargetSizeX = Clamp<int>(8, chatSize.fX, pGraphics->GetViewportWidth());
        int iRenderTargetSizeY = Clamp<int>(8, chatSize.fY, pGraphics->GetViewportHeight());
        m_pCacheTexture = pGraphics->GetRenderItemManager()->CreateRenderTarget(iRenderTargetSizeX, iRenderTargetSizeY, true, true);
        if (m_pCacheTexture)
            m_RenderTargetChatSize = chatSize;
        else
        {
            m_lastRenderTargetCreationFail = CTickCount::Now();

            // Maybe log render target creation fail
            if (m_iReportCount < 5)
            {
                m_iReportCount++;
                AddReportLog(6532, SString("Chat rt chatSize:%2.0f %2.0f   rtsize:%d %d   card:%s", chatSize.fX, chatSize.fY, iRenderTargetSizeX,
                                           iRenderTargetSizeY, g_pDeviceState->AdapterState.Name.c_str()));
            }
        }
        m_iCacheTextureRevision = -1;            // Make sure the graphics will be updated
    }

    // If we can't get a rendertarget for some reason, just render the text directly to the screen
    if (!m_pCacheTexture)
    {
        drawList.bOutline = false;            // Outline too slow without cache texture
        DrawDrawList(drawList, chatTopLeft);
        return;
    }

    // Update the cached graphics if the draw list has changed, or the render target has been recreated
    if (m_PrevDrawList != drawList || m_pCacheTexture->GetRevision() != m_iCacheTextureRevision)
    {
        m_PrevDrawList = drawList;
        m_iCacheTextureRevision = m_pCacheTexture->GetRevision();

        pGraphics->GetRenderItemManager()->SetRenderTarget(m_pCacheTexture, true);

        // Draw new stuff
        pGraphics->SetBlendMode(EBlendMode::MODULATE_ADD);
        DrawDrawList(drawList);
        pGraphics->SetBlendMode(EBlendMode::BLEND);

        pGraphics->GetRenderItemManager()->RestoreDefaultRenderTarget();
    }

    // Draw the cache texture
    pGraphics->SetBlendMode(EBlendMode::ADD);
    pGraphics->DrawTexture(m_pCacheTexture, chatTopLeft.fX, chatTopLeft.fY);
    pGraphics->SetBlendMode(EBlendMode::BLEND);
}

//
// DrawDrawList
//
// Draw the contents of the drawList
//
void CChat::DrawDrawList(const SDrawList& drawList, const CVector2D& topLeftOffset)
{
    CVector2D chatTopLeft(drawList.renderBounds.fX1, drawList.renderBounds.fY1);
    CVector2D chatBotRight(drawList.renderBounds.fX2, drawList.renderBounds.fY2);
    CVector2D chatSize = chatBotRight - chatTopLeft;

    float fRight = topLeftOffset.fX + chatSize.fX;
    float fLineHeight = CChat::GetFontHeight(g_pChat->m_vecScale.fY);

    CGraphics::GetSingleton().BeginDrawBatch();

    for (const auto& item : drawList.lineItemList)
    {
        CVector2D vecPosition = item.vecPosition - chatTopLeft + topLeftOffset;
        CRect2D   chatBounds(vecPosition.fX, vecPosition.fY, fRight, vecPosition.fY + fLineHeight);
        m_Lines[item.uiLine].Draw(vecPosition, item.ucAlpha, drawList.bShadow, drawList.bOutline, chatBounds);
    }

    CGraphics::GetSingleton().EndDrawBatch();
}

//
// GetDrawList
//
// Get list of text lines to draw
//
void CChat::GetDrawList(SDrawList& outDrawList, bool bUsingOutline)
{
    float     fLineDifference = CChat::GetFontHeight(m_vecScale.fY);
    CVector2D vecPosition(m_vecBackgroundPosition.fX + (5.0f * m_vecScale.fX), m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY - (fLineDifference * 1.25f));
    float     fMaxLineWidth = m_vecBackgroundSize.fX - (10.0f * m_vecScale.fX);
    unsigned long ulTime = GetTickCount32();
    float         fRcpChatLineFadeOut = 1.0f / m_ulChatLineFadeOut;
    bool          bShadow = (m_Color.A * m_fBackgroundAlpha == 0.f) && !bUsingOutline;

    if (m_Color.A * m_fBackgroundAlpha > 0.f)
    {
        // Hack to draw the background behind the text.
        m_pBackground->SetAlpha(m_fBackgroundAlpha);
        m_pBackground->SetVisible(true);
        m_pBackground->Render();
        m_pBackground->SetVisible(false);
    }

    // Used for render clipping in CChat::DrawTextString
    CRect2D RenderBounds(m_vecBackgroundPosition.fX, m_vecBackgroundPosition.fY + 1, m_vecBackgroundPosition.fX + m_vecBackgroundSize.fX,
                         m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY);

    outDrawList.renderBounds = RenderBounds;
    outDrawList.bShadow = bShadow;
    outDrawList.bOutline = bUsingOutline;

    // Smooth scroll
    int   iLineScroll;
    float fPixelScroll;
    UpdateSmoothScroll(&fPixelScroll, &iLineScroll);

    // Apply pixel smooth scroll
    vecPosition.fY -= fPixelScroll * fLineDifference;

    // Apply line smooth scroll
    unsigned int uiLine = (m_uiMostRecentLine + m_uiScrollOffset - iLineScroll) % CHAT_MAX_LINES;
    unsigned int uiLinesDrawn = 0;
    // Loop over the circular buffer
    while (m_Lines[uiLine].IsActive() && uiLinesDrawn < m_uiNumLines + (fabsf(m_fSmoothScroll) > 0.1f ? 1 : 0))
    {
        // Calculate line alpha
        long  ulLineAge = ulTime - m_Lines[uiLine].GetCreationTime();
        float fLineAlpha = (long)(m_ulChatLineLife + m_ulChatLineFadeOut - ulLineAge) * fRcpChatLineFadeOut;

        // Clamp line alpha range and combine with Css override
        fLineAlpha = Clamp(0.0f, fLineAlpha, 1.0f);
        fLineAlpha += (1.0f - fLineAlpha) * m_fCssStyleOverrideAlpha;

        if (fLineAlpha > 0.f)
        {
            CVector2D vecOffset;

            if (m_eTextAlign == Chat::Text::Align::RIGHT)
            {
                vecOffset.fX = fMaxLineWidth - m_Lines[uiLine].GetWidth();
            }

            SDrawListLineItem item;
            item.uiLine = uiLine;
            item.vecPosition = vecPosition + vecOffset;
            item.ucAlpha = static_cast<unsigned char>(fLineAlpha * 255.0f);
            outDrawList.lineItemList.push_back(item);
        }

        vecPosition.fY -= fLineDifference;

        uiLine = (uiLine + 1) % CHAT_MAX_LINES;
        uiLinesDrawn++;
        if (uiLine == m_uiMostRecentLine)            // Went through all lines?
            break;
    }
}

//
// CChat::DrawInputLine
//
void CChat::DrawInputLine(bool bUsingOutline)
{
    if (m_InputColor.A * m_fInputBackgroundAlpha > 0.f)
    {
        if (m_pInput)
        {
            // Hack to draw the input background behind the text.
            m_pInput->SetAlpha(m_fInputBackgroundAlpha);
            m_pInput->SetVisible(true);
            m_pInput->Render();
            m_pInput->SetVisible(false);
        }
    }

    if (m_bInputVisible)
    {
        float     fLineDifference = CChat::GetFontHeight(m_vecScale.fY);
        bool      bInputShadow = (m_InputColor.A * m_fInputBackgroundAlpha == 0.f) && !bUsingOutline;
        CVector2D vecPosition(m_vecInputPosition.fX + (5.0f * m_vecScale.fX), m_vecInputPosition.fY + (fLineDifference * 0.125f));
        m_InputLine.Draw(vecPosition, 255, bInputShadow, bUsingOutline);
    }
}

//
// CChat::UpdateSmoothScroll
//
void CChat::UpdateSmoothScroll(float* pfPixelScroll, int* piLineScroll)
{
    // Calc frame time
    float fTimeSeconds = GetSecondCount();
    float fDeltaSeconds = fTimeSeconds - m_fSmoothLastTimeSeconds;
    m_fSmoothLastTimeSeconds = fTimeSeconds;

    // Handle auto repeat
    if (m_iScrollState == 0)
    {
        m_fSmoothRepeatTimer = -100;
    }
    else
    {
        if (m_fSmoothRepeatTimer < 0.0f)
        {
            if (m_fSmoothRepeatTimer < -50)
                m_fSmoothRepeatTimer = 0.250f;
            else
                m_fSmoothRepeatTimer += 1 / 22.f;

            if (m_iScrollState == 1)
                ScrollUp();
            else if (m_iScrollState == -1)
                ScrollDown();
        }
        m_fSmoothRepeatTimer -= fDeltaSeconds;
    }

    // Time to reset?
    if (m_fSmoothScrollResetTime && m_fSmoothScrollResetTime < GetSecondCount())
    {
        m_fSmoothScrollResetTime = 0;
        m_fSmoothScroll -= m_uiScrollOffset;
        m_uiScrollOffset -= m_uiScrollOffset;
    }

    // Calc smooth scroll deceleration
    float fFixedChange = (m_fSmoothScroll < 0 ? -1 : 1) * -std::min(fDeltaSeconds / 0.2f, 1.0f);
    float fLerpChange = m_fSmoothScroll * (1 - std::min(fDeltaSeconds / 0.3f, 1.0f)) - m_fSmoothScroll;
    float fChange = fLerpChange * 0.7f + fFixedChange * 0.4f;
    fChange = Clamp(-fabsf(m_fSmoothScroll), fChange, fabsf(m_fSmoothScroll));

    // Apply smooth scroll deceleration
    m_fSmoothScroll += fChange;

    // Break into line and pixels
    int iLineScroll = (int)(m_fSmoothScroll);
    if (m_fSmoothScroll > 0.14)
        iLineScroll += 1;
    float fPixelScroll = m_fSmoothScroll - iLineScroll;

    // Inhibit pixel scroll if frame rate falls to low
    if (fDeltaSeconds > 0.1f)
        m_fSmoothAllowAfter = fTimeSeconds + 1.0f;

    if (m_fSmoothAllowAfter > fTimeSeconds)
        fPixelScroll = 0;

    // Set return values
    *pfPixelScroll = fPixelScroll;
    *piLineScroll = iLineScroll;

    // Update 'scrolling back' flag
    if (m_uiScrollOffset != 0)
        m_iScrollingBack = 10;
    else
        m_iScrollingBack = std::max(0, m_iScrollingBack - 1);

    //
    // Also update CssStyle override alpha
    //
    float fTarget = (!m_bCssStyleText || m_bInputVisible || m_iScrollingBack) ? 1.0f : 0.0f;
    float fMaxAmount = fDeltaSeconds * 2.0f;            // 0.5 seconds fade time
    m_fCssStyleOverrideAlpha += Clamp(-fMaxAmount, fTarget - m_fCssStyleOverrideAlpha, fMaxAmount);

    //
    // Also update background alpha
    //
    fTarget = (!m_bCssStyleBackground || m_bInputVisible || m_iScrollingBack) ? 1.0f : 0.0f;
    fMaxAmount = fDeltaSeconds * 5.0f;            // 0.2 seconds fade time
    m_fBackgroundAlpha += Clamp(-fMaxAmount, fTarget - m_fBackgroundAlpha, fMaxAmount);

    //
    // Also update input background alpha
    //
    fTarget = (m_bInputVisible) ? 1.0f : 0.0f;
    fMaxAmount = fDeltaSeconds * 5.0f;            // 0.2 seconds fade time
    m_fInputBackgroundAlpha += Clamp(-fMaxAmount, fTarget - m_fInputBackgroundAlpha, fMaxAmount);
}

void CChat::Output(const char* szText, bool bColorCoded)
{
    CChatLine*  pLine = NULL;
    const char* szRemainingText = szText;
    CColor      color = m_TextColor;

    // Allow smooth scroll when text is added if game FX Quality is not low
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    if (gameSettings->GetFXQuality() > 0)
        m_fSmoothScroll -= 1.0f;

    do
    {
        m_uiMostRecentLine = (m_uiMostRecentLine == 0 ? CHAT_MAX_LINES - 1 : m_uiMostRecentLine - 1);
        pLine = &m_Lines[m_uiMostRecentLine];
        szRemainingText = pLine->Format(szRemainingText, (m_vecBackgroundSize.fX - (10.0f * m_vecScale.fX)), color, bColorCoded);
        pLine->SetActive(true);
        pLine->UpdateCreationTime();
    } while (szRemainingText);
}

void CChat::Clear()
{
    for (int i = 0; i < CHAT_MAX_LINES; i++)
    {
        m_Lines[i].SetActive(false);
    }
    m_uiMostRecentLine = 0;
    m_fSmoothScroll = 0;
    m_iCacheTextureRevision = -1;
}

void CChat::ClearInput()
{
    m_strInputText.clear();
    m_InputLine.Clear();
    m_vecInputSize = CalcInputSize();

    if (m_pInput)
    {
        m_pInput->SetSize(m_vecInputSize);
        UpdatePosition();
    }
}

void CChat::ScrollUp()
{
    if (m_Lines[(m_uiMostRecentLine + m_uiScrollOffset + m_uiNumLines) % CHAT_MAX_LINES].IsActive() &&
        !(((m_uiMostRecentLine + m_uiScrollOffset) % CHAT_MAX_LINES < m_uiMostRecentLine) &&
          ((m_uiMostRecentLine + m_uiScrollOffset + m_uiNumLines) % CHAT_MAX_LINES >= m_uiMostRecentLine)))
    {
        m_uiScrollOffset += 1;
        m_fSmoothScroll += 1;
        m_fSmoothScrollResetTime = GetSecondCount() + 5;
    }
}

void CChat::ScrollDown()
{
    if (m_uiScrollOffset <= 0)
    {
        m_fSmoothScroll -= m_uiScrollOffset;
        m_uiScrollOffset -= m_uiScrollOffset;
    }
    else
    {
        m_uiScrollOffset -= 1;
        m_fSmoothScroll -= 1;
        m_fSmoothScrollResetTime = GetSecondCount() + 5;
    }
}

void CChat::ResetHistoryChanges()
{
    // Reset history selection, any history changes and our saved input
    m_iSelectedInputHistoryEntry = -1;
    m_pInputHistory->ResetChanges();
    m_strSavedInputText.clear();
}

void CChat::SelectInputHistoryEntry(int iEntry)
{
    int iPreviouslySelectedInputHistoryEntry = m_iSelectedInputHistoryEntry;

    // Check if we're in bounds, otherwise clear selection
    if (!m_pInputHistory->Empty() && iEntry >= 0 && iEntry < m_pInputHistory->Size())
        m_iSelectedInputHistoryEntry = iEntry;
    else
        m_iSelectedInputHistoryEntry = -1;

    // Save current input as a temporary input value
    if (iPreviouslySelectedInputHistoryEntry == -1)
        m_strSavedInputText = m_strInputText;
    else
        m_pInputHistory->Get(iPreviouslySelectedInputHistoryEntry)->temp = m_strInputText;

    // Clear input
    ClearInput();

    // If we haven't selected any history entry, use our saved input text
    if (m_iSelectedInputHistoryEntry == -1)
        SetInputText(m_strSavedInputText.c_str());
    else
    {
        SString& strSelectedInputHistoryEntry = m_pInputHistory->Get(m_iSelectedInputHistoryEntry)->temp;
        // If the selected entry isn't empty, fill it in
        if (!strSelectedInputHistoryEntry.empty())
            SetInputText(strSelectedInputHistoryEntry.c_str());
    }
}

bool CChat::SetNextHistoryText()
{
    // If we can't take input or we're at the end of the list, stop here
    if (!CanTakeInput() || m_iSelectedInputHistoryEntry >= m_pInputHistory->Size() - 1)
        return false;

    // Select the previous entry
    SelectInputHistoryEntry(m_iSelectedInputHistoryEntry + 1);

    return true;
}

bool CChat::SetPreviousHistoryText()
{
    // If we can't take input, stop here
    if (!CanTakeInput())
        return false;

    // Select the next entry, or the default entry
    if (m_pInputHistory->Size() > 0 && m_iSelectedInputHistoryEntry > 0)
        SelectInputHistoryEntry(m_iSelectedInputHistoryEntry - 1);
    else
        SelectInputHistoryEntry(-1);

    return true;
}

bool CChat::CharacterKeyHandler(CGUIKeyEventArgs KeyboardArgs)
{
    if (!CanTakeInput())
        return false;

    // Check if it's a special key like enter and backspace, if not, add it as a character to the message
    switch (KeyboardArgs.codepoint)
    {
        case VK_BACK:
        {
            if (m_strInputText.size() > 0)
            {
                // Convert our string to UTF8 before resizing, then back to ANSI.
                std::wstring strText = MbUTF8ToUTF16(m_strInputText);
                strText.resize(strText.size() - 1);
                SetInputText(UTF16ToMbUTF8(strText).c_str());
            }
            break;
        }

        case VK_RETURN:
        {
            // Empty the chat and hide the input stuff
            // If theres a command to call, call it
            if (!m_strCommand.empty() && !m_strInputText.empty())
                CCommands::GetSingleton().Execute(m_strCommand.c_str(), m_strInputText.c_str());

            // If the input isn't empty and isn't identical to the previous entry in history, add it to the history
            if (!m_strInputText.empty() && (m_pInputHistory->Empty() || m_pInputHistory->GetLast() != m_strInputText))
                m_pInputHistory->Add(m_strInputText);

            SetInputVisible(false);

            m_fSmoothScrollResetTime = GetSecondCount();

            ResetHistoryChanges();
            break;
        }

        case VK_TAB:
        {
            if (m_bNickCompletion && m_strInputText.size() > 0)
            {
                bool bSuccess = false;

                SString strCurrentInput = GetInputText();
                size_t  iFound;
                iFound = strCurrentInput.find_last_of(" ");
                if (iFound == std::string::npos)
                    iFound = 0;
                else
                    ++iFound;

                SString strPlayerNamePart = strCurrentInput.substr(iFound);

                CModManager* pModManager = CModManager::GetSingletonPtr();
                if (pModManager && pModManager->GetCurrentMod())
                {
                    // Create vector and get playernames from deathmatch module
                    std::vector<SString> vPlayerNames;
                    pModManager->GetCurrentMod()->GetPlayerNames(vPlayerNames);

                    for (std::vector<SString>::iterator iter = vPlayerNames.begin(); iter != vPlayerNames.end(); ++iter)
                    {
                        SString strPlayerName = *iter;

                        // Check if there is another player after our last result
                        if (m_strLastPlayerName.size() != 0)
                        {
                            if (strPlayerName.CompareI(m_strLastPlayerName))
                            {
                                m_strLastPlayerName.clear();
                                if (*iter == vPlayerNames.back())
                                {
                                    CharacterKeyHandler(KeyboardArgs);
                                    return true;
                                }
                            }
                            continue;
                        }

                        // Already a part?
                        if (m_strLastPlayerNamePart.size() != 0)
                        {
                            strPlayerNamePart = m_strLastPlayerNamePart;
                        }

                        // Check namepart
                        if (!RemoveColorCodes(strPlayerName).BeginsWith(strPlayerNamePart))
                            continue;
                        else
                        {
                            // Check size if it's ok, then output
                            SString strOutput = strCurrentInput.replace(iFound, std::string::npos, strPlayerName);
                            if (MbUTF8ToUTF16(strOutput).size() < m_iCharacterLimit)
                            {
                                bSuccess = true;
                                m_strLastPlayerNamePart = strPlayerNamePart;
                                m_strLastPlayerName = strPlayerName;
                                SetInputText(strOutput);
                            }

                            break;
                        }
                    }

                    // No success? Try again!
                    if (!bSuccess)
                        m_strLastPlayerName.clear();
                }
            }
            break;
        }

        default:
        {
            // Clear last namepart when pressing letter
            if (m_strLastPlayerNamePart.size() != 0)
                m_strLastPlayerNamePart.clear();

            // Clear last name when pressing letter
            if (m_strLastPlayerName.size() != 0)
                m_strLastPlayerName.clear();

            if (KeyboardArgs.codepoint == 127)            // "delete" char, used to remove the previous word from input
            {
                if (m_strInputText.size() > 0)
                {
                    // Convert our string to UTF8 before resizing, then back to ANSI.
                    std::wstring      wstrText = MbUTF8ToUTF16(m_strInputText);
                    std::wstring_view wstrTextView = wstrText;

                    if (wstrTextView.back() == L' ' || wstrTextView.back() == L'-')
                    {
                        size_t lastPos = wstrTextView.find_last_not_of(wstrTextView.back());
                        if (lastPos != std::string::npos)
                            wstrTextView.remove_suffix(wstrTextView.size() - lastPos);
                        else
                            wstrText.clear();
                    }

                    size_t lastSpacePos = wstrTextView.find_last_of(L' ');
                    size_t lastDashPos = wstrTextView.find_last_of(L'-');
                    size_t lastPos = lastSpacePos;

                    if ((lastSpacePos == std::string::npos || lastDashPos > lastSpacePos) && lastDashPos != std::string::npos)
                        lastPos = lastDashPos;

                    if (lastPos != std::string::npos)
                        wstrText.resize(lastPos + 1);
                    else
                        wstrText.clear();

                    SetInputText(UTF16ToMbUTF8(wstrText).c_str());
                }
                break;
            }

            // If we haven't exceeded the maximum number of characters per chat message, append the char to the message and update the input control
            if (MbUTF8ToUTF16(m_strInputText).size() < m_iCharacterLimit)
            {
                if (KeyboardArgs.codepoint >= 32)
                {
                    unsigned int uiCharacter = KeyboardArgs.codepoint;
                    if (uiCharacter < 127)            // we have any char from ASCII
                    {
                        // injecting as is
                        m_strInputText += static_cast<char>(KeyboardArgs.codepoint);
                        SetInputText(m_strInputText.c_str());
                    }
                    else            // we have any char from Extended ASCII, any ANSI code page or UNICODE range
                    {
                        // Generate a null-terminating string for our character
                        wchar_t wUNICODE[2] = {static_cast<wchar_t>(uiCharacter), '\0'};

                        // Convert our UTF character into an ANSI string
                        std::string strANSI = UTF16ToMbUTF8(wUNICODE);

                        // Append the ANSI string, and update
                        m_strInputText.append(strANSI);
                        SetInputText(m_strInputText.c_str());
                    }
                }
            }
            break;
        }
    }
    return true;
}

void CChat::SetVisible(bool bVisible, bool bInputBlocked)
{
    m_bVisible = bVisible;
    m_bInputBlocked = bInputBlocked;

    if (m_bInputBlocked)
        SetInputVisible(false);
}

void CChat::SetInputVisible(bool bVisible)
{
    if (m_bInputBlocked)
        bVisible = false;

    if (!bVisible)
    {
        ClearInput();
        ResetHistoryChanges();
    }

    m_bInputVisible = bVisible;
}

void CChat::SetNumLines(unsigned int uiNumLines)
{
    if (uiNumLines <= CHAT_MAX_LINES)
    {
        m_uiNumLines = uiNumLines;
        UpdateGUI();
    }
}

void CChat::SetChatFont(eChatFont Font)
{
    CGUIFont*  pFont = g_pCore->GetGUI()->GetDefaultFont();
    ID3DXFont* pDXFont = g_pCore->GetGraphics()->GetFont();
    float      fUsingDxFontScale = 1;
    float      fReqestedDxFontScale = std::max(m_vecScale.fX, m_vecScale.fY);
    switch (Font)
    {
        case Chat::Font::DEFAULT:
            pFont = g_pCore->GetGUI()->GetDefaultFont();
            pDXFont = g_pCore->GetGraphics()->GetFont(FONT_DEFAULT, &fUsingDxFontScale, fReqestedDxFontScale, "chat");
            break;
        case Chat::Font::CLEAR:
            pFont = g_pCore->GetGUI()->GetClearFont();
            pDXFont = g_pCore->GetGraphics()->GetFont(FONT_CLEAR, &fUsingDxFontScale, fReqestedDxFontScale, "chat");
            break;
        case Chat::Font::BOLD:
            pFont = g_pCore->GetGUI()->GetBoldFont();
            pDXFont = g_pCore->GetGraphics()->GetFont(FONT_DEFAULT_BOLD, &fUsingDxFontScale, fReqestedDxFontScale, "chat");
            break;
        case Chat::Font::ARIAL:
            pDXFont = g_pCore->GetGraphics()->GetFont(FONT_ARIAL, &fUsingDxFontScale, fReqestedDxFontScale, "chat");
            break;
    }

    m_fRcpUsingDxFontScale = 1 / fUsingDxFontScale;
    SAFE_RELEASE(m_pCacheTexture);

    // Set fonts
    m_pFont = pFont;
    SetDxFont(pDXFont);
}

void CChat::SetDxFont(LPD3DXFONT pDXFont)
{
    if (m_pDXFont != pDXFont)
    {
        if (m_pDXFont)
            m_pDXFont->Release();

        m_pDXFont = pDXFont;

        if (m_pDXFont)
            m_pDXFont->AddRef();
    }
}

void CChat::UpdateGUI()
{
    m_vecBackgroundSize = CVector2D(m_fNativeWidth * m_vecScale.fX, CChat::GetFontHeight(m_vecScale.fY) * (float(m_uiNumLines) + 0.5f));
    m_vecBackgroundSize.fX = Round(m_vecBackgroundSize.fX);
    m_vecBackgroundSize.fY = Round(m_vecBackgroundSize.fY);
    m_vecBackgroundPosition.fX = Round(m_vecBackgroundPosition.fX);
    m_vecBackgroundPosition.fY = Round(m_vecBackgroundPosition.fY);
    m_pBackground->SetSize(m_vecBackgroundSize);

    // Make sure there is enough room for all the lines
    uint uiMaxNumLines = g_pCore->GetGraphics()->GetViewportHeight() / std::max(1.f, CChat::GetFontHeight(m_vecScale.fY)) - m_iMaxInputLines;
    if (m_uiNumLines > uiMaxNumLines)
        SetNumLines(uiMaxNumLines);

    m_vecInputSize = CalcInputSize();
    if (m_pInput)
    {
        m_pInput->SetSize(m_vecInputSize);
    }

    UpdatePosition();
}

void CChat::UpdatePosition()
{
    CVector2D vecResolution = m_pManager->GetResolution();

    float fRelativeWidth = (m_vecBackgroundSize.fX / vecResolution.fX), fRelativeHeight = ((m_vecBackgroundSize.fY + m_vecInputSize.fY) / vecResolution.fY),
          fPosX, fPosY;

    switch (m_ePositionHorizontal)
    {
        case Chat::Position::Horizontal::RIGHT:
            fPosX = 1.0 - fRelativeWidth + m_fPositionOffsetX;
            break;
        case Chat::Position::Horizontal::CENTER:
            fPosX = (1.0 - fRelativeWidth) / 2 + m_fPositionOffsetX;
            break;
        case Chat::Position::Horizontal::LEFT:
        default:
            fPosX = m_fPositionOffsetX;
            break;
    }

    switch (m_ePositionVertical)
    {
        case Chat::Position::Vertical::BOTTOM:
            fPosY = 1.0 - fRelativeHeight + m_fPositionOffsetY;
            break;
        case Chat::Position::Vertical::CENTER:
            fPosY = (1.0 - fRelativeHeight) / 2 + m_fPositionOffsetY;
            break;
        case Chat::Position::Vertical::TOP:
        default:
            fPosY = m_fPositionOffsetY;
            break;
    }

    m_vecBackgroundPosition = CVector2D(std::floor(fPosX * vecResolution.fX), std::floor(fPosY * vecResolution.fY));
    m_vecInputPosition = CVector2D(m_vecBackgroundPosition.fX, m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY);

    m_pBackground->SetPosition(m_vecBackgroundPosition);

    if (m_pInput)
    {
        m_pInput->SetPosition(m_vecInputPosition);
    }
}

void CChat::SetColor(const CColor& Color)
{
    unsigned long ulBackgroundColor = COLOR_ARGB(Color.A, Color.R, Color.G, Color.B);

    m_pBackgroundTexture->LoadFromMemory(&ulBackgroundColor, 1, 1);
    m_pBackground->LoadFromTexture(m_pBackgroundTexture);
}

void CChat::SetInputColor(const CColor& Color)
{
    unsigned long ulInputColor = COLOR_ARGB(Color.A, Color.R, Color.G, Color.B);

    if (m_pInputTexture)
        m_pInputTexture->LoadFromMemory(&ulInputColor, 1, 1);

    if (m_pInput)
        m_pInput->LoadFromTexture(m_pInputTexture);
}

const char* CChat::GetInputPrefix()
{
    return m_InputLine.m_Prefix.GetText();
}

void CChat::SetInputPrefix(const char* szPrefix)
{
    m_InputLine.m_Prefix.SetText(szPrefix);
}

CVector2D CChat::CalcInputSize()
{
    return CVector2D(m_vecBackgroundSize.fX, (GetFontHeight(m_vecScale.fY) * ((float)m_InputLine.m_ExtraLines.size() + 1.25f)));
}

void CChat::SetInputText(const char* szText)
{
    m_InputLine.Clear();

    CColor      color = m_InputTextColor;
    const char* szRemainingText = m_InputLine.Format(szText, (m_vecInputSize.fX - (10.0f * m_vecScale.fX) - m_InputLine.m_Prefix.GetWidth()), color, false);

    CChatLine* pLine = NULL;

    while (szRemainingText && m_InputLine.m_ExtraLines.size() < m_iMaxInputLines)
    {
        m_InputLine.m_ExtraLines.resize(m_InputLine.m_ExtraLines.size() + 1);
        CChatLine& line = *(m_InputLine.m_ExtraLines.end() - 1);
        szRemainingText = line.Format(szRemainingText, (m_vecInputSize.fX - (10.0f * m_vecScale.fX)), color, false);
    }

    if (szText != m_strInputText.c_str())
        m_strInputText = szText;

    if (szRemainingText)
        m_strInputText.resize(szRemainingText - szText);

    m_vecInputSize = CalcInputSize();

    if (m_pInput)
    {
        m_pInput->SetSize(m_vecInputSize);
        UpdatePosition();
    }
}

void CChat::SetCommand(const char* szCommand)
{
    if (!szCommand)
        return;

    m_strCommand = szCommand;

    if (strcmp(szCommand, "chatboxsay") == 0)
    {
        SetInputPrefix("Say: ");
    }
    else
    {
        std::string strPrefix = m_strCommand + ": ";
        strPrefix[0] = toupper(strPrefix[0]);
        SetInputPrefix(strPrefix.c_str());
    }
}

float CChat::GetFontHeight(float fScale)
{
    if (!g_pChat)
        return 0.0f;

    if (g_pChat->m_bUseCEGUI)
    {
        return g_pChat->m_pFont->GetFontHeight(fScale);
    }

    fScale *= g_pChat->m_fRcpUsingDxFontScale;

    return g_pCore->GetGraphics()->GetDXFontHeight(fScale, g_pChat->m_pDXFont);
}

float CChat::GetTextExtent(const char* szText, float fScale)
{
    if (!g_pChat)
        return 0.0f;

    if (g_pChat->m_bUseCEGUI)
    {
        return g_pChat->m_pFont->GetTextExtent(szText, fScale);
    }

    fScale *= g_pChat->m_fRcpUsingDxFontScale;

    return g_pCore->GetGraphics()->GetDXTextExtent(szText, fScale, g_pChat->m_pDXFont);
}

void CChat::DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX,
                           float fScaleY, bool bOutline, const CRect2D& RenderBounds)
{
    if (!g_pChat)
        return;

    if (g_pChat->m_bUseCEGUI)
    {
        g_pChat->m_pFont->DrawTextString(szText, DrawArea, fZ, ClipRect, ulFormat, ulColor, fScaleX, fScaleY);
    }
    else
    {
        float fLineHeight = CChat::GetFontHeight(g_pChat->m_vecScale.fY);
        fScaleX *= g_pChat->m_fRcpUsingDxFontScale;
        fScaleY *= g_pChat->m_fRcpUsingDxFontScale;

        if (DrawArea.fY1 <= RenderBounds.fY1)
        {
            // Clip text at the top
            if (DrawArea.fY1 + fLineHeight - RenderBounds.fY1 > 1)
                g_pCore->GetGraphics()->DrawString((int)DrawArea.fX1, (int)RenderBounds.fY1, (int)DrawArea.fX2, (int)DrawArea.fY1 + fLineHeight, ulColor,
                                                   szText, fScaleX, fScaleY, DT_LEFT | DT_BOTTOM | DT_SINGLELINE, g_pChat->m_pDXFont, bOutline);
        }
        else if (DrawArea.fY1 + fLineHeight >= RenderBounds.fY2)
        {
            // Clip text at the bottom
            if (RenderBounds.fY2 - DrawArea.fY1 > 1)
                g_pCore->GetGraphics()->DrawString((int)DrawArea.fX1, (int)DrawArea.fY1, (int)DrawArea.fX2, (int)RenderBounds.fY2, ulColor, szText, fScaleX,
                                                   fScaleY, DT_LEFT | DT_TOP | DT_SINGLELINE, g_pChat->m_pDXFont, bOutline);
        }
        else
        {
            // Text not clipped
            g_pCore->GetGraphics()->DrawString((int)DrawArea.fX1, (int)DrawArea.fY1, (int)DrawArea.fX1, (int)DrawArea.fY1, ulColor, szText, fScaleX, fScaleY,
                                               DT_LEFT | DT_TOP | DT_NOCLIP, g_pChat->m_pDXFont, bOutline);
        }
    }
}

void CChat::SetCharacterLimit(int charLimit)
{
    m_iCharacterLimit = charLimit;
}

CChatLine::CChatLine()
{
    m_bActive = false;
    UpdateCreationTime();
}

void CChatLine::UpdateCreationTime()
{
    m_ulCreationTime = GetTickCount32();
}

//
// Calculate the equivalent ansi string pointer of szPosition.
//
const char* CalcAnsiPtr(const char* szStringAnsi, const wchar_t* szPosition)
{
    int iOrigSize = strlen(szStringAnsi);
    int iEndSize = UTF16ToMbUTF8(szPosition).length();
    int iOffset = iOrigSize - iEndSize;
    return szStringAnsi + iOffset;
}

const char* CChatLine::Format(const char* szStringAnsi, float fWidth, CColor& color, bool bColorCoded)
{
    std::wstring   wString = MbUTF8ToUTF16(szStringAnsi);
    const wchar_t* szString = wString.c_str();

    float fPrevSectionsWidth = 0.0f;
    m_Sections.clear();

    const wchar_t* szSectionStart = szString;
    const wchar_t* szSectionEnd = szString;
    const wchar_t* szLastWrapPoint = szString;
    bool           bLastSection = false;
    while (!bLastSection)            // iterate over sections
    {
        m_Sections.resize(m_Sections.size() + 1);
        CChatLineSection& section = *(m_Sections.end() - 1);
        section.SetColor(color);

        if (m_Sections.size() > 1 && bColorCoded)            // If we've processed sections before
            szSectionEnd += 7;                               // skip the color code

        szSectionStart = szSectionEnd;
        szLastWrapPoint = szSectionStart;
        unsigned int uiSeekPos = 0;
        std::wstring strSectionStart = szSectionStart;

        while (true)            // find end of this section
        {
            float fSectionWidth = CChat::GetTextExtent(UTF16ToMbUTF8(strSectionStart.substr(0, uiSeekPos)).c_str(), g_pChat->m_vecScale.fX);

            if (*szSectionEnd == '\0' || *szSectionEnd == '\n' || std::ceil(fPrevSectionsWidth + fSectionWidth) > fWidth)
            {
                bLastSection = true;
                break;
            }
            if (bColorCoded && IsColorCode(UTF16ToMbUTF8(szSectionEnd).c_str()))
            {
                unsigned long ulColor = 0;
                sscanf(UTF16ToMbUTF8(szSectionEnd).c_str() + 1, "%06x", &ulColor);
                color = ulColor;
                fPrevSectionsWidth += fSectionWidth;
                break;
            }
            if (isspace((unsigned char)*szSectionEnd) || ispunct((unsigned char)*szSectionEnd))
            {
                szLastWrapPoint = szSectionEnd;
            }
            szSectionEnd++;
            uiSeekPos++;
        }
        section.m_strText = UTF16ToMbUTF8(strSectionStart.substr(0, uiSeekPos));
    }

    if (*szSectionEnd == '\0')
    {
        return NULL;
    }
    else if (*szSectionEnd == '\n')
    {
        return CalcAnsiPtr(szStringAnsi, szSectionEnd + 1);
    }
    else
    {
        // Do word wrap
        if (szLastWrapPoint == szSectionStart)
        {
            // Wrapping point coincides with the start of a section.
            if (szLastWrapPoint == szString)
            {
                // The line consists of one huge word. Leave the one section we created as it
                // is (with the huge word cut off) and return szRemaining as the rest of the word
                return CalcAnsiPtr(szStringAnsi, szSectionEnd);
            }
            else
            {
                // There's more than one section, remove the last one (where our wrap point is)
                m_Sections.pop_back();
            }
        }
        else
        {
            // Wrapping point is in the middle of a section, truncate
            CChatLineSection& last = *(m_Sections.end() - 1);
            std::wstring      wstrTemp = MbUTF8ToUTF16(last.m_strText);
            wstrTemp.resize(szLastWrapPoint - szSectionStart);
            last.m_strText = UTF16ToMbUTF8(wstrTemp);
        }
        return CalcAnsiPtr(szStringAnsi, szLastWrapPoint);
    }
}

void CChatLine::Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds)
{
    float                                   fCurrentX = vecPosition.fX;
    std::vector<CChatLineSection>::iterator iter = m_Sections.begin();
    for (; iter != m_Sections.end(); iter++)
    {
        (*iter).Draw(CVector2D(fCurrentX, vecPosition.fY), ucAlpha, bShadow, bOutline, RenderBounds);
        fCurrentX += (*iter).GetWidth();
    }
}

float CChatLine::GetWidth()
{
    float                                   fWidth = 0.0f;
    std::vector<CChatLineSection>::iterator it;
    for (it = m_Sections.begin(); it != m_Sections.end(); it++)
    {
        fWidth += (*it).GetWidth();
    }
    return fWidth;
}

void CChatInputLine::Draw(CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline)
{
    CRect2D RenderBounds(0, 0, 9999, 9999);

    CColor colPrefix;
    m_Prefix.GetColor(colPrefix);
    if (colPrefix.A > 0)
        m_Prefix.Draw(vecPosition, colPrefix.A, bShadow, bOutline, RenderBounds);

    if (g_pChat->m_InputTextColor.A > 0 && m_Sections.size() > 0)
    {
        m_Sections[0].Draw(CVector2D(vecPosition.fX + m_Prefix.GetWidth(), vecPosition.fY), g_pChat->m_InputTextColor.A, bShadow, bOutline, RenderBounds);

        float fLineDifference = CChat::GetFontHeight(g_pChat->m_vecScale.fY);

        vector<CChatLine>::iterator iter = m_ExtraLines.begin();
        for (; iter != m_ExtraLines.end(); iter++)
        {
            vecPosition.fY += fLineDifference;
            (*iter).Draw(vecPosition, g_pChat->m_InputTextColor.A, bShadow, bOutline, RenderBounds);
        }
    }
}

void CChatInputLine::Clear()
{
    m_Sections.clear();
    m_ExtraLines.clear();
}

CChatLineSection::CChatLineSection()
{
    m_fCachedWidth = -1.0f;
    m_uiCachedLength = 0;
}

CChatLineSection::CChatLineSection(const CChatLineSection& other)
{
    *this = other;
}

CChatLineSection& CChatLineSection::operator=(const CChatLineSection& other)
{
    m_strText = other.m_strText;
    m_Color = other.m_Color;
    m_fCachedWidth = other.m_fCachedWidth;
    m_uiCachedLength = other.m_uiCachedLength;
    return *this;
}

void CChatLineSection::Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds)
{
    if (!m_strText.empty() && ucAlpha > 0)
    {
        if (bShadow)
        {
            CRect2D drawShadowAt(vecPosition.fX + 1.0f, vecPosition.fY + 1.0f, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f);
            CChat::DrawTextString(m_strText.c_str(), drawShadowAt, 0.0f, drawShadowAt, 0, COLOR_ARGB(ucAlpha, 0, 0, 0), g_pChat->m_vecScale.fX,
                                  g_pChat->m_vecScale.fY, bOutline, RenderBounds);
        }
        CRect2D drawAt(vecPosition.fX, vecPosition.fY, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f);
        CChat::DrawTextString(m_strText.c_str(), drawAt, 0.0f, drawAt, 0, COLOR_ARGB(ucAlpha, m_Color.R, m_Color.G, m_Color.B), g_pChat->m_vecScale.fX,
                              g_pChat->m_vecScale.fY, bOutline, RenderBounds);
    }
}

float CChatLineSection::GetWidth()
{
    if (m_fCachedWidth < 0.0f || m_strText.size() != m_uiCachedLength)
    {
        m_fCachedWidth = std::ceil(CChat::GetTextExtent(m_strText.c_str(), g_pChat->m_vecScale.fX) / std::max(0.01f, g_pChat->m_vecScale.fX));
        m_uiCachedLength = m_strText.size();
    }
    return m_fCachedWidth * g_pChat->m_vecScale.fX;
}
