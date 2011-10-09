/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CChat.cpp
*  PURPOSE:     In-game chat box user interface implementation
*  DEVELOPERS:  Jax <>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

using std::vector;

extern CCore* g_pCore;

CChat * g_pChat = NULL;

CChat::CChat ( CGUI* pManager, const CVector2D & vecPosition )
{    
    g_pChat = this;
    m_pManager = pManager;  

    // Calculate relative position (assuming a 800x600 native resolution for our defined CCHAT_* values)
    CVector2D vecResolution = m_pManager->GetResolution ();
    m_vecScale = CVector2D ( vecResolution.fX / 800.0f, vecResolution.fY / 600.0f );
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
    m_bInputVisible = false;
    m_pFont = m_pManager->GetClearFont ();
    m_pDXFont = g_pCore->GetGraphics ()->GetFont ();
    m_fNativeWidth = CHAT_WIDTH;
    m_bCanChangeWidth = true;
    m_iScrollingBack = 0;
    m_fCssStyleOverrideAlpha = 0.0f;
    m_fBackgroundAlpha = 0.0f;
    m_fInputBackgroundAlpha = 0.f;
    m_pCacheTexture = NULL;
    m_iCacheTextureRevision = 0;

    // Background area
    m_pBackground = m_pManager->CreateStaticImage ();
    m_pBackgroundTexture = m_pManager->CreateTexture ();
    m_pBackground->LoadFromTexture ( m_pBackgroundTexture );
    m_pBackground->MoveToBack ();
    m_pBackground->SetPosition ( m_vecBackgroundPosition );
    m_pBackground->SetSize ( m_vecBackgroundSize );
    m_pBackground->SetEnabled ( false );
    m_pBackground->SetVisible ( false );

    // Input area
    m_pInput = m_pManager->CreateStaticImage ();
    m_pInputTexture = m_pManager->CreateTexture ();
    m_pInput->LoadFromTexture ( m_pInputTexture );
    m_pInput->MoveToBack ();
    m_pInput->SetPosition ( m_vecInputPosition );
    m_pInput->SetSize ( m_vecInputSize );
    m_pInput->SetEnabled ( false );
    m_pInput->SetVisible ( false );
    SetInputPrefix ( "Say: " );

    // Load cvars and position the GUI
    LoadCVars ();
    UpdateGUI ();
}


CChat::~CChat ( void )
{
    Clear ();
    ClearInput ();

    SAFE_DELETE ( m_pBackground );
    SAFE_DELETE ( m_pBackgroundTexture );
    SAFE_DELETE ( m_pInput );
    SAFE_DELETE ( m_pInputTexture );

    if ( g_pChat == this )
        g_pChat = NULL;
}

void CChat::OnModLoad ( void )
{
    // Set handlers
    m_pManager->SetCharacterKeyHandler ( INPUT_MOD, GUI_CALLBACK_KEY ( &CChat::CharacterKeyHandler, this ) );
}

void CChat::LoadCVars ( void )
{
    unsigned int Font;
    float fWidth = 1;

    CVARS_GET ( "chat_color",                   m_Color );              if( m_bCanChangeWidth ) SetColor ( m_Color );
    CVARS_GET ( "chat_input_color",             m_InputColor );         SetInputColor ( m_InputColor );
    CVARS_GET ( "chat_input_text_color",        m_InputTextColor );
    CVARS_GET ( "chat_use_cegui",               m_bUseCEGUI );
    CVARS_GET ( "chat_lines",                   m_uiNumLines );         SetNumLines ( m_uiNumLines);
    CVARS_GET ( "chat_text_color",              m_TextColor );
    CVARS_GET ( "chat_scale",                   m_vecScale );
    CVARS_GET ( "chat_width",                   fWidth );               if( m_bCanChangeWidth ) m_fNativeWidth = fWidth * CHAT_WIDTH;
    CVARS_GET ( "chat_css_style_text",          m_bCssStyleText );
    CVARS_GET ( "chat_css_style_background",    m_bCssStyleBackground );
    CVARS_GET ( "chat_line_life",               (unsigned int &)m_ulChatLineLife );
    CVARS_GET ( "chat_line_fade_out",           (unsigned int &)m_ulChatLineFadeOut );
    CVARS_GET ( "chat_font",                    (unsigned int &)Font ); SetChatFont ( (eChatFont)Font );

    // Modify default chat box to be like 'Transparent' preset
    SString strFlags;
    CVARS_GET ( "fixup_flags", strFlags );
    if ( strFlags.find ( "-cb" ) == std::string::npos )
    {
        CVARS_SET ( "fixup_flags", strFlags + "-cb" );

        if ( Font                       == 0                            // chat_font
             && m_uiNumLines            == 7                            // chat_lines
             && m_Color                 == CColor (0,0,128,100)         // chat_color
             && m_InputColor            == CColor (0,0,191,110)         // chat_input_color
             && m_InputTextColor        == CColor (172,213,254,255)     // chat_input_text_color
             && m_vecScale              == CVector2D ( 1.0f, 1.0f )     // chat_scale
             && fWidth                  == 1.0f                         // chat_width
             && m_bCssStyleText         == false                        // chat_css_style_text
             && m_bCssStyleBackground   == false                        // chat_css_style_background
             && m_ulChatLineLife        == 12000                        // chat_line_life
             && m_ulChatLineFadeOut     == 3000                         // chat_line_fade_out
            )
        {
            CVARS_SET ( "chat_font",                    (unsigned int)2 );
            CVARS_SET ( "chat_color",                   CColor (0,0,0,0) );
            CVARS_SET ( "chat_input_color",             CColor (0,0,0,0) );
            CVARS_SET ( "chat_lines",                   10 );
            CVARS_SET ( "chat_width",                   1.5f );
            LoadCVars ();
        }
    }

    // Fix mapalpha being set incorrectly in 1.0.4-9.1752.1
    CVARS_GET ( "fixup_flags", strFlags );
    if ( strFlags.find ( "-ma" ) == std::string::npos )
    {
        CVARS_SET ( "fixup_flags", strFlags + "-ma" );

        int iVar = -1;
        CVARS_GET ( "mapalpha", iVar );
        if ( iVar < 3 )
        {
            iVar = 155;
            CVARS_SET ( "mapalpha", iVar );
        }
    }
}


//
// Draw
//
void CChat::Draw ( bool bUseCacheTexture )
{
    // Are we visible?
    if ( !m_bVisible )
        return;

    // Is it time to update all the chat related cvars?
    if( m_iCVarsRevision != CClientVariables::GetSingleton ().GetRevision () ) {
        m_iCVarsRevision = CClientVariables::GetSingleton ().GetRevision ();
        LoadCVars ();
        UpdateGUI ();
    }

    // Get drawList for the chat box text
    SDrawList drawList;
    GetDrawList ( drawList );

    // Calc some size info
    CVector2D chatTopLeft ( drawList.renderBounds.fX1, drawList.renderBounds.fY1 );
    CVector2D chatBotRight ( drawList.renderBounds.fX2, drawList.renderBounds.fY2 );
    CVector2D chatSize = chatBotRight - chatTopLeft;
    CRect2D chatBounds ( 0, 0, chatSize.fX, chatSize.fY );

    // If we are not using a cache texture, just render the text directly to the screen
    if ( !bUseCacheTexture )
    {
        DrawDrawList ( drawList, chatTopLeft );
        return;
    }

    CGraphics* pGraphics = CGraphics::GetSingletonPtr ();

    // Validate rendertarget
    if ( m_pCacheTexture && ( chatSize.fX != m_pCacheTexture->m_uiSizeX || chatSize.fY != m_pCacheTexture->m_uiSizeY ) )
    {
        m_pCacheTexture->ReleaseRenderItem();
        m_pCacheTexture = NULL;
    }
    // Create rendertarget if required
    if ( !m_pCacheTexture )
    {
        m_pCacheTexture = pGraphics->GetRenderItemManager ()->CreateRenderTarget ( chatSize.fX, chatSize.fY, true, true );
        m_iCacheTextureRevision = -1;   // Make sure the graphics will be updated
    }

    // If we can't get a rendertarget for some reason, just render the text directly to the screen
    if ( !m_pCacheTexture )
    {
        DrawDrawList ( drawList, chatTopLeft );
        return;
    }

    // Update the cached graphics if the draw list has changed, or the render target has been recreated
    if ( m_PrevDrawList != drawList || m_pCacheTexture->GetRevision () != m_iCacheTextureRevision )
    {
        m_PrevDrawList = drawList;
        m_iCacheTextureRevision = m_pCacheTexture->GetRevision ();

        pGraphics->EnableSetRenderTarget ( true );
        pGraphics->GetRenderItemManager ()->SetRenderTarget ( m_pCacheTexture, true );

        // Draw new stuff
        DrawDrawList ( drawList );

        pGraphics->GetRenderItemManager ()->RestoreDefaultRenderTarget ();
        pGraphics->EnableSetRenderTarget ( false );
    }

    // Draw the cache texture
    pGraphics->DrawTexture ( m_pCacheTexture, chatTopLeft.fX, chatTopLeft.fY );
}


//
// DrawDrawList
//
// Draw the contents of the drawList
//
void CChat::DrawDrawList ( const SDrawList& drawList, const CVector2D& topLeftOffset )
{
    CGraphics::GetSingleton ().BeginDrawBatch ();

    CVector2D chatTopLeft ( drawList.renderBounds.fX1, drawList.renderBounds.fY1 );
    CVector2D chatBotRight ( drawList.renderBounds.fX2, drawList.renderBounds.fY2 );
    CVector2D chatSize = chatBotRight - chatTopLeft;
    CRect2D chatBounds ( 0, 0, chatSize.fX, chatSize.fY );

    chatBounds.fX1 += topLeftOffset.fX;
    chatBounds.fY1 += topLeftOffset.fY;
    chatBounds.fX2 += topLeftOffset.fX;
    chatBounds.fY2 += topLeftOffset.fY;

    for ( uint i = 0 ; i < drawList.lineItemList.size () ; i++ )
    {
        const SDrawListLineItem& item = drawList.lineItemList[i];
        m_Lines [ item.uiLine ].Draw ( item.vecPosition - chatTopLeft + topLeftOffset, item.ucAlpha, drawList.bShadow, chatBounds );
    }

    CGraphics::GetSingleton ().EndDrawBatch ();
}

//
// GetDrawList
//
// Get list of text lines to draw
//
void CChat::GetDrawList ( SDrawList& outDrawList )
{
    float fLineDifference = CChat::GetFontHeight ( m_vecScale.fY );
    CVector2D vecPosition ( m_vecBackgroundPosition.fX + ( 5.0f * m_vecScale.fX ), m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY - ( fLineDifference * 1.25f ) );
    unsigned long ulTime = GetTickCount32 ();
    float fRcpChatLineFadeOut = 1.0f / m_ulChatLineFadeOut;
    bool bShadow = ( m_Color.A * m_fBackgroundAlpha == 0.f );
    bool bInputShadow = ( m_InputColor.A * m_fInputBackgroundAlpha == 0.f );

    if ( m_Color.A * m_fBackgroundAlpha > 0.f )
    {
        // Hack to draw the background behind the text.
        m_pBackground->SetAlpha ( m_fBackgroundAlpha );
        m_pBackground->SetVisible ( true );
        m_pBackground->Render ();
        m_pBackground->SetVisible ( false );
    }

    if ( m_bInputVisible )
    {
        // ChrML: Hack so chatbox input always works. It might get unfocused..
        if ( !m_pBackground->IsActive () )
        {
            m_pBackground->Activate ();
        }
    }

    // Used for render clipping in CChat::DrawTextString
    CRect2D RenderBounds ( m_vecBackgroundPosition.fX, m_vecBackgroundPosition.fY + 1, m_vecBackgroundPosition.fX + m_vecBackgroundSize.fX, m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY );

    outDrawList.renderBounds = RenderBounds;
    outDrawList.bShadow = bShadow;

    // Smooth scroll
    int iLineScroll;
    float fPixelScroll;
    UpdateSmoothScroll ( &fPixelScroll, &iLineScroll );

    // Apply pixel smooth scroll
    vecPosition.fY -= fPixelScroll * fLineDifference;

    // Apply line smooth scroll
    unsigned int uiLine = (m_uiMostRecentLine + m_uiScrollOffset - iLineScroll) % CHAT_MAX_LINES;
    unsigned int uiLinesDrawn = 0;
    // Loop over the circular buffer
    while ( m_Lines [ uiLine ].IsActive () && uiLinesDrawn < m_uiNumLines + (fabsf( m_fSmoothScroll ) > 0.1f ? 1 : 0) )
    {
        // Calculate line alpha
        long ulLineAge = ulTime - m_Lines [ uiLine ].GetCreationTime ();
        float fLineAlpha = (long)( m_ulChatLineLife + m_ulChatLineFadeOut - ulLineAge ) * fRcpChatLineFadeOut;

        // Clamp line alpha range and combine with Css override
        fLineAlpha = Clamp ( 0.0f, fLineAlpha, 1.0f );
        fLineAlpha += ( 1.0f - fLineAlpha ) * m_fCssStyleOverrideAlpha;

        if ( fLineAlpha > 0.f )
        {
            SDrawListLineItem item;
            item.uiLine = uiLine;
            item.vecPosition = vecPosition;
            item.ucAlpha = static_cast < unsigned char >( fLineAlpha * 255.0f );
            outDrawList.lineItemList.push_back( item );
        }

        vecPosition.fY -= fLineDifference;

        uiLine = (uiLine + 1) % CHAT_MAX_LINES;
        uiLinesDrawn++;
        if ( uiLine == m_uiMostRecentLine )     // Went through all lines?
            break;
    }

    if ( m_InputColor.A * m_fInputBackgroundAlpha > 0.f )
    {
        if ( m_pInput )
        {
            // Hack to draw the input background behind the text.
            m_pInput->SetAlpha ( m_fInputBackgroundAlpha );
            m_pInput->SetVisible ( true );
            m_pInput->Render ();
            m_pInput->SetVisible ( false );
        }
    }

    if ( m_bInputVisible )
    {
        CVector2D vecPosition ( m_vecInputPosition.fX + ( 5.0f * m_vecScale.fX ), m_vecInputPosition.fY + ( fLineDifference * 0.125f ) );
        m_InputLine.Draw ( vecPosition, 255, bInputShadow );
    }
}

//
// CChat::UpdateSmoothScroll
//
void CChat::UpdateSmoothScroll ( float* pfPixelScroll, int *piLineScroll )
{
    // Calc frame time
    float fTimeSeconds = GetSecondCount ();
    float fDeltaSeconds = fTimeSeconds - m_fSmoothLastTimeSeconds;
    m_fSmoothLastTimeSeconds = fTimeSeconds;

    // Handle auto repeat
    if ( m_iScrollState == 0 )
    {
        m_fSmoothRepeatTimer = -100;
    }
    else
    {
        if ( m_fSmoothRepeatTimer < 0.0f )
        {
            if ( m_fSmoothRepeatTimer < -50 )
                m_fSmoothRepeatTimer = 0.250f;
            else
                m_fSmoothRepeatTimer += 1/22.f;

            if ( m_iScrollState == 1 )
                ScrollUp ();
            else if ( m_iScrollState == -1 )
                ScrollDown ();
        }
        m_fSmoothRepeatTimer -= fDeltaSeconds;
    }

    // Time to reset?
    if ( m_fSmoothScrollResetTime && m_fSmoothScrollResetTime < GetSecondCount () )
    {
        m_fSmoothScrollResetTime = 0;
        m_fSmoothScroll -= m_uiScrollOffset;
        m_uiScrollOffset -= m_uiScrollOffset;
    }


    // Calc smooth scroll deceleration
    float fFixedChange = ( m_fSmoothScroll < 0 ? -1 : 1 ) * -Min ( fDeltaSeconds / 0.2f, 1.0f );
    float fLerpChange = m_fSmoothScroll * ( 1 - Min ( fDeltaSeconds / 0.3f, 1.0f ) ) - m_fSmoothScroll;
    float fChange = fLerpChange * 0.7f + fFixedChange * 0.4f;
    fChange = Clamp ( -fabsf ( m_fSmoothScroll ), fChange, fabsf ( m_fSmoothScroll ) );

    // Apply smooth scroll deceleration
    m_fSmoothScroll += fChange;

    // Break into line and pixels
    int iLineScroll = (int)( m_fSmoothScroll );
    if ( m_fSmoothScroll > 0.14 )
        iLineScroll += 1;
    float fPixelScroll = m_fSmoothScroll - iLineScroll;

    // Inhibit pixel scroll if frame rate falls to low
    if ( fDeltaSeconds > 0.1f )
        m_fSmoothAllowAfter = fTimeSeconds + 1.0f;

    if ( m_fSmoothAllowAfter > fTimeSeconds )
        fPixelScroll = 0;

    // Set return values
    *pfPixelScroll = fPixelScroll;
    *piLineScroll = iLineScroll;

    // Update 'scrolling back' flag
    if ( m_uiScrollOffset != 0 )
        m_iScrollingBack = 10;
    else
        m_iScrollingBack = Max ( 0, m_iScrollingBack - 1 );

    //
    // Also update CssStyle override alpha
    //
    float fTarget = ( !m_bCssStyleText || m_bInputVisible || m_iScrollingBack ) ? 1.0f : 0.0f;
    float fMaxAmount = fDeltaSeconds * 2.0f;    // 0.5 seconds fade time
    m_fCssStyleOverrideAlpha += Clamp ( -fMaxAmount, fTarget - m_fCssStyleOverrideAlpha, fMaxAmount );

    //
    // Also update background alpha
    //
    fTarget = ( !m_bCssStyleBackground || m_bInputVisible || m_iScrollingBack ) ? 1.0f : 0.0f;
    fMaxAmount = fDeltaSeconds * 5.0f;          // 0.2 seconds fade time
    m_fBackgroundAlpha += Clamp ( -fMaxAmount, fTarget - m_fBackgroundAlpha, fMaxAmount );

    //
    // Also update input background alpha
    //
    fTarget = ( m_bInputVisible ) ? 1.0f : 0.0f;
    fMaxAmount = fDeltaSeconds * 5.0f;          // 0.2 seconds fade time
    m_fInputBackgroundAlpha += Clamp ( -fMaxAmount, fTarget - m_fInputBackgroundAlpha, fMaxAmount );
}


void CChat::Output ( const char* szText, bool bColorCoded )
{
    CChatLine* pLine = NULL;
    const char* szRemainingText = szText;
    CColor color = m_TextColor;

    // Allow smooth scroll when text is added if game FX Quality is not low
    CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();
    if ( gameSettings->GetFXQuality() > 0 )
        m_fSmoothScroll -= 1.0f;

    do
    {
        m_uiMostRecentLine = (m_uiMostRecentLine == 0 ? CHAT_MAX_LINES - 1 : m_uiMostRecentLine - 1);
        pLine = &m_Lines [ m_uiMostRecentLine ];
        szRemainingText = pLine->Format ( szRemainingText,
            ( m_vecBackgroundSize.fX - ( 10.0f * m_vecScale.fX ) ), color, bColorCoded );
        pLine->SetActive ( true );
        pLine->UpdateCreationTime ();
    }
    while ( szRemainingText );
}


void CChat::Outputf ( bool bColorCoded, const char* szFormat, ... )
{
    SString str;
    
    va_list ap;
    va_start ( ap, szFormat );
    str.vFormat ( szFormat, ap );
    va_end ( ap );

    Output ( str.c_str (), bColorCoded );
}


void CChat::Clear ( void )
{
    for ( int i = 0; i < CHAT_MAX_LINES; i++ )
    {
        m_Lines [ i ].SetActive ( false );
    }
    m_uiMostRecentLine = 0;
    m_fSmoothScroll = 0;
}


void CChat::ClearInput ( void )
{
    m_strInputText.clear ();
    m_InputLine.Clear ();
    m_vecInputSize = CalcInputSize ();
    if ( m_pInput )
        m_pInput->SetSize ( m_vecInputSize );
}

void CChat::ScrollUp ()
{
    if ( m_Lines [ (m_uiMostRecentLine + m_uiScrollOffset + m_uiNumLines) % CHAT_MAX_LINES ].IsActive ()
        && !(
            ((m_uiMostRecentLine + m_uiScrollOffset) % CHAT_MAX_LINES < m_uiMostRecentLine) &&
            ((m_uiMostRecentLine + m_uiScrollOffset + m_uiNumLines) % CHAT_MAX_LINES >= m_uiMostRecentLine)
            )
        )
    {
        m_uiScrollOffset += 1;
        m_fSmoothScroll += 1;
        m_fSmoothScrollResetTime = GetSecondCount () + 5;
    }
}

void CChat::ScrollDown ()
{
    if ( m_uiScrollOffset <= 0 )
    {
        m_fSmoothScroll -= m_uiScrollOffset;
        m_uiScrollOffset -= m_uiScrollOffset;
    }
    else
    {
        m_uiScrollOffset -= 1;
        m_fSmoothScroll -= 1;
        m_fSmoothScrollResetTime = GetSecondCount () + 5;
    }
}

bool CChat::CharacterKeyHandler ( CGUIKeyEventArgs KeyboardArgs )
{
    // If we can take input
    if ( CLocalGUI::GetSingleton ().GetVisibleWindows () == 0 &&
        !CLocalGUI::GetSingleton ().GetConsole ()->IsVisible () &&
        m_bInputVisible )
    {
        // Check if it's a special key like enter and backspace, if not, add it as a character to the message
        switch ( KeyboardArgs.codepoint )
        {
            case VK_BACK:
            {
                if ( m_strInputText.size () > 0 )
                {
                    // Convert our string to UTF8 before resizing, then back to ANSI.
                    std::wstring strText = MbUTF8ToUTF16(m_strInputText);
                    strText.resize ( strText.size () - 1 );
                    SetInputText ( UTF16ToMbUTF8(strText).c_str() );
                }
                break;
            }

            case VK_RETURN:
            {
                // Empty the chat and hide the input stuff
                // If theres a command to call, call it
                if ( !m_strCommand.empty () && !m_strInputText.empty () )
                    CCommands::GetSingleton().Execute ( m_strCommand.c_str (), m_strInputText.c_str () );
            
                // Deactivate the VisibleWindows counter
                CLocalGUI::GetSingleton ().SetVisibleWindows ( false );
                SetInputVisible ( false );

                m_fSmoothScrollResetTime = GetSecondCount ();
                break;
            }
            
            default:
            {
                // If we haven't exceeded the maximum number of characters per chat message, append the char to the message and update the input control
                if ( MbUTF8ToUTF16(m_strInputText).size () < CHAT_MAX_CHAT_LENGTH )
                {                    
                    if ( KeyboardArgs.codepoint >= 32 )
                    {
                        unsigned int uiCharacter = KeyboardArgs.codepoint;
                        if ( uiCharacter < 127 ) // we have any char from ASCII 
                        {
                            // injecting as is
                            m_strInputText += static_cast < char > ( KeyboardArgs.codepoint );
                            SetInputText ( m_strInputText.c_str () );
                        }
                        else // we have any char from Extended ASCII, any ANSI code page or UNICODE range
                        {
                            // Generate a null-terminating string for our character
                            wchar_t wUNICODE[2] = { uiCharacter, '\0' };

                            // Convert our UTF character into an ANSI string
                            std::string strANSI = UTF16ToMbUTF8(wUNICODE);

                            // Append the ANSI string, and update
                            m_strInputText.append(strANSI);
                            SetInputText ( m_strInputText.c_str () );
                        }
                    }
                }
                break;
            }
        }
    }

    return true;
}


void CChat::SetVisible ( bool bVisible )
{
    m_bVisible = bVisible;
}


void CChat::SetInputVisible ( bool bVisible )
{    
    if ( !IsVisible () )
        bVisible = false;

    if ( bVisible )
    {
        ClearInput ();
    }

    m_bInputVisible = bVisible;
}


void CChat::SetNumLines ( unsigned int uiNumLines )
{
    if ( uiNumLines <= CHAT_MAX_LINES )
    {
        m_uiNumLines = uiNumLines;
        UpdateGUI ();
    }
}


void CChat::SetChatFont ( eChatFont Font )
{
    CGUIFont* pFont = g_pCore->GetGUI ()->GetDefaultFont ();
    ID3DXFont* pDXFont = g_pCore->GetGraphics ()->GetFont ();
    switch ( Font )
    {
        case ChatFonts::CHAT_FONT_DEFAULT:
            pFont = g_pCore->GetGUI ()->GetDefaultFont ();
            pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_DEFAULT );
            break;
        case ChatFonts::CHAT_FONT_CLEAR:
            pFont = g_pCore->GetGUI ()->GetClearFont ();
            pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_CLEAR );
            break;
        case ChatFonts::CHAT_FONT_BOLD:
            pFont = g_pCore->GetGUI ()->GetBoldFont ();
            pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_DEFAULT_BOLD );
            break;
        case ChatFonts::CHAT_FONT_ARIAL:
            pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_ARIAL );
            break;                
    }

    // Set fonts
    m_pFont = pFont;
    m_pDXFont = pDXFont;
}


void CChat::UpdateGUI ( void )
{
    m_vecBackgroundSize = CVector2D (
        m_fNativeWidth * m_vecScale.fX,
        CChat::GetFontHeight ( m_vecScale.fY ) * (float(m_uiNumLines) + 0.5f)
    );
    m_vecBackgroundSize.fX = Round ( m_vecBackgroundSize.fX );
    m_vecBackgroundSize.fY = Round ( m_vecBackgroundSize.fY );
    m_vecBackgroundPosition.fX = Round ( m_vecBackgroundPosition.fX );
    m_vecBackgroundPosition.fY = Round ( m_vecBackgroundPosition.fY );
    m_pBackground->SetSize ( m_vecBackgroundSize );

    m_vecInputPosition = CVector2D (
        m_vecBackgroundPosition.fX,
        m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY
    );
    m_vecInputSize = CalcInputSize ();
    if ( m_pInput )
    {
        m_pInput->SetPosition ( m_vecInputPosition );
        m_pInput->SetSize ( m_vecInputSize );
    }
}


void CChat::SetColor ( const CColor& Color )
{
    unsigned long ulBackgroundColor = COLOR_ARGB ( Color.A, Color.R, Color.G, Color.B );

    m_pBackgroundTexture->LoadFromMemory ( &ulBackgroundColor, 1, 1 );
    m_pBackground->LoadFromTexture ( m_pBackgroundTexture );
}


void CChat::SetInputColor ( const CColor& Color )
{
    unsigned long ulInputColor = COLOR_ARGB ( Color.A, Color.R, Color.G, Color.B );

    if ( m_pInputTexture )
        m_pInputTexture->LoadFromMemory ( &ulInputColor, 1, 1 );

    if ( m_pInput )
        m_pInput->LoadFromTexture ( m_pInputTexture );
}


const char* CChat::GetInputPrefix ( void )
{
    return m_InputLine.m_Prefix.GetText ();
}


void CChat::SetInputPrefix ( const char* szPrefix )
{
    m_InputLine.m_Prefix.SetText ( szPrefix );
}

CVector2D CChat::CalcInputSize ( void )
{
    return CVector2D ( m_vecBackgroundSize.fX, ( GetFontHeight ( m_vecScale.fY ) * ( ( float ) m_InputLine.m_ExtraLines.size () + 1.25f ) ) );
}

void CChat::SetInputText ( const char* szText )
{
    m_InputLine.Clear ();
    
    CColor color = m_InputTextColor;
    const char* szRemainingText = m_InputLine.Format ( szText,
        ( m_vecInputSize.fX - ( 10.0f * m_vecScale.fX ) - m_InputLine.m_Prefix.GetWidth () ),
        color, false );

    CChatLine* pLine = NULL;
    
    while ( szRemainingText && m_InputLine.m_ExtraLines.size () < 3 )
    {
        m_InputLine.m_ExtraLines.resize ( m_InputLine.m_ExtraLines.size () + 1 );
        CChatLine& line = *(m_InputLine.m_ExtraLines.end () - 1);
        szRemainingText = line.Format ( szRemainingText,
            ( m_vecInputSize.fX - ( 10.0f * m_vecScale.fX ) ), color, false );
    }

    if ( szText != m_strInputText.c_str () )
        m_strInputText = szText;

    if ( szRemainingText )
        m_strInputText.resize ( szRemainingText - szText );

    m_vecInputSize = CalcInputSize ();
    if ( m_pInput )
        m_pInput->SetSize ( m_vecInputSize );
}


void CChat::SetCommand ( const char* szCommand )
{
    if ( !szCommand )
        return;

    m_strCommand = szCommand;

    if ( strcmp ( szCommand, "chatboxsay" ) == 0 )
    {
        SetInputPrefix ( "Say: " );
    }
    else
    {
        std::string strPrefix = m_strCommand + ": ";
        strPrefix[0] = toupper ( strPrefix[0] );
        SetInputPrefix ( strPrefix.c_str () );
    }
}


float CChat::GetFontHeight ( float fScale )
{
    if ( !g_pChat )
        return 0.0f;

    if ( g_pChat->m_bUseCEGUI )
    {
        return g_pChat->m_pFont->GetFontHeight ( fScale );
    }
    return g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, g_pChat->m_pDXFont );
}


float CChat::GetTextExtent ( const char * szText, float fScale )
{
    if ( !g_pChat )
        return 0.0f;

    if ( g_pChat->m_bUseCEGUI )
    {
        return g_pChat->m_pFont->GetTextExtent ( szText, fScale );
    }
    return g_pCore->GetGraphics ()->GetDXTextExtent ( szText, fScale, g_pChat->m_pDXFont );
}


void CChat::DrawTextString ( const char * szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY, const CRect2D& RenderBounds )
{
    if ( !g_pChat )
        return;

    if ( g_pChat->m_bUseCEGUI )
    {
        g_pChat->m_pFont->DrawTextString ( szText, DrawArea, fZ, ClipRect, ulFormat, ulColor, fScaleX, fScaleY );
    }
    else
    {
        float fLineHeight   = CChat::GetFontHeight ( g_pChat->m_vecScale.fY );

        if ( DrawArea.fY1 < RenderBounds.fY1 )
        {
            // Clip text at the top
            if ( DrawArea.fY1 + fLineHeight - RenderBounds.fY1 > 1 )
               g_pCore->GetGraphics ()->DrawText ( ( int ) DrawArea.fX1, ( int ) RenderBounds.fY1, ( int ) DrawArea.fX2, ( int ) DrawArea.fY1 + fLineHeight, ulColor, szText, fScaleX, fScaleY, DT_LEFT | DT_BOTTOM | DT_SINGLELINE , g_pChat->m_pDXFont );
        }
        else
        if ( DrawArea.fY1 + fLineHeight > RenderBounds.fY2 )
        {
            // Clip text at the bottom
            if ( RenderBounds.fY2 - DrawArea.fY1 > 1 )
                g_pCore->GetGraphics ()->DrawText ( ( int ) DrawArea.fX1, ( int ) DrawArea.fY1, ( int ) DrawArea.fX2, ( int ) RenderBounds.fY2, ulColor, szText, fScaleX, fScaleY, DT_LEFT | DT_TOP | DT_SINGLELINE , g_pChat->m_pDXFont );
        }
        else
        {
            // Text not clipped
            g_pCore->GetGraphics ()->DrawText ( ( int ) DrawArea.fX1, ( int ) DrawArea.fY1, ( int ) DrawArea.fX1, ( int ) DrawArea.fY1, ulColor, szText, fScaleX, fScaleY, DT_LEFT | DT_TOP | DT_NOCLIP, g_pChat->m_pDXFont );
        }
    }
}


CChatLine::CChatLine ( void )
{
    m_bActive = false;
    UpdateCreationTime ();
}

void CChatLine::UpdateCreationTime ()
{
    m_ulCreationTime = GetTickCount32 ();
}

bool CChatLine::IsColorCode ( const char* szColorCode )
{
    if ( *szColorCode != '#' )
        return false;

    bool bValid = true;
    for ( int i = 0; i < 6; i++ )
    {
        char c = szColorCode [ 1 + i ];
        if ( !isdigit ( (unsigned char)c ) && (c < 'A' || c > 'F') && (c < 'a' || c > 'f') )
        {
            bValid = false;
            break;
        }
    }
    return bValid;
}


//
// Calculate the equivalent ansi string pointer of szPosition.
//
const char* CalcAnsiPtr ( const char* szStringAnsi, const wchar_t* szPosition )
{
    int iOrigSize = strlen ( szStringAnsi );
    int iEndSize = UTF16ToMbUTF8 ( szPosition ).length();
    int iOffset = iOrigSize - iEndSize;
    return szStringAnsi + iOffset;
}


const char* CChatLine::Format ( const char* szStringAnsi, float fWidth, CColor& color, bool bColorCoded )
{
    std::wstring wString = MbUTF8ToUTF16 ( szStringAnsi );
    const wchar_t* szString = wString.c_str ();

    float fPrevSectionsWidth = 0.0f;
    m_Sections.clear ();

    const wchar_t* szSectionStart = szString;
    const wchar_t* szSectionEnd = szString;
    const wchar_t* szLastWrapPoint = szString;
    bool bLastSection = false;
    while ( !bLastSection )      // iterate over sections
    {
        m_Sections.resize ( m_Sections.size () + 1 );
        CChatLineSection& section = *(m_Sections.end () - 1);
        section.SetColor ( color );

        if ( m_Sections.size () > 1 && bColorCoded)      // If we've processed sections before
            szSectionEnd += 7;                           // skip the color code

        szSectionStart = szSectionEnd;
        szLastWrapPoint = szSectionStart;
        unsigned int uiSeekPos = 0;
        std::wstring strSectionStart = szSectionStart;

        while ( true )      // find end of this section
        {
            float fSectionWidth = CChat::GetTextExtent ( UTF16ToMbUTF8 ( strSectionStart.substr ( 0 , uiSeekPos ) ).c_str (), g_pChat->m_vecScale.fX );

            if ( *szSectionEnd == '\0' || *szSectionEnd == '\n' || fPrevSectionsWidth + fSectionWidth > fWidth )
            {
                bLastSection = true;
                break;
            }
            if ( bColorCoded && IsColorCode ( UTF16ToMbUTF8 ( szSectionEnd ).c_str () ) )
            {
                unsigned long ulColor = 0;
                sscanf ( UTF16ToMbUTF8 ( szSectionEnd ).c_str () + 1, "%06x", &ulColor );
                color = ulColor;
                fPrevSectionsWidth += fSectionWidth;
                break;
            }
            if ( isspace ( (unsigned char)*szSectionEnd ) || ispunct ( (unsigned char)*szSectionEnd ) )
            {
                szLastWrapPoint = szSectionEnd;
            }
            szSectionEnd++;
            uiSeekPos++;
        }
        section.m_strText = UTF16ToMbUTF8 ( strSectionStart.substr ( 0 , uiSeekPos ) );
    }

    if ( *szSectionEnd == '\0' )
    {
        return NULL;
    }
    else if( *szSectionEnd == '\n' )
    {
        return CalcAnsiPtr ( szStringAnsi, szSectionEnd + 1 );
    }
    else
    {
        // Do word wrap
        if ( szLastWrapPoint == szSectionStart )
        {
            // Wrapping point coincides with the start of a section.
            if ( szLastWrapPoint == szString )
            {
                // The line consists of one huge word. Leave the one section we created as it
                // is (with the huge word cut off) and return szRemaining as the rest of the word
                return CalcAnsiPtr ( szStringAnsi, szSectionEnd );
            }
            else
            {
                // There's more than one section, remove the last one (where our wrap point is)
                m_Sections.pop_back ();
            }
        }
        else
        {
            // Wrapping point is in the middle of a section, truncate
            CChatLineSection& last = *( m_Sections.end () - 1 );
            std::wstring wstrTemp = MbUTF8ToUTF16 ( last.m_strText );
            wstrTemp.resize ( szLastWrapPoint - szSectionStart );
            last.m_strText = UTF16ToMbUTF8 ( wstrTemp );
        }
        return CalcAnsiPtr ( szStringAnsi, szLastWrapPoint );
    }
}


void CChatLine::Draw ( const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds )
{
    float fCurrentX = vecPosition.fX;
    std::vector < CChatLineSection >::iterator iter = m_Sections.begin ();
    for ( ; iter != m_Sections.end () ; iter++ )
    {
        (*iter).Draw ( CVector2D ( fCurrentX, vecPosition.fY ), ucAlpha, bShadow, RenderBounds );
        fCurrentX += (*iter).GetWidth ();
    }
}


float CChatLine::GetWidth ()
{
    float fWidth = 0.0f;
    std::vector < CChatLineSection >::iterator it;
    for ( it = m_Sections.begin (); it != m_Sections.end (); it++ )
    {
        fWidth += (*it).GetWidth ();
    }
    return fWidth;
}

void CChatLine::RemoveColorCode ( const char* szString, std::string& strOut )
{
    strOut.clear ();
    const char* szStart = szString;
    const char* szEnd = szString;

    while ( true )
    {
        if ( *szEnd == '\0' )
        {
            strOut.append ( szStart, szEnd - szStart );
            break;
        }
        else if ( IsColorCode ( szEnd ) )
        {
            strOut.append ( szStart, szEnd - szStart );
            szStart = szEnd + 7;
            szEnd = szStart;
        }
        else
        {
            szEnd++;
        }
    }
}

void CChatInputLine::Draw ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow )
{
    CRect2D RenderBounds ( 0, 0, 9999, 9999 );

    CColor colPrefix;
    m_Prefix.GetColor ( colPrefix );
    if ( colPrefix.A > 0 )
        m_Prefix.Draw ( vecPosition, colPrefix.A, bShadow, RenderBounds );
    
    if ( g_pChat->m_InputTextColor.A > 0 && m_Sections.size () > 0 )
    {
        m_Sections [ 0 ].Draw ( CVector2D ( vecPosition.fX + m_Prefix.GetWidth (), vecPosition.fY ),
            g_pChat->m_InputTextColor.A, bShadow, RenderBounds );

        float fLineDifference = CChat::GetFontHeight ( g_pChat->m_vecScale.fY );

        vector < CChatLine >::iterator iter = m_ExtraLines.begin ();
        for ( ; iter != m_ExtraLines.end () ; iter++ )
        {
            vecPosition.fY += fLineDifference;
            (*iter).Draw ( vecPosition, g_pChat->m_InputTextColor.A, bShadow, RenderBounds );
        }
    }
}


void CChatInputLine::Clear ( void )
{
    m_Sections.clear ();
    m_ExtraLines.clear ();
}

CChatLineSection::CChatLineSection ()
{
    m_fCachedWidth = -1.0f;
    m_uiCachedLength = 0;
}

CChatLineSection::CChatLineSection ( const CChatLineSection& other )
{
    *this = other;
}

CChatLineSection& CChatLineSection::operator = ( const CChatLineSection& other )
{
    m_strText = other.m_strText;
    m_Color = other.m_Color;
    m_fCachedWidth = other.m_fCachedWidth;
    m_uiCachedLength = other.m_uiCachedLength;
    return *this;
}

void CChatLineSection::Draw ( const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds )
{
    if ( !m_strText.empty () && ucAlpha > 0 )
    {
        if ( bShadow )
        {
            CRect2D drawShadowAt ( vecPosition.fX + 1.0f, vecPosition.fY + 1.0f, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f );
            CChat::DrawTextString ( m_strText.c_str (), drawShadowAt, 0.0f, drawShadowAt, 0, COLOR_ARGB ( ucAlpha, 0, 0, 0 ), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY, RenderBounds );                
        }
        CRect2D drawAt ( vecPosition.fX, vecPosition.fY, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f );
        CChat::DrawTextString ( m_strText.c_str (), drawAt, 0.0f, drawAt, 0, COLOR_ARGB ( ucAlpha, m_Color.R, m_Color.G, m_Color.B ), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY, RenderBounds );
    }
}


float CChatLineSection::GetWidth ()
{
    if ( m_fCachedWidth < 0.0f || m_strText.size () != m_uiCachedLength )
    {
        m_fCachedWidth = CChat::GetTextExtent ( m_strText.c_str (), g_pChat->m_vecScale.fX ) / Max ( 0.01f, g_pChat->m_vecScale.fX );
        m_uiCachedLength = m_strText.size ();
    }
    return m_fCachedWidth * g_pChat->m_vecScale.fX;
}
