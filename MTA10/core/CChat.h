/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CChat.h
*  PURPOSE:		Header file for the chatbox class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCHAT_H
#define __CCHAT_H

#include "CGUI.h"
#include <list>
using namespace std;

class CChatLineSection;

#define MAX_LINES 30
#define CCHATBOX_MAXCHATLENGTH	96

#define DEFAULT_CHAT_LINE_LIFE			12000
#define DEFAULT_CHAT_LINE_FADE_OUT		3000
#define CCHAT_BUFFER					1024

class CColor
{
public:
    inline                      CColor              ( void )
    {
        this->R = 255; this->G = 255; this->B = 255; this->A = 255;
    }
    inline                      CColor              ( unsigned char R, unsigned char G, unsigned char B, unsigned char A )
    {
        this->R = R; this->G = G; this->B = B; this->A = A;
    }
    inline CColor&              operator =          ( CColor& color )
    {
        this->R = color.R; this->G = color.G; this->B = color.B; this->A = color.A;
        return *this;
    }
    unsigned char               R, G, B, A;
};

class CChatLine
{
public:
                                CChatLine               ( void );
    virtual                     ~CChatLine              ( void );

    static bool                 IsNumber                ( char c );
    static unsigned char        GetNumber               ( char c );
    virtual char*               Format                  ( char* szText, float fWidth, CColor& color, bool bColorCoded, unsigned int& uiCharsUsed );
    virtual void                Draw                    ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow );    
    virtual float               GetWidth                ( unsigned int uiLength = 0 );
    virtual unsigned int        GetLength               ( void )    { return m_uiLength; }
    
    static void                 RemoveColorCode         ( char* szText, char* szReturn, unsigned int uiReturnLength );

    inline unsigned long        GetCreationTime         ( void )    { return m_ulCreationTime; }

protected:
    virtual void                SetText                 ( char* szText );
    virtual void                AddText                 ( char* szText );

private:

    char*                       m_szText;
    list < CChatLineSection* >  m_Sections;
    unsigned long               m_ulCreationTime;
    unsigned int                m_uiLength;

    struct SMarker
    {
        unsigned int            uiPosition;
        CColor                  m_Color;
    };
};

class CChatInputLine : public CChatLine
{
public:
                                CChatInputLine          ( void );
                                ~CChatInputLine         ( void );

    char*                       Format                  ( char* szText, float fWidth, CColor& color, bool bColorCoded, unsigned int& uiCharsUsed );
    void                        Draw                    ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow );
    void                        Clear                   ( void );
    inline unsigned int         GetEditPosition         ( void )                        { return m_uiEditPosition; }
    void                        SetEditPosition         ( CVector2D& vecOrigin, unsigned int uiPosition );

    CChatLineSection*           m_pPrefix;
    CChatLineSection*           m_pText;
    list < CChatLine* >         m_ExtraLines;

protected:
    unsigned int                m_uiEditPosition;
    CVector2D                   m_vecEditPosition;
};

class CChat
{
    friend CChatLine;
    friend CChatInputLine;
    friend CChatLineSection;
public:
    inline                      CChat                   ( void ) {};
                                CChat                   ( CGUI* pManager, CVector2D & vecPosition );
                                ~CChat                  ( void );

    virtual void                Draw                    ( void );
    virtual void                Output                  ( char* szText, bool bColorCoded = true );
    virtual void                Outputf                 ( bool bColorCoded, char* szText, ... );
    void                        Clear                   ( void );
    void                        ClearInput              ( void );
    bool                        CharacterKeyHandler     ( CGUIKeyEventArgs KeyboardArgs );
    inline bool                 IsVisible               ( void )            { return m_bVisible; }
    void                        SetVisible              ( bool bVisible );
    inline bool                 IsInputVisible          ( void )            { return m_bInputVisible; }
    void                        SetInputVisible         ( bool bVisible );
    inline unsigned int         GetNumLines             ( void )            { return m_uiNumLines; }
    void                        SetNumLines             ( unsigned int uiNumLines );
    inline CGUIFont*            GetFont                 ( void )            { return m_pFont; }
    void                        SetFont                 ( CGUIFont* pFont );
    inline LPD3DXFONT           GetDXFont               ( void )            { return m_pDXFont; }
    void                        SetDXFont               ( LPD3DXFONT pFont );
    void                        GetScale                ( CVector2D& vecScale );
    void                        SetScale                ( CVector2D& vecScale );
    float                       GetWidth                ( void );
    void                        SetWidth                ( float fWidth );
    inline void                 GetColor                ( CColor& color )   { color = m_Color; }
    void                        SetColor                ( CColor& color );
    inline void                 GetTextColor            ( CColor& color )   { color = m_TextColor; }
    inline void                 SetTextColor            ( CColor& color )   { m_TextColor = color; }
    inline void                 GetInputColor           ( CColor& color )   { color = m_InputColor; }
    void                        SetInputColor           ( CColor& color );
    void                        SetInputColor           ( unsigned long& ulColor );
    char*                       GetInputPrefix          ( void );
    void                        SetInputPrefix          ( char* szPrefix );
    void                        GetInputPrefixColor     ( CColor& color );
    void                        SetInputPrefixColor     ( CColor& color );
    char*                       GetInputText            ( void );
    void                        SetInputText            ( char* szText );
    void                        GetInputTextColor       ( CColor& color );
    void                        SetInputTextColor       ( CColor& color );
    inline char*                GetCommand              ( void );
    void                        SetCommand              ( char* szCommand );
    inline bool                 GetCssStyleText         ( void )                    { return m_bCssStyleText; }
    void                        SetCssStyleText         ( bool bEnabled );
    inline bool                 GetCssStyleBackground   ( void )                    { return m_bCssStyleBackground; }
    void                        SetCssStyleBackground   ( bool bEnabled );
    inline unsigned long        GetChatLineLife         ( void )                    { return m_ulChatLineLife; }
    void                        SetChatLineLife         ( unsigned long ulTime )    { m_ulChatLineLife = ulTime; }
    inline unsigned long        GetChatLineFadeOut      ( void )                    { return m_ulChatLineFadeOut; }
    void                        SetChatLineFadeOut      ( unsigned long ulTime )    { m_ulChatLineFadeOut = ulTime; }
    inline bool                 GetUseCEGUI             ( void )                    { return m_bUseCEGUI; }
    virtual void                SetUseCEGUI             ( bool bUseCEGUI );

    static float                GetFontHeight           ( float fScale = 1.0f );
    static float                GetCharacterWidth       ( int iChar, float fScale = 1.0f );
    static float                GetTextExtent           ( const char * szText, float fScale = 1.0f );
    static void                 DrawTextString          ( const char * szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY );

protected:
    list < CChatLine* >         m_Lines;
    unsigned int                m_uiNumLines;
    CGUI*                       m_pManager;
    CGUIFont*                   m_pFont;
    LPD3DXFONT                  m_pDXFont;
    CGUIStaticImage*            m_pBackground;
    CGUITexture*                m_pBackgroundTexture;
    CVector2D                   m_vecBackgroundPosition;
    CVector2D                   m_vecBackgroundSize;
    CColor                      m_Color;
    CColor                      m_TextColor;
    CGUIStaticImage*            m_pInput;
    CGUITexture*                m_pInputTexture;
    CVector2D                   m_vecInputPosition;
    CVector2D                   m_vecInputSize;
    CColor                      m_InputColor;
    CChatInputLine*             m_pInputLine;
    char*                       m_szInputText;
    bool                        m_bVisible;
    bool                        m_bInputVisible;
    float                       m_fWidth;
    char*                       m_szCommand;
    bool                        m_bCssStyleText;
    bool                        m_bCssStyleBackground;
    unsigned long               m_ulChatLineLife;
    unsigned long               m_ulChatLineFadeOut;
    bool                        m_bUseCEGUI;
    CVector2D                   m_vecScale;
};

class CChatLineSection
{
public:
                                CChatLineSection        ( void );
                                ~CChatLineSection       ( void );

    void                        Draw                    ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow );
    float                       GetWidth                ( unsigned int uiLength = 0 );
    inline char*                GetText                 ( void )            { return m_szText; }
    void                        SetText                 ( char* szText );
    inline void                 GetColor                ( CColor& color )   { color = m_Color; }
    inline void                 SetColor                ( CColor& color )   { m_Color = color; }
    inline unsigned int         GetLength               ( void )            { return m_uiLength; }

protected:
    char*                       m_szText;
    CColor                      m_Color;
    unsigned int                m_uiLength;
};

#endif