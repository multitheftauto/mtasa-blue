/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollisionManager.h
 *  PURPOSE:     Lua static collision manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsStaticCollisionManager;
#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollision.h"

#pragma once

class CLuaPhysicsStaticCollisionManager : public CLuaPhysicsBaseManager<CLuaPhysicsStaticCollision*>
{
public:
    CLuaPhysicsStaticCollisionManager() : CLuaPhysicsBaseManager<CLuaPhysicsStaticCollision*>(EIdClassType::STATIC_COLLISION) {}
    void                                     Remove(CLuaPhysicsStaticCollision* pStaticCollision);
};
