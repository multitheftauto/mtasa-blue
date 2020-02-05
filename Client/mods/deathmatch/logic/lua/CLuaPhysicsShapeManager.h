/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShapeManager.h
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsShapeManager;

#pragma once

#include "LuaCommon.h"
#include "CLuaPhysicsShape.h"
#include <list>

class CLuaPhysicsShapeManager
{
public:
    CLuaPhysicsShapeManager(){};
    ~CLuaPhysicsShapeManager(){};

    CLuaPhysicsShape* GetShapeFromScriptID(unsigned int uiScriptID);

    CLuaPhysicsShape*     AddShape(CClientPhysics* pPhysics);
    void                  RemoveShape(CLuaPhysicsShape* pShape);
    CLuaPhysicsShape*     GetShape(const btCollisionShape* pShape);
    unsigned long         GetShapeCount() const { return m_ShapeList.size(); }

    CFastList<CLuaPhysicsShape*>::const_iterator IterBegin() { return m_ShapeList.begin(); }
    CFastList<CLuaPhysicsShape*>::const_iterator IterEnd() { return m_ShapeList.end(); }

private:
    CFastList<CLuaPhysicsShape*> m_ShapeList;
};
