/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicle.h
*  PURPOSE:     Vehicle entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

enum eClientVehicleUpgrade;
enum eClientVehicleType;
struct CClientVehicleProperties;
class CClientVehicle;

#ifndef __CCLIENTVEHICLE_H
#define __CCLIENTVEHICLE_H

#ifdef COMPILE_FOR_VC
    #include <game/CVehicle.h>
#else if COMPILE_FOR_SA
    #include <game/CPlane.h>
    #include <game/CVehicle.h>
#endif

#include "CClientCommon.h"
#include "CClientCamera.h"
#include "CClientModelRequestManager.h"
#include "CClientPed.h"
#include "CClientStreamElement.h"
#include "CClientVehicleManager.h"
#include "CVehicleUpgrades.h"
#include "CClientTime.h"

#define INVALID_PASSENGER_SEAT 0xFF
#define DEFAULT_VEHICLE_HEALTH 1000
#define MAX_VEHICLE_HEALTH 10000

enum eClientVehicleType
{
    CLIENTVEHICLE_NONE,
    CLIENTVEHICLE_CAR,
    CLIENTVEHICLE_BOAT,
    CLIENTVEHICLE_TRAIN,
    CLIENTVEHICLE_HELI,
    CLIENTVEHICLE_PLANE,
    CLIENTVEHICLE_BIKE,
    CLIENTVEHICLE_MONSTERTRUCK,
    CLIENTVEHICLE_QUADBIKE,
    CLIENTVEHICLE_BMX,
    CLIENTVEHICLE_TRAILER
};

enum eDelayedSyncVehicleData
{
    DELAYEDSYNC_VEHICLE_KEYSYNC,
    DELAYEDSYNC_VEHICLE_ROTATION,
    DELAYEDSYNC_VEHICLE_MOVESPEED,
    DELAYEDSYNC_VEHICLE_TURNSPEED,
};

struct SDelayedSyncVehicleData
{
    unsigned long       ulTime;
    unsigned char       ucType;
    CControllerState    State;
    CVector             vecTarget;
    CVector             vecTarget2;
    CVector             vecTarget3;
};

struct SLastSyncedVehData
{
    CVector             vecPosition;
    CVector             vecRotation;
    CVector             vecMoveSpeed;
    CVector             vecTurnSpeed;
    float               fHealth;
    ElementID           Trailer;
};

class CClientProjectile;

class CClientVehicle : public CClientStreamElement
{
    friend CClientCamera;
    friend CClientPed;
    friend CClientVehicleManager;
    friend class CClientGame; // TEMP HACK

protected: // Use CDeathmatchVehicle constructor for now. Will get removed later when this class is
           // cleaned up.
                                CClientVehicle          ( CClientManager* pManager, ElementID ID, unsigned short usModel );

public:
                                ~CClientVehicle         ( void );
    
    void                        Unlink                  ( void );

    inline eClientEntityType    GetType                 ( void ) const                      { return CCLIENTVEHICLE; };

    void                        GetName                 ( char* szBuf );
    inline const char*          GetNamePointer          ( void )                            { return m_pModelInfo->GetNameIfVehicle (); };
    inline eClientVehicleType   GetVehicleType          ( void )                            { return m_eVehicleType; };

    void                        GetPosition             ( CVector& vecPosition ) const;
    void                        SetPosition             ( const CVector& vecPosition );
    void                        SetRoll                 ( const CVector& vecRoll );
    void                        SetDirection            ( const CVector& vecDir );
    void                        SetWas                  ( const CVector& vecWas );

    void                        GetRotationDegrees      ( CVector& vecRotation ) const;
    void                        GetRotationRadians      ( CVector& vecRotation ) const;
    void                        SetRotationDegrees      ( const CVector& vecRotation );
    void                        SetRotationRadians      ( const CVector& vecRotation );
    
	void                        AttachTo                ( CClientEntity * pEntity );

    float                       GetDistanceFromCentreOfMassToBaseOfModel ( void );

    bool                        GetMatrix               ( CMatrix& Matrix ) const;
    bool                        SetMatrix               ( const CMatrix& Matrix );

   	void                        GetMoveSpeed            ( CVector& vecMoveSpeed ) const;
	void						GetMoveSpeedMeters		( CVector& vecMoveSpeed ) const;
    void                        SetMoveSpeed            ( const CVector& vecMoveSpeed );
	void                        GetTurnSpeed            ( CVector& vecTurnSpeed ) const;
	void                        SetTurnSpeed            ( const CVector& vecTurnSpeed );

    bool                        IsVisible               ( void );
    void                        SetVisible              ( bool bVisible );

    bool                        AreDoorsLocked          ( void );
    void                        SetDoorsLocked          ( bool bLocked );

    bool                        AreDoorsUndamageable    ( void );
    void                        SetDoorsUndamageable    ( bool bUndamageable );

    float                       GetHealth               ( void ) const;
    void                        SetHealth               ( float fHealth );
    void                        Fix                     ( void );
    void                        Blow                    ( bool bAllowMovement = false );
    inline bool                 IsVehicleBlown          ( void ) { return m_bBlown; };

    void                        GetColor                ( unsigned char& ucColor1, unsigned char& ucColor2, unsigned char& ucColor3, unsigned char& ucColor4 );
	void						SetColor				( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 );

	void						GetTurretRotation		( float& fHorizontal, float& fVertical );
    void                        SetTurretRotation       ( float fHorizontal, float fVertical );

    inline unsigned short       GetModel                ( void )                            { return m_usModel; };
    void                        SetModelBlocking        ( unsigned short usModel, bool bLoadImmediately = false );

    bool                        IsEngineBroken          ( void );
    void                        SetEngineBroken         ( bool bEngineBroken );

    bool                        IsEngineOn              ( void );
    void                        SetEngineOn             ( bool bEngineOn );

    bool                        CanBeDamaged            ( void );
    void                        CalcAndUpdateCanBeDamagedFlag     ( void );
    void                        SetScriptCanBeDamaged   ( bool bCanBeDamaged );
    void                        SetSyncUnoccupiedDamage ( bool bCanBeDamaged );

    bool                        GetTyresCanBurst        ( void );
    void                        CalcAndUpdateTyresCanBurstFlag    ( void );

    float                       GetGasPedal             ( void );

    bool                        IsBelowWater            ( void ) const;
    bool                        IsDrowning              ( void ) const;
    bool                        IsDriven                ( void ) const;
    bool                        IsUpsideDown            ( void ) const;
    bool                        IsBlown                 ( void ) const;

    bool                        IsSirenOrAlarmActive    ( void );
    void                        SetSirenOrAlarmActive   ( bool bActive );

    inline bool                 HasLandingGear          ( void )                            { return m_bHasLandingGear; };
    float                       GetLandingGearPosition  ( void );
    void                        SetLandingGearPosition  ( float fPosition );
    bool                        IsLandingGearDown       ( void );
    void                        SetLandingGearDown      ( bool bLandingGearDown );

    inline bool                 HasAdjustableProperty       ( void )                        { return m_bHasAdjustableProperty; };
    unsigned short              GetAdjustablePropertyValue  ( void );
    void                        SetAdjustablePropertyValue  ( unsigned short usAdjustableProperty );

    bool                        HasDamageModel          ( void )                            { return m_bHasDamageModel; }
    unsigned char               GetDoorStatus           ( unsigned char ucDoor );
    unsigned char               GetWheelStatus          ( unsigned char ucWheel );
    unsigned char               GetPanelStatus          ( unsigned char ucPanel );
    unsigned char               GetLightStatus          ( unsigned char ucLight );

    void                        SetDoorStatus           ( unsigned char ucDoor, unsigned char ucStatus );
    void                        SetWheelStatus          ( unsigned char ucWheel, unsigned char ucStatus, bool bSilent = true );
	void						SetPanelStatus			( unsigned char ucPanel, unsigned char ucStatus );
	void						SetLightStatus			( unsigned char ucLight, unsigned char ucStatus );

    // TODO: Make the class remember on virtualization
    float                       GetHelicopterRotorSpeed ( void );
    void                        SetHelicopterRotorSpeed ( float fSpeed );

	void						ReportMissionAudioEvent ( unsigned short usSound );

    inline bool                 IsCollisionEnabled      ( void )                            { return m_bIsCollisionEnabled; };
	void						SetCollisionEnabled     ( bool bCollisionEnabled );

    bool                        GetCanShootPetrolTank   ( void );
    void                        SetCanShootPetrolTank   ( bool bCanShoot );
    
    bool                        GetCanBeTargettedByHeatSeekingMissiles      ( void );
    void                        SetCanBeTargettedByHeatSeekingMissiles      ( bool bEnabled );

    inline unsigned char        GetAlpha                ( void )                            { return m_ucAlpha; }
	void						SetAlpha			    ( unsigned char ucAlpha );

    CClientPed*                 GetOccupant             ( int iSeat = 0 ) const;
    CClientPed*                 GetControllingPlayer    ( void );
    void                        ClearForOccupants       ( void );

	void						PlaceProperlyOnGround   ( void );

    void                        FuckCarCompletely       ( bool bKeepWheels );

    unsigned long               GetMemoryValue          ( unsigned long ulOffset );
    unsigned long               GetGameBaseAddress      ( void );
    void                        WorldIgnore             ( bool bWorldIgnore );

    inline bool                 IsVirtual               ( void )                            { return m_pVehicle == NULL; };

    void                        FadeOut                 ( bool bFadeOut );
    bool                        IsFadingOut             ( void );

    inline bool                 IsFrozen                ( void )                            { return m_bIsFrozen; };
    void                        SetFrozen               ( bool bFrozen );
    void                        SetScriptFrozen         ( bool bFrozen )                    { m_bScriptFrozen = bFrozen; };

    CClientVehicle*             GetPreviousTrainCarriage( void );
    CClientVehicle*             GetNextTrainCarriage    ( void );
    void                        SetPreviousTrainCarriage( CClientVehicle* pPrevious );
    void                        SetNextTrainCarriage    ( CClientVehicle* pNext );

    bool                        IsDerailed              ( void );
    void                        SetDerailed             ( bool bDerailed );
    bool                        IsDerailable            ( void );
    void                        SetDerailable           ( bool bDerailable );

    bool                        GetTrainDirection       ( void );
    void                        SetTrainDirection       ( bool bDirection );

    float                       GetTrainSpeed           ( void );
    void                        SetTrainSpeed           ( float fSpeed );

    inline unsigned char        GetOverrideLights       ( void )                            { return m_ucOverrideLights; }
    void                        SetOverrideLights       ( unsigned char ucOverrideLights );
    bool                        SetTaxiLightOn          ( bool bLightOn );
    bool                        IsTaxiLightOn           ( ) { return m_bTaxiLightOn; }
    inline CVehicle*            GetGameVehicle          ( void )                            { return m_pVehicle; }
    inline CEntity*             GetGameEntity           ( void )                            { return m_pVehicle; }
    inline const CEntity*       GetGameEntity           ( void ) const                      { return m_pVehicle; }
    inline CVehicleUpgrades*    GetUpgrades             ( void )                            { return m_pUpgrades; }
	inline CModelInfo*          GetModelInfo            ( void )                            { return m_pModelInfo; }

    CClientVehicle*             GetTowedVehicle         ( void );
    CClientVehicle*             GetRealTowedVehicle     ( void );
    bool                        SetTowedVehicle         ( CClientVehicle* pVehicle );
    inline CClientVehicle *     GetTowedByVehicle       ( void )                            { return m_pTowedByVehicle; }

    eWinchType                  GetWinchType            ( void )                            { return m_eWinchType; }
    bool                        SetWinchType            ( eWinchType winchType );
    bool                        PickupEntityWithWinch   ( CClientEntity* pEntity );
    bool                        ReleasePickedUpEntityWithWinch ( void );
    void                        SetRopeHeightForHeli    ( float fRopeHeight );
    CClientEntity*              GetPickedUpEntityWithWinch ( void );

    inline const char*          GetRegPlate             ( void )                            { return m_strRegPlate.empty () ? NULL : m_strRegPlate.c_str (); }
    void                        SetRegPlate             ( const char* szPlate );

    unsigned char               GetPaintjob             ( void );
    void                        SetPaintjob             ( unsigned char ucPaintjob );
    
    float                       GetDirtLevel            ( void );
    void                        SetDirtLevel            ( float fDirtLevel );

    void                        SetInWater              ( bool bState )                     { m_bIsInWater = bState; }
    bool                        IsInWater               ( void );
    bool                        IsOnGround              ( void );
    bool                        IsOnWater               ( void );
	void						LockSteering			( bool bLock );

    bool                        IsSmokeTrailEnabled     ( void );
    void                        SetSmokeTrailEnabled    ( bool bEnabled );

    void                        ResetInterpolation      ( void );

    void                        Interpolate             ( void );
    void                        UpdateKeysync           ( void );

    void                        GetInitialDoorStates    ( unsigned char * pucDoorStates );

	void						AddMatrix				( CMatrix& Matrix, double dTime, unsigned short usTickRate );
	void						AddVelocity				( CVector& vecVelocity );

    inline void                 GetTargetPosition       ( CVector& vecPosition )            { vecPosition = m_vecTargetPosition; }
    void                        SetTargetPosition       ( CVector& vecPosition );
    void                        RemoveTargetPosition    ( void );
    inline bool                 HasTargetPosition       ( void )                            { return m_bHasTargetPosition; }

    inline void                 GetTargetRotation       ( CVector& vecRotation )            { vecRotation = m_vecTargetRotation; }
    void                        SetTargetRotation       ( CVector& vecRotation );
    void                        RemoveTargetRotation    ( void );
    inline bool                 HasTargetRotation       ( void )                            { return m_bHasTargetRotation; }

	void						UpdateTargetPosition	( void );
    void						UpdateTargetRotation	( void );

    inline unsigned long        GetIllegalTowBreakTime  ( void )                            { return m_ulIllegalTowBreakTime; }
    inline void                 SetIllegalTowBreakTime  ( unsigned long ulTime )            { m_ulIllegalTowBreakTime = ulTime; }

    void                        GetGravity              ( CVector& vecGravity ) const       { vecGravity = m_vecGravity; }
    void                        SetGravity              ( const CVector& vecGravity );

    bool                        IsEnterable             ( void );

    void                        ReCreate                ( void );

    void                        ModelRequestCallback    ( CModelInfo* pModelInfo );

    // Warning: Don't use this to create a vehicle if CClientVehicleManager::IsVehicleLimitReached
    //          returns true. Also this messes with streaming so don't Destroy something unless
    //          you're going to recreate it very quickly again. CClientVehicleManager::IsVehicleLimitReached
    //          returns true often when the player is in an area full of vehicles so don't fatal
    //          error or something if it does return true.
    void                        Create                  ( void );
    void                        Destroy                 ( void );

    inline void                 AddProjectile           ( CClientProjectile * pProjectile )         { m_Projectiles.push_back ( pProjectile ); }
    inline void                 RemoveProjectile        ( CClientProjectile * pProjectile )         { m_Projectiles.remove ( pProjectile ); }
    std::list < CClientProjectile* > ::iterator ProjectilesBegin ( void )                               { return m_Projectiles.begin (); }
    std::list < CClientProjectile* > ::iterator ProjectilesEnd   ( void )                               { return m_Projectiles.end (); }

    void                        RemoveAllProjectiles    ( void );

protected:
    void                        StreamIn                ( bool bInstantly );
    void                        StreamOut               ( void );

    void                        NotifyCreate            ( void );
    void                        NotifyDestroy           ( void );

    bool                        DoCheckHasLandingGear   ( void );

    void                        StreamedInPulse         ( void );
    void                        Dump                    ( FILE* pFile, bool bDumpDetails, unsigned int uiIndex );

    class CClientObjectManager* m_pObjectManager;
    CClientVehicleManager*      m_pVehicleManager;
    CClientModelRequestManager* m_pModelRequester;
    unsigned short              m_usModel;
    bool                        m_bHasLandingGear;
    eClientVehicleType          m_eVehicleType;
    unsigned char               m_ucMaxPassengers;
    bool                        m_bIsVirtualized;
    CVehicle*                   m_pVehicle;
    CClientPed*                 m_pDriver;
    CClientPed*                 m_pPassengers [8];
    CClientPed*                 m_pOccupyingDriver;
    CClientPed*                 m_pOccupyingPassengers [8];
	RpClump*					m_pClump;
	short						m_usRemoveTimer;

    CClientVehicle*             m_pPreviousLink;
    CClientVehicle*             m_pNextLink;
    CMatrix                     m_Matrix;
	CMatrix						m_MatrixLast;
	CMatrix						m_MatrixPure;	
	CQuat						m_QuatA;
	CQuat						m_QuatB;
	CQuat						m_QuatLERP;
	float						m_fLERP;
	CVector						m_vecMoveSpeedInterpolate;
	CVector						m_vecMoveSpeedMeters;
	CVector                     m_vecMoveSpeed;
    CVector                     m_vecTurnSpeed;
    float                       m_fHealth;
    float                       m_fTurretHorizontal;
    float                       m_fTurretVertical;
    float                       m_fGasPedal;
    bool                        m_bVisible;
    bool                        m_bIsCollisionEnabled;
    bool                        m_bEngineOn;
    bool                        m_bEngineBroken;
    bool                        m_bSireneOrAlarmActive;
    bool                        m_bLandingGearDown;
    bool                        m_bHasAdjustableProperty;
    unsigned short              m_usAdjustablePropertyValue;
    bool                        m_bDoorsLocked;
    bool                        m_bDoorsUndamageable;
    bool                        m_bCanShootPetrolTank;
    bool                        m_bCanBeTargettedByHeatSeekingMissiles;
    bool                        m_bCanBeDamaged;
    bool                        m_bScriptCanBeDamaged;
    bool                        m_bSyncUnoccupiedDamage;
    bool                        m_bTyresCanBurst;
    unsigned char               m_ucDoorStates [MAX_DOORS];
    unsigned char               m_ucWheelStates [MAX_WHEELS];
    unsigned char               m_ucPanelStates [MAX_PANELS];
    unsigned char               m_ucLightStates [MAX_LIGHTS];
	bool						m_bJustBlewUp;
    eEntityStatus				m_NormalStatus;
    bool                        m_bColorSaved;
    unsigned char               m_ucColor1;
    unsigned char               m_ucColor2;
    unsigned char               m_ucColor3;
    unsigned char               m_ucColor4;
    bool                        m_bIsFrozen;
    bool                        m_bScriptFrozen;
    CMatrix                     m_matFrozen;
	CVehicleUpgrades*			m_pUpgrades;
    unsigned char               m_ucOverrideLights;
    CClientVehicle*             m_pTowedVehicle;
    CClientVehicle*             m_pTowedByVehicle;
    eWinchType                  m_eWinchType;
    CClientEntity*              m_pPickedUpWinchEntity;
    std::string                 m_strRegPlate;
    unsigned char               m_ucPaintjob;
    float                       m_fDirtLevel;
    bool                        m_bSmokeTrail;
    unsigned char               m_ucAlpha;
    bool                        m_bAlphaChanged;
	double						m_dLastRotationTime;
    bool                        m_bBlowNextFrame;
    bool                        m_bIsOnGround;

    bool                        m_bIsDerailed;
    bool                        m_bIsDerailable;
    bool                        m_bTrainDirection;
    float                       m_fTrainSpeed;

    bool                        m_bInterpolationEnabled;
    double                      m_dResetInterpolationTime;

    CVector                     m_vecTargetPosition;
    bool                        m_bTargetPositionDirections [ 3 ];
    bool                        m_bHasTargetPosition;
    CVector                     m_vecTargetRotation;
    bool                        m_bHasTargetRotation;

    unsigned long               m_ulIllegalTowBreakTime;

    bool                        m_bBlown;
    bool                        m_bHasDamageModel;

    bool                        m_bTaxiLightOn;
    std::list < CClientProjectile* > m_Projectiles;

    bool                        m_bIsInWater;

    CVector                     m_vecGravity;

public:
    CClientPlayer *             m_pLastSyncer;
    unsigned long               m_ulLastSyncTime;
    char *                      m_szLastSyncType;
    SLastSyncedVehData*         m_LastSyncedData;
};

#endif
