/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaDefs.h
 *  PURPOSE:     Lua definitions header
 *
 *****************************************************************************/

#pragma once
#define LUA_DECLARE(x) static int x ( lua_State * luaVM );
#define LUA_DECLARE_OOP(x) LUA_DECLARE(x) LUA_DECLARE(OOP_##x)

class CClientColManager;
class CClientColModelManager;
class CClientDFFManager;
class CClientEntity;
class CClientGame;
class CClientGUIManager;
class CClientManager;
class CClientMarkerManager;
class CClientObjectManager;
class CClientPickupManager;
class CClientPlayerManager;
class CClientRadarMarkerManager;
class CClientTeamManager;
class CClientVehicleManager;
class CElementDeleter;
class CLuaManager;
class CRegisteredCommands;
class CRenderWare;
class CResourceManager;
class CScriptDebugging;

class CLuaDefs
{
public:
    static void Initialize(class CClientGame* pClientGame, CLuaManager* pLuaManager, CScriptDebugging* pScriptDebugging);

    static int  CanUseFunction(lua_CFunction f, lua_State* luaVM);
    static void DidUseFunction(lua_CFunction f, lua_State* luaVM);

    // This is just for the Lua funcs. Please don't public this and use it other
    // places in the client.
public:
    static CElementDeleter*           m_pElementDeleter;
    static CLuaManager*               m_pLuaManager;
    static CScriptDebugging*          m_pScriptDebugging;
    static class CClientGame*         m_pClientGame;
    static CClientManager*            m_pManager;
    static CClientEntity*             m_pRootEntity;
    static CClientGUIManager*         m_pGUIManager;
    static CClientPlayerManager*      m_pPlayerManager;
    static CClientRadarMarkerManager* m_pRadarMarkerManager;
    static CResourceManager*          m_pResourceManager;
    static CClientVehicleManager*     m_pVehicleManager;
    static CClientColManager*         m_pColManager;
    static CClientObjectManager*      m_pObjectManager;
    static CClientTeamManager*        m_pTeamManager;
    static CRenderWare*               m_pRenderWare;
    static CClientMarkerManager*      m_pMarkerManager;
    static CClientPickupManager*      m_pPickupManager;
    static CClientDFFManager*         m_pDFFManager;
    static CClientColModelManager*    m_pColModelManager;
    static CRegisteredCommands*       m_pRegisteredCommands;
    static CClientIMGManager*         m_pImgManager;
    static CClientBuildingManager*    m_pBuildingManager;

protected:
    // Old style: Only warn on failure. This should
    // not be used for new functions. ReturnOnError
    // must be a value to use as result on invalid argument
    template <auto ReturnOnError, auto T>
    static inline int ArgumentParserWarn(lua_State* L)
    {
        return CLuaFunctionParser<false, ReturnOnError, T>()(L, m_pScriptDebugging);
    }

    // Special case for overloads
    // This combines multiple functions into one (via CLuaOverloadParser)
    template <auto ReturnOnError, auto FunctionA, auto FunctionB, auto... Functions>
    static inline int ArgumentParserWarn(lua_State* L)
    {
        // Pad functions to have the same number of parameters by
        // filling both up to the larger number of parameters with dummy_type arguments
        using PaddedFunctionA = pad_func_with_func<FunctionA, FunctionB>;
        using PaddedFunctionB = pad_func_with_func<FunctionB, FunctionA>;
        // Combine functions
        using Overload = CLuaOverloadParser<PaddedFunctionA::Call, PaddedFunctionB::Call>;

        return ArgumentParserWarn<ReturnOnError, Overload::Call, Functions...>(L);
    }

    // New style: hard error on usage mistakes
    template <auto T>
    static inline int ArgumentParser(lua_State* L)
    {
        return CLuaFunctionParser<true, nullptr, T>()(L, m_pScriptDebugging);
    }

    // Special case for overloads
    // This combines multiple functions into one (via CLuaOverloadParser)
    template <auto FunctionA, auto FunctionB, auto... Functions>
    static inline int ArgumentParser(lua_State* L)
    {
        // Pad functions to have the same number of parameters by
        // filling both up to the larger number of parameters with dummy_type arguments
        using PaddedFunctionA = pad_func_with_func<FunctionA, FunctionB>;
        using PaddedFunctionB = pad_func_with_func<FunctionB, FunctionA>;
        // Combine functions
        using Overload = CLuaOverloadParser<PaddedFunctionA::Call, PaddedFunctionB::Call>;

        return ArgumentParser<Overload::Call, Functions...>(L);
    }
};
