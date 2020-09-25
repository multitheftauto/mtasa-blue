#include "StdInc.h"

unsigned int CLuaElementDefs::GetElementCount(EElementCountType elementType)
{
    switch (elementType)
    {
        case ELEMENT_PLAYER:
            return m_pPlayerManager->Count();
        case ELEMENT_PED:
            return m_pPedManager->Count() - m_pPlayerManager->Count();
        case ELEMENT_WATER:
            return m_pWaterManager->Count();
        case ELEMENT_VEHICLE:
            return m_pVehicleManager->Count();
        case ELEMENT_OBJECT:
            return m_pObjectManager->Count();
        case ELEMENT_PICKUP:
            return m_pPickupManager->Count();
        case ELEMENT_MARKER:
            return m_pMarkerManager->Count();
        case ELEMENT_COLSHAPE:
        {
            auto iter = m_pColManager->IterBegin();
            int  count = 0;
            for (; iter != m_pColManager->IterEnd(); iter++)
                if (!(*iter)->GetAutoCallEvent())            // filter away markers and pickups
                    count++;

            return count;
        }
        case ELEMENT_BLIP:
#ifdef MTA_CLIENT
            return m_pRadarMarkerManager->Count();
#else
            return m_pBlipManager->Count();
#endif
        case ELEMENT_RADARAREA:
            return m_pRadarAreaManager->Count();
        case ELEMENT_TEAM:
            return m_pTeamManager->Count();
    }

    throw std::invalid_argument("Invalid element type");
}


int CLuaElementDefs::GetElementData(lua_State* luaVM)
{
    //  var getElementData ( element theElement, string key [, inherit = true] )

#ifdef MTA_CLIENT
    CClientEntity* pElement;
#else
    CElement* pElement;
#endif
    SString strKey;
    bool    bInherit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadBool(bInherit, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                             *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
                strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
            }

#ifdef MTA_CLIENT
            CLuaArgument* pVariable = pElement->GetCustomData(strKey, bInherit);
#else
            CLuaArgument* pVariable = CStaticFunctionDefinitions::GetElementData(pElement, strKey, bInherit);
#endif
            if (pVariable)
            {
                pVariable->Push(luaVM);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::HasElementData(lua_State* luaVM)
{
    //  bool hasElementData ( element theElement, string key [, bool inherit = true ] )

#ifdef MTA_CLIENT
    CClientEntity* pElement;
#else
    CElement* pElement;
#endif
    SString   strKey;
    bool      bInherit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadBool(bInherit, true);

    if (argStream.HasErrors())
    {
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
    {
        // Warn and truncate if key is too long
        m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                     *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
        strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
    }

    // Check if data exists with the given key
    bool exists = pElement->GetCustomData(strKey, bInherit) != nullptr;
    lua_pushboolean(luaVM, exists);
    return 1;
}
