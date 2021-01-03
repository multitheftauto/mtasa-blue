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

class CLuaPhysicsStaticCollisionManager
{
public:
    CLuaPhysicsStaticCollisionManager(){};
    ~CLuaPhysicsStaticCollisionManager();

    void                        RemoveAllStaticCollisions();
    CLuaPhysicsStaticCollision* GetStaticCollisionFromScriptID(unsigned int uiScriptID);

    void                                     AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    void                                     RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    unsigned long                            GetRigidBodyCount() const { return m_StaticCollisionList.size(); }
    std::vector<CLuaPhysicsStaticCollision*>::const_iterator IterBegin() { return m_StaticCollisionList.begin(); }
    std::vector<CLuaPhysicsStaticCollision*>::const_iterator IterEnd() { return m_StaticCollisionList.end(); }

private:
    std::vector<CLuaPhysicsStaticCollision*> m_StaticCollisionList;
};
