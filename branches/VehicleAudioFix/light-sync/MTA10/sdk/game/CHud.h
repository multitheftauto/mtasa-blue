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

class CHud
{
public:
    virtual VOID                SetHelpMessage( char * szMessage )=0;
    virtual VOID                SetBigMessage ( char * szBigMessage )=0;
    virtual VOID                Disable ( bool bDisabled )=0;
    //virtual VOID                DrawBarChart ( float fX, float fY, DWORD dwWidth, DWORD dwHeight, float fPercentage, DWORD dwForeColor, DWORD dwBorderColor )=0;
    virtual bool                CalcScreenCoors ( CVector * vecPosition1, CVector * vecPosition2, float * fX, float * fY, bool bSetting1, bool bSetting2 )=0;
    // Do not use - Seems to changes render/internal states
    //virtual void                Draw2DPolygon ( float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, float fX4, float fY4, DWORD dwColor )=0;
//  virtual VOID                SetVehicleName( char * szName )=0;
    //virtual VOID              SetZoneName( char * szName )=0;

    virtual void                DisableAmmo ( bool bDisabled )=0;
    virtual void                DisableWeaponIcon ( bool bDisabled )=0;
    virtual void                DisableHealth ( bool bDisabled )=0;
    virtual void                DisableBreath ( bool bDisabled )=0;
    virtual void                DisableArmour ( bool bDisabled )=0;
    virtual void                DisableVitalStats ( bool bDisabled )=0;
    virtual void                DisableMoney ( bool bDisabled )=0;
    virtual void                DisableVehicleName ( bool bDisabled )=0;
    virtual void                DisableHelpText ( bool bDisabled )=0;
    virtual void                DisableAreaName ( bool bDisabled )=0;
    virtual void                DisableRadar ( bool bDisabled )=0;
    virtual void                DisableClock ( bool bDisabled )=0;
    virtual void                DisableRadioName ( bool bDisabled )=0;
    virtual void                DisableWantedLevel ( bool bDisabled )=0;
    virtual void                DisableCrosshair ( bool bDisabled )=0;
    virtual void                DisableAll ( bool bDisabled )=0;
};

#endif
