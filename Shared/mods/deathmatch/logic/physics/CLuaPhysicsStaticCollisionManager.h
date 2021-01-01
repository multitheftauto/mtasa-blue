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
    ~CLuaPhysicsStaticCollisionManager(){};

    CLuaPhysicsStaticCollision* GetStaticCollisionFromScriptID(unsigned int uiScriptID);

    void                                     AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);
    void                                     RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    unsigned long                            GetRigidBodyCount() const { return m_StaticCollisionList.size(); }
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>>::const_iterator IterBegin() { return m_StaticCollisionList.begin(); }
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>>::const_iterator IterEnd() { return m_StaticCollisionList.end(); }

private:
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> m_StaticCollisionList;
};
