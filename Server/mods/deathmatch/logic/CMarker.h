/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMarker.h
 *  PURPOSE:     Marker entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "CColCallback.h"
#include "CColManager.h"
#include "CColShape.h"
#include "CEvents.h"
#include "CPerPlayerEntity.h"

class CMarker : public CPerPlayerEntity, private CColCallback
{
    friend class CMarkerManager;

public:
    enum
    {
        TYPE_CHECKPOINT,
        TYPE_RING,
        TYPE_CYLINDER,
        TYPE_ARROW,
        TYPE_CORONA,
        TYPE_INVALID = 0xFF,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
        ICON_INVALID,
    };

public:
    CMarker(class CMarkerManager* pMarkerManager, CColManager* pColManager, CElement* pParent);
    ~CMarker(void);
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    void Unlink(void);

    bool           HasTarget(void) { return m_bHasTarget; };
    const CVector& GetTarget(void) { return m_vecTarget; };
    unsigned char  GetMarkerType(void) { return m_ucType; };
    float          GetSize(void) { return m_fSize; };
    SColor         GetColor(void) const { return m_Color; };
    unsigned char  GetIcon(void) { return m_ucIcon; }

    void SetPosition(const CVector& vecPosition);
    void SetTarget(const CVector* pTargetVector);
    void SetMarkerType(unsigned char ucType);
    void SetSize(float fSize);
    void SetColor(const SColor color);

    void SetIcon(unsigned char ucIcon);

    CColShape* GetColShape(void) { return m_pCollision; }

    virtual CSphere GetWorldBoundingSphere(void);

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    void Callback_OnCollision(CColShape& Shape, CElement& Element);
    void Callback_OnLeave(CColShape& Shape, CElement& Element);
    void Callback_OnCollisionDestroy(CColShape* pShape);

    void UpdateCollisionObject(unsigned char ucOldType);

    class CMarkerManager* m_pMarkerManager;
    CColManager*          m_pColManager;
    bool                  m_bHasTarget;
    CVector               m_vecTarget;
    unsigned char         m_ucType;
    float                 m_fSize;
    SColor                m_Color;
    unsigned char         m_ucIcon;

    CColShape* m_pCollision;
};
