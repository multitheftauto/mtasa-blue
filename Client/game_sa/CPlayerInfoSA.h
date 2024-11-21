/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlayerInfoSA.h
 *  PURPOSE:     Header file for player ped type information class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlayerInfo.h>
#include "CPlayerPedSA.h"

/**
 * \todo Confirm that AssocGroupId is a DWORD
 */
typedef DWORD AssocGroupId;

class CPedClothesDesc;

class CPlayerCrossHairSAInterface
{
public:
    bool  bActivated;
    float TargetX, TargetY;            // -1 ... 1 on screen
};

// Note: Information below may be incorrect. Please check before using.
#pragma pack(push)
#pragma pack(1)
class CPlayerPedDataSAInterface
{
public:
    CWantedSAInterface* m_Wanted;              // 0
    CPedClothesDesc*    m_pClothes;            // 4

    CPedSAInterface* m_ArrestingOfficer;            // actually CCopPed *        // 8

    CVector2D m_vecFightMovement;            // 12
    float     m_moveBlendRatio;              // 20
    float     m_fTimeCanRun;
    float     m_fSprintEnergy;

    BYTE                    m_nChosenWeapon;                   // 28
    BYTE                    m_nCarDangerCounter;               // 29
    BYTE                    m_pad0;                            // 30
    BYTE                    m_pad1;                            // 31
    long                    m_nStandStillTimer;                // 32
    DWORD                   m_nHitAnimDelayTimer;              // 36
    float                   m_fAttackButtonCounter;            // 40
    CAutomobileSAInterface* m_pDangerCar;                      // 44

    DWORD m_bStoppedMoving : 1;            // 48
    DWORD m_bAdrenaline : 1;
    DWORD m_bHaveTargetSelected : 1;            // Needed to work out whether we lost target this frame
    DWORD m_bFreeAiming : 1;
    DWORD bCanBeDamaged : 1;
    DWORD bAllMeleeAttackPtsBlocked : 1;            // if all of m_pMeleeAttackers[] is blocked by collision, just attack straight ahead
    DWORD m_JustBeenSnacking : 1;                   // If this bit is true we have just bought something from a vending machine
    DWORD m_bRequireHandleBreath : 1;               //

    // The player runs a group. Player is the leader. Player can go up to gang-members and make them join his group.  // 50
    DWORD m_GroupStuffDisabled : 1;                         // 52 // if this is true the player can't recrout or give his group commands.
    DWORD m_GroupAlwaysFollow : 1;                          // The group is told to always follow the player (used for girlfriend missions)
    DWORD m_GroupNeverFollow : 1;                           // The group is told to always follow the player (used for girlfriend missions)
    DWORD m_bInVehicleDontAllowWeaponChange : 1;            // stop weapon change once driveby weapon has been given
    DWORD m_bRenderWeapon : 1;                              //  set to false during cutscenes so that knuckledusters are not rendered

    long m_PlayerGroup;            // 60

    DWORD m_AdrenalineEndTime;                 // 64
    BYTE  m_nDrunkenness;                      // 68
    bool  m_bFadeDrunkenness;                  // 69
    BYTE  m_nDrugLevel;                        // 70
    BYTE  m_nScriptLimitToGangSize;            // 71

    float m_fBreath;            // for holding breath (ie underwater) // 72

    // once a set of melee weapon anims have been loaded and referenced for the player
    // we need to remember what we've referenced
    AssocGroupId m_MeleeWeaponAnimReferenced;
    // this set is for the player's extra hand combo they can learn
    AssocGroupId m_MeleeWeaponAnimReferencedExtra;

    float m_fFPSMoveHeading;
    float m_fLookPitch;
    float m_fSkateBoardSpeed;
    float m_fSkateBoardLean;

    DWORD* m_pSpecialAtomic;            // was rpAtomic
    float  m_fGunSpinSpeed;
    float  m_fGunSpinAngle;

    DWORD   m_LastTimeFiring;
    DWORD   m_nTargetBone;
    CVector m_vecTargetBoneOffset;

    DWORD m_busFaresCollected;
    bool  m_bPlayerSprintDisabled;
    bool  m_bDontAllowWeaponChange;
    bool  m_bForceInteriorLighting;
    BYTE  m_pad3;
    WORD  m_DPadDownPressedInMilliseconds;
    WORD  m_DPadUpPressedInMilliseconds;

    BYTE  m_wetness;
    BYTE  m_playersGangActive;
    BYTE  m_waterCoverPerc;
    BYTE  m_pad4;
    float m_waterHeight;

    // used for doing lock-on with HS missiles
    DWORD               m_FireHSMissilePressedTime;
    CEntitySAInterface* m_LastHSMissileTarget;

    long m_nModelIndexOfLastBuildingShot;

    DWORD m_LastHSMissileLOSTime : 31;
    DWORD m_bLastHSMissileLOS : 1;

    CPedSAInterface* m_pCurrentProstitutePed;
    CPedSAInterface* m_pLastProstituteShagged;
};
static_assert(sizeof(CPlayerPedDataSAInterface) == 172, "Invalid size for CPlayerPedDataSAInterface");
#pragma pack(pop)

class CPlayerInfoSAInterface
{
public:
    enum
    {
        PLAYERSTATE_PLAYING,
        PLAYERSTATE_HASDIED,
        PLAYERSTATE_HASBEENARRESTED,
        PLAYERSTATE_FAILEDMISSION,
        PLAYERSTATE_LEFTGAME
    };

    class CPlayerPedSAInterface* pPed;                            // Pointer to the player ped (should always be set)
    CPlayerPedDataSAInterface    PlayerPedData;                   // instance of player variables
    CVehicleSAInterface*         pRemoteVehicle;                  // Pointer to vehicle player is driving remotely at the moment.(NULL if on foot)
    CVehicleSAInterface*         pSpecCar;                        // which car is using the special collision model
    long                         Score;                           // Points for this player
    long                         DisplayScore;                    // Points as they would be displayed
    long                         CollectablesPickedUp;            // How many bags of sugar do we have
    long                         TotalNumCollectables;            // How many bags of sugar are there to be had in the game

    DWORD nLastBumpPlayerCarTimer;            // Keeps track of when the last ped bumped into the player car

    DWORD TaxiTimer;                       // Keeps track of how long the player has been in a taxi with a passenger (in msecs)
    DWORD vehicle_time_counter;            // keeps track of how long player has been in car for driving skill
    bool  bTaxiTimerScore;                 // If TRUE then add 1 to score for each second that the player is driving a taxi
    bool  m_bTryingToExitCar;              // if player holds exit car button, want to trigger getout once car slowed enough
                                           //  with a passenger

    CVehicleSAInterface* pLastTargetVehicle;            // Last vehicle player tried to enter.

    BYTE PlayerState;            // What's going on.

    bool  bAfterRemoteVehicleExplosion;
    bool  bCreateRemoteVehicleExplosion;
    bool  bFadeAfterRemoteVehicleExplosion;
    DWORD TimeOfRemoteVehicleExplosion;

    DWORD LastTimeEnergyLost;            // To make numbers flash on the HUD
    DWORD LastTimeArmourLost;

    DWORD LastTimeBigGunFired;              // Tank guns etc
    DWORD TimesUpsideDownInARow;            // Make car blow up if car upside down
    DWORD TimesStuckInARow;                 // Make car blow up if player cannot get out.

    // working counters to calculate how long player managed stuff
    DWORD nCarTwoWheelCounter;            // how long has player's car been on two wheels
    float fCarTwoWheelDist;
    DWORD nCarLess3WheelCounter;            // how long has player's car been on less than 3 wheels
    DWORD nBikeRearWheelCounter;            // how long has player's bike been on rear wheel only
    float fBikeRearWheelDist;
    DWORD nBikeFrontWheelCounter;            // how long has player's bike been on front wheel only
    float fBikeFrontWheelDist;
    DWORD nTempBufferCounter;            // so wheels can leave the ground for a few frames without stopping above counters
    // best values for the script to check - will be zero most of the time, only value
    // when finished trick - script should retreve value then reset to zero
    DWORD nBestCarTwoWheelsTimeMs;
    float fBestCarTwoWheelsDistM;
    DWORD nBestBikeWheelieTimeMs;
    float fBestBikeWheelieDistM;
    DWORD nBestBikeStoppieTimeMs;
    float fBestBikeStoppieDistM;

    WORD  CarDensityForCurrentZone;
    float RoadDensityAroundPlayer;            // 1.0f for an average city.

    DWORD TimeOfLastCarExplosionCaused;
    long  ExplosionMultiplier;
    long  HavocCaused;              // A counter going up when the player does bad stuff.
    short TimeLastEaten;            // A counter that starts at 5000, increases by 1 every minute

    float CurrentChaseValue;            // How 'ill' is the chase at the moment

    // 'Special' abilities that gets awarded during the game
    bool DoesNotGetTired;
    bool FastReload;
    bool FireProof;
    BYTE MaxHealth;
    BYTE MaxArmour;

    // Get-out-of-jail-free & Free-medical-care cards
    bool bGetOutOfJailFree;            // Player doesn't lose money/weapons next time arrested
    bool bFreeHealthCare;              // Player doesn't lose money nexed time patched up at hospital

    bool bCanDoDriveBy;

    BYTE m_nBustedAudioStatus;
    WORD m_nLastBustMessageNumber;

    CPlayerCrossHairSAInterface CrossHair;

    char       m_skinName[32];
    RwTexture* m_pSkinTexture;

    bool  m_bParachuteReferenced;
    DWORD m_nRequireParachuteTimer;
};

class CPlayerInfoSA : public CPlayerInfo
{
private:
    CPlayerInfoSAInterface* internalInterface;
    CWantedSA*              wanted;

public:
    CPlayerInfoSA(CPlayerInfoSAInterface* playerInfoInterface)
    {
        internalInterface = playerInfoInterface;
        wanted = NULL;            // we can't init it yet, as our interface hasn't been inited yet
    }

    ~CPlayerInfoSA()
    {
        if (wanted)
        {
            delete wanted;
        }
    }

    CPlayerInfoSAInterface* GetInterface() { return internalInterface; }

    CWanted* GetWanted();
    long     GetPlayerMoney();
    void     SetPlayerMoney(long lMoney, bool bInstant);

    void SetLastTimeEaten(short sTime);

    float GetFPSMoveHeading();

    void SetDoesNotGetTired(bool bDoesNotGetTired);

    byte GetCamDrunkLevel();
    void SetCamDrunkLevel(byte level);

    DWORD GetCarTwoWheelCounter() { return internalInterface->nCarTwoWheelCounter; }
    float GetCarTwoWheelDist() { return internalInterface->fCarTwoWheelDist; }
    DWORD GetCarLess3WheelCounter() { return internalInterface->nCarLess3WheelCounter; }
    DWORD GetBikeRearWheelCounter() { return internalInterface->nBikeRearWheelCounter; }
    float GetBikeRearWheelDist() { return internalInterface->fBikeRearWheelDist; }
    DWORD GetBikeFrontWheelCounter() { return internalInterface->nBikeFrontWheelCounter; }
    float GetBikeFrontWheelDist() { return internalInterface->fBikeFrontWheelDist; }
};
