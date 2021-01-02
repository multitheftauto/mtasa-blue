/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShapeManager.h
 *  PURPOSE:     Lua physics shapes manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsShapeManager;

#pragma once

class CLuaPhysicsShapeManager
{
public:
    CLuaPhysicsShapeManager(){};
    ~CLuaPhysicsShapeManager(){};

    CLuaPhysicsShape* GetShapeFromScriptID(unsigned int uiScriptID);

    void AddShape(CLuaPhysicsShape* pShape);

    void          RemoveShape(CLuaPhysicsShape* pShape);
    bool          IsShapeValid(CLuaPhysicsShape* pShape);
    unsigned long GetShapeCount() const { return m_ShapeList.size(); }

    std::vector<CLuaPhysicsShape*>::const_iterator IterBegin() { return m_ShapeList.begin(); }
    std::vector<CLuaPhysicsShape*>::const_iterator IterEnd() { return m_ShapeList.end(); }

private:
    std::vector<CLuaPhysicsShape*> m_ShapeList;
    std::mutex                     m_lock;
};
