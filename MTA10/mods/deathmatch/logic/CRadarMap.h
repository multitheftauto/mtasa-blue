/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarMap.h
*  PURPOSE:     Header for radar map class
*  DEVELOPERS:  Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRADARMAP_H
#define __CRADARMAP_H

#include "CClientCommon.h"
#include <CClientManager.h>
#include <CClientTextDisplay.h>
#include <gui/CGUI.h>

#define RADAR_TEXTURE_WIDTH 1152
#define RADAR_TEXTURE_HEIGHT 1152

class CRadarMap
{
public:
                                        CRadarMap                           ( class CClientManager* pManager );
    virtual                             ~CRadarMap                          ( void );

    void                                DoPulse                             ( void );

    bool                                IsRadarShowing                      ( void );

    inline bool                         GetRadarEnabled                     ( void ) const                  { return m_bIsRadarEnabled; };
    void                                SetRadarEnabled                     ( bool bIsRadarEnabled );

    inline bool                         GetForcedState                      ( void ) const                  { return m_bForcedState; }
    void                                SetForcedState                      ( bool bState );

	bool                                GetBoundingBox                      ( CVector &vecMin, CVector &vecMax );

    int                                 GetRadarAlpha                       ( void ) const                  { return m_iRadarAlpha; }
    void                                SetRadarAlpha                       ( int iRadarAlpha );

protected:
    void                                InternalSetRadarEnabled             ( bool bEnabled );
public:

    inline bool                         IsAttachedToLocalPlayer             ( void ) const                  { return m_bIsAttachedToLocal; };
    void                                SetAttachedToLocalPlayer            ( bool bIsAttachedToLocal );

    inline bool                         IsMovingNorth                       ( void ) const                  { return m_bIsMovingNorth; };
    inline void                         SetMovingNorth                      ( bool bIsMovingNorth )         { m_bIsMovingNorth = bIsMovingNorth; };

    inline bool                         IsMovingSouth                       ( void ) const                  { return m_bIsMovingSouth; };
    inline void                         SetMovingSouth                      ( bool bIsMovingSouth )         { m_bIsMovingSouth = bIsMovingSouth; };

    inline bool                         IsMovingEast                        ( void ) const                  { return m_bIsMovingEast; };
    inline void                         SetMovingEast                       ( bool bIsMovingEast )          { m_bIsMovingEast = bIsMovingEast; };

    inline bool                         IsMovingWest                        ( void ) const                  { return m_bIsMovingWest; };
    inline void                         SetMovingWest                       ( bool bIsMovingWest )          { m_bIsMovingWest = bIsMovingWest; };

    void                                ZoomIn                              ( void );
    void                                ZoomOut                             ( void );

private:
    bool                                CalculateEntityOnScreenPosition     ( class CClientEntity* pEntity, CVector2D& vecLocalPos );
    bool                                CalculateEntityOnScreenPosition     ( CVector vecPosition, CVector2D& vecLocalPos );
    void                                SetupMapVariables                   ( void );

    void                                MoveNorth                           ( void );
    void                                MoveSouth                           ( void );
    void                                MoveEast                            ( void );
    void                                MoveWest                            ( void );

private:
    class CClientManager*               m_pManager;
    class CClientRadarMarkerManager*    m_pRadarMarkerManager;
    class CClientRadarAreaManager*      m_pRadarAreaManager;

    IDirect3DTexture9*                  m_pRadarImage;

    IDirect3DTexture9*                  m_pLocalPlayerBlip;

    unsigned int                        m_uiHeight;
    unsigned int                        m_uiWidth;
    
    float                               m_fMapSize;
    int                                 m_iMapMinX;
    int                                 m_iMapMaxX;
    int                                 m_iMapMinY;
    int                                 m_iMapMaxY;

    int                                 m_iHorizontalMovement;
    int                                 m_iVerticalMovement;

    int                                 m_iRadarAlpha;

    unsigned char                       m_ucZoom;

    bool                                m_bIsRadarEnabled;
    bool                                m_bForcedState;
    bool                                m_bIsAttachedToLocal;

    bool                                m_bIsMovingNorth;
    bool                                m_bIsMovingSouth;
    bool                                m_bIsMovingEast;
    bool                                m_bIsMovingWest;

    unsigned long                       m_ulUpdateTime;

    CClientTextDisplay*                 m_pModeText;
    CClientTextDisplay*                 m_pHelpTextZooming;
    CClientTextDisplay*                 m_pHelpTextMovement;
    CClientTextDisplay*                 m_pHelpTextAttachment;

    bool                                m_bHudVisible;
    bool                                m_bChatVisible;
    bool                                m_bRadarVisible;
    bool                                m_bDebugVisible;
    bool                                m_bTextVisible;
};

#endif