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
#include <core/CCoreInterface.h>
#include <list>
using namespace std;

class CChatLineSection;

#define CHAT_WIDTH              320                             // Chatbox default width
#define CHAT_TEXT_COLOR         CColor(235, 221, 178, 255)      // Chatbox default text color
#define CHAT_MAX_LINES          30                              // Chatbox maximum chat lines
#define CHAT_MAX_CHAT_LENGTH    96                              // Chatbox maximum chat message length
#define CHAT_BUFFER             1024                            // Chatbox buffer size

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

    char*                       GetInputPrefix          ( void );
    void                        SetInputPrefix          ( char* szPrefix );
    char*                       GetInputText            ( void );
    void                        SetInputText            ( char* szText );
    inline char*                GetCommand              ( void );
    void                        SetCommand              ( char* szCommand );

    static float                GetFontHeight           ( float fScale = 1.0f );
    static float                GetCharacterWidth       ( int iChar, float fScale = 1.0f );
    static float                GetTextExtent           ( const char * szText, float fScale = 1.0f );
    static void                 DrawTextString          ( const char * szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY );

    void                        SetColor                ( CColor& Color );
    void                        SetInputColor           ( CColor& Color );
    void                        SetTextColor            ( CColor& Color )   { m_TextColor = Color; };
    void                        SetNumLines             ( unsigned int uiNumLines );

    void                        SetChatFont             ( eChatFont Font );

private:
    void                        LoadCVars               ( void );

protected:
    void                        UpdateGUI               ( void );

    list < CChatLine* >         m_Lines;
    CChatInputLine*             m_pInputLine;

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

    char*                       m_szInputText;
    char*                       m_szCommand;

    bool                        m_bVisible;
    bool                        m_bInputVisible;
    
    unsigned int                m_uiNumLines;
    CColor                      m_Color;
    CColor                      m_TextColor;
    CColor                      m_InputColor;
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