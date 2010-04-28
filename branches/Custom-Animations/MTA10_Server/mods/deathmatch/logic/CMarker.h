/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMarker.h
*  PURPOSE:     Marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMARKER_H
#define __CMARKER_H

#include <CVector.h>
#include "CColCallback.h"
#include "CColManager.h"
#include "CColShape.h"
#include "CEvents.h"
#include "CPerPlayerEntity.h"
#include "Utils.h"

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
                            CMarker                 ( class CMarkerManager* pMarkerManager, CColManager* pColManager, CElement* pParent, CXMLNode* pNode );
                            ~CMarker                ( void );

    void                    Unlink                  ( void );
    bool                    ReadSpecialData         ( void );

    inline bool             IsNear                  ( const CVector& vecPosition, float fDistance ) { return IsPointNearPoint2D ( m_vecPosition, vecPosition, fDistance ); };
    inline bool             HasTarget               ( void )                                        { return m_bHasTarget; };
    inline const CVector&   GetTarget               ( void )                                        { return m_vecTarget; };
    inline unsigned char    GetMarkerType           ( void )                                        { return m_ucType; };
    inline float            GetSize                 ( void )                                        { return m_fSize; };
    inline SColor           GetColor                ( void ) const                                  { return m_Color; };
    inline unsigned char    GetIcon                 ( void )                                        { return m_ucIcon; }
    
    void                    SetPosition             ( const CVector& vecPosition );
    void                    SetTarget               ( const CVector* pTargetVector );
    void                    SetMarkerType           ( unsigned char ucType );
    void                    SetSize                 ( float fSize );
    void                    SetColor                ( const SColor color );

    void                    SetIcon                 ( unsigned char ucIcon );

    inline CColShape*       GetColShape             ( void )    { return m_pCollision; }

private:
    void                    Callback_OnCollision    ( CColShape& Shape, CElement& Element );
    void                    Callback_OnLeave        ( CColShape& Shape, CElement& Element );
    void                    Callback_OnCollisionDestroy ( CColShape* pShape );

    void                    UpdateCollisionObject   ( unsigned char ucOldType );

    class CMarkerManager*   m_pMarkerManager;
    CColManager*            m_pColManager;
    bool                    m_bHasTarget;
    CVector                 m_vecTarget;
    unsigned char           m_ucType;
    float                   m_fSize;
    SColor                  m_Color;
    unsigned char           m_ucIcon;

    CColShape*              m_pCollision;
};

#endif
