/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CAutomobile.h
*  PURPOSE:		Automobile vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_AUTOMOBILE
#define __CGAME_AUTOMOBILE

#include "CDamageManager.h"
#include "CDoor.h"
#include "Common.h"
#include "CVehicle.h"

class CAutomobile : public virtual CVehicle
{
public:
    virtual                         ~CAutomobile ( void ) {};

    virtual bool                    BurstTyre ( DWORD dwTyreID ) = 0;
    virtual bool                    BreakTowLink ( void ) = 0;
    virtual void                    BlowUpCar ( CEntity* pEntity ) = 0;
    virtual void                    BlowUpCarsInPath ( void ) = 0;
    virtual void                    CloseAllDoors ( void ) = 0;
    virtual void                    CloseBoot ( void ) = 0;
    virtual float                   FindWheelWidth ( bool bUnknown ) = 0;
   // virtual void                    Fix ( void ) = 0;
    virtual void                    FixDoor ( int iCarNodeIndex, eDoorsSA Door ) = 0;
    virtual int                     FixPanel ( int iCarNodeIndex, ePanelsSA Panel ) = 0;
    virtual bool                    GetAllWheelsOffGround ( void ) = 0;
    virtual float                   GetCarPitch ( void ) = 0;
    virtual float                   GetCarRoll ( void ) = 0;
    virtual void                    GetComponentWorldPosition ( int iComponentID, CVector* pVector) = 0;
    //virtual float                   GetHeightAboveRoad ( void ) = 0;  /* TODO */
    virtual DWORD                   GetNumContactWheels ( void ) = 0;
    virtual float                   GetRearHeightAboveRoad ( void ) = 0;
    virtual bool                    IsComponentPresent ( int iComponentID ) = 0;
    virtual bool                    IsDoorClosed ( eDoorsSA Door ) = 0;
    virtual bool                    IsDoorFullyOpen ( eDoorsSA Door ) = 0;
    virtual bool                    IsDoorMissing ( eDoorsSA Door ) = 0;
    virtual bool                    IsDoorReady ( eDoorsSA Door ) = 0;
    virtual bool                    IsInAir ( void ) = 0;
    virtual bool                    IsOpenTopCar ( void ) = 0;
    //virtual void                    PlaceOnRoadProperly ( void ) = 0;
    virtual void                    PlayCarHorn ( void ) = 0;
    virtual void                    PopBoot ( void ) = 0;
    virtual void                    PopBootUsingPhysics ( void ) = 0;
    virtual void                    PopDoor ( int iCarNodeIndex, eDoorsSA Door, bool bUnknown ) = 0;
    virtual void                    PopPanel ( int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast ) = 0;
    virtual void                    ResetSuspension ( void ) = 0;
    virtual void                    SetRandomDamage ( bool bUnknown ) = 0;
    virtual void                    SetTaxiLight ( bool bState ) = 0;
    virtual void                    SetTotalDamage ( bool bUnknown ) = 0;
    virtual void                    SetTowLink ( CVehicle* pVehicle, bool bRemoveAdd ) = 0;
    virtual CPhysical*              SpawnFlyingComponent ( int iCarNodeIndex, int iUnknown ) = 0;

	virtual CDoor					* GetDoor(eDoors doorID) = 0;
};

#endif