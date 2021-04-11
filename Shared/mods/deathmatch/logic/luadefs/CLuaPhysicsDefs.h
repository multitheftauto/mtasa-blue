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
    static CBulletPhysics* GetPhysics();
    static void            LoadFunctions(void);
    static void            AddClass(lua_State* luaVM);

    static CLuaPhysicsRigidBody* PhysicsCreateRigidBody(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options);

    static CLuaPhysicsStaticCollision* PhysicsCreateStaticCollision(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                    std::optional<CVector> rotation);

    static CLuaPhysicsShape* PhysicsCreateBoxShape(lua_State* luaVM, std::variant<CVector, float> variant);

#ifdef MTA_CLIENT
    static bool PhysicsDrawDebug();
#endif
    static std::vector<std::vector<float>> PhysicsGetDebugLines(CVector vecPosition, float fRadius);

    template <typename T, typename U>
    static U getOption(const T& options, const std::string& szProperty, const U defaultValue)
    {
        if (const auto it = options.find(szProperty); it != options.end())
        {
            if (!std::holds_alternative<U>(it->second))
                throw std::invalid_argument(SString("'%s' value must be ...", szProperty.c_str()).c_str());
            return std::get<U>(it->second);
        }
        return defaultValue;
    }
};
