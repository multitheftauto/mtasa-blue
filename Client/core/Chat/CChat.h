/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CChat.h
*  PURPOSE:     Header file for the chatbox class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once 

#include "CGUI.h"
#include <core/CCoreInterface.h>

#include "CColor.h"
#include "CChatLineSection.h"
#include "CChatLine.h"
#include "CChatInputLine.h"


#define CHAT_WIDTH              320                             // Chatbox default width
#define CHAT_TEXT_COLOR         CColor(235, 221, 178)           // Chatbox default text color
#define CHAT_MAX_LINES          100                             // Chatbox maximum chat lines
#define CHAT_MAX_CHAT_LENGTH    96                              // Chatbox maximum chat message length
#define CHAT_BUFFER             1024                            // Chatbox buffer size

//
// SDrawListLineItem
//
struct SDrawListLineItem
{
    uint        uiLine;
    CVector2D   vecPosition;
    uchar       ucAlpha;

    bool operator!= ( const SDrawListLineItem& other ) const
    {
        return !operator==( other );
    }
    bool operator== ( const SDrawListLineItem& other ) const
    {
        return uiLine == other.uiLine
          && vecPosition == other.vecPosition
          && ucAlpha == other.ucAlpha;
    }
};


//
// SDrawList - Used to store a snapshot of what the chatbox is currently rendering
//
struct SDrawList
{
    CRect2D                             renderBounds;
    bool                                bShadow;
    std::vector < SDrawListLineItem >   lineItemList;

    bool operator!= ( const SDrawList& other ) const
    {
        return !operator==( other );
    }
    bool operator== ( const SDrawList& other ) const
    {
        if ( lineItemList.size () != other.lineItemList.size ()
          || bShadow != other.bShadow
          || renderBounds != other.renderBounds )
            return false;

        for ( uint i = 0 ; i < lineItemList.size () ; i++ )
            if ( lineItemList[i] != other.lineItemList[i] )
                return false;

        return true;
    }
};



class CChat
{
    friend class CChatLine;
    friend class CChatInputLine;
    friend class CChatLineSection;

public:
    inline                      CChat                   ( void ) {};
                                CChat                   ( CGUI* pManager, const CVector2D & vecPosition );
                                ~CChat                  ( void );

    virtual void                Draw                    ( bool bUseCacheTexture );
    virtual void                Output                  ( const char* szText, bool bColorCoded = true );
    void                        Clear                   ( void );
    void                        ClearInput              ( void );
    bool                        CharacterKeyHandler     ( CGUIKeyEventArgs KeyboardArgs );
    void                        SetDxFont               ( LPD3DXFONT pDXFont );

    inline bool                 IsVisible               ( void )            { return m_bVisible; }
    void                        SetVisible              ( bool bVisible );
    inline bool                 IsInputVisible          ( void )            { return m_bVisible && m_bInputVisible; }
    void                        SetInputVisible         ( bool bVisible );

    const char*                 GetInputPrefix          ( void );
    void                        SetInputPrefix          ( const char* szPrefix );
    const char*                 GetInputText            ( void )    { return m_strInputText.c_str (); }
    void                        SetInputText            ( const char* szText );
    const char*                 GetCommand              ( void )    { return m_strCommand.c_str (); }
    void                        SetCommand              ( const char* szCommand );
    CVector2D                   CalcInputSize           ( void );

    static float                GetFontHeight           ( float fScale = 1.0f );
    static float                GetTextExtent           ( const char * szText, float fScale = 1.0f );
    static void                 DrawTextString          ( const char * szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY, const CRect2D& RenderBounds );

    void                        SetColor                ( const CColor& Color );
    void                        SetInputColor           ( const CColor& Color );
    void                        SetTextColor            ( const CColor& Color )   { m_TextColor = Color; };
    void                        SetNumLines             ( unsigned int uiNumLines );

    void                        Scroll                  ( int iState )  { m_iScrollState = iState; };
    void                        ScrollUp                ( void );
    void                        ScrollDown              ( void );

    void                        SetChatFont             ( eChatFont Font );
    void                        OnModLoad               ( void );

private:
    void                        LoadCVars               ( void );

protected:
    void                        UpdateGUI               ( void );
    void                        UpdateSmoothScroll      ( float* pfPixelScroll, int *piLineScroll );
    void                        DrawDrawList            ( const SDrawList& drawList, const CVector2D& topLeftOffset = CVector2D ( 0, 0 ) );
    void                        GetDrawList             ( SDrawList& outDrawList );

    CChatLine                   m_Lines [ CHAT_MAX_LINES ];     // Circular buffer
    int                         m_iScrollState;                 // 1 up, 0 stop, -1 down 
    unsigned int                m_uiMostRecentLine;
    unsigned int                m_uiScrollOffset;
    float                       m_fSmoothScroll;
    float                       m_fSmoothLastTimeSeconds;
    float                       m_fSmoothAllowAfter;
    float                       m_fSmoothScrollResetTime;
    float                       m_fSmoothRepeatTimer;
    CChatInputLine              m_InputLine;
    SString                     m_strLastPlayerNamePart;
    SString                     m_strLastPlayerName;

    CGUI*                       m_pManager;
    CGUIFont*                   m_pFont;
    LPD3DXFONT                  m_pDXFont;

    CVector2D                   m_vecBackgroundPosition;
    CVector2D                   m_vecBackgroundSize;
    CVector2D                   m_vecInputPosition;
    CVector2D                   m_vecInputSize;

    std::unique_ptr<CGUITexture>     m_pBackgroundTexture;
    std::unique_ptr<CGUITexture>     m_pInputTexture;
    std::unique_ptr<CGUIStaticImage> m_pBackground;
    std::unique_ptr<CGUIStaticImage> m_pInput;

    std::string                 m_strInputText;
    std::string                 m_strCommand;

    bool                        m_bVisible;
    bool                        m_bInputVisible;
    int                         m_iScrollingBack;           // Non zero if currently scrolling back
    float                       m_fCssStyleOverrideAlpha;   // For fading out 'CssStyle' effect. (When entering text or scrolling back)
    float                       m_fBackgroundAlpha;
    float                       m_fInputBackgroundAlpha;
    
    unsigned int                m_uiNumLines;
    CColor                      m_Color;
    CColor                      m_TextColor;
    CColor                      m_InputColor;
    CColor                      m_InputTextColor;
    bool                        m_bCssStyleText;
    bool                        m_bCssStyleBackground;
    unsigned long               m_ulChatLineLife;
    unsigned long               m_ulChatLineFadeOut;
    bool                        m_bUseCEGUI;
    CVector2D                   m_vecScale;
    float                       m_fNativeWidth;
    float                       m_fRcpUsingDxFontScale;

    bool                        m_bCanChangeWidth;
    int                         m_iCVarsRevision;

    SDrawList                   m_PrevDrawList;
    CRenderTargetItem*          m_pCacheTexture;
    int                         m_iCacheTextureRevision;

    CVector2D                   m_RenderTargetChatSize;
    int                         m_iReportCount;
    CTickCount                  m_lastRenderTargetCreationFail;

    bool                        m_bNickCompletion;
};

