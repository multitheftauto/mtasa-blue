/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptKeyBinds.cpp
*  PURPOSE:     Key binds manager
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

SScriptBindableKey g_bkKeys [ NUMBER_OF_KEYS ] = 
{ 
    { "mouse1" }, { "mouse2" }, { "mouse3" }, { "mouse_wheel_up" }, { "mouse_wheel_down" },
    { "mouse4" }, { "mouse5" }, { "backspace" }, { "tab" }, { "lshift" }, { "rshift" },
    { "lctrl" }, { "rctrl" }, { "lalt" }, { "ralt" }, { "pause" }, { "capslock" },
    { "enter" }, { "space" }, { "pgup" }, { "pgdn" }, { "end" }, { "home" }, { "arrow_l" },
    { "arrow_u" }, { "arrow_r" }, { "arrow_d" }, { "insert" }, { "delete" }, { "0" }, 
    { "1" }, { "2" }, { "3" }, { "4" }, { "5" }, { "6" }, { "7" }, { "8" }, { "9" },
    { "a" }, { "b" }, { "c" }, { "d" }, { "e" }, { "f" }, { "g" }, { "h" }, { "i" }, 
    { "j" }, { "k" }, { "l" }, { "m" }, { "n" }, { "o" }, { "p" }, { "q" }, { "r" }, 
    { "s" }, { "t" }, { "u" }, { "v" }, { "w" }, { "x" }, { "y" }, { "z" }, { "num_0" }, 
    { "num_1" }, { "num_2" }, { "num_3" }, { "num_4" }, { "num_5" }, { "num_6" },
    { "num_7" }, { "num_8" }, { "num_9" }, { "num_mul" }, { "num_add" },
    { "num_sep" }, { "num_sub" }, { "num_dec" }, { "num_div" }, { "F1" }, { "F2" }, 
    { "F3" }, { "F4" }, { "F5" }, { "F6" }, { "F7" }, /* { "F8" },  * Used for console */
    { "F9" }, { "F10" }, { "F11" }, { "F12" }, { "scroll" }, { ";" }, { "=" }, { "," }, { "-" },
    { "." }, { "/" }, { "'" }, { "[" }, { "\\" }, { "]" },  { "#" },
    { "" }
};


SScriptBindableGTAControl g_bcControls[] =
{
    { "fire" }, { "next_weapon" }, { "previous_weapon" }, { "forwards" }, { "backwards" },
    { "left" }, { "right" }, { "zoom_in" }, { "zoom_out" }, { "enter_exit" },
    { "change_camera" }, { "jump" }, { "sprint" }, { "look_behind" }, { "crouch" },
    { "action" }, { "walk" }, { "vehicle_fire" }, { "vehicle_secondary_fire" },
    { "vehicle_left" }, { "vehicle_right" }, { "steer_forward" }, { "steer_back" },
    { "accelerate" }, { "brake_reverse" }, { "radio_next" }, { "radio_previous" },
    { "radio_user_track_skip" }, { "horn" }, { "sub_mission" }, { "handbrake" },
    { "vehicle_look_left" }, { "vehicle_look_right" }, { "vehicle_look_behind" },
    { "vehicle_mouse_look" }, { "special_control_left"  }, { "special_control_right" },
    { "special_control_down" }, { "special_control_up" }, { "aim_weapon" },
    { "conversation_yes" }, { "conversation_no" }, { "group_control_forwards" },
    { "group_control_back" }, { "" }
};


CScriptKeyBinds::~CScriptKeyBinds ( void )
{
    Clear ();
}


SScriptBindableKey* CScriptKeyBinds::GetBindableFromKey ( const char* szKey )
{
    for ( int i = 0 ; *g_bkKeys [ i ].szKey != NULL ; i++ )
    {
        SScriptBindableKey* temp = &g_bkKeys [ i ];
        if ( !stricmp ( temp->szKey, szKey ) )
        {
            return temp;
        }
    }

    return NULL;
}


SScriptBindableGTAControl* CScriptKeyBinds::GetBindableFromControl ( const char* szControl )
{
    for ( int i = 0 ; *g_bcControls [ i ].szControl != NULL ; i++ )
    {
        SScriptBindableGTAControl* temp = &g_bcControls [ i ];
        if ( !stricmp ( temp->szControl, szControl ) )
        {
            return temp;
        }
    }

    return NULL;
}


void CScriptKeyBinds::Add ( CScriptKeyBind* pKeyBind )
{
    if ( pKeyBind )
    {
        m_List.push_back ( pKeyBind );
    }
}


void CScriptKeyBinds::Clear ( eScriptKeyBindType bindType )
{
    list < CScriptKeyBind* > ::iterator iter = m_List.begin ();
    while ( iter != m_List.end () )
    {
        if ( !(*iter)->IsBeingDeleted () && bindType == SCRIPT_KEY_BIND_UNDEFINED || (*iter)->GetType () == bindType )
        {
            if ( m_bProcessingKey ) (*iter)->beingDeleted = true;
            else
            {
                delete *iter;
                iter = m_List.erase ( iter );
                continue;
            }
        }
        iter++;
    }
}


void CScriptKeyBinds::Call ( CScriptKeyBind* pKeyBind )
{
    if ( pKeyBind && !pKeyBind->IsBeingDeleted () )
    {
        switch ( pKeyBind->GetType () )
        {
            case SCRIPT_KEY_BIND_FUNCTION:
            {
                CScriptKeyFunctionBind* pBind = static_cast < CScriptKeyFunctionBind* > ( pKeyBind );
                if ( pBind->luaMain && pBind->m_iLuaFunction )
                {
                    CLuaArguments Arguments;
                    Arguments.PushString ( pBind->boundKey->szKey );
                    Arguments.PushString ( ( pBind->bHitState ) ? "down" : "up" );
                    Arguments.PushArguments ( pBind->m_Arguments );
                    Arguments.Call ( pBind->luaMain, pBind->m_iLuaFunction );
                }
                break;
            }
            case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
            {
                CScriptControlFunctionBind* pBind = static_cast < CScriptControlFunctionBind* > ( pKeyBind );
                if ( pBind->luaMain && pBind->m_iLuaFunction )
                {
                    CLuaArguments Arguments;
                    Arguments.PushString ( pBind->boundControl->szControl );
                    Arguments.PushString ( ( pBind->bHitState ) ? "down" : "up" );
                    Arguments.PushArguments ( pBind->m_Arguments );
                    Arguments.Call ( pBind->luaMain, pBind->m_iLuaFunction );
                }
                break;
            }
        }
    }
}


bool CScriptKeyBinds::ProcessKey ( const char* szKey, bool bHitState, eScriptKeyBindType bindType )
{
    m_bProcessingKey = true;
    SScriptBindableKey * pKey = NULL;
    SScriptBindableGTAControl * pControl = NULL;
    if ( bindType == SCRIPT_KEY_BIND_FUNCTION )
    {
        pKey = GetBindableFromKey ( szKey );
        if ( !pKey )
            return false;
    }
    else if ( bindType == SCRIPT_KEY_BIND_CONTROL_FUNCTION )
    {
        pControl = GetBindableFromControl ( szKey );
        if ( !pControl )
            return false;
    }

    bool bFound = false;
    CScriptKeyBind * pKeyBind = NULL;
    list < CScriptKeyBind* > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end () ; ++iter )
    {
        pKeyBind = *iter;
        if ( !pKeyBind->IsBeingDeleted () && pKeyBind->GetType () == bindType )
        {
            switch ( bindType )
            {
                case SCRIPT_KEY_BIND_FUNCTION:
                {
                    CScriptKeyFunctionBind* pBind = static_cast < CScriptKeyFunctionBind* > ( pKeyBind );
                    if ( pBind->boundKey == pKey )
                    {
                        if ( pBind->bHitState == bHitState )
                        {
                            Call ( pBind );
                            bFound = true;
                        }
                    }

                    break;
                }
                case SCRIPT_KEY_BIND_CONTROL_FUNCTION:
                {
                    CScriptControlFunctionBind* pBind = static_cast < CScriptControlFunctionBind* > ( pKeyBind );
                    if ( pBind->boundControl == pControl )
                    {
                        if ( pBind->bHitState == bHitState )
                        {
                            Call ( pBind );                            
                            bFound = true;
                        }
                    }

                    break;
                }
            }
        }
    }
    m_bProcessingKey = false;
    RemoveDeletedBinds ();
    return bFound;
}


bool CScriptKeyBinds::AddKeyFunction ( const char* szKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    if ( szKey == NULL )
        return false;

    SScriptBindableKey* pKey = GetBindableFromKey ( szKey );
    if ( pKey )
    {
        CScriptKeyFunctionBind* pBind = new CScriptKeyFunctionBind;
        pBind->boundKey = pKey;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;        
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back ( pBind );

        return true;
    }
    return false;
}


bool CScriptKeyBinds::AddKeyFunction ( SScriptBindableKey* pKey, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    if ( pKey )
    {
        CScriptKeyFunctionBind* pBind = new CScriptKeyFunctionBind;
        pBind->boundKey = pKey;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;        
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back ( pBind );

        return true;
    }
    return false;
}


bool CScriptKeyBinds::RemoveKeyFunction ( const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    SScriptBindableKey * pKey = GetBindableFromKey ( szKey );
    if ( pKey )
    {
        return RemoveKeyFunction ( pKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction );
    }
    return false;
}


bool CScriptKeyBinds::RemoveKeyFunction ( SScriptBindableKey* pKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    bool bFound = false;
    CScriptKeyFunctionBind* pBind = NULL;
    list < CScriptKeyBind * > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    while ( iter != cloneList.end () )
    {
        if ( !(*iter)->IsBeingDeleted () && (*iter)->GetType () == SCRIPT_KEY_BIND_FUNCTION )
        {
            pBind = static_cast < CScriptKeyFunctionBind* > ( *iter );
            if ( pKey == pBind->boundKey )
            {
                if ( pBind->luaMain == pLuaMain )
                {
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( iLuaFunction == 0 || pBind->m_iLuaFunction == iLuaFunction )
                        {
                            bFound = true;

                            if ( m_bProcessingKey ) pBind->beingDeleted = true;
                            else
                            {
                                m_List.remove ( pBind );
                                delete pBind;
                                iter = cloneList.erase ( iter );
                                continue;
                            }
                        }
                    }
                }
            }
        }
        iter++;
    }
    return bFound;
}


bool CScriptKeyBinds::KeyFunctionExists ( const char* szKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    SScriptBindableKey * pKey = GetBindableFromKey ( szKey );
    if ( pKey )
    {
        return KeyFunctionExists ( pKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction );
    }
    return false;
}


bool CScriptKeyBinds::KeyFunctionExists ( SScriptBindableKey* pKey, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    bool bFound = false;
    list < CScriptKeyBind* > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end () ; ++iter )
    {
        if ( !(*iter)->IsBeingDeleted () && (*iter)->GetType () == SCRIPT_KEY_BIND_FUNCTION )
        {
            CScriptKeyFunctionBind* pBind = static_cast < CScriptKeyFunctionBind* > ( *iter );
            if ( pKey == pBind->boundKey )
            {
                if ( pLuaMain == NULL || pBind->luaMain == pLuaMain )
                {
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( iLuaFunction == 0 || pBind->m_iLuaFunction == iLuaFunction )
                        {
                            bFound = true;
                        }
                    }
                }
            }
        }
    }
    return bFound;
}


void CScriptKeyBinds::RemoveAllKeys ( CLuaMain* pLuaMain )
{
    CScriptKeyBind * pBind = NULL;
    list < CScriptKeyBind * > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    while ( iter != cloneList.end () )
    {
        pBind = *iter;
        if ( !pBind->IsBeingDeleted () && pBind->luaMain == pLuaMain )
        {
            if ( m_bProcessingKey ) pBind->beingDeleted = true;
            else
            {
                m_List.remove ( pBind );
                delete pBind;
                iter = cloneList.erase ( iter );
                continue;
            }
        }
        ++iter;
    }
}


bool CScriptKeyBinds::AddControlFunction ( const char* szControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    if ( szControl == NULL )
        return false;

    SScriptBindableGTAControl* pControl = GetBindableFromControl ( szControl );
    if ( pControl )
    {
        CScriptControlFunctionBind* pBind = new CScriptControlFunctionBind;
        pBind->boundKey = NULL;
        pBind->boundControl = pControl;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back ( pBind );

        return true;
    }
    return false;
}


bool CScriptKeyBinds::AddControlFunction ( SScriptBindableGTAControl* pControl, bool bHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    if ( pControl )
    {
        CScriptControlFunctionBind* pBind = new CScriptControlFunctionBind;
        pBind->boundKey = NULL;
        pBind->boundControl = pControl;
        pBind->bHitState = bHitState;
        pBind->luaMain = pLuaMain;
        pBind->m_iLuaFunction = iLuaFunction;
        pBind->m_Arguments = Arguments;

        m_List.push_back ( pBind );

        return true;
    }
    return false;
}


bool CScriptKeyBinds::RemoveControlFunction ( const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    SScriptBindableGTAControl * pControl = GetBindableFromControl ( szControl );
    if ( pControl )
    {
        return RemoveControlFunction ( pControl, pLuaMain, bCheckHitState, bHitState, iLuaFunction );
    }
    return false;
}


bool CScriptKeyBinds::RemoveControlFunction ( SScriptBindableGTAControl* pControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    bool bFound = false;
    CScriptControlFunctionBind* pBind = NULL;
    list < CScriptKeyBind * > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    while ( iter != cloneList.end () )
    {
        if ( !(*iter)->IsBeingDeleted () && (*iter)->GetType () == SCRIPT_KEY_BIND_CONTROL_FUNCTION )
        {
            pBind = static_cast < CScriptControlFunctionBind* > ( *iter );
            if ( pControl == pBind->boundControl )
            {
                if ( pBind->luaMain == pLuaMain )
                {
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {
                        if ( iLuaFunction == 0 || pBind->m_iLuaFunction == iLuaFunction )
                        {
                            bFound = true;
                            if ( m_bProcessingKey ) (*iter)->beingDeleted = true;
                            else
                            {
                                m_List.remove ( *iter );
                                delete *iter;
                                iter = cloneList.erase ( iter );
                                continue;
                            }
                        }
                    }
                }
            }
        }
        ++iter;
    }
    return bFound;
}


bool CScriptKeyBinds::ControlFunctionExists ( const char* szControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    SScriptBindableGTAControl * pControl = GetBindableFromControl ( szControl );
    if ( pControl )
    {
        return ControlFunctionExists ( pControl, pLuaMain, bCheckHitState, bHitState, iLuaFunction );
    }
    return false;
}


bool CScriptKeyBinds::ControlFunctionExists ( SScriptBindableGTAControl* pControl, CLuaMain* pLuaMain, bool bCheckHitState, bool bHitState, const CLuaFunctionRef& iLuaFunction )
{
    bool bFound = false;
    list < CScriptKeyBind* > cloneList = m_List;
    list < CScriptKeyBind* > ::iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end () ; ++iter )
    {
        if ( !(*iter)->IsBeingDeleted () && (*iter)->GetType () == SCRIPT_KEY_BIND_CONTROL_FUNCTION )
        {
            CScriptControlFunctionBind* pBind = static_cast < CScriptControlFunctionBind* > ( *iter );
            if ( pControl == pBind->boundControl )
            {
                if ( pLuaMain == NULL || pBind->luaMain == pLuaMain )
                {
                    if ( !bCheckHitState || pBind->bHitState == bHitState )
                    {                    
                        if ( iLuaFunction == 0 || pBind->m_iLuaFunction == iLuaFunction )
                        {
                            bFound = true;
                        }
                    }
                }
            }
        }
    }
    return bFound;
}


void CScriptKeyBinds::RemoveDeletedBinds ( void )
{
    list < CScriptKeyBind* > ::iterator iter = m_List.begin ();
    while ( iter != m_List.end () )
    {
        if ( (*iter)->IsBeingDeleted () )
        {
            delete *iter;
            iter = m_List.erase ( iter );
        }
        else
            ++iter;
    }
}

bool CScriptKeyBinds::IsMouse ( SScriptBindableKey* pKey )
{
    if ( !pKey )
        return false;

    const char * pKeyName = pKey->szKey;
    return ( pKeyName[0] == 'm' &&
             pKeyName[1] == 'o' &&
             pKeyName[2] == 'u' &&
             pKeyName[3] == 's' &&
             pKeyName[4] == 'e');
}
