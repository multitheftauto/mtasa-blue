/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CProjectile.h
 *  PURPOSE:     Projectile entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CObject.h"

class CProjectile : public virtual CObject
{
public:
    virtual ~CProjectile(){};

    virtual void Destroy(bool blow) = 0;
    virtual bool CorrectPhysics() = 0;
};
