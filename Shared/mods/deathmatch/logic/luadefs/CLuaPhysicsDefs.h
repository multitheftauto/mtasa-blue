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
#pragma once

#include "lua/CLuaFunctionParser.h"
#include "Enums.h"
typedef std::unordered_map<std::string, std::variant<float, CVector>>       RigidBodyOptions;
typedef std::unordered_map<std::string, std::variant<int, double, CVector>> CreateWorldOptions;

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CBulletPhysics*       PhysicsCreateWorld(lua_State* luaVM, std::optional<CreateWorldOptions> vecGravity);
    static CLuaPhysicsRigidBody* PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options);

    static CLuaPhysicsStaticCollision* PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position, std::optional<CVector> rotation);

    static CLuaPhysicsShape* PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant);

#ifdef MTA_CLIENT
    static bool PhysicsDrawDebug(CBulletPhysics* pPhysics);
#endif
    static std::vector<std::vector<float>> PhysicsGetDebugLines(CBulletPhysics* pPhysics, CVector vecPosition, float fRadius);

    template <typename T, typename U>
    static U getOption(const T& options, const std::string& szProperty, const U& default)
    {
        if (const auto it = options.find(szProperty); it != options.end())
        {
            if (!std::holds_alternative<U>(it->second))
                throw std::invalid_argument(SString("'%s' value must be ...", szProperty.c_str()).c_str());
            return std::get<U>(it->second);
        }
        return default;
    }
};
