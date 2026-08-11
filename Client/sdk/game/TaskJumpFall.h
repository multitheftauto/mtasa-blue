/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskJumpFall.h
 *  PURPOSE:     Jump/fall task interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"
#include "CPed.h"

enum eClimbHeights : std::int8_t;

class CTaskSimpleClimb : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleClimb() {};

    virtual eClimbHeights GetHeightForPos() const = 0;

    static class CEntitySAInterface* TestForClimb(CPed* ped, CVector& climbPos, float& climbAngle, int& surfaceType, bool launch)
    {
        if (!ped)
            return nullptr;

        // CTaskSimpleClimb::TestForClimb
        return ((class CEntitySAInterface * (__cdecl*)(class CPedSAInterface*, CVector*, float*, int*, bool))0x6803A0)(ped->GetPedInterface(), &climbPos,
                                                                                                                       &climbAngle, &surfaceType, launch);
    }
};

class CTaskSimpleJetPack : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleJetPack() {};

    virtual bool IsFinished() const = 0;
};
