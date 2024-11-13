/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedIntelligenceSA.h
 *  PURPOSE:     Header file for ped entity AI class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedIntelligence.h>

class CPedSAInterface;
class CTaskManagerSA;

#define FUNC_CPedIntelligence_TestForStealthKill    0x601E00

class CFightManagerInterface
{
public:
    BYTE  Pad1[16];
    BYTE  UnknownState;
    BYTE  Pad2[3];
    float fStrafeState;
    float fForwardBackwardState;
};

class CPedIntelligenceSAInterface
{
public:
    // CEventHandlerHistory @ + 56
    CPedSAInterface*        pPed;
    DWORD                   taskManager;            // +4 (really CTaskManagerSAInterface)
    BYTE                    bPad[16];
    CFightManagerInterface* fightInterface;            // +24
    BYTE                    bPad2[184];
    DWORD                   vehicleScanner;            // +212 (really CVehicleScannerSAInterface)
};

class CPedIntelligenceSA : public CPedIntelligence
{
private:
    CPedIntelligenceSAInterface* internalInterface;
    CPed*                        ped;
    CTaskManagerSA*              TaskManager;

public:
    CPedIntelligenceSA(CPedIntelligenceSAInterface* pedIntelligenceSAInterface, CPed* ped);
    ~CPedIntelligenceSA();
    CPedIntelligenceSAInterface* GetInterface() { return internalInterface; }
    CTaskManager*                GetTaskManager();
    bool                         TestForStealthKill(CPed* pPed, bool bUnk);
    CTaskSAInterface*            SetTaskDuckSecondary(unsigned short nLengthOfDuck);
    CTaskSimpleUseGun*           GetTaskUseGun();
};
