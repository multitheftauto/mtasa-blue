/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPlayerPed.h
 *  PURPOSE:     Played ped entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPed.h"

class CWanted;

class CPlayerPed : public virtual CPed
{
public:
    virtual ~CPlayerPed(){};

    virtual CWanted* GetWanted() = 0;

    virtual void SetInitialState() = 0;

    virtual eMoveAnim GetMoveAnim() = 0;
    virtual void      SetMoveAnim(eMoveAnim iAnimGroup) = 0;
};
