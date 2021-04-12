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

class CLuaPhysicsShapeManager : public CLuaPhysicsBaseManager<CLuaPhysicsShape*>
{
public:
    CLuaPhysicsShapeManager() : CLuaPhysicsBaseManager<CLuaPhysicsShape*>(EIdClassType::SHAPE) {}
    ~CLuaPhysicsShapeManager();
    void Remove(CLuaPhysicsShape* pShape);
    bool IsShapeValid(CLuaPhysicsShape* pShape);
};
