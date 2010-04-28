/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerInfoSA.h
*  PURPOSE:     Header file for player ped type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PLAYERINFO
#define __CGAMESA_PLAYERINFO

#include <game/CPlayerInfo.h>
#include "Common.h"
#include "CPlayerPedSA.h"

#define FUNC_MakePlayerSafe                         0x56e870
#define FUNC_CancelPlayerEnteringCars               0x56e860
#define FUNC_ArrestPlayer                           0x56e5d0
#define FUNC_KillPlayer                             0x56e580

#define FUNC_TakeRemoteControlledCarFromPlayer      0x45ae80
#define FUNC_GivePlayerRemoteControlledCar          0x442020
#define FUNC_TakeRemoteControlOfCar                 0x45ad40
#define FUNC_StreamParachuteWeapon                  0x56eb30
#define FUNC_GivePlayerParachute                    0x56ec40

/**
 * \todo Implement FUNC_TakeRemoteControlOfCar, FUNC_StreamParachuteWeapon and FUNC_StreamParachuteWeapon
 */

#define VAR_PlayerPed                               0x94AD28
#define VAR_PlayerRCCar                             0x94AD2C
#define VAR_PlayerMoney                             0x94ADC8

#define NUM_CHARS_IN_NAME                           34

/**
 * \todo Confirm that AssocGroupId is a DWORD
 */
typedef DWORD AssocGroupId ;

class CPedClothesDesc;

class CPlayerCrossHairSAInterface
{
public:
    bool    bActivated;
    float   TargetX, TargetY;   // -1 ... 1 on screen
};


class CPlayerPedDataSAInterface
{
public:
    CWantedSAInterface* m_Wanted;       // 0
    CPedClothesDesc* m_pClothes;        // 4

    CPedSAInterface* m_ArrestingOfficer; // actually CCopPed *        // 8

    CVector2D m_vecFightMovement;        // 12
    FLOAT   m_moveBlendRatio;        // 20
    FLOAT   m_fSprintEnergy;        // 24
    //FLOAT m_fSprintControlCounter; // Removed arbitatrily to aligned next byte, should be here really
    BYTE    m_nChosenWeapon;        // 28
    BYTE    m_nCarDangerCounter;        // 29
    long    m_nStandStillTimer;        // 30
    DWORD   m_nHitAnimDelayTimer;        // 34
    FLOAT   m_fAttackButtonCounter;        // 38
    CAutomobile *m_pDangerCar;        // 42

    DWORD   m_bStoppedMoving : 1;        // 46
    DWORD   m_bAdrenaline : 1;
    DWORD   m_bHaveTargetSelected : 1;  // Needed to work out whether we lost target this frame
    DWORD   m_bFreeAiming : 1;
    DWORD   bCanBeDamaged : 1;
    DWORD   bAllMeleeAttackPtsBlocked : 1;  // if all of m_pMeleeAttackers[] is blocked by collision, just attack straight ahead
    DWORD   m_JustBeenSnacking : 1;     // If this bit is true we have just bought something from a vending machine
    DWORD   m_bRequireHandleBreath : 1; // 
    
    // The player runs a group. Player is the leader. Player can go up to gang-members and make them join his group.    
    DWORD   m_GroupStuffDisabled : 1;   // if this is true the player can't recrout or give his group commands.
    DWORD   m_GroupAlwaysFollow : 1;    // The group is told to always follow the player (used for girlfriend missions)
    DWORD   m_GroupNeverFollow : 1; // The group is told to always follow the player (used for girlfriend missions)
    DWORD   m_bInVehicleDontAllowWeaponChange : 1; // stop weapon change once driveby weapon has been given
    DWORD   m_bRenderWeapon : 1;    //  set to false during cutscenes so that knuckledusters are not rendered

    long    m_PlayerGroup;        // 48?

    DWORD   m_AdrenalineEndTime;        // 52
    BYTE    m_nDrunkenness;
    bool    m_bFadeDrunkenness;
    BYTE    m_nDrugLevel;
    BYTE    m_nScriptLimitToGangSize;
    
    FLOAT   m_fBreath;  // for holding breath (ie underwater)

    // once a set of melee weapon anims have been loaded and referenced for the player
    // we need to remember what we've referenced
    AssocGroupId m_MeleeWeaponAnimReferenced;
    // this set is for the player's extra hand combo they can learn
    AssocGroupId m_MeleeWeaponAnimReferencedExtra;

    FLOAT   m_fFPSMoveHeading;
    FLOAT   m_fLookPitch;
    FLOAT   m_fSkateBoardSpeed;
    FLOAT   m_fSkateBoardLean;
    
    DWORD * m_pSpecialAtomic; // was rpAtomic
    FLOAT   m_fGunSpinSpeed;
    FLOAT   m_fGunSpinAngle;

    DWORD   m_LastTimeFiring;
    DWORD   m_nTargetBone;
    CVector m_vecTargetBoneOffset;
    
    DWORD   m_busFaresCollected;
    bool    m_bPlayerSprintDisabled;
    bool    m_bDontAllowWeaponChange;
    bool    m_bForceInteriorLighting;
    WORD    m_DPadDownPressedInMilliseconds;
    WORD    m_DPadUpPressedInMilliseconds;
        
    BYTE    m_wetness;
    BYTE    m_playersGangActive;
    BYTE    m_waterCoverPerc;
    FLOAT   m_waterHeight;

    // used for doing lock-on with HS missiles
    DWORD   m_FireHSMissilePressedTime;
    CEntitySAInterface * m_LastHSMissileTarget;
    
    long    m_nModelIndexOfLastBuildingShot;
    
    DWORD   m_LastHSMissileLOSTime  :31;
    DWORD   m_bLastHSMissileLOS     :1;
    
    CPedSAInterface * m_pCurrentProstitutePed;
    CPedSAInterface * m_pLastProstituteShagged;

    DWORD padding; // we're missing something here - fixes a crash (ugly hack #4432525) - eAi
};
    

class CPlayerInfoSAInterface
{
public:
    enum { PLAYERSTATE_PLAYING, PLAYERSTATE_HASDIED, PLAYERSTATE_HASBEENARRESTED, PLAYERSTATE_FAILEDMISSION, PLAYERSTATE_LEFTGAME };

    class CPlayerPedSAInterface *pPed;          // Pointer to the player ped (should always be set)
    CPlayerPedDataSAInterface PlayerPedData;    // instance of player variables
    CVehicleSAInterface *pRemoteVehicle;// Pointer to vehicle player is driving remotely at the moment.(NULL if on foot)
    CVehicleSAInterface *pSpecCar;      // which car is using the special collision model
    long        Score;          // Points for this player
    long        DisplayScore;   // Points as they would be displayed
    long        CollectablesPickedUp;   // How many bags of sugar do we have
    long        TotalNumCollectables;   // How many bags of sugar are there to be had in the game

    DWORD       nLastBumpPlayerCarTimer;            // Keeps track of when the last ped bumped into the player car

    DWORD       TaxiTimer;          // Keeps track of how long the player has been in a taxi with a passenger (in msecs)
    DWORD       vehicle_time_counter;  // keeps track of how long player has been in car for driving skill
    bool        bTaxiTimerScore;    // If TRUE then add 1 to score for each second that the player is driving a taxi 
    bool        m_bTryingToExitCar; // if player holds exit car button, want to trigger getout once car slowed enough
                        //  with a passenger

    CVehicleSAInterface *pLastTargetVehicle;//Last vehicle player tried to enter.

    BYTE        PlayerState;        // What's going on.

    bool        bAfterRemoteVehicleExplosion;
    bool        bCreateRemoteVehicleExplosion;
    bool        bFadeAfterRemoteVehicleExplosion;
    DWORD       TimeOfRemoteVehicleExplosion;

    DWORD       LastTimeEnergyLost; // To make numbers flash on the HUD
    DWORD       LastTimeArmourLost;

    DWORD       LastTimeBigGunFired;    // Tank guns etc
    DWORD       TimesUpsideDownInARow;  // Make car blow up if car upside down
    DWORD       TimesStuckInARow;       // Make car blow up if player cannot get out.
    
    // working counters to calculate how long player managed stuff
    DWORD       nCarTwoWheelCounter;        // how long has player's car been on two wheels
    FLOAT       fCarTwoWheelDist;
    DWORD       nCarLess3WheelCounter;  // how long has player's car been on less than 3 wheels
    DWORD       nBikeRearWheelCounter;  // how long has player's bike been on rear wheel only
    FLOAT       fBikeRearWheelDist;
    DWORD       nBikeFrontWheelCounter; // how long has player's bike been on front wheel only
    FLOAT       fBikeFrontWheelDist;
    DWORD       nTempBufferCounter;     // so wheels can leave the ground for a few frames without stopping above counters
    // best values for the script to check - will be zero most of the time, only value
    // when finished trick - script should retreve value then reset to zero
    DWORD       nBestCarTwoWheelsTimeMs;
    FLOAT       fBestCarTwoWheelsDistM;
    DWORD       nBestBikeWheelieTimeMs;
    FLOAT       fBestBikeWheelieDistM;
    DWORD       nBestBikeStoppieTimeMs;
    FLOAT       fBestBikeStoppieDistM;

    WORD        CarDensityForCurrentZone;
    FLOAT       RoadDensityAroundPlayer;    // 1.0f for an average city.

    DWORD       TimeOfLastCarExplosionCaused;
    long        ExplosionMultiplier;
    long        HavocCaused;            // A counter going up when the player does bad stuff.
    short       TimeLastEaten;          // A counter that starts at 5000, increases by 1 every minute

    FLOAT       CurrentChaseValue;      // How 'ill' is the chase at the moment
    
    // 'Special' abilities that gets awarded during the game
    bool        DoesNotGetTired;
    bool        FastReload;
    bool        FireProof;
    BYTE        MaxHealth;
    BYTE        MaxArmour;

    // Get-out-of-jail-free & Free-medical-care cards
    bool        bGetOutOfJailFree;      // Player doesn't lose money/weapons next time arrested
    bool        bFreeHealthCare;        // Player doesn't lose money nexed time patched up at hospital

    bool        bCanDoDriveBy;
    
    BYTE        m_nBustedAudioStatus;
    WORD        m_nLastBustMessageNumber;

    CPlayerCrossHairSAInterface  CrossHair;

    char        m_skinName[32];
    RwTexture*  m_pSkinTexture;

    bool        m_bParachuteReferenced;
    DWORD       m_nRequireParachuteTimer;
};

class CPlayerInfoSA : public CPlayerInfo
{
private:
    CPlayerInfoSAInterface  * internalInterface;
    CWantedSA               * wanted;
public:
    CPlayerInfoSA ( CPlayerInfoSAInterface * playerInfoInterface ) 
    { 
        this->internalInterface = playerInfoInterface;
        this->wanted = NULL; // we can't init it yet, as our interface hasn't been inited yet
    }

    ~CPlayerInfoSA ( void )
    {
        if ( wanted )
        {
            delete wanted;
        }
    }

    CPlayerInfoSAInterface * GetInterface ( void ) { return internalInterface; }

    CPlayerPed      * GetPlayerPed (  );
    CWanted         * GetWanted ( );
    long            GetPlayerMoney ( void );
    void            SetPlayerMoney ( long lMoney );

    void            GetCrossHair ( bool &bActivated, float &fTargetX, float &fTargetY );

    VOID            GivePlayerParachute ( VOID );
    VOID            StreamParachuteWeapon ( bool bAllowParachute );

    short           GetLastTimeEaten ( void );
    void            SetLastTimeEaten ( short sTime );

    VOID            MakePlayerSafe ( BOOL boolSafe );
    VOID            CancelPlayerEnteringCars ( CVehicle * vehicle );
    VOID            ArrestPlayer (  );
    VOID            KillPlayer (  );

    CVehicle        * GivePlayerRemoteControlledCar ( eVehicleTypes vehicletype );  // these are really members of CRemote, but they make more sense here
    VOID            TakeRemoteControlledCarFromPlayer (  );
    CVehicle        * GetPlayerRemoteControlledCar (  );

    float           GetFPSMoveHeading ( void );

    bool            GetDoesNotGetTired              ( void );
    void            SetDoesNotGetTired              ( bool bDoesNotGetTired );

    DWORD           GetLastTimeBigGunFired ( void );
    void            SetLastTimeBigGunFired ( DWORD dwTime );
    
    inline DWORD    GetCarTwoWheelCounter       ( void )            { return internalInterface->nCarTwoWheelCounter; }
    inline float    GetCarTwoWheelDist          ( void )            { return internalInterface->fCarTwoWheelDist; }
    inline DWORD    GetCarLess3WheelCounter     ( void )            { return internalInterface->nCarLess3WheelCounter; }
    inline DWORD    GetBikeRearWheelCounter     ( void )            { return internalInterface->nBikeRearWheelCounter; }
    inline float    GetBikeRearWheelDist        ( void )            { return internalInterface->fBikeRearWheelDist; }
    inline DWORD    GetBikeFrontWheelCounter    ( void )            { return internalInterface->nBikeFrontWheelCounter; }
    inline float    GetBikeFrontWheelDist       ( void )            { return internalInterface->fBikeFrontWheelDist; }
};

#endif
