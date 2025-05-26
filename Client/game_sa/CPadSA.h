/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPadSA.h
 *  PURPOSE:     Header file for controller pad input class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPad.h>

#define MAX_HORN_HISTORY        5
#define STEERINGBUFFERLENGTH    10

#define FUNC_CPad_UpdatePads                0x541DD0

class CPadSAInterface
{
public:
    CControllerState NewState;            // 0
    CControllerState OldState;            // 48

    signed short SteeringLeftRightBuffer[STEERINGBUFFERLENGTH];            // 96
    signed long  DrunkDrivingBufferUsed;                                   // 116

    // 120
    CControllerState PCTempKeyState;
    // 168
    CControllerState PCTempJoyState;
    // 216
    CControllerState PCTempMouseState;
    // 264
    BYTE         Phase;                                      // needed for switching into right state (analogue, pressure sensitive etc)
    WORD         Mode;                                       // Configuration of keys as selected by the player
    signed short ShakeDur;                                   // How long will shake go on for (in msecs)
    WORD         DisablePlayerControls;                      // If TRUE then the player cannot move, shoot, etc.
    BYTE         ShakeFreq;                                  // What is the frequency of the shake
    bool         bHornHistory[MAX_HORN_HISTORY];             // Store last 4 states of the horn key TRUE on else FALSE
    BYTE         iCurrHornHistory;                           // Where to store new history value
    BYTE         JustOutOfFrontEnd;                          // Number of frames we want some of the controls disabled for.
    bool         bApplyBrakes;                               // If TRUE then apply the brakes to the player's vehicle
    bool         bDisablePlayerEnterCar;                     // Script can set this so that the Enter Car button can be used to pick up objects
    bool         bDisablePlayerDuck;                         // Script can set this
    bool         bDisablePlayerFireWeapon;                   // Script can set this
    bool         bDisablePlayerFireWeaponWithL1;             //  Script can set this - for Judith's mission where L1 is needed to pick up objects
    bool         bDisablePlayerCycleWeapon;                  // Script can set this
    bool         bDisablePlayerJump;                         // Script can set this
    bool         bDisablePlayerDisplayVitalStats;            // Script can set this
    DWORD        LastTimeTouched;                            // The time the last input was applied by the player
    signed long  AverageWeapon;                              // Average value of the weapon button (analogue) since last reset
    signed long  AverageEntries;

    DWORD NoShakeBeforeThis;
    BYTE  NoShakeFreq;
};

class CPadSA : public CPad
{
private:
    CPadSAInterface* internalInterface;
    CPadSAInterface  StoredPad;

public:
    CPadSA(CPadSAInterface* padInterface) { internalInterface = padInterface; };

    CControllerState* GetCurrentControllerState(CControllerState* ControllerState);
    CControllerState* GetLastControllerState(CControllerState* ControllerState);
    void              SetCurrentControllerState(CControllerState* ControllerState);
    void              SetLastControllerState(CControllerState* ControllerState);
    void              Store();
    void              Restore();
    bool              IsEnabled();
    void              Disable(bool bDisable);
    void              Clear();
    CPadSAInterface*  GetInterface() { return internalInterface; };
    void              SetHornHistoryValue(bool value);
    void              SetLastTimeTouched(DWORD dwTime);
};
