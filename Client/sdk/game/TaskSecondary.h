/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskSecondary.h
 *  PURPOSE:     Secondary task interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Task.h"

enum eDuckControlTypes
{
    DUCK_STANDALONE = 0,                      // duck anim removed when task removed
    DUCK_STANDALONE_WEAPON_CROUCH,            // duck anim removed when task removed
    DUCK_TASK_CONTROLLED,                     // duck directly linked to a controlling task
    DUCK_ANIM_CONTROLLED,                     // duck linked to duck anim (make duck partial?)
    DUCK_SCRIPT_CONTROLLED,
};

class CTaskSimpleDuck : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleDuck(){};
};
