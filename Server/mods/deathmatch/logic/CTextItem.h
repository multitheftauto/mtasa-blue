/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTextItem.h
 *  PURPOSE:     Text display item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CTextItem;

#pragma once

#include <CVector2D.h>
#include "CTextDisplay.h"
#include <list>

enum eTextPriority
{
    PRIORITY_LOW = 0,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
};

class CTextItem
{
    friend class CTextDisplay;
    friend class CPlayerTextManager;

public:
    CTextItem(const char* szText, const CVector2D& vecPosition, eTextPriority Priority = PRIORITY_LOW, const SColor color = -1, float fScale = 1.0f,
              unsigned char ucFormat = 0, unsigned char ucShadowAlpha = 0);
    CTextItem(const CTextItem& TextItem);
    ~CTextItem();

    bool operator=(const CTextItem& TextItem);

    void           SetText(const char* szText);
    const SString& GetText() { return m_strText; };

    const CVector2D& GetPosition() { return m_vecPosition; };
    void             SetPosition(const CVector2D& vecPosition);

    void   SetColor(const SColor color);
    SColor GetColor() const { return m_Color; }

    float GetScale() { return m_fScale; };
    void  SetScale(float fScale);

    unsigned char GetFormat() { return m_ucFormat; }
    void          SetFormat(unsigned char ucFormat) { m_ucFormat = ucFormat; }

    void          SetPriority(eTextPriority Priority) { m_Priority = Priority; };
    eTextPriority GetPriority() { return m_Priority; };

    unsigned long GetUniqueID() { return m_ulUniqueId; };
    bool          IsBeingDeleted() { return m_bDeletable; };
    uint          GetScriptID() const { return m_uiScriptID; }

private:
    SString       m_strText;
    CVector2D     m_vecPosition;
    SColor        m_Color;
    float         m_fScale;
    unsigned char m_ucFormat;
    unsigned char m_ucShadowAlpha;
    unsigned long m_ulUniqueId;
    eTextPriority m_Priority;
    bool          m_bDeletable;
    uint          m_uiScriptID;

    std::list<CTextDisplay*> m_Observers;

    void AddObserver(CTextDisplay* pObserver);
    void RemoveObserver(CTextDisplay* pObserver) { m_Observers.remove(pObserver); };
    void NotifyObservers();
};
