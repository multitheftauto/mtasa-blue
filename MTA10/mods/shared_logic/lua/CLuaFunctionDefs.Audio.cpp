/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Audio.cpp
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
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::PlaySound ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        SString strSound = lua_tostring ( luaVM, 1 );
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
                SString strFilename;
                if ( CResourceManager::ParseResourcePathInput( strSound, pResource, strFilename ) )
                {
                    bool bLoop = false;
                    if ( lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
                    {
                        bLoop = ( lua_toboolean ( luaVM, 2 ) ) ? true : false;
                    }
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound ( pResource, strFilename, bLoop );
                    if ( pSound )
                    {
                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlaySound3D ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_istype ( luaVM, 1, LUA_TSTRING ) ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        SString strSound = lua_tostring ( luaVM, 1 );

        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
                SString strFilename;
                if ( CResourceManager::ParseResourcePathInput( strSound, pResource, strFilename ) )
                {
                    bool bLoop = false;
                    if ( lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
                    {
                        bLoop = ( lua_toboolean ( luaVM, 5 ) ) ? true : false;
                    }
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D ( pResource, strFilename, vecPosition, bLoop );
                    if ( pSound )
                    {
                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::StopSound ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::StopSound ( *pSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiPosition = ( unsigned int ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundPosition ( *pSound, uiPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiPosition = 0;
            if ( CStaticFunctionDefinitions::GetSoundPosition ( *pSound, uiPosition ) )
            {
                lua_pushnumber ( luaVM, uiPosition );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundLength ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiLength = 0;
            if ( CStaticFunctionDefinitions::GetSoundLength ( *pSound, uiLength ) )
            {
                lua_pushnumber ( luaVM, uiLength );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundPaused ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            bool bPaused = ( lua_toboolean ( luaVM, 2 ) ) ? true : false;
            if ( CStaticFunctionDefinitions::SetSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsSoundPaused ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            bool bPaused = false;
            if ( CStaticFunctionDefinitions::IsSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, bPaused );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundVolume ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fVolume = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundVolume ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fVolume = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushnumber ( luaVM, fVolume );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundSpeed ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fSpeed = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundSpeed ( *pSound, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundSpeed ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fSpeed = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundSpeed ( *pSound, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundMinDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundMinDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundMaxDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundMaxDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlayMissionAudio ( lua_State* luaVM )
{
    // Grab the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    // Correct argument types?
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the sound and the position
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        unsigned short usSound = static_cast < short > ( lua_tonumber ( luaVM, 4 ) );

        // Play the sound
        if ( CStaticFunctionDefinitions::PlayMissionAudio ( vecPosition, usSound ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "playMissionAudio" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlaySoundFrontEnd ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned long ulSound = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        if ( ulSound <= 101 )
        {
            if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( ulSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "playSoundFrontEnd" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PreloadMissionAudio ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        unsigned short usSound = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        unsigned short usSlot = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

        if ( CStaticFunctionDefinitions::PreloadMissionAudio ( usSound, usSlot ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "preloadMissionAudio" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



