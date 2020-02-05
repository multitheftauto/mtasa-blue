/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollisionManager.h
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsStaticCollisionManager;

#pragma once

#include "LuaCommon.h"
#include "CLuaPhysicsStaticCollision.h"
#include <list>

class CLuaPhysicsStaticCollisionManager
{
public:
    CLuaPhysicsStaticCollisionManager(){};
    ~CLuaPhysicsStaticCollisionManager(){};

    CLuaPhysicsStaticCollision* GetStaticCollisionFromScriptID(unsigned int uiScriptID);

    CLuaPhysicsStaticCollision* AddStaticCollision(btDiscreteDynamicsWorld* pWorld);
    void                  RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    unsigned long               GetRigidBodyCount() const { return m_StaticCollisionList.size(); }
    CLuaPhysicsStaticCollision* GetStaticCollisionFromCollisionShape(const btCollisionShape* pCollisionShape);
    std::vector<CLuaPhysicsStaticCollision*> GetStaticCollisionsFromCollisionShape(const btCollisionShape* pCollisionShape);

	CFastList<CLuaPhysicsStaticCollision*>::const_iterator IterBegin() { return m_StaticCollisionList.begin(); }
    CFastList<CLuaPhysicsStaticCollision*>::const_iterator IterEnd() { return m_StaticCollisionList.end(); }

private:
    CFastList<CLuaPhysicsStaticCollision*> m_StaticCollisionList;
};
