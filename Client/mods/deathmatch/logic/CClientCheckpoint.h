/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCheckpoint.h
 *  PURPOSE:     Checkpoint marker entity class
 *
 *****************************************************************************/

#pragma once

#include "CClientMarkerCommon.h"
#include <list>

class CClientMarker;
class CCheckpoint;

class CClientCheckpoint : public CClientMarkerCommon
{
    friend class CClientMarkerManager;

public:
    enum
    {
        TYPE_NORMAL,
        TYPE_RING,
        TYPE_INVALID,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
    };

    CClientCheckpoint(CClientMarker* pThis);
    ~CClientCheckpoint();

    unsigned int GetMarkerType() const { return CClientMarkerCommon::CLASS_CHECKPOINT; };

    unsigned long GetCheckpointType() const;
    void          SetCheckpointType(unsigned long ulType);

    bool IsHit(const CVector& vecPosition) const;

    void GetPosition(CVector& vecPosition) const { vecPosition = m_Matrix.vPos; };
    void SetPosition(const CVector& vecPosition);
    void SetDirection(const CVector& vecDirection);
    void SetNextPosition(const CVector& vecPosition);

    void GetMatrix(CMatrix& mat);
    void SetMatrix(CMatrix& mat);

    bool IsVisible() const { return m_bVisible; }
    void SetVisible(bool bVisible);

    unsigned int GetIcon() const { return m_uiIcon; }
    void         SetIcon(unsigned int uiIcon);

    SColor GetColor() const { return m_Color; }
    void   SetColor(const SColor& color);

    float GetSize() const { return m_fSize; };
    void  SetSize(float fSize);

    bool HasTarget() { return m_bHasTarget; }
    void SetHasTarget(bool bHasTarget) { m_bHasTarget = bHasTarget; }
    void GetTarget(CVector& vecTarget) { vecTarget = m_vecTarget; }
    void SetTarget(const CVector& vecTarget) { m_vecTarget = vecTarget; }

    static unsigned char StringToIcon(const char* szString);
    static bool          IconToString(unsigned char ucIcon, SString& strOutString);
    void                 ReCreateWithSameIdentifier();

protected:
    bool IsStreamedIn() { return m_bStreamedIn; };
    void StreamIn();
    void StreamOut();

private:
    void Create(unsigned long ulIdentifier = 0);
    void Destroy();
    void ReCreate();

    CClientMarkerPtr m_pThis;
    bool             m_bStreamedIn;
    CMatrix          m_Matrix;
    CVector          m_vecDirection;
    CVector          m_vecTargetPosition;
    bool             m_bVisible;
    unsigned int     m_uiIcon;
    DWORD            m_dwType;
    float            m_fSize;
    SColor           m_Color;
    CCheckpoint*     m_pCheckpoint;

    DWORD   m_dwIdentifier;
    bool    m_bHasTarget;
    CVector m_vecTarget;
};
