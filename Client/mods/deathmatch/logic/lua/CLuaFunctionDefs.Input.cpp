/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Input.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

int CLuaFunctionDefs::GetCursorPosition(lua_State* luaVM)
{
    CVector2D vecCursor;
    CVector   vecWorld;
    if (CStaticFunctionDefinitions::GetCursorPosition(vecCursor, vecWorld))
    {
        lua_pushnumber(luaVM, (double)vecCursor.fX);
        lua_pushnumber(luaVM, (double)vecCursor.fY);
        lua_pushnumber(luaVM, (double)vecWorld.fX);
        lua_pushnumber(luaVM, (double)vecWorld.fY);
        lua_pushnumber(luaVM, (double)vecWorld.fZ);
        return 5;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetCursorPosition(lua_State* luaVM)
{
    CVector2D        vecPosition;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);

    if (!argStream.HasErrors())
    {
        HWND hookedWindow = g_pCore->GetHookedWindow();

        tagPOINT windowPos = {0};
        ClientToScreen(hookedWindow, &windowPos);

        CVector2D vecResolution = g_pCore->GetGUI()->GetResolution();

        if (vecPosition.fX < 0)
            vecPosition.fX = 0.0f;
        else if (vecPosition.fX > vecResolution.fX)
            vecPosition.fX = vecResolution.fX;
        if (vecPosition.fY < 0)
            vecPosition.fY = 0.0f;
        else if (vecPosition.fY > vecResolution.fY)
            vecPosition.fY = vecResolution.fY;

        g_pCore->CallSetCursorPos((int)vecPosition.fX + (int)windowPos.x, (int)vecPosition.fY + (int)windowPos.y);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::IsCursorShowing(lua_State* luaVM)
{
    bool bShowing = false;
    if (CStaticFunctionDefinitions::IsCursorShowing(bShowing))
    {
        lua_pushboolean(luaVM, bShowing);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetCursorAlpha(lua_State* luaVM)
{
    //  float getCursorAlpha ()
    float fAlpha;

    if (CStaticFunctionDefinitions::GetCursorAlpha(fAlpha))
    {
        lua_pushnumber(luaVM, Round(fAlpha * 255.f));
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetCursorAlpha(lua_State* luaVM)
{
    //  bool setCursorAlpha ( float alpha )
    float fAlpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fAlpha);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCursorAlpha(fAlpha / 255.f))
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

int CLuaFunctionDefs::ShowCursor(lua_State* luaVM)
{
    bool             bShow = false, bToggleControls = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bShow);
    argStream.ReadBool(bToggleControls, true);

    if (!argStream.HasErrors())
    {
        // Get the VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab the resource belonging to this VM
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Show/hide it inside that resource
                pResource->ShowCursor(bShow, bToggleControls);
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Fail
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::BindKey(lua_State* luaVM)
{
    SString          strKey = "", strHitState = "", strCommand = "", strArguments = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadString(strHitState);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (argStream.NextIsString())
            {
                // bindKey ( string key, string keyState, string commandName, [ string arguments ] )
                SString strResource = pLuaMain->GetResource()->GetName();
                argStream.ReadString(strCommand);
                argStream.ReadString(strArguments, "");
                if (!argStream.HasErrors())
                {
                    if (CStaticFunctionDefinitions::BindKey(strKey, strHitState, strCommand, strArguments, strResource))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
            else
            {
                // bindKey ( string key, string keyState, function handlerFunction,  [ var arguments, ... ] )
                CLuaFunctionRef iLuaFunction;
                CLuaArguments   Arguments;
                argStream.ReadFunction(iLuaFunction);
                argStream.ReadLuaArguments(Arguments);
                argStream.ReadFunctionComplete();
                if (!argStream.HasErrors())
                {
                    if (CStaticFunctionDefinitions::BindKey(strKey, strHitState, pLuaMain, iLuaFunction, Arguments))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::UnbindKey(lua_State* luaVM)
{
    SString          strKey = "", strHitState = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (argStream.NextIsString(1))            // Check if has command
            {
                // bool unbindKey ( string key, string keyState, string command )
                SString strResource = pLuaMain->GetResource()->GetName();
                SString strCommand = "";
                argStream.ReadString(strHitState);
                argStream.ReadString(strCommand);
                if (!argStream.HasErrors())
                {
                    if (CStaticFunctionDefinitions::UnbindKey(strKey, strHitState, strCommand, strResource))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
            else
            {
                // bool unbindKey ( string key, [ string keyState, function handler ] )
                CLuaFunctionRef iLuaFunction;
                argStream.ReadString(strHitState, "");
                argStream.ReadFunction(iLuaFunction, LUA_REFNIL);
                argStream.ReadFunctionComplete();
                if (!argStream.HasErrors())
                {
                    const char* szHitState = strHitState == "" ? NULL : strHitState.c_str();
                    if (CStaticFunctionDefinitions::UnbindKey(strKey, pLuaMain, szHitState, iLuaFunction))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetKeyState(lua_State* luaVM)
{
    SString          strKey = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        bool bState;
        if (CStaticFunctionDefinitions::GetKeyState(strKey, bState))
        {
            lua_pushboolean(luaVM, bState);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetAnalogControlState(lua_State* luaVM)
{
    SString          strControlState = "";
    bool             bRawInput;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strControlState);
    argStream.ReadBool(bRawInput, false);

    if (!argStream.HasErrors())
    {
        float fState;
        if (CStaticFunctionDefinitions::GetAnalogControlState(strControlState, fState, bRawInput))
        {
            lua_pushnumber(luaVM, fState);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::SetAnalogControlState(lua_State* luaVM)
{
    //  bool setAnalogControlState ( string controlName [, float state][, bFrameForced] )
    SString          strControlState = "";
    float            fState = 0.0f;
    bool             bForceOverrideNextFrame = false;            // if user input effect should be forcefully overriden for the next frame
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strControlState);

    if (!argStream.HasErrors())
    {
        if (argStream.NextIsNumber())
        {
            argStream.ReadNumber(fState);
            if (argStream.NextIsBool())
                argStream.ReadBool(bForceOverrideNextFrame, false);

            if (CClientPad::SetAnalogControlState(strControlState, fState, bForceOverrideNextFrame))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else if (argStream.NextIsNone())
        {
            CClientPad::RemoveSetAnalogControlState(strControlState);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType(luaVM);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::IsControlEnabled(lua_State* luaVM)
{
    SString          strControlState = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strControlState);

    if (!argStream.HasErrors())
    {
        bool bEnabled;
        if (CStaticFunctionDefinitions::IsControlEnabled(strControlState, bEnabled))
        {
            lua_pushboolean(luaVM, bEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetBoundKeys(lua_State* luaVM)
{
    SString          strKey = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        SBindableGTAControl* pControl = g_pCore->GetKeyBinds()->GetBindableFromControl(strKey);
        // Did we find a control matching the string given?
        if (pControl)
        {
            list<CGTAControlBind*> controlBinds;
            g_pCore->GetKeyBinds()->GetBoundControls(pControl, controlBinds);
            if (!controlBinds.empty())
            {
                lua_newtable(luaVM);
                list<CGTAControlBind*>::iterator iter = controlBinds.begin();
                for (; iter != controlBinds.end(); iter++)
                {
                    lua_pushstring(luaVM, (*iter)->boundKey->szKey);
                    lua_pushstring(luaVM, "down");
                    lua_settable(luaVM, -3);
                }
            }
            else
                lua_pushboolean(luaVM, false);
            return 1;
        }
        // If not, assume its a command
        else
        {
            list<CCommandBind*> commandBinds;
            g_pCore->GetKeyBinds()->GetBoundCommands(strKey, commandBinds);
            if (!commandBinds.empty())
            {
                lua_newtable(luaVM);
                list<CCommandBind*>::iterator iter = commandBinds.begin();
                for (; iter != commandBinds.end(); iter++)
                {
                    lua_pushstring(luaVM, (*iter)->boundKey->szKey);
                    lua_pushstring(luaVM, (*iter)->triggerState ? "down" : "up");
                    lua_settable(luaVM, -3);
                }
            }
            else
                lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetFunctionsBoundToKey(lua_State* luaVM)
{
    SString          strKey = "", strHitState = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadString(strHitState, "");

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            const char* szHitState = strHitState == "" ? NULL : strHitState.c_str();

            bool bCheckHitState = false, bHitState = true;
            if (szHitState)
            {
                if (stricmp(szHitState, "down") == 0)
                    bCheckHitState = true, bHitState = true;
                else if (stricmp(szHitState, "up") == 0)
                    bCheckHitState = true, bHitState = false;
            }

            // Create a new table
            lua_newtable(luaVM);

            // Add all the bound functions to it
            unsigned int                          uiIndex = 0;
            list<CScriptKeyBind*>::const_iterator iter = m_pClientGame->GetScriptKeyBinds()->IterBegin();
            for (; iter != m_pClientGame->GetScriptKeyBinds()->IterEnd(); iter++)
            {
                CScriptKeyBind* pScriptKeyBind = *iter;
                if (!pScriptKeyBind->IsBeingDeleted())
                {
                    switch (pScriptKeyBind->GetType())
                    {
                        case SCRIPT_KEY_BIND_FUNCTION:
                        {
                            CScriptKeyFunctionBind* pBind = static_cast<CScriptKeyFunctionBind*>(pScriptKeyBind);
                            if (!bCheckHitState || pBind->bHitState == bHitState)
                            {
                                if (strcmp(strKey, pBind->boundKey->szKey) == 0)
                                {
                                    lua_pushnumber(luaVM, ++uiIndex);
                                    lua_rawgeti(luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt());
                                    lua_settable(luaVM, -3);
                                }
                            }
                            break;
                        }
                        case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
                        {
                            CScriptControlFunctionBind* pBind = static_cast<CScriptControlFunctionBind*>(pScriptKeyBind);
                            if (!bCheckHitState || pBind->bHitState == bHitState)
                            {
                                if (strcmp(strKey, pBind->boundControl->szControl) == 0)
                                {
                                    lua_pushnumber(luaVM, ++uiIndex);
                                    lua_rawgeti(luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt());
                                    lua_settable(luaVM, -3);
                                }
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetKeyBoundToFunction(lua_State* luaVM)
{
    SString          strKey = "", strHitState = "";
    CScriptArgReader argStream(luaVM);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        if (argStream.NextIsFunction())
        {
            CLuaFunctionRef iLuaFunction = luaM_toref(luaVM, 1);
            // get the key
            list<CScriptKeyBind*>::const_iterator iter = m_pClientGame->GetScriptKeyBinds()->IterBegin();
            for (; iter != m_pClientGame->GetScriptKeyBinds()->IterEnd(); iter++)
            {
                CScriptKeyBind* pScriptKeyBind = *iter;
                if (!pScriptKeyBind->IsBeingDeleted())
                {
                    switch (pScriptKeyBind->GetType())
                    {
                        case SCRIPT_KEY_BIND_FUNCTION:
                        {
                            CScriptKeyFunctionBind* pBind = static_cast<CScriptKeyFunctionBind*>(pScriptKeyBind);
                            // ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
                            if (iLuaFunction == pBind->m_iLuaFunction)
                            {
                                lua_pushstring(luaVM, pBind->boundKey->szKey);
                                return 1;
                            }
                            break;
                        }
                        case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
                        {
                            CScriptControlFunctionBind* pBind = static_cast<CScriptControlFunctionBind*>(pScriptKeyBind);
                            // ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
                            if (iLuaFunction == pBind->m_iLuaFunction)
                            {
                                lua_pushstring(luaVM, pBind->boundControl->szControl);
                                return 1;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            lua_pushboolean(luaVM, false);
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType(luaVM);
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetCommandsBoundToKey(lua_State* luaVM)
{
    SString          strKey = "", strHitState = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadString(strHitState, "");

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            const char* szHitState = strHitState == "" ? NULL : strHitState.c_str();
            bool        bCheckHitState = false, bHitState = true;

            if (szHitState)
            {
                if (stricmp(szHitState, "down") == 0)
                    bCheckHitState = true, bHitState = true;
                else if (stricmp(szHitState, "up") == 0)
                    bCheckHitState = true, bHitState = false;
            }

            // Create a new table
            lua_newtable(luaVM);

            // Add all the bound commands to it
            using KeyBindPtr = CKeyBindsInterface::KeyBindPtr;

            for (const KeyBindPtr& bind : *g_pCore->GetKeyBinds())
            {
                if (bind->isBeingDeleted || !bind->isActive || bind->type != KeyBindType::COMMAND)
                    continue;

                auto commandBind = static_cast<const CCommandBind*>(bind.get());

                if (bCheckHitState && commandBind->triggerState != bHitState)
                    continue;

                if (strKey != commandBind->boundKey->szKey)
                    continue;

                lua_pushstring(luaVM, commandBind->command.c_str());

                if (!commandBind->arguments.empty() && commandBind->wasCreatedByScript)
                    lua_pushstring(luaVM, commandBind->arguments.c_str());
                else
                    lua_pushliteral(luaVM, "");

                lua_settable(luaVM, -3);
            }

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetKeyBoundToCommand(lua_State* luaVM)
{
    SString          strCommand = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strCommand);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            using KeyBindPtr = CKeyBindsInterface::KeyBindPtr;

            for (const KeyBindPtr& bind : *g_pCore->GetKeyBinds())
            {
                if (bind->isBeingDeleted || !bind->isActive || bind->type != KeyBindType::COMMAND)
                    continue;

                auto commandBind = static_cast<const CCommandBind*>(bind.get());

                if (strCommand != commandBind->command)
                    continue;

                lua_pushstring(luaVM, commandBind->boundKey->szKey);
                return 1;
            }

            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::ToggleControl(lua_State* luaVM)
{
    SString          strControl = "";
    bool             bState = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strControl);
    argStream.ReadBool(bState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ToggleControl(strControl, bState))
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

int CLuaFunctionDefs::ToggleAllControls(lua_State* luaVM)
{
    bool             bEnabled = false;
    bool             bGTAControls = true, bMTAControls = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);
    argStream.ReadBool(bGTAControls, true);
    argStream.ReadBool(bMTAControls, true);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ToggleAllControls(bGTAControls, bMTAControls, bEnabled))
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
