/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CChat.h
 *  PURPOSE:     Header file for the chatbox class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUI.h"
#include <core/CCoreInterface.h>

class CChatLineSection;

#define CHAT_WIDTH 320                                   // Chatbox default width
#define CHAT_TEXT_COLOR CColor(235, 221, 178)            // Chatbox default text color
#define CHAT_MAX_LINES 100                               // Chatbox maximum chat lines
#define CHAT_BUFFER 1024                                 // Chatbox buffer size
#define CHAT_INPUT_HISTORY_LENGTH 128                    // Chatbox input history length

class CColor
{
public:
    CColor() { R = G = B = A = 255; }
    CColor(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A = 255)
    {
        R = _R;
        G = _G;
        B = _B;
        A = _A;
    }
    CColor(const CColor& other) { *this = other; }
    CColor(unsigned long ulColor) { *this = ulColor; }
    CColor& operator=(const CColor& color)
    {
        R = color.R;
        G = color.G;
        B = color.B;
        A = color.A;
        return *this;
    }
    CColor& operator=(unsigned long ulColor)
    {
        R = (ulColor >> 16) & 0xFF;
        G = (ulColor >> 8) & 0xFF;
        B = (ulColor)&0xFF;
        return *this;
    }
    bool operator==(const CColor& other) const { return R == other.R && G == other.G && B == other.B && A == other.A; }

    unsigned char R, G, B, A;
};

class CChatLineSection
{
public:
    friend class CChatLine;

    CChatLineSection();
    CChatLineSection(const CChatLineSection& other);

    CChatLineSection& operator=(const CChatLineSection& other);

    void        Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds);
    float       GetWidth();
    const char* GetText() { return m_strText.c_str(); }
    void        SetText(const char* szText) { m_strText = szText; }
    void        GetColor(CColor& color) { color = m_Color; }
    void        SetColor(const CColor& color) { m_Color = color; }

protected:
    std::string  m_strText;
    CColor       m_Color;
    float        m_fCachedWidth;
    unsigned int m_uiCachedLength;
};

class CChatLine
{
public:
    CChatLine();

    virtual const char* Format(const char* szText, float fWidth, CColor& color, bool bColorCoded);
    virtual void        Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds);
    virtual float       GetWidth();
    bool                IsActive() { return m_bActive; }
    void                SetActive(bool bActive) { m_bActive = bActive; }

    unsigned long GetCreationTime() { return m_ulCreationTime; }
    void          UpdateCreationTime();

protected:
    bool                          m_bActive;
    std::vector<CChatLineSection> m_Sections;
    unsigned long                 m_ulCreationTime;
};

class CChatInputLine : public CChatLine
{
public:
    void Draw(CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline);
    void Clear();

    CChatLineSection       m_Prefix;
    std::vector<CChatLine> m_ExtraLines;
};

//
// SDrawListLineItem
//
struct SDrawListLineItem
{
    uint      uiLine;
    CVector2D vecPosition;
    uchar     ucAlpha;

    bool operator!=(const SDrawListLineItem& other) const { return !operator==(other); }
    bool operator==(const SDrawListLineItem& other) const { return uiLine == other.uiLine && vecPosition == other.vecPosition && ucAlpha == other.ucAlpha; }
};

//
// SDrawList - Used to store a snapshot of what the chatbox is currently rendering
//
struct SDrawList
{
    CRect2D                        renderBounds;
    bool                           bShadow;
    bool                           bOutline;
    std::vector<SDrawListLineItem> lineItemList;

    bool operator!=(const SDrawList& other) const { return !operator==(other); }
    bool operator==(const SDrawList& other) const
    {
        if (lineItemList.size() != other.lineItemList.size() || bShadow != other.bShadow || bOutline != other.bOutline || renderBounds != other.renderBounds)
            return false;

        for (uint i = 0; i < lineItemList.size(); i++)
            if (lineItemList[i] != other.lineItemList[i])
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
    CChat(){};
    CChat(CGUI* pManager, const CVector2D& vecPosition);
    virtual ~CChat();

    virtual void Draw(bool bUseCacheTexture, bool bAllowOutline);
    virtual void Output(const char* szText, bool bColorCoded = true);
    void         Clear();
    void         ClearInput();
    bool         CharacterKeyHandler(CGUIKeyEventArgs KeyboardArgs);
    void         SetDxFont(LPD3DXFONT pDXFont);

    bool IsVisible() { return m_bVisible; }
    void SetVisible(bool bVisible, bool bInputBlocked);
    bool IsInputBlocked() const { return m_bInputBlocked; }
    bool IsInputVisible() const { return !m_bInputBlocked && m_bInputVisible; }
    void SetInputVisible(bool bVisible);

    bool CanTakeInput() { return !CLocalGUI::GetSingleton().GetConsole()->IsVisible() && IsInputVisible(); };

    void ResetHistoryChanges();
    void SelectInputHistoryEntry(int iEntry);
    bool SetNextHistoryText();
    bool SetPreviousHistoryText();

    const char* GetInputPrefix();
    void        SetInputPrefix(const char* szPrefix);
    const char* GetInputText() { return m_strInputText.c_str(); }
    void        SetInputText(const char* szText);
    const char* GetCommand() { return m_strCommand.c_str(); }
    void        SetCommand(const char* szCommand);
    CVector2D   CalcInputSize();

    static float GetFontHeight(float fScale = 1.0f);
    static float GetTextExtent(const char* szText, float fScale = 1.0f);
    static void  DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX,
                                float fScaleY, bool bOutline, const CRect2D& RenderBounds);

    void SetColor(const CColor& Color);
    void SetInputColor(const CColor& Color);
    void SetTextColor(const CColor& Color) { m_TextColor = Color; };
    void SetNumLines(unsigned int uiNumLines);

    void Scroll(int iState) { m_iScrollState = iState; };
    void ScrollUp();
    void ScrollDown();

    void SetChatFont(eChatFont Font);
    void OnModLoad();

    void          SetCharacterLimit(int charLimit);
    int           GetCharacterLimit() const { return m_iCharacterLimit; }
    constexpr int GetDefaultCharacterLimit() const { return m_iDefaultCharacterLimit; }
    constexpr int GetMaxCharacterLimit() const { return m_iMaxCharacterLimit; }

    float GetChatBottomPosition() const noexcept;

private:
    void LoadCVars();

protected:
    virtual void UpdatePosition();

    void UpdateGUI();
    void UpdateSmoothScroll(float* pfPixelScroll, int* piLineScroll);
    void DrawDrawList(const SDrawList& drawList, const CVector2D& topLeftOffset = CVector2D(0, 0));
    void GetDrawList(SDrawList& outDrawList, bool bUsingOutline);
    void DrawInputLine(bool bUsingOutline);

    CChatLine      m_Lines[CHAT_MAX_LINES];            // Circular buffer
    int            m_iScrollState;                     // 1 up, 0 stop, -1 down
    unsigned int   m_uiMostRecentLine;
    unsigned int   m_uiScrollOffset;
    float          m_fSmoothScroll;
    float          m_fSmoothLastTimeSeconds;
    float          m_fSmoothAllowAfter;
    float          m_fSmoothScrollResetTime;
    float          m_fSmoothRepeatTimer;
    CChatInputLine m_InputLine;
    SString        m_strLastPlayerNamePart;
    SString        m_strLastPlayerName;

    float                   m_fPositionOffsetX;
    float                   m_fPositionOffsetY;
    eChatPositionHorizontal m_ePositionHorizontal;
    eChatPositionVertical   m_ePositionVertical;
    eChatTextAlign          m_eTextAlign;

    CGUI*      m_pManager;
    CGUIFont*  m_pFont;
    LPD3DXFONT m_pDXFont;

    CVector2D m_vecBackgroundPosition;
    CVector2D m_vecBackgroundSize;
    CVector2D m_vecInputPosition;
    CVector2D m_vecInputSize;

    CGUITexture*     m_pBackgroundTexture;
    CGUITexture*     m_pInputTexture;
    CGUIStaticImage* m_pBackground;
    CGUIStaticImage* m_pInput;

    std::string m_strInputText;
    std::string m_strCommand;

    // Contains a saved copy of initial input text when navigating history entries
    std::string m_strSavedInputText;

    CEntryHistory* m_pInputHistory = new CEntryHistory(CHAT_INPUT_HISTORY_LENGTH);
    int            m_iSelectedInputHistoryEntry;

    bool  m_bVisible;
    bool  m_bInputBlocked;
    bool  m_bInputVisible;
    int   m_iScrollingBack;                    // Non zero if currently scrolling back
    float m_fCssStyleOverrideAlpha;            // For fading out 'CssStyle' effect. (When entering text or scrolling back)
    float m_fBackgroundAlpha;
    float m_fInputBackgroundAlpha;

    unsigned int  m_uiNumLines;
    CColor        m_Color;
    CColor        m_TextColor;
    CColor        m_InputColor;
    CColor        m_InputTextColor;
    bool          m_bCssStyleText;
    bool          m_bCssStyleBackground;
    bool          m_bTextBlackOutline;
    unsigned long m_ulChatLineLife;
    unsigned long m_ulChatLineFadeOut;
    bool          m_bUseCEGUI;
    CVector2D     m_vecScale;
    float         m_fNativeWidth;
    float         m_fRcpUsingDxFontScale;

    bool m_bCanChangeWidth;
    int  m_iCVarsRevision;

    SDrawList          m_PrevDrawList;
    CRenderTargetItem* m_pCacheTexture;
    int                m_iCacheTextureRevision;

    CVector2D  m_RenderTargetChatSize;
    int        m_iReportCount;
    CTickCount m_lastRenderTargetCreationFail;

    bool m_bNickCompletion;

    int                         m_iCharacterLimit;
    static inline constexpr int m_iDefaultCharacterLimit = 96;
    static inline constexpr int m_iMaxCharacterLimit = 255;
    static inline constexpr int m_iMaxInputLines = 5;
};
