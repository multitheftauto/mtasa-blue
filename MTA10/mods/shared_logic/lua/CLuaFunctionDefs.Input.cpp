/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Input.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

int CLuaFunctionDefs::GetCursorPosition ( lua_State* luaVM )
{
    CVector2D vecCursor;
    CVector vecWorld;
    if ( CStaticFunctionDefinitions::GetCursorPosition ( vecCursor, vecWorld ) )
    {
        lua_pushnumber ( luaVM, ( double ) vecCursor.fX );
        lua_pushnumber ( luaVM, ( double ) vecCursor.fY );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fX );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fY );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fZ );
        return 5;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCursorPosition ( lua_State* luaVM )
{
    float fX = 0.0f;
    float fY = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fX );
    argStream.ReadNumber ( fY );

    if ( !argStream.HasErrors ( ) )
    {
        HWND hookedWindow = g_pCore->GetHookedWindow ();

        tagPOINT windowPos = { 0 };
        ClientToScreen( hookedWindow, &windowPos );

        CVector2D vecResolution = g_pCore->GetGUI ()->GetResolution ();

        if ( fX < 0 )
            fX = 0.0f;
        else if ( fX > vecResolution.fX )
            fX = vecResolution.fX;
        if ( fY < 0 )
            fY = 0.0f;
        else if ( fY > vecResolution.fY )
            fY = vecResolution.fY;

        g_pCore->CallSetCursorPos ( ( int ) fX + ( int ) windowPos.x, ( int ) fY + ( int ) windowPos.y );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsCursorShowing ( lua_State* luaVM )
{
    bool bShowing = false;
    if ( CStaticFunctionDefinitions::IsCursorShowing ( bShowing ) )
    {
        lua_pushboolean ( luaVM, bShowing );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCursorAlpha ( lua_State* luaVM )
{
//  bool setCursorAlpha ( float alpha )
    float fAlpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fAlpha );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetCursorAlpha ( fAlpha ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ShowCursor ( lua_State* luaVM )
{
    bool bShow = false, bToggleControls = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bShow );
    argStream.ReadBool ( bToggleControls, true );

    if ( !argStream.HasErrors ( ) )
    {
        // Get the VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab the resource belonging to this VM
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Show/hide it inside that resource
                pResource->ShowCursor ( bShow, bToggleControls );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Fail
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BindKey ( lua_State* luaVM )
{
    SString strKey = "", strHitState = "", strCommand = "", strArguments = "", strResource = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( argStream.NextIsString ( ) )
            {
                argStream.ReadString ( strCommand );
                argStream.ReadString ( strArguments, "" );
                if ( CStaticFunctionDefinitions::BindKey ( strKey, strHitState, strCommand, strArguments, strResource ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }  
            }
            else
            { 
                // Jax: grab our arguments first, luaM_toref pops the stack!
                CLuaArguments Arguments;
                Arguments.ReadArguments ( luaVM, 4 );
                CLuaFunctionRef iLuaFunction = luaM_toref ( luaVM, 3 );

                if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    if ( CStaticFunctionDefinitions::BindKey ( strKey, strHitState, pLuaMain, iLuaFunction, Arguments ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "function", 3 );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::UnbindKey ( lua_State* luaVM )
{
    SString strKey = "", strHitState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            const char* szHitState = strHitState == "" ? NULL : strHitState;

            if ( argStream.NextIsString ( ) )
            {
                SString strResource = pLuaMain->GetResource()->GetName();
                SString strCommand = "";
                argStream.ReadString ( strCommand );
                if ( CStaticFunctionDefinitions::UnbindKey ( strCommand, szHitState, strCommand, strResource ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
            {   
                CLuaFunctionRef iLuaFunction;
                if ( argStream.NextIsFunction ( ) )
                    iLuaFunction = luaM_toref ( luaVM, 3 );

                if ( IS_REFNIL ( iLuaFunction ) || VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    if ( CStaticFunctionDefinitions::UnbindKey ( strKey, pLuaMain, szHitState, iLuaFunction ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                {
                    m_pScriptDebugging->LogBadType ( luaVM );
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetKeyState ( lua_State * luaVM )
{
    SString strKey = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors ( ) )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetKeyState ( strKey, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetControlState ( lua_State * luaVM )
{
    SString strControlState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControlState );

    if ( !argStream.HasErrors ( ) )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetControlState ( strControlState , bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetAnalogControlState ( lua_State * luaVM )
{
    SString strControlState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControlState );

    if ( !argStream.HasErrors ( ) )
    {
        float fState;
        if ( CStaticFunctionDefinitions::GetAnalogControlState ( strControlState , fState ) )
        {
            lua_pushnumber ( luaVM, fState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetAnalogControlState ( lua_State * luaVM )
{
    SString strControlState = "";
    float fState = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControlState );

    if ( !argStream.HasErrors () )
    {
        if ( argStream.NextIsNumber ( ) )
        {
            argStream.ReadNumber ( fState );
            if ( CClientPad::SetAnalogControlState ( strControlState , fState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( argStream.NextIsNil ( ) )
        {
            CClientPad::RemoveSetAnalogControlState ( strControlState );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsControlEnabled ( lua_State * luaVM )
{
    SString strControlState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControlState );

    if ( !argStream.HasErrors ( ) )
    {
        bool bEnabled;
        if ( CStaticFunctionDefinitions::IsControlEnabled ( strControlState, bEnabled ) )
        {
            lua_pushboolean ( luaVM, bEnabled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBoundKeys ( lua_State * luaVM )
{
    SString strKey = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors ( ) )
    {
        SBindableGTAControl * pControl = g_pCore->GetKeyBinds ()->GetBindableFromControl ( strKey );
        // Did we find a control matching the string given?
        if ( pControl )
        {
            list < CGTAControlBind * > controlBinds;
            g_pCore->GetKeyBinds ()->GetBoundControls ( pControl, controlBinds );
            if ( !controlBinds.empty () )
            {
                lua_newtable ( luaVM );
                list < CGTAControlBind * > ::iterator iter = controlBinds.begin ();
                for ( ; iter != controlBinds.end () ; iter++ )
                {
                    lua_pushstring ( luaVM, (*iter)->boundKey->szKey );
                    lua_pushstring ( luaVM, "down" );
                    lua_settable ( luaVM, -3 );
                }
            }
            else
                lua_pushboolean ( luaVM, false );
            return 1;
        }
        // If not, assume its a command
        else
        {
            list < CCommandBind * > commandBinds;
            g_pCore->GetKeyBinds ()->GetBoundCommands ( strKey, commandBinds );
            if ( !commandBinds.empty () )
            {
                lua_newtable ( luaVM );
                list < CCommandBind * > ::iterator iter = commandBinds.begin ();
                for ( ; iter != commandBinds.end () ; iter++ )
                {
                    lua_pushstring ( luaVM, (*iter)->boundKey->szKey );
                    lua_pushstring ( luaVM, (*iter)->bHitState ? "down" : "up" );
                    lua_settable ( luaVM, -3 );
                }
            }
            else
                lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetFunctionsBoundToKey ( lua_State* luaVM )
{
    SString strKey = "", strHitState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            const char* szHitState = strHitState == "" ? NULL : strHitState;

            bool bCheckHitState = false, bHitState = true;
            if ( szHitState )
            {
                if ( stricmp ( szHitState, "down" ) == 0 )
                    bCheckHitState = true, bHitState = true;
                else if ( stricmp ( szHitState, "up" ) == 0 )
                    bCheckHitState = true, bHitState = false;
            }

            // Create a new table
            lua_newtable ( luaVM );

            // Add all the bound functions to it
            unsigned int uiIndex = 0;
            list < CScriptKeyBind* > ::const_iterator iter = m_pClientGame->GetScriptKeyBinds ()->IterBegin ();
            for ( ; iter != m_pClientGame->GetScriptKeyBinds ()->IterEnd (); iter++ )
            {
                CScriptKeyBind* pScriptKeyBind = *iter;
                if ( !pScriptKeyBind->IsBeingDeleted () )
                {
                    switch ( pScriptKeyBind->GetType () )
                    {
                        case SCRIPT_KEY_BIND_FUNCTION:
                        {
                            CScriptKeyFunctionBind* pBind = static_cast < CScriptKeyFunctionBind* > ( pScriptKeyBind );
                            if ( !bCheckHitState || pBind->bHitState == bHitState )
                            {
                                if ( strcmp ( strKey, pBind->boundKey->szKey ) == 0 )
                                {
                                    lua_pushnumber ( luaVM, ++uiIndex );
                                    lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                                    lua_settable ( luaVM, -3 );
                                }
                            }
                            break;
                        }
                        case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
                        {
                            CScriptControlFunctionBind* pBind = static_cast < CScriptControlFunctionBind* > ( pScriptKeyBind );
                            if ( !bCheckHitState || pBind->bHitState == bHitState )
                            {
                                if ( strcmp ( strKey, pBind->boundControl->szControl ) == 0 )
                                {
                                    lua_pushnumber ( luaVM, ++uiIndex );
                                    lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                                    lua_settable ( luaVM, -3 );
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetKeyBoundToFunction ( lua_State* luaVM )
{
    SString strKey = "", strHitState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( argStream.NextIsFunction ( ) )
            {
                CLuaFunctionRef iLuaFunction = luaM_toref ( luaVM, 1 );
                // get the key
                list < CScriptKeyBind* > ::const_iterator iter =  m_pClientGame->GetScriptKeyBinds ()->IterBegin ();
                for ( ; iter !=  m_pClientGame->GetScriptKeyBinds ()->IterEnd (); iter++ )
                {
                    CScriptKeyBind* pScriptKeyBind = *iter;
                    if ( !pScriptKeyBind->IsBeingDeleted () )
                    {
                        switch ( pScriptKeyBind->GetType () )
                        {
                            case SCRIPT_KEY_BIND_FUNCTION:
                            {
                                CScriptKeyFunctionBind* pBind = static_cast < CScriptKeyFunctionBind* > ( pScriptKeyBind );
                                // ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
                                if ( iLuaFunction == pBind->m_iLuaFunction )
                                {
                                    lua_pushstring ( luaVM, pBind->boundKey->szKey );
                                    return 1;
                                }
                                break;
                            }
                            case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
                            {
                                CScriptControlFunctionBind* pBind = static_cast < CScriptControlFunctionBind* > ( pScriptKeyBind );
                                // ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
                                if ( iLuaFunction == pBind->m_iLuaFunction )
                                {
                                    lua_pushstring ( luaVM, pBind->boundKey->szKey );
                                    return 1;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                lua_pushboolean ( luaVM, false );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetCommandsBoundToKey ( lua_State* luaVM )
{
    SString strKey = "", strHitState = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strHitState, "" );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            const char* szHitState = strHitState == "" ? NULL : strHitState;

            bool bCheckHitState = false, bHitState = true;
            if ( szHitState )
            {
                if ( stricmp ( szHitState, "down" ) == 0 )
                    bCheckHitState = true, bHitState = true;
                else if ( stricmp ( szHitState, "up" ) == 0 )
                    bCheckHitState = true, bHitState = false;
            }

            // Create a new table
            lua_newtable ( luaVM );

            // Add all the bound functions to it
            unsigned int uiIndex = 0;
            list < CKeyBind* > ::const_iterator iter = g_pCore->GetKeyBinds ()->IterBegin ();
            for ( ; iter != g_pCore->GetKeyBinds ()->IterEnd (); iter++ )
            {
                CKeyBind* pKeyBind = *iter;
                if ( !pKeyBind->IsBeingDeleted () && pKeyBind->GetType () == KEY_BIND_COMMAND )
                {
                    CCommandBind* pBind = static_cast < CCommandBind* > ( pKeyBind );
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( strcmp ( strKey, pBind->boundKey->szKey ) == 0 )
                        {
                            lua_pushstring ( luaVM, pBind->szCommand );
                            lua_pushstring ( luaVM, pBind->szArguments );
                            lua_settable ( luaVM, -3 );
                        }
                    }
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetKeyBoundToCommand ( lua_State* luaVM )
{
    SString strCommand = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strCommand );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // get the key
            list < CKeyBind* > ::const_iterator iter = g_pCore->GetKeyBinds ()->IterBegin ();
            for ( ; iter != g_pCore->GetKeyBinds ()->IterEnd (); iter++ )
            {
                CKeyBind* pKeyBind = *iter;
                if ( !pKeyBind->IsBeingDeleted () )
                {
                    if ( pKeyBind->GetType () == KEY_BIND_COMMAND )
                    {
                        CCommandBind* pBind = static_cast < CCommandBind* > ( pKeyBind );
                        if ( strcmp ( strCommand, pBind->szCommand ) == 0 )
                        {
                            lua_pushstring ( luaVM, pBind->boundKey->szKey );
                            return 1;
                        }
                    }
                }
            }
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetControlState ( lua_State * luaVM )
{
    SString strControl = "";
    bool bState = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetControlState ( strControl, bState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ToggleControl ( lua_State * luaVM )
{
    SString strControl = "";
    bool bState = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ToggleControl ( strControl, bState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ToggleAllControls ( lua_State* luaVM )
{
    bool bEnabled = false;
    bool bGTAControls = true, bMTAControls = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bEnabled );
    argStream.ReadBool ( bGTAControls, true );
    argStream.ReadBool ( bMTAControls, true );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ToggleAllControls ( bGTAControls, bMTAControls, bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}
