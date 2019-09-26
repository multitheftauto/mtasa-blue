/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", PhysicsCreateWorld},
        {"physicsCreateRigidBody", PhysicsCreateRigidBody},
        {"physicsAddShape", PhysicsAddShape},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "set", "PhysicsTestSet");
    // lua_classfunction(luaVM, "get", "PhysicsTestGet");

    lua_registerstaticclass(luaVM, "Physics");
}

int CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM)
{
    CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID);
    lua_pushelement(luaVM, pPhysics);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CLuaPhysicsRigidBody* pRigidBody = pPhysics->CreateRigidBody(luaMain);
            CVector vector;
            float   fRadius;
            switch (shapeType)
            {
                case PHYSICS_SHAPE_BOX:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->InitializeWithBox(vector);
                    }
                    break;
                case PHYSICS_SHAPE_SPHERE:
                    argStream.ReadNumber(fRadius);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->InitializeWithSphere(fRadius);
                    }
                    break;
            }
            lua_pushrigidbody(luaVM, pRigidBody);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsAddShape(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    ePhysicsShapeType     shapeType;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CVector vector;
        float   fRadius;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddBox(vector);
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddSphere(fRadius);
                }
                break;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}
