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
    void                    GetColor                ( unsigned char & R, unsigned char & G, unsigned char & B, unsigned char & A );
    inline unsigned long    GetColor                ( void )                                        { return m_ulColor; };
    inline unsigned char    GetColorRed             ( void )                                        { return static_cast < unsigned char > ( m_ulColor ); };
    inline unsigned char    GetColorGreen           ( void )                                        { return static_cast < unsigned char > ( m_ulColor >> 8 ); };
    inline unsigned char    GetColorBlue            ( void )                                        { return static_cast < unsigned char > ( m_ulColor >> 16 ); };
    inline unsigned char    GetColorAlpha           ( void )                                        { return static_cast < unsigned char > ( m_ulColor >> 24 ); };
    inline unsigned char    GetIcon                 ( void )                                        { return m_ucIcon; }
    
    void                    SetPosition             ( const CVector& vecPosition );
    void                    SetTarget               ( const CVector* pTargetVector );
    void                    SetMarkerType           ( unsigned char ucType );
    void                    SetSize                 ( float fSize );
    void                    SetColor                ( unsigned long ulColor );
    void                    SetColor                ( unsigned char ucRed,
                                                      unsigned char ucGreen,
                                                      unsigned char ucBlue,
                                                      unsigned char ucAlpha );

    void                    SetIcon                 ( unsigned char ucIcon );

    inline CColShape*       GetColShape             ( void )    { return m_pCollision; }

private:
    void                    Callback_OnCollision    ( CColShape& Shape, CElement& Element );
    void                    Callback_OnLeave        ( CColShape& Shape, CElement& Element );

    void                    UpdateCollisionObject   ( unsigned char ucOldType );

    class CMarkerManager*         m_pMarkerManager;
    CColManager*            m_pColManager;
    bool                    m_bHasTarget;
    CVector                 m_vecTarget;
    unsigned char           m_ucType;
    float                   m_fSize;
    unsigned long           m_ulColor;
    unsigned char           m_ucIcon;

    CColShape*              m_pCollision;
};

#endif
