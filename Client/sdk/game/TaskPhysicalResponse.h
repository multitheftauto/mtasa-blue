/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskPhysicalResponse.h
 *  PURPOSE:     Physical response task interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"

class CTaskSimpleChoking : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleChoking(){};

    virtual CPed*        GetAttacker() = 0;
    virtual unsigned int GetTimeRemaining() = 0;
    virtual unsigned int GetTimeStarted() = 0;
    virtual bool         IsTeargas() = 0;
    virtual bool         IsFinished() = 0;

    virtual void UpdateChoke(CPed* pPed, CPed* pAttacker, bool bIsTearGas) = 0;
};
