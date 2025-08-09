/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.h
 *  PURPOSE:     Header file for projectile entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileSA : public virtual CProjectile, public virtual CObjectSA
{
private:
    class CProjectileInfo*  m_projectileInfo;
    bool                    m_destroyed;
    
public:
    CProjectileSA(CObjectSAInterface* object, class CProjectileInfoSA* info);
    ~CProjectileSA();

    void Destroy(bool blow = true) override;
    bool CorrectPhysics() override;

public:
    CObjectSAInterface* m_object;
};
