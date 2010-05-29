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

#ifdef COMPILE_FOR_VC
    #include <game/CMarker.h>
#else if COMPILE_FOR_SA
    #include <game/CMarker.h>
#endif

#define MAP_MARKER_WIDTH 19
#define MAP_MARKER_HEIGHT 20
#define RADAR_MARKER_LIMIT 63


class CClientRadarMarker : public CClientEntity
{
    friend CClientRadarMarkerManager;

public:
    enum EMapMarkerState
    {
        MAP_MARKER_SQUARE,
        MAP_MARKER_TRIANGLE_UP,
        MAP_MARKER_TRIANGLE_DOWN,
        MAP_MARKER_OTHER,
    };

public:
                                        CClientRadarMarker              ( class CClientManager* pManager, ElementID ID, short usOrdering = 0, float fVisibleDistance = 99999.0f );
                                        ~CClientRadarMarker             ( void );

    void                                Unlink                          ( void );

    inline eClientEntityType            GetType                         ( void ) const                  { return CCLIENTRADARMARKER; };

    void                                DoPulse                         ( void );

    inline bool                         IsVisible                       ( void )                        { return m_bIsVisible; }

    void                                SetPosition                     ( const CVector& vecPosition );
    void                                GetPosition                     ( CVector& vecPosition ) const;

    inline unsigned short               GetScale                        ( void )                        { return m_usScale; };
    void                                SetScale                        ( unsigned short usScale );

    inline SColor                       GetColor                        ( void ) const                  { return m_Color; }
    void                                SetColor                        ( const SColor color );

    inline unsigned long                GetSprite                       ( void ) const                  { return m_ulSprite; };
    void                                SetSprite                       ( unsigned long ulSprite );

    inline bool                         IsVisible                       ( void ) const                  { return  m_pMarker != NULL; };
    void                                SetVisible                      ( bool bVisible );

    inline IDirect3DTexture9*           GetMapMarkerImage               ( void )                        { return m_pMapMarkerImage; };
    inline EMapMarkerState              GetMapMarkerState               ( void )                        { return m_eMapMarkerState; };
    void                                SetMapMarkerState               ( EMapMarkerState eMapMarkerState );

    void                                SetDimension                    ( unsigned short usDimension );
    void                                RelateDimension                 ( unsigned short usDimension );

    inline short                        GetOrdering                     ( void )                        { return m_sOrdering; }
    void                                SetOrdering                     ( short sOrdering );

    bool                                IsInVisibleDistance             ( void );

private:
    void                                GetSquareTexture                ( DWORD dwBitMap[] );
    void                                GetUpTriangleTexture            ( DWORD dwBitMap[] );
    void                                GetDownTriangleTexture          ( DWORD dwBitMap[] );

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

    IDirect3DTexture9*                  m_pMapMarkerImage;
    EMapMarkerState                     m_eMapMarkerState;

    bool                                m_bIsVisible;
    short                               m_sOrdering;
    float                               m_fVisibleDistance;
};

#endif
