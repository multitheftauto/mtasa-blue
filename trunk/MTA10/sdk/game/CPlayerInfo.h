/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPlayerInfo.h
*  PURPOSE:     Player entity information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PLAYERINFO
#define __CGAME_PLAYERINFO

#include "Common.h"
#include "CWanted.h"
#include "CVehicle.h"

class CPlayerInfo
{
public:
    virtual long            GetPlayerMoney                  ( void ) = 0;
    virtual void            SetPlayerMoney                  ( long lMoney ) = 0;

    virtual void            GivePlayerParachute             ( void ) = 0;
    virtual void            StreamParachuteWeapon           ( bool bAllowParachute ) = 0;

    virtual short           GetLastTimeEaten                ( void ) = 0;
    virtual void            SetLastTimeEaten                ( short sTime ) = 0;

    virtual CWanted*        GetWanted                       ( void ) = 0;
    virtual float           GetFPSMoveHeading               ( void ) = 0;

    virtual void            GetCrossHair                    ( bool &bActivated, float &fTargetX, float &fTargetY ) = 0;

    virtual bool            GetDoesNotGetTired              ( void ) = 0;
    virtual void            SetDoesNotGetTired              ( bool bDoesNotGetTired ) = 0;

    virtual CVehicle*       GivePlayerRemoteControlledCar   ( eVehicleTypes vehicletype ) = 0;

    virtual DWORD           GetLastTimeBigGunFired ( void ) = 0;
    virtual void            SetLastTimeBigGunFired ( DWORD dwTime ) = 0;

    virtual DWORD           GetCarTwoWheelCounter       ( void ) = 0;
    virtual float           GetCarTwoWheelDist          ( void ) = 0;
    virtual DWORD           GetCarLess3WheelCounter     ( void ) = 0;
    virtual DWORD           GetBikeRearWheelCounter     ( void ) = 0;
    virtual float           GetBikeRearWheelDist        ( void ) = 0;
    virtual DWORD           GetBikeFrontWheelCounter    ( void ) = 0;
    virtual float           GetBikeFrontWheelDist       ( void ) = 0;
};

#endif
