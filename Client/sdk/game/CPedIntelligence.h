/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPedIntelligence.h
 *  PURPOSE:     Ped artificial intelligence interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CPed;
class CTaskSAInterface;
class CTaskManager;
class CTaskSimpleUseGun;

class CPedIntelligence
{
public:
    virtual CTaskManager*     GetTaskManager() = 0;
    virtual bool              TestForStealthKill(CPed* pPed, bool bUnk) = 0;
    virtual CTaskSAInterface* SetTaskDuckSecondary(unsigned short nLengthOfDuck) = 0;
    virtual CTaskSimpleUseGun* GetTaskUseGun() = 0;
};
