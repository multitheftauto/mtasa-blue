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
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

struct CClientVehicleProperties;
class CClientVehicle;

#ifndef __CCLIENTVEHICLE_H
#define __CCLIENTVEHICLE_H

#include <game/CPlane.h>
#include <game/CVehicle.h>

#include "CClientCommon.h"
#include "CClientCamera.h"
#include "CClientModelRequestManager.h"
#include "CClientPed.h"
#include "CClientStreamElement.h"
#include "CClientVehicleManager.h"
#include "CVehicleUpgrades.h"

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
    SLastSyncedVehData ( void )
    {
        // Initialize to a known state
        memset ( this, 0, sizeof ( *this ) );
    }

    CVector             vecPosition;
    CVector             vecRotation;
    CVector             vecMoveSpeed;
    CVector             vecTurnSpeed;
    float               fHealth;
    ElementID           Trailer;
    bool                bEngineOn;
    bool                bDerailed;
    bool                bIsInWater;
};
struct SVehicleComponentData
{
    SVehicleComponentData ()
    {
        m_bPositionChanged = false;
        m_bRotationChanged = false;
        m_bVisible = true;
    }
    CVector m_vecComponentPosition;
    CVector m_vecComponentRotation;
    CVector m_vecOriginalComponentPosition;
    CVector m_vecOriginalComponentRotation;
    bool m_bPositionChanged;
    bool m_bRotationChanged;
    bool m_bVisible;
};
class CClientProjectile;

class CClientVehicle : public CClientStreamElement
{
    DECLARE_CLASS( CClientVehicle, CClientStreamElement )
    friend class CClientCamera;
    friend class CClientPed;
    friend class CClientVehicleManager;
    friend class CClientGame; // TEMP HACK

protected: // Use CDeathmatchVehicle constructor for now. Will get removed later when this class is
           // cleaned up.
                                CClientVehicle          ( CClientManager* pManager, ElementID ID, unsigned short usModel, unsigned char ucVariation, unsigned char ucVariation2 );

public:
                                ~CClientVehicle         ( void );
    
    void                        Unlink                  ( void );

    inline eClientEntityType    GetType                 ( void ) const                      { return CCLIENTVEHICLE; };

    inline const char*          GetNamePointer          ( void )                            { return m_pModelInfo->GetNameIfVehicle (); };
    inline eClientVehicleType   GetVehicleType          ( void )                            { return m_eVehicleType; };

    void                        GetPosition             ( CVector& vecPosition ) const;
    void                        SetPosition             ( const CVector& vecPosition )      { SetPosition ( vecPosition, true ); }
    void                        SetPosition             ( const CVector& vecPosition, bool bResetInterpolation );

    void                        UpdatePedPositions      ( const CVector& vecPosition );

    void                        GetRotationDegrees      ( CVector& vecRotation ) const;
    void                        GetRotationRadians      ( CVector& vecRotation ) const;
    void                        SetRotationDegrees      ( const CVector& vecRotation )      { SetRotationDegrees ( vecRotation, true ); }
    void                        SetRotationDegrees      ( const CVector& vecRotation, bool bResetInterpolation );
    void                        SetRotationRadians      ( const CVector& vecRotation )      { SetRotationRadians ( vecRotation, true ); }
    void                        SetRotationRadians      ( const CVector& vecRotation, bool bResetInterpolation );
    
    float                       GetDistanceFromCentreOfMassToBaseOfModel ( void );

    bool                        GetMatrix               ( CMatrix& Matrix ) const;
    bool                        SetMatrix               ( const CMatrix& Matrix );
    virtual CSphere             GetWorldBoundingSphere  ( void );

    void                        GetMoveSpeed            ( CVector& vecMoveSpeed ) const;
    void                        GetMoveSpeedMeters      ( CVector& vecMoveSpeed ) const;
    void                        SetMoveSpeed            ( const CVector& vecMoveSpeed );
    void                        GetTurnSpeed            ( CVector& vecTurnSpeed ) const;
    void                        SetTurnSpeed            ( const CVector& vecTurnSpeed );

    bool                        IsVisible               ( void );
    void                        SetVisible              ( bool bVisible );

    void                        SetDoorOpenRatio        ( unsigned char ucDoor, float fRatio, unsigned long ulDelay = 0, bool bForced = false );
    float                       GetDoorOpenRatio        ( unsigned char ucDoor );
    void                        SetSwingingDoorsAllowed ( bool bAllowed );
    bool                        AreSwingingDoorsAllowed () const;
    void                        AllowDoorRatioSetting   ( unsigned char ucDoor, bool bAllow, bool bAutoReallowAfterDelay = true );
    bool                        AreDoorsLocked          ( void );
    void                        SetDoorsLocked          ( bool bLocked );

private:
    void                        SetDoorOpenRatioInterpolated    ( unsigned char ucDoor, float fRatio, unsigned long ulDelay );
    void                        ResetDoorInterpolation          ();
    void                        CancelDoorInterpolation         ( unsigned char ucDoor );
    void                        ProcessDoorInterpolation        ();

public:
    bool                        AreDoorsUndamageable    ( void );
    void                        SetDoorsUndamageable    ( bool bUndamageable );

    float                       GetHealth               ( void ) const;
    void                        SetHealth               ( float fHealth );
    void                        Fix                     ( void );
    void                        Blow                    ( bool bAllowMovement = false );
    inline bool                 IsVehicleBlown          ( void ) { return m_bBlown; };

    CVehicleColor&              GetColor                ( void );
    void                        SetColor                ( const CVehicleColor& color );

    void                        GetTurretRotation       ( float& fHorizontal, float& fVertical );
    void                        SetTurretRotation       ( float fHorizontal, float fVertical );

    inline unsigned short       GetModel                ( void )                            { return m_usModel; };
    void                        SetModelBlocking        ( unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2 );

    inline unsigned char        GetVariant              ( void )                        { return m_ucVariation; };
    inline unsigned char        GetVariant2             ( void )                        { return m_ucVariation2; };

    void                        SetVariant              ( unsigned char ucVariant, unsigned char ucVariant2 );

    bool                        IsEngineBroken          ( void );
    void                        SetEngineBroken         ( bool bEngineBroken );

    bool                        IsEngineOn              ( void );
    void                        SetEngineOn             ( bool bEngineOn );

    bool                        CanBeDamaged            ( void );
    void                        CalcAndUpdateCanBeDamagedFlag     ( void );
    void                        SetScriptCanBeDamaged   ( bool bCanBeDamaged );
    void                        SetSyncUnoccupiedDamage ( bool bCanBeDamaged );
    bool                        GetScriptCanBeDamaged   ( void )                            { return m_bScriptCanBeDamaged; };

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
    void                        SetAdjustablePropertyValue  ( unsigned short usValue );
    bool                        HasMovingCollision          ( void );
private:
    void                        _SetAdjustablePropertyValue ( unsigned short usValue );
public:

    bool                        HasDamageModel          ( void )                            { return m_bHasDamageModel; }
    unsigned char               GetDoorStatus           ( unsigned char ucDoor );
    unsigned char               GetWheelStatus          ( unsigned char ucWheel );
    unsigned char               GetPanelStatus          ( unsigned char ucPanel );
    unsigned char               GetLightStatus          ( unsigned char ucLight );

    void                        SetDoorStatus           ( unsigned char ucDoor, unsigned char ucStatus );
    void                        SetWheelStatus          ( unsigned char ucWheel, unsigned char ucStatus, bool bSilent = true );
    void                        SetPanelStatus          ( unsigned char ucPanel, unsigned char ucStatus );
    void                        SetLightStatus          ( unsigned char ucLight, unsigned char ucStatus );
    bool                        GetWheelMissing         ( unsigned char ucWheel, const SString& strWheelName = "" );

    // TODO: Make the class remember on virtualization
    float                       GetHeliRotorSpeed       ( void );
    void                        SetHeliRotorSpeed       ( float fSpeed );

    bool                        IsHeliSearchLightVisible    ( void );
    void                        SetHeliSearchLightVisible   ( bool bVisible );

    void                        ReportMissionAudioEvent ( unsigned short usSound );

    inline bool                 IsCollisionEnabled      ( void )                            { return m_bIsCollisionEnabled; };
    void                        SetCollisionEnabled     ( bool bCollisionEnabled );

    bool                        GetCanShootPetrolTank   ( void );
    void                        SetCanShootPetrolTank   ( bool bCanShoot );
    
    bool                        GetCanBeTargettedByHeatSeekingMissiles      ( void );
    void                        SetCanBeTargettedByHeatSeekingMissiles      ( bool bEnabled );

    inline unsigned char        GetAlpha                ( void )                            { return m_ucAlpha; }
    void                        SetAlpha                ( unsigned char ucAlpha );

    CClientPed*                 GetOccupant             ( int iSeat = 0 ) const;
    CClientPed*                 GetControllingPlayer    ( void );
    void                        ClearForOccupants       ( void );

    void                        PlaceProperlyOnGround   ( void );

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
    bool                        IsFrozenWaitingForGroundToLoad      ( void ) const;
    void                        SetFrozenWaitingForGroundToLoad     ( bool bFrozen );

    CClientVehicle*             GetPreviousTrainCarriage( void );
    CClientVehicle*             GetNextTrainCarriage    ( void );
    void                        SetPreviousTrainCarriage( CClientVehicle* pPrevious );
    void                        SetNextTrainCarriage    ( CClientVehicle* pNext );
    inline bool                 IsChainEngine           ( void )                            { return m_bChainEngine; };
    void                        SetIsChainEngine        ( bool bChainEngine = true, bool bTemporary = false );
    CClientVehicle*             GetChainEngine          ( void );
    bool                        IsTrainConnectedTo      ( CClientVehicle * pTrailer );

    bool                        IsDerailed              ( void );
    void                        SetDerailed             ( bool bDerailed );
    bool                        IsDerailable            ( void );
    void                        SetDerailable           ( bool bDerailable );

    bool                        GetTrainDirection       ( void );
    void                        SetTrainDirection       ( bool bDirection );

    float                       GetTrainSpeed           ( void );
    void                        SetTrainSpeed           ( float fSpeed );

    float                       GetTrainPosition        ( void );
    void                        SetTrainPosition        ( float fTrainPosition, bool bRecalcOnRailDistance = true );

    uchar                       GetTrainTrack           ( void );
    void                        SetTrainTrack           ( uchar ucTrack );


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
    bool                        SetTowedVehicle         ( CClientVehicle* pVehicle, const CVector* vecRotationDegrees = NULL );
    inline CClientVehicle *     GetTowedByVehicle       ( void )                            { return m_pTowedByVehicle; }
    bool                        InternalSetTowLink      ( CClientVehicle* pTrailer );

    eWinchType                  GetWinchType            ( void )                            { return m_eWinchType; }
    bool                        SetWinchType            ( eWinchType winchType );
    bool                        PickupEntityWithWinch   ( CClientEntity* pEntity );
    bool                        ReleasePickedUpEntityWithWinch ( void );
    void                        SetRopeHeightForHeli    ( float fRopeHeight );
    CClientEntity*              GetPickedUpEntityWithWinch ( void );

    inline const char*          GetRegPlate             ( void )                            { return m_strRegPlate.empty () ? NULL : m_strRegPlate.c_str (); }
    bool                        SetRegPlate             ( const char* szPlate );

    unsigned char               GetPaintjob             ( void );
    void                        SetPaintjob             ( unsigned char ucPaintjob );
    
    float                       GetDirtLevel            ( void );
    void                        SetDirtLevel            ( float fDirtLevel );
	
    inline char                 GetNitroCount           ( void )                            { return m_pVehicle->GetNitroCount (); }
    inline float                GetNitroLevel           ( void )                            { return m_pVehicle->GetNitroLevel (); }
    inline void                 SetNitroCount           ( char cCount )                     { m_pVehicle->SetNitroCount ( cCount ); }
    inline void                 SetNitroLevel           ( float fLevel )                    { m_pVehicle->SetNitroLevel ( fLevel ); }

    bool                        IsNitroInstalled        ( void );

    float                       GetDistanceFromGround   ( void );

    void                        SetInWater              ( bool bState )                     { m_bIsInWater = bState; }
    bool                        IsInWater               ( void );
    bool                        IsOnGround              ( void );
    bool                        IsOnWater               ( void );
    void                        LockSteering            ( bool bLock );

    bool                        IsSmokeTrailEnabled     ( void );
    void                        SetSmokeTrailEnabled    ( bool bEnabled );

    void                        ResetInterpolation      ( void );

    void                        Interpolate             ( void );
    void                        UpdateKeysync           ( void );

    void                        GetInitialDoorStates    ( SFixedArray < unsigned char, MAX_DOORS >& ucOutDoorStates );

    // Time dependent interpolation
    inline void                 GetTargetPosition       ( CVector& vecPosition )            { vecPosition = m_interp.pos.vecTarget; }
    void                        SetTargetPosition       ( const CVector& vecPosition, unsigned long ulDelay, bool bValidVelocityZ = false, float fVelocityZ = 0.f );
    void                        RemoveTargetPosition    ( void );
    inline bool                 HasTargetPosition       ( void )                            { return ( m_interp.pos.ulFinishTime != 0 ); }

    inline void                 GetTargetRotation       ( CVector& vecRotation )            { vecRotation = m_interp.rot.vecTarget; }
    void                        SetTargetRotation       ( const CVector& vecRotation, unsigned long ulDelay );
    void                        RemoveTargetRotation    ( void );
    inline bool                 HasTargetRotation       ( void )                            { return ( m_interp.rot.ulFinishTime != 0 ); }

    void                        UpdateTargetPosition    ( void );
    void                        UpdateTargetRotation    ( void );
    void                        UpdateUnderFloorFix     ( const CVector& vecTargetPosition, bool bValidVelocityZ, float fVelocityZ );

    inline unsigned long        GetIllegalTowBreakTime  ( void )                            { return m_ulIllegalTowBreakTime; }
    inline void                 SetIllegalTowBreakTime  ( unsigned long ulTime )            { m_ulIllegalTowBreakTime = ulTime; }

    void                        GetGravity              ( CVector& vecGravity ) const       { vecGravity = m_vecGravity; }
    void                        SetGravity              ( const CVector& vecGravity );

    SColor                      GetHeadLightColor       ( void );
    void                        SetHeadLightColor       ( const SColor color );

    int                         GetCurrentGear          ( void );

    bool                        IsEnterable             ( void );
    bool                        HasRadio                ( void );
    bool                        HasPoliceRadio          ( void );

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

    static void                 SetPedOccupiedVehicle   ( CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor );
    static void                 SetPedOccupyingVehicle  ( CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor );
    static void                 ValidatePedAndVehiclePair ( CClientPed* pClientPed, CClientVehicle* pVehicle );
    static void                 UnpairPedAndVehicle     ( CClientPed* pClientPed, CClientVehicle* pVehicle );
    static void                 UnpairPedAndVehicle     ( CClientPed* pClientPed );
    
    void                        ApplyHandling           ( void );
    CHandlingEntry*             GetHandlingData         ( void );
    const CHandlingEntry*       GetOriginalHandlingData ( void )    { return m_pOriginalHandlingEntry; }

    uint                            GetTimeSinceLastPush    ( void )                      { return (uint)( CTickCount::Now () - m_LastPushedTime ).ToLongLong (); }
    void                            ResetLastPushTime       ( void )                      { m_LastPushedTime = CTickCount::Now (); }

    bool                        DoesVehicleHaveSirens       ( void ) { return m_tSirenBeaconInfo.m_bOverrideSirens; }

    bool                        GiveVehicleSirens           ( unsigned char ucSirenType, unsigned char ucSirenCount );
    void                        SetVehicleSirenPosition     ( unsigned char ucSirenID, CVector vecPos );
    void                        SetVehicleSirenMinimumAlpha ( unsigned char ucSirenID, DWORD dwPercentage );
    void                        SetVehicleSirenColour       ( unsigned char ucSirenID, SColor tVehicleSirenColour );
    void                        SetVehicleFlags             ( bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent );
    void                        RemoveVehicleSirens         ( void );

    bool                        ResetComponentPosition  ( SString vehicleComponent );
    bool                        SetComponentPosition    ( SString vehicleComponent, CVector vecPosition );
    bool                        GetComponentPosition    ( SString vehicleComponent, CVector &vecPosition );
    
    bool                        ResetComponentRotation  ( SString vehicleComponent );
    bool                        SetComponentRotation    ( SString vehicleComponent, CVector vecRotation );
    bool                        GetComponentRotation    ( SString vehicleComponent, CVector &vecRotation );

    bool                        SetComponentVisible     ( SString vehicleComponent, bool bVisible );
    bool                        GetComponentVisible     ( SString vehicleComponent, bool &bVisible );
    std::map < SString, SVehicleComponentData > ::iterator ComponentsBegin ( void )                               { return m_ComponentData.begin (); }
    std::map < SString, SVehicleComponentData > ::iterator ComponentsEnd   ( void )                               { return m_ComponentData.end (); }
    
    bool                        DoesSupportUpgrade      ( SString strFrameName );

    bool                        AreHeliBladeCollisionsEnabled              ( void )                              { return m_bEnableHeliBladeCollisions; }

    void                        SetHeliBladeCollisionsEnabled              ( bool bEnable )                    { m_bEnableHeliBladeCollisions = bEnable; }

    bool                        OnVehicleFallThroughMap                    ( );

protected:
    void                        StreamIn                ( bool bInstantly );
    void                        StreamOut               ( void );

    void                        NotifyCreate            ( void );
    void                        NotifyDestroy           ( void );

    bool                        DoCheckHasLandingGear   ( void );
    void                        HandleWaitingForGroundToLoad ( void );
    bool                        DoesNeedToWaitForGroundToLoad ( void );

    void                        StreamedInPulse         ( void );

    class CClientObjectManager* m_pObjectManager;
    CClientVehicleManager*      m_pVehicleManager;
    CClientModelRequestManager* m_pModelRequester;
    unsigned short              m_usModel;
    bool                        m_bHasLandingGear;
    eClientVehicleType          m_eVehicleType;
    unsigned char               m_ucMaxPassengers;
    bool                        m_bIsVirtualized;
    CVehicle*                   m_pVehicle;
    CClientPedPtr               m_pDriver;
    SFixedArray < CClientPed*, 8 >  m_pPassengers;
    CClientPedPtr               m_pOccupyingDriver;
    SFixedArray < CClientPed*, 8 >  m_pOccupyingPassengers;
    RpClump*                    m_pClump;
    short                       m_usRemoveTimer;

    CClientVehiclePtr           m_pPreviousLink;
    CClientVehiclePtr           m_pNextLink;
    CMatrix                     m_Matrix;
    CMatrix                     m_MatrixLast;
    CMatrix                     m_MatrixPure;   
    CVector                     m_vecMoveSpeedInterpolate;
    CVector                     m_vecMoveSpeedMeters;
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
    std::map < eDoors, CTickCount > m_AutoReallowDoorRatioMap;
    SFixedArray < bool, 6 >     m_bAllowDoorRatioSetting;
    SFixedArray < float, 6 >    m_fDoorOpenRatio;
    struct
    {
        SFixedArray < float, 6 >            fStart;
        SFixedArray < float, 6 >            fTarget;
        SFixedArray < unsigned long, 6 >    ulStartTime;
        SFixedArray < unsigned long, 6 >    ulTargetTime;
    } m_doorInterp;
    bool                        m_bSwingingDoorsAllowed;
    bool                        m_bDoorsLocked;
    bool                        m_bDoorsUndamageable;
    bool                        m_bCanShootPetrolTank;
    bool                        m_bCanBeTargettedByHeatSeekingMissiles;
    bool                        m_bCanBeDamaged;
    bool                        m_bScriptCanBeDamaged;
    bool                        m_bSyncUnoccupiedDamage;
    bool                        m_bTyresCanBurst;
    SFixedArray < unsigned char, MAX_DOORS >   m_ucDoorStates;
    SFixedArray < unsigned char, MAX_WHEELS >  m_ucWheelStates;
    SFixedArray < unsigned char, MAX_PANELS >  m_ucPanelStates;
    SFixedArray < unsigned char, MAX_LIGHTS >  m_ucLightStates;
    bool                        m_bJustBlewUp;
    eEntityStatus               m_NormalStatus;
    bool                        m_bColorSaved;
    CVehicleColor               m_Color;
    bool                        m_bIsFrozen;
    bool                        m_bScriptFrozen;
    bool                        m_bFrozenWaitingForGroundToLoad;
    float                       m_fGroundCheckTolerance;
    float                       m_fObjectsAroundTolerance;
    CVector                     m_vecWaitingForGroundSavedMoveSpeed;
    CVector                     m_vecWaitingForGroundSavedTurnSpeed;
    CMatrix                     m_matFrozen;
    CVehicleUpgrades*           m_pUpgrades;
    unsigned char               m_ucOverrideLights;
    CClientVehiclePtr           m_pTowedVehicle;
    CClientVehiclePtr           m_pTowedByVehicle;
    eWinchType                  m_eWinchType;
    CClientEntityPtr            m_pPickedUpWinchEntity;
    SString                     m_strRegPlate;
    unsigned char               m_ucPaintjob;
    float                       m_fDirtLevel;
    bool                        m_bSmokeTrail;
    unsigned char               m_ucAlpha;
    bool                        m_bAlphaChanged;
    double                      m_dLastRotationTime;
    bool                        m_bBlowNextFrame;
    bool                        m_bIsOnGround;
    bool                        m_bHeliSearchLightVisible;
    float                       m_fHeliRotorSpeed;
    const CHandlingEntry*       m_pOriginalHandlingEntry;
    CHandlingEntry*             m_pHandlingEntry;

    bool                        m_bChainEngine;
    bool                        m_bIsDerailed;
    bool                        m_bIsDerailable;
    bool                        m_bTrainDirection;
    float                       m_fTrainSpeed;
    float                       m_fTrainPosition;
    uchar                       m_ucTrackID;

    // Time dependent error compensation interpolation
    struct
    {
        struct
        {
#ifdef MTA_DEBUG
            CVector         vecStart;
#endif
            CVector         vecTarget;
            CVector         vecError;
            float           fLastAlpha;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
        } pos;

        struct
        {
#ifdef MTA_DEBUG
            CVector         vecStart;
#endif
            CVector         vecTarget;
            CVector         vecError;
            float           fLastAlpha;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
        } rot;
    } m_interp;

    unsigned long               m_ulIllegalTowBreakTime;

    bool                        m_bBlown;
    bool                        m_bHasDamageModel;

    bool                        m_bTaxiLightOn;
    std::list < CClientProjectile* > m_Projectiles;

    bool                        m_bIsInWater;

    bool                        m_bNitroActivated;

    CVector                     m_vecGravity;
    SColor                      m_HeadLightColor;

    bool                        m_bHasCustomHandling;

    unsigned char               m_ucVariation;
    unsigned char               m_ucVariation2;

    CTickCount                  m_LastPushedTime;
    uint                        m_uiForceLocalZCounter;

    bool                        m_bEnableHeliBladeCollisions;
    CMatrix                     m_matCreate;
    unsigned char               m_ucFellThroughMapCount;

public:
#ifdef MTA_DEBUG
    CClientPlayer *             m_pLastSyncer;
    unsigned long               m_ulLastSyncTime;
    const char *                m_szLastSyncType;
#endif
    SLastSyncedVehData*         m_LastSyncedData;
    SSirenInfo                  m_tSirenBeaconInfo;
    std::map<SString, SVehicleComponentData>  m_ComponentData;

};

#endif
