/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CModelInfo.h
*  PURPOSE:		Entity model info interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MODELINFO
#define __CGAME_MODELINFO

#include "Common.h"
#include "RenderWare.h"
#include <CVector.h>

#include <windows.h>
#include "CColModel.h"

class CBoundingBox
{
public:
	CVector vecBoundMin;
	CVector vecBoundMax;
	CVector vecBoundOffset;
	float	fRadius;
};

enum eVehicleUpgradePosn
{
    VEHICLE_UPGRADE_POSN_BONET = 0,
    VEHICLE_UPGRADE_POSN_VENT,
    VEHICLE_UPGRADE_POSN_SPOILER,
    VEHICLE_UPGRADE_POSN_SIDESKIRT,
    VEHICLE_UPGRADE_POSN_FRONT_BULLBARS,
    VEHICLE_UPGRADE_POSN_REAR_BULLBARS,
    VEHICLE_UPGRADE_POSN_HEADLIGHTS,
    VEHICLE_UPGRADE_POSN_ROOF,
    VEHICLE_UPGRADE_POSN_NITRO,
    VEHICLE_UPGRADE_POSN_HYDRAULICS,
    VEHICLE_UPGRADE_POSN_STEREO,
    VEHICLE_UPGRADE_POSN_UNKNOWN,
    VEHICLE_UPGRADE_POSN_WHEELS,
    VEHICLE_UPGRADE_POSN_EXHAUST,
    VEHICLE_UPGRADE_POSN_FRONT_BUMPER,
    VEHICLE_UPGRADE_POSN_REAR_BUMPER,
    VEHICLE_UPGRADE_POSN_MISC,
};

#include "CPedModelInfo.h"

class CModelInfo
{
public:
    virtual class CBaseModelInfoSAInterface *     GetInterface            ( void ) = 0;

	virtual BOOL			IsBoat ( )=0;
	virtual BOOL			IsCar ( )=0;
	virtual BOOL			IsTrain ( )=0;
	virtual BOOL			IsHeli ( )=0;
	virtual BOOL			IsPlane ( )=0;
	virtual BOOL			IsBike ( )=0;
	virtual BOOL			IsFakePlane ( )=0;
	virtual BOOL			IsMonsterTruck ( )=0;
	virtual BOOL			IsQuadBike ( )=0;
	virtual BOOL			IsBmx ( )=0;
	virtual BOOL			IsTrailer ( )=0;
	virtual BOOL			IsVehicle ( )=0;

	virtual char			* GetNameIfVehicle ( )=0;

	virtual VOID			Request ( bool bAndLoad = false, bool bWaitForLoad = false )=0;
	virtual VOID			Remove ( )=0;
	virtual VOID			LoadAllRequestedModels ( )=0;
	virtual BYTE			GetLevelFromPosition ( CVector * vecPosition )=0;
	virtual BOOL			IsLoaded ( )=0;
	virtual BYTE			GetFlags ( )=0;
	virtual CBoundingBox	* GetBoundingBox ( )=0;
	virtual bool			IsValid ( )=0;

	virtual void			AddRef ( bool bWaitForLoad )=0;
	virtual void			RemoveRef ( )=0;
    virtual int             GetRefCount () = 0;

	virtual float			GetDistanceFromCentreOfMassToBaseOfModel ( )=0;

    // ONLY use for CVehicleModelInfos
    virtual short           GetAvailableVehicleMod ( unsigned short usSlot )=0;
    virtual bool            IsUpgradeAvailable ( eVehicleUpgradePosn posn )=0;
    virtual void            SetCustomCarPlateText ( char * szText )=0;
    virtual unsigned int    GetNumRemaps ( void )=0;

    // ONLY use for CPedModelInfos
    virtual void            SetPedVoice ( eVoiceGens VoiceGen, char szVoiceBankFirst[PED_VOICE_BANK_LENGTH], char szVoiceBankLast[PED_VOICE_BANK_LENGTH] ) = 0;
    virtual void            SetPedAudioType ( const char *szPedAudioType ) = 0;

    // ONLY use for upgrade models
    virtual void            RequestVehicleUpgrade ( void )=0;

	// Custom collision related functions
	virtual void			SetCustomModel ( RpClump* pClump )=0;
	virtual void			RestoreOriginalModel ( void )=0;
	virtual void			SetColModel	( CColModel* pColModel )=0;
	virtual void			RestoreColModel	( void )=0;

    // Call this to make sure the custom vehicle models are being used after a load.
    virtual void            MakeCustomModel ( void ) = 0;
    virtual RwObject*       GetRwObject ( void ) = 0;
};

#endif