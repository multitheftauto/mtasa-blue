/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CHud.h
*  PURPOSE:     HUD interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_HUD
#define __CGAME_HUD

#include <windows.h>
#include <CVector.h>

enum eHudComponent
{
    // Order must be preserved for net comms
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
    HUD_VITAL_STATS,
    HUD_HELP_TEXT,
};

class CHud
{
public:
    virtual VOID                SetHelpMessage( char * szMessage )=0;
    virtual VOID                SetBigMessage ( char * szBigMessage )=0;
    virtual VOID                Disable ( bool bDisabled )=0;
    virtual bool                IsDisabled ( void )=0;
    //virtual VOID                DrawBarChart ( float fX, float fY, DWORD dwWidth, DWORD dwHeight, float fPercentage, DWORD dwForeColor, DWORD dwBorderColor )=0;
    virtual bool                CalcScreenCoors ( CVector * vecPosition1, CVector * vecPosition2, float * fX, float * fY, bool bSetting1, bool bSetting2 )=0;
    // Do not use - Seems to changes render/internal states
    //virtual void                Draw2DPolygon ( float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, float fX4, float fY4, DWORD dwColor )=0;
//  virtual VOID                SetVehicleName( char * szName )=0;
    //virtual VOID              SetZoneName( char * szName )=0;
    virtual void                SetComponentVisible ( eHudComponent component, bool bVisible )=0;
    virtual bool                IsComponentVisible ( eHudComponent component )=0;
    virtual void                AdjustComponents ( void ) = 0;
};

#endif
