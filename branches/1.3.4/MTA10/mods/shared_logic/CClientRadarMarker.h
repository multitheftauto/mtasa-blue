/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarker.h
*  PURPOSE:     Radar marker entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

class CClientRadarMarker;

#ifndef __CCLIENTRADARMARKER_H
#define __CCLIENTRADARMARKER_H

#include "CClientCommon.h"
#include "CClientEntity.h"
#include "CClientRadarMarkerManager.h"
#include <gui/CGUI.h>

#include <game/CMarker.h>

#define RADAR_MARKER_LIMIT 63


class CClientRadarMarker : public CClientEntity
{
    DECLARE_CLASS( CClientRadarMarker, CClientEntity )
    friend class CClientRadarMarkerManager;

public:
                                        CClientRadarMarker              ( class CClientManager* pManager, ElementID ID, short usOrdering = 0, unsigned short usVisibleDistance = 16383 );
                                        ~CClientRadarMarker             ( void );

    void                                Unlink                          ( void );

    inline eClientEntityType            GetType                         ( void ) const                  { return CCLIENTRADARMARKER; };

    void                                DoPulse                         ( void );

    void                                SetPosition                     ( const CVector& vecPosition );
    void                                GetPosition                     ( CVector& vecPosition ) const;

    inline unsigned short               GetScale                        ( void )                        { return m_usScale; };
    void                                SetScale                        ( unsigned short usScale );

    inline SColor                       GetColor                        ( void ) const                  { return m_Color; }
    void                                SetColor                        ( const SColor color );

    inline unsigned long                GetSprite                       ( void ) const                  { return m_ulSprite; };
    void                                SetSprite                       ( unsigned long ulSprite );

    inline bool                         IsVisible                       ( void ) const                  { return m_pMarker != NULL && m_bIsVisible; };
    void                                SetVisible                      ( bool bVisible );

    void                                SetDimension                    ( unsigned short usDimension );
    void                                RelateDimension                 ( unsigned short usDimension );

    inline short                        GetOrdering                     ( void )                        { return m_sOrdering; }
    void                                SetOrdering                     ( short sOrdering );

    inline unsigned short               GetVisibleDistance              ( void )                        { return m_usVisibleDistance; }
    inline void                         SetVisibleDistance              ( unsigned short usVisibleDistance ) { m_usVisibleDistance = usVisibleDistance; }

    bool                                IsInVisibleDistance             ( void );

private:
    bool                                Create                          ( void );
    void                                InternalCreate                  ( void );
    void                                Destroy                         ( void );    

    void                                CreateMarker                    ( void );
    void                                DestroyMarker                   ( void );

    class CClientRadarMarkerManager*    m_pRadarMarkerManager;
    CMarker*                            m_pMarker;

    CVector                             m_vecPosition;
    unsigned short                      m_usScale;
    SColor                              m_Color;
    unsigned long                       m_ulSprite;

    bool                                m_bIsVisible;
    short                               m_sOrdering;
    unsigned short                      m_usVisibleDistance;
};

#endif
