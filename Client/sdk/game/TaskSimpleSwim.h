/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/TaskSimpleSwim.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "Task.h"

enum swimState : std::uint16_t
{
    SWIM_TREAD = 0,
    SWIM_SPRINT,
    SWIM_SPRINTING,
    SWIM_DIVE_UNDERWATER,
    SWIM_UNDERWATER_SPRINTING,
    SWIM_BACK_TO_SURFACE,
};

class CTaskSimpleSwim : public virtual CTaskSimple
{
public:
    virtual ~CTaskSimpleSwim() {};

    virtual swimState GetSwimState() const = 0;
};
