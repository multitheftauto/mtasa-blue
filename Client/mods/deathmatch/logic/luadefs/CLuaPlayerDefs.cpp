/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPlayerDefs.cpp
 *  PURPOSE:     Lua player definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaPlayerDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Player get funcs
        {"getLocalPlayer", GetLocalPlayer},
        {"getPlayerName", GetPlayerName},
        {"getPlayerNametagText", GetPlayerNametagText},
        {"getPlayerNametagColor", GetPlayerNametagColor},
        {"isPlayerNametagShowing", IsPlayerNametagShowing},
        {"getPlayerPing", GetPlayerPing},
        {"getPlayerTeam", GetPlayerTeam},
        {"getPlayerFromNick", GetPlayerFromName},
        {"getPlayerFromName", GetPlayerFromName},
        {"isPlayerHudComponentVisible", IsPlayerHudComponentVisible},
        {"getPlayerMoney", GetPlayerMoney},
        {"getPlayerWantedLevel", GetPlayerWantedLevel},
        {"getPlayerScriptDebugLevel", ArgumentParser<GetPlayerScriptDebugLevel>},
        {"isPlayerCrosshairVisible", ArgumentParser<IsPlayerCrosshairVisible>},

        // Player set funcs
        {"showPlayerHudComponent", ShowPlayerHudComponent},
        {"setPlayerHudComponentVisible", ShowPlayerHudComponent},
        {"setPlayerMoney", SetPlayerMoney},
        {"givePlayerMoney", GivePlayerMoney},
        {"takePlayerMoney", TakePlayerMoney},
        {"setPlayerNametagText", SetPlayerNametagText},
        {"setPlayerNametagColor", SetPlayerNametagColor},
        {"setPlayerNametagShowing", SetPlayerNametagShowing},
        {"setPlayerHudComponentProperty", ArgumentParser<SetPlayerHudComponentProperty>},
        {"resetPlayerHudComponentProperty", ArgumentParser<ResetPlayerHudComponentProperty>},

        // Community funcs
        {"getPlayerUserName", GetPlayerUserName},
        {"getPlayerSerial", GetPlayerSerial},

        // Map funcs
        {"forcePlayerMap", ForcePlayerMap},
        {"isPlayerMapForced", IsPlayerMapForced},
        {"isPlayerMapVisible", IsPlayerMapVisible},
        {"getPlayerMapBoundingBox", GetPlayerMapBoundingBox},
        {"getPlayerMapOpacity", ArgumentParser<GetPlayerMapOpacity>},
        {"getPlayerHudComponentProperty", ArgumentParser<GetPlayerHudComponentProperty>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPlayerDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // localPlayer only
    lua_classfunction(luaVM, "giveMoney", "givePlayerMoney");
    lua_classfunction(luaVM, "takeMoney", "takePlayerMoney");
    lua_classfunction(luaVM, "showHudComponent", "showPlayerHudComponent");
    lua_classfunction(luaVM, "setMoney", "setPlayerMoney");
    lua_classfunction(luaVM, "getMoney", "getPlayerMoney");
    lua_classfunction(luaVM, "getSerial", "getPlayerSerial");
    lua_classfunction(luaVM, "getWantedLevel", "getPlayerWantedLevel");
    lua_classfunction(luaVM, "getMapBoundingBox", "getPlayerMapBoundingBox");
    lua_classfunction(luaVM, "getMapOpacity", "getPlayerMapOpacity");
    lua_classfunction(luaVM, "forceMap", "forcePlayerMap");
    lua_classfunction(luaVM, "isMapForced", "isPlayerMapForced");
    lua_classfunction(luaVM, "isMapVisible", "isPlayerMapVisible");
    lua_classfunction(luaVM, "isHudComponentVisible", "isPlayerHudComponentVisible");
    lua_classfunction(luaVM, "toggleControl", "toggleControl");
    lua_classfunction(luaVM, "setHudComponentProperty", "setPlayerHudComponentProperty");
    lua_classfunction(luaVM, "resetHudComponentProperty", "resetPlayerHudComponentProperty");

    lua_classfunction(luaVM, "create", "getPlayerFromName");

    lua_classfunction(luaVM, "setNametagText", "setPlayerNametagText");
    lua_classfunction(luaVM, "setNametagShowing", "setPlayerNametagShowing");
    lua_classfunction(luaVM, "setNametagColor", "setPlayerNametagColor");

    lua_classfunction(luaVM, "getPing", "getPlayerPing");
    lua_classfunction(luaVM, "getName", "getPlayerName");
    lua_classfunction(luaVM, "getTeam", "getPlayerTeam");
    lua_classfunction(luaVM, "getNametagText", "getPlayerNametagText");
    lua_classfunction(luaVM, "getNametagColor", "getPlayerNametagColor");
    lua_classfunction(luaVM, "getScriptDebugLevel", "getPlayerScriptDebugLevel");
    lua_classfunction(luaVM, "getHudComponentProperty", "getPlayerHudComponentProperty");

    lua_classfunction(luaVM, "isNametagShowing", "isPlayerNametagShowing");
    lua_classfunction(luaVM, "isCrosshairVisible", "isPlayerCrosshairVisible");

    lua_classvariable(luaVM, "ping", NULL, "getPlayerPing");
    lua_classvariable(luaVM, "name", NULL, "getPlayerName");
    lua_classvariable(luaVM, "team", NULL, "getPlayerTeam");
    lua_classvariable(luaVM, "scriptDebugLevel", nullptr, "getPlayerScriptDebugLevel");
    lua_classvariable(luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText");
    lua_classvariable(luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing");
    lua_classvariable(luaVM, "crosshairVisible", nullptr, "isPlayerCrosshairVisible");

    lua_registerclass(luaVM, "Player", "Ped");
}

int CLuaPlayerDefs::GetLocalPlayer(lua_State* luaVM)
{
    CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetLocalPlayer();

    // Return the local player
    lua_pushelement(luaVM, pPlayer);
    return 1;
}

int CLuaPlayerDefs::GetPlayerName(lua_State* luaVM)
{
    //  string getPlayerName ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Grab his nametag text and return it
        const char* szName = pPlayer->GetNick();
        if (szName)
        {
            lua_pushstring(luaVM, szName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerFromName(lua_State* luaVM)
{
    //  player getPlayerFromName ( string playerName )
    SString strNick;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strNick);

    if (!argStream.HasErrors())
    {
        // Grab the player with that nick
        CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName(strNick);
        if (pPlayer)
        {
            // Return the player
            lua_pushelement(luaVM, pPlayer);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Doesn't exist
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerNametagText(lua_State* luaVM)
{
    //  string getPlayerNametagText ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Grab his nametag text and return it
        const char* szName = pPlayer->GetNametagText();
        if (szName)
        {
            lua_pushstring(luaVM, szName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerNametagColor(lua_State* luaVM)
{
    //  int, int, int getPlayerNametagColor ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Grab his nametag color and return it
        unsigned char ucR, ucG, ucB;
        pPlayer->GetNametagColor(ucR, ucG, ucB);

        lua_pushnumber(luaVM, ucR);
        lua_pushnumber(luaVM, ucG);
        lua_pushnumber(luaVM, ucB);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::IsPlayerNametagShowing(lua_State* luaVM)
{
    //  bool isPlayerNametagShowing ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        bool bIsNametagShowing = pPlayer->IsNametagShowing();
        lua_pushboolean(luaVM, bIsNametagShowing);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerPing(lua_State* luaVM)
{
    //  int getPlayerPing ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Grab his ping
        unsigned int uiPing = pPlayer->GetPing();
        lua_pushnumber(luaVM, uiPing);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerTeam(lua_State* luaVM)
{
    //  team getPlayerTeam ( player thePlayer )
    CClientPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        // Grab his team and return it
        CClientTeam* pTeam = pPlayer->GetTeam();
        if (pTeam)
        {
            lua_pushelement(luaVM, pTeam);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerMoney(lua_State* luaVM)
{
    long lMoney;
    if (CStaticFunctionDefinitions::GetPlayerMoney(lMoney))
    {
        lua_pushnumber(luaVM, lMoney);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerWantedLevel(lua_State* luaVM)
{
    char cWanted;
    if (CStaticFunctionDefinitions::GetPlayerWantedLevel(cWanted))
    {
        lua_pushnumber(luaVM, cWanted);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

std::uint8_t CLuaPlayerDefs::GetPlayerScriptDebugLevel() noexcept
{
    return g_pClientGame->GetPlayerManager()->GetLocalPlayer()->GetPlayerScriptDebugLevel();
}

int CLuaPlayerDefs::ShowPlayerHudComponent(lua_State* luaVM)
{
    //  bool showPlayerHudComponent ( string component, bool show )
    eHudComponent component;
    bool          bShow;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(component);
    argStream.ReadBool(bShow);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ShowPlayerHudComponent(component, bShow))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::IsPlayerHudComponentVisible(lua_State* luaVM)
{
    //  bool isPlayerHudComponentVisible ( string componen )
    eHudComponent component;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(component);

    if (!argStream.HasErrors())
    {
        bool bIsVisible;
        if (CStaticFunctionDefinitions::IsPlayerHudComponentVisible(component, bIsVisible))
        {
            lua_pushboolean(luaVM, bIsVisible);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::SetPlayerMoney(lua_State* luaVM)
{
    //  bool setPlayerMoney ( int amount, bool instant = false )
    int  lMoney;
    bool bInstant;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(lMoney);
    argStream.ReadBool(bInstant, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPlayerMoney(lMoney, bInstant))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GivePlayerMoney(lua_State* luaVM)
{
    //  bool givePlayerMoney ( int amount )
    int lMoney;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(lMoney);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GivePlayerMoney(lMoney))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::TakePlayerMoney(lua_State* luaVM)
{
    //  bool takePlayerMoney ( int amount )
    int lMoney;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(lMoney);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::TakePlayerMoney(lMoney))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::SetPlayerNametagText(lua_State* luaVM)
{
    //  bool setPlayerNametagText ( player thePlayer, string text )
    CClientEntity* pPlayer;
    SString        strText;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadString(strText);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPlayerNametagText(*pPlayer, strText))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::SetPlayerNametagColor(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    if (!argStream.NextIsBool(1))
    {
        // Call type 1
        //  bool setPlayerNametagColor ( player thePlayer, int r, int g, int b )
        CClientEntity* pPlayer;
        int            iR;
        int            iG;
        int            iB;

        argStream.ReadUserData(pPlayer);
        argStream.ReadNumber(iR);
        argStream.ReadNumber(iG);
        argStream.ReadNumber(iB);

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetPlayerNametagColor(*pPlayer, false, iR, iG, iB))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  bool setPlayerNametagColor ( player thePlayer, false )
        CClientEntity* pPlayer;
        bool           bFalse;

        argStream.ReadUserData(pPlayer);
        argStream.ReadBool(bFalse);

        if (!argStream.HasErrors())
        {
            if (bFalse == false)
            {
                if (CStaticFunctionDefinitions::SetPlayerNametagColor(*pPlayer, true, 255, 255, 255))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::SetPlayerNametagShowing(lua_State* luaVM)
{
    //  bool setPlayerNametagShowing ( player thePlayer, bool showing )
    CClientEntity* pPlayer;
    bool           bShowing;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bShowing);

    if (!argStream.HasErrors())
    {
        // Set the new rotation
        if (CStaticFunctionDefinitions::SetPlayerNametagShowing(*pPlayer, bShowing))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

// Community

int CLuaPlayerDefs::GetPlayerUserName(lua_State* luaVM)
{
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerSerial(lua_State* luaVM)
{
    char szSerial[64];
    g_pCore->GetNetwork()->GetSerial(szSerial, sizeof(szSerial));

    lua_pushstring(luaVM, szSerial);
    return 1;
}

// Player Map

int CLuaPlayerDefs::ForcePlayerMap(lua_State* luaVM)
{
    bool             bForced;
    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bForced);

    if (!argStream.HasErrors())
    {
        // Force the map to open or close
        if (CStaticFunctionDefinitions::ForcePlayerMap(bForced))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::IsPlayerMapForced(lua_State* luaVM)
{
    bool bForced;
    if (CStaticFunctionDefinitions::IsPlayerMapForced(bForced))
    {
        lua_pushboolean(luaVM, bForced);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::IsPlayerMapVisible(lua_State* luaVM)
{
    bool bForced;
    if (CStaticFunctionDefinitions::IsPlayerMapVisible(bForced))
    {
        lua_pushboolean(luaVM, bForced);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPlayerDefs::GetPlayerMapBoundingBox(lua_State* luaVM)
{
    // Grab the bounding box and return it
    CVector vecMin, vecMax;
    if (CStaticFunctionDefinitions::GetPlayerMapBoundingBox(vecMin, vecMax))
    {
        lua_pushnumber(luaVM, vecMin.fX);
        lua_pushnumber(luaVM, vecMin.fY);
        lua_pushnumber(luaVM, vecMax.fX);
        lua_pushnumber(luaVM, vecMax.fY);
        return 4;
    }
    // The map is invisible
    lua_pushboolean(luaVM, false);
    return 1;
}

unsigned char CLuaPlayerDefs::GetPlayerMapOpacity()
{
    int iMapOpacity = g_pCore->GetCVars()->GetValue<int>("mapalpha");
    return static_cast<unsigned char>(Clamp(0, iMapOpacity, 255));
}

bool CLuaPlayerDefs::IsPlayerCrosshairVisible()
{
    return g_pGame->GetHud()->IsCrosshairVisible();
}

bool CLuaPlayerDefs::SetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property, std::variant<CVector2D, float, bool, std::string> value)
{
    if (component == HUD_ALL || component == HUD_CROSSHAIR || component == HUD_VITAL_STATS || component == HUD_HELP_TEXT || component == HUD_RADAR)
        return false;

    CHud* hud = g_pGame->GetHud();

    switch (property)
    {
        case eHudComponentProperty::POSITION:
        {
            if (!std::holds_alternative<CVector2D>(value))
                return false;

            hud->SetComponentPosition(component, std::get<CVector2D>(value));
            return true;
        }
        case eHudComponentProperty::SIZE:
        {
            if (!std::holds_alternative<CVector2D>(value))
                return false;

            hud->SetComponentSize(component, std::get<CVector2D>(value));
            return true;
        }
        case eHudComponentProperty::FILL_COLOR:
        case eHudComponentProperty::FILL_COLOR_SECONDARY:
        {
            if (!hud->IsComponentBar(component) && !hud->IsComponentText(component) && component != HUD_WEAPON)
                return false;

            if (!std::holds_alternative<float>(value))
                return false;

            hud->SetComponentColor(component, static_cast<std::uint32_t>(std::get<float>(value)), property == eHudComponentProperty::FILL_COLOR_SECONDARY);
            return true;
        }
        case eHudComponentProperty::DRAW_BLACK_BORDER:
        {
            if (!hud->IsComponentBar(component))
                return false;

            if (!std::holds_alternative<bool>(value))
                return false;

            hud->SetComponentDrawBlackBorder(component, std::get<bool>(value));
            return true;
        }
        case eHudComponentProperty::DRAW_PERCENTAGE:
        {
            if (!hud->IsComponentBar(component))
                return false;

            if (!std::holds_alternative<bool>(value))
                return false;

            hud->SetComponentDrawPercentage(component, std::get<bool>(value));
            return true;
        }
        case eHudComponentProperty::BLINKING_HP_VALUE:
        {
            if (component != HUD_HEALTH)
                return false;

            if (!std::holds_alternative<float>(value))
                return false;

            hud->SetHealthBarBlinkingValue(std::get<float>(value));
            return true;
        }
        case eHudComponentProperty::DROP_COLOR:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<float>(value))
                return false;

            hud->SetComponentFontDropColor(component, static_cast<std::uint32_t>(std::get<float>(value)));
            return true;
        }
        case eHudComponentProperty::TEXT_OUTLINE:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<float>(value))
                return false;

            hud->SetComponentFontOutline(component, std::get<float>(value));
            return true;
        }
        case eHudComponentProperty::TEXT_SHADOW:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<float>(value))
                return false;

            hud->SetComponentFontShadow(component, std::get<float>(value));
            return true;
        }
        case eHudComponentProperty::TEXT_STYLE:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<std::string>(value))
                return false;

            eFontStyle val;
            if (!StringToEnum(std::get<std::string>(value), val))
                return false;

            // I don't know why, but calling StringToEnum causes the "hud" pointer to become invalid, leading to a crash
            g_pGame->GetHud()->SetComponentFontStyle(component, val);
            return true;
        }
        case eHudComponentProperty::TEXT_ALIGNMENT:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<std::string>(value))
                return false;

            eFontAlignment val;
            if (!StringToEnum(std::get<std::string>(value), val))
                return false;

            // I don't know why, but calling StringToEnum causes the "hud" pointer to become invalid, leading to a crash
            g_pGame->GetHud()->SetComponentFontAlignment(component, val);
            return true;
        }
        case eHudComponentProperty::TEXT_PROPORTIONAL:
        {
            if (!hud->IsComponentText(component))
                return false;

            if (!std::holds_alternative<bool>(value))
                return false;

            hud->SetComponentFontProportional(component, std::get<bool>(value));
            return true;
        }
        case eHudComponentProperty::CUSTOM_ALPHA:
        {
            if (!std::holds_alternative<bool>(value))
                return false;

            hud->SetComponentUseCustomAlpha(component, std::get<bool>(value));
            return true;
        }
    }

    return false;
}

bool CLuaPlayerDefs::ResetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property) noexcept
{
    if (component == HUD_CROSSHAIR || component == HUD_VITAL_STATS || component == HUD_HELP_TEXT || component == HUD_RADAR)
        return false;

    CHud* hud = g_pGame->GetHud();

    if (component == HUD_ALL)
    {
        for (std::size_t iComp = 0; iComp < static_cast<std::size_t>(HUD_HELP_TEXT); iComp++)
        {
            eHudComponent comp = static_cast<eHudComponent>(iComp);
            if (comp == HUD_ALL)
                continue;

            ResetPlayerHudComponentProperty(comp, property);
        }

        return true;
    }

    switch (property)
    {
        case eHudComponentProperty::ALL_PROPERTIES:
        {
            for (std::size_t i = 0; i < static_cast<std::size_t>(eHudComponentProperty::ALL_PROPERTIES); i++)
                ResetPlayerHudComponentProperty(component, static_cast<eHudComponentProperty>(i));

            return true;
        }
        case eHudComponentProperty::POSITION:
            hud->ResetComponentPlacement(component, false);
            return true;
        case eHudComponentProperty::SIZE:
            hud->ResetComponentPlacement(component, true);
            return true;
        case eHudComponentProperty::FILL_COLOR:
        case eHudComponentProperty::FILL_COLOR_SECONDARY:
        {
            if (!hud->IsComponentBar(component) && !hud->IsComponentText(component) && component != HUD_WEAPON)
                return false;

            bool second = property == eHudComponentProperty::FILL_COLOR_SECONDARY;
            if (second && (component != HUD_RADIO && component != HUD_MONEY && component != HUD_WANTED && component != HUD_WEAPON))
                return false;

            hud->ResetComponentColor(component, second);
            return true;
        }
        case eHudComponentProperty::DROP_COLOR:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->SetComponentFontDropColor(component, COLOR_RGBA(0, 0, 0, 255));
            return true;
        }
        case eHudComponentProperty::DRAW_BLACK_BORDER:
        {
            if (!hud->IsComponentBar(component))
                return false;

            hud->SetComponentDrawBlackBorder(component, true);
            return true;
        }
        case eHudComponentProperty::DRAW_PERCENTAGE:
        {
            if (!hud->IsComponentBar(component))
                return false;

            hud->SetComponentDrawPercentage(component, false);
            return true;
        }
        case eHudComponentProperty::BLINKING_HP_VALUE:
        {
            if (!component != HUD_HEALTH)
                return false;

            hud->SetHealthBarBlinkingValue(10.0f);
            return true;
        }
        case eHudComponentProperty::TEXT_OUTLINE:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->ResetComponentFontOutline(component);
            return true;
        }
        case eHudComponentProperty::TEXT_SHADOW:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->ResetComponentFontShadow(component);
            return true;
        }
        case eHudComponentProperty::TEXT_STYLE:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->ResetComponentFontStyle(component);
            return true;
        }
        case eHudComponentProperty::TEXT_ALIGNMENT:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->ResetComponentFontAlignment(component);
            return true;
        }
        case eHudComponentProperty::TEXT_PROPORTIONAL:
        {
            if (!hud->IsComponentText(component))
                return false;

            hud->ResetComponentFontProportional(component);
            return true;
        }
        case eHudComponentProperty::CUSTOM_ALPHA:
            hud->SetComponentUseCustomAlpha(component, false);
            return true;
    }

    return false;
} 

std::variant<float, bool, std::string, CLuaMultiReturn<float, float>, CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>> CLuaPlayerDefs::GetPlayerHudComponentProperty(eHudComponent component, eHudComponentProperty property)
{
    if (component == HUD_ALL || component == HUD_CROSSHAIR || component == HUD_VITAL_STATS || component == HUD_HELP_TEXT || component == HUD_RADAR)
        return false;

    CHud* hud = g_pGame->GetHud();

    switch (property)
    {
        case eHudComponentProperty::POSITION:
        {
            CVector2D& pos = hud->GetComponentPosition(component);
            return CLuaMultiReturn<float, float>{pos.fX, pos.fY};
        }
        case eHudComponentProperty::SIZE:
        {
            CVector2D& size = hud->GetComponentSize(component);
            return CLuaMultiReturn<float, float>{size.fX, size.fY};
        }
        case eHudComponentProperty::FILL_COLOR:
        {
            if (!hud->IsComponentBar(component) && !hud->IsComponentText(component) && component != HUD_WEAPON)
                return false;

            SColor& color = hud->GetComponentColor(component);
            return CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>{color.R, color.G, color.B, color.A};
        }
        case eHudComponentProperty::FILL_COLOR_SECONDARY:
        {
            if (component != HUD_RADIO && component != HUD_MONEY && component != HUD_WANTED && component != HUD_WEAPON)
                return false;

            SColor& color = hud->GetComponentSecondaryColor(component);
            return CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>{color.R, color.G, color.B, color.A};
        }
        case eHudComponentProperty::DROP_COLOR:
        {
            if (!hud->IsComponentText(component))
                return false;

            SColor& color = hud->GetComponentFontDropColor(component);
            return CLuaMultiReturn<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>{color.R, color.G, color.B, color.A};
        }
        case eHudComponentProperty::DRAW_BLACK_BORDER:
        {
            if (!hud->IsComponentBar(component))
                return false;

            return hud->GetComponentDrawBlackBorder(component);
        }
        case eHudComponentProperty::DRAW_PERCENTAGE:
        {
            if (!hud->IsComponentBar(component))
                return false;

            return hud->GetComponentDrawPercentage(component);
        }
        case eHudComponentProperty::BLINKING_HP_VALUE:
        {
            if (component != HUD_HEALTH)
                return false;

            return hud->GetHealthBarBlinkingValue(component);
        }
        case eHudComponentProperty::TEXT_OUTLINE:
        {
            if (!hud->IsComponentText(component))
                return false;

            return hud->GetComponentFontOutline(component);
        }
        case eHudComponentProperty::TEXT_SHADOW:
        {
            if (!hud->IsComponentText(component))
                return false;

            return hud->GetComponentFontShadow(component);
        }
        case eHudComponentProperty::TEXT_STYLE:
        {
            if (!hud->IsComponentText(component))
                return false;

            return EnumToString(hud->GetComponentFontStyle(component));
        }
        case eHudComponentProperty::TEXT_ALIGNMENT:
        {
            if (!hud->IsComponentText(component))
                return false;

            return EnumToString(hud->GetComponentFontAlignment(component));
        }
        case eHudComponentProperty::TEXT_PROPORTIONAL:
        {
            if (!hud->IsComponentText(component))
                return false;

            return hud->GetComponentFontProportional(component);
        }
        case eHudComponentProperty::CUSTOM_ALPHA:
            return hud->GetComponentUseCustomAlpha(component);
        case eHudComponentProperty::TEXT_SIZE:
        {
            CVector2D& size = hud->GetComponentTextSize(component);
            return CLuaMultiReturn<float, float>{size.fX, size.fY};
        }
    }

    return false;
}
