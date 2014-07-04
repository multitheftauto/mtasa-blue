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

#ifndef __CCHAT_H
#define __CCHAT_H

#include "CGUI.h"
#include <core/CCoreInterface.h>

class CChatLineSection;

#define CHAT_WIDTH              320                             // Chatbox default width
#define CHAT_TEXT_COLOR         CColor(235, 221, 178)           // Chatbox default text color
#define CHAT_MAX_LINES          100                             // Chatbox maximum chat lines
#define CHAT_MAX_CHAT_LENGTH    96                              // Chatbox maximum chat message length
#define CHAT_BUFFER             1024                            // Chatbox buffer size

class CColor
{
public:
    inline                      CColor              ( void )
    {
        R = G = B = A = 255;
    }
    inline                      CColor              ( unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A = 255 )
    {
        R = _R;
        G = _G;
        B = _B;
        A = _A;
    }
    inline                      CColor              ( const CColor& other )
    {
        *this = other;
    }
    inline                      CColor              ( unsigned long ulColor )
    {
        *this = ulColor;
    }
    inline CColor&              operator =          ( const CColor& color )
    {
        R = color.R;
        G = color.G;
        B = color.B;
        A = color.A;
        return *this;
    }
    inline CColor&              operator =          ( unsigned long ulColor )
    {
        R = (ulColor >> 16) & 0xFF;
        G = (ulColor >>  8) & 0xFF;
        B = (ulColor      ) & 0xFF;
        return *this;
    }
    bool                        operator ==         ( const CColor& other ) const
    {
        return R == other.R
            && G == other.G
            && B == other.B
            && A == other.A;
    }

    unsigned char               R, G, B, A;
};

class CChatLineSection
{
public:
    friend class CChatLine;

                                CChatLineSection        ();
                                CChatLineSection        ( const CChatLineSection& other );

    CChatLineSection&           operator =              ( const CChatLineSection& other );

    void                        Draw                    ( const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds );
    float                       GetWidth                ( void );
    inline const char*          GetText                 ( void )            { return m_strText.c_str (); }
    void                        SetText                 ( const char* szText )  { m_strText = szText; }
    inline void                 GetColor                ( CColor& color )   { color = m_Color; }
    inline void                 SetColor                ( const CColor& color )   { m_Color = color; }

protected:
    std::string                 m_strText;
    CColor                      m_Color;
    float                       m_fCachedWidth;
    unsigned int                m_uiCachedLength;
};

class CChatLine
{
public:
                                CChatLine               ( void );

    static bool                 IsColorCode             ( const char* szColorCode );

    virtual const char*         Format                  ( const char* szText, float fWidth, CColor& color, bool bColorCoded );
    virtual void                Draw                    ( const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, const CRect2D& RenderBounds );    
    virtual float               GetWidth                ( void );
    bool                        IsActive                ( void )    { return m_bActive; }
    void                        SetActive               ( bool bActive )    { m_bActive = bActive; }
    
    static void                 RemoveColorCode         ( const char* szText, std::string& strOut );

    inline unsigned long        GetCreationTime         ( void )    { return m_ulCreationTime; }
    inline void                 UpdateCreationTime      ( void );

protected:

    bool                                m_bActive;
    std::vector < CChatLineSection >    m_Sections;
    unsigned long                       m_ulCreationTime;
};

class CChatInputLine : public CChatLine
{
public:
    void                        Draw                    ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow );
    void                        Clear                   ( void );

    CChatLineSection            m_Prefix;
    std::vector < CChatLine >   m_ExtraLines;
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

    virtual void                Draw                    ( void );
    virtual void                Output                  ( const char* szText, bool bColorCoded = true );
    virtual void                Outputf                 ( bool bColorCoded, const char* szFormat, ... );
    void                        Clear                   ( void );
    void                        ClearInput              ( void );
    bool                        CharacterKeyHandler     ( CGUIKeyEventArgs KeyboardArgs );

    inline bool                 IsVisible               ( void )            { return m_bVisible; }
    void                        SetVisible              ( bool bVisible );
    inline bool                 IsInputVisible          ( void )            { return m_bInputVisible; }
    void                        SetInputVisible         ( bool bVisible );

    const char*                 GetInputPrefix          ( void );
    void                        SetInputPrefix          ( const char* szPrefix );
    const char*                 GetInputText            ( void )    { return m_strInputText.c_str (); }
    void                        SetInputText            ( const char* szText );
    const char*                 GetCommand              ( void )    { return m_strCommand.c_str (); }
    void                        SetCommand              ( const char* szCommand );

    static float                GetFontHeight           ( float fScale = 1.0f );
    static float                GetCharacterWidth       ( int iChar, float fScale = 1.0f );
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

    CGUI*                       m_pManager;
    CGUIFont*                   m_pFont;
    LPD3DXFONT                  m_pDXFont;

    CVector2D                   m_vecBackgroundPosition;
    CVector2D                   m_vecBackgroundSize;
    CVector2D                   m_vecInputPosition;
    CVector2D                   m_vecInputSize;

    CGUITexture*                m_pBackgroundTexture;
    CGUITexture*                m_pInputTexture;
    CGUIStaticImage*            m_pBackground;
    CGUIStaticImage*            m_pInput;

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

    bool                        m_bCanChangeWidth;
    int                         m_iCVarsRevision;
};

#endif
