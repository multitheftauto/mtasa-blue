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

#pragma once

#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollision.h"

class CLuaPhysicsStaticCollisionManager : public CLuaPhysicsBaseManager<CLuaPhysicsStaticCollision*>
{
public:
    CLuaPhysicsStaticCollisionManager() : CLuaPhysicsBaseManager<CLuaPhysicsStaticCollision*>(EIdClassType::STATIC_COLLISION) {}
    void Remove(CLuaPhysicsStaticCollision* pStaticCollision, bool deleteFromList = true);
};
