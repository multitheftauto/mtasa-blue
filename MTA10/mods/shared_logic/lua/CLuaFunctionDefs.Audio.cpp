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
    SString strSound = "";
    bool bLoop = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSound );
    argStream.ReadBool ( bLoop, false );

    if ( !argStream.HasErrors() )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
                SString strFilename;
                bool bIsURL = false;
                if ( CResourceManager::ParseResourcePathInput( strSound, pResource, strFilename ) )
                    strSound = strFilename;
                else
                    bIsURL = true;

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3" )
                // Fixes #6507 - Caz
                if ( pResource )
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound ( pResource, strSound, bIsURL, bLoop );
                    if ( pSound )
                    {
                        // call onClientSoundStarted
                        CLuaArguments Arguments;
                        Arguments.PushString ( "play" );     // Reason
                        pSound->CallEvent ( "onClientSoundStarted", Arguments, false );

                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlaySound3D ( lua_State* luaVM )
{
    SString strSound = "";
    CVector vecPosition;
    bool bLoop = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSound );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadBool ( bLoop, false );

    if ( !argStream.HasErrors() )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
                SString strFilename;
                bool bIsURL = false;
                if ( CResourceManager::ParseResourcePathInput( strSound, pResource, strFilename ) )
                    strSound = strFilename;
                else
                    bIsURL = true;

                // ParseResourcePathInput changes pResource in some cases e.g. an invalid resource URL - crun playSound( ":myNotRunningResource/music/track.mp3" )
                // Fixes #6507 - Caz
                if ( pResource )
                {
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D ( pResource, strSound, bIsURL, vecPosition, bLoop );
                    if ( pSound )
                    {
                        // call onClientSoundStarted
                        CLuaArguments Arguments;
                        Arguments.PushString ( "play" );     // Reason
                        pSound->CallEvent ( "onClientSoundStarted", Arguments, false );

                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::StopSound ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::StopSound ( *pSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundPosition ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    double dPosition = 0.0;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadNumber ( dPosition );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundPosition ( *pSound, dPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetSoundPosition ( *pPlayer, dPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundPosition ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            double dPosition = 0;
            if ( CStaticFunctionDefinitions::GetSoundPosition ( *pSound, dPosition ) )
            {
                lua_pushnumber ( luaVM, dPosition );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            double dPosition = 0;
            if ( CStaticFunctionDefinitions::GetSoundPosition ( *pPlayer, dPosition ) )
            {
                lua_pushnumber ( luaVM, dPosition );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundLength ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            double dLength = 0;
            if ( CStaticFunctionDefinitions::GetSoundLength ( *pSound, dLength ) )
            {
                lua_pushnumber ( luaVM, dLength );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            double dLength = 0;
            if ( CStaticFunctionDefinitions::GetSoundLength ( *pPlayer, dLength ) )
            {
                lua_pushnumber ( luaVM, dLength );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundPaused ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    bool bPaused = false;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadBool ( bPaused );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetSoundPaused ( *pPlayer, bPaused ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsSoundPaused ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            bool bPaused = false;
            if ( CStaticFunctionDefinitions::IsSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, bPaused );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            bool bPaused = false;
            if ( CStaticFunctionDefinitions::IsSoundPaused ( *pPlayer, bPaused ) )
            {
                lua_pushboolean ( luaVM, bPaused );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundVolume ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    float fVolume = 0.0f;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadNumber ( fVolume );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetSoundVolume ( *pPlayer, fVolume ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundVolume ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            float fVolume = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushnumber ( luaVM, fVolume );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            float fVolume = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundVolume ( *pPlayer, fVolume ) )
            {
                lua_pushnumber ( luaVM, fVolume );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundSpeed ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    float fSpeed = 0.0f;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadNumber ( fSpeed );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundSpeed ( *pSound, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetSoundSpeed ( *pPlayer, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetSoundProperties ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    bool bReversed = false;
    float fSampleRate = 0.0f, fTempo = 0.0f, fPitch = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadNumber ( fSampleRate );
    argStream.ReadNumber ( fTempo );
    argStream.ReadNumber ( fPitch );
    argStream.ReadBool ( bReversed, false );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundProperties ( *pSound, fSampleRate, fTempo, fPitch, bReversed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundProperties ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    bool bReversed = false;
    float fSampleRate = 0.0f, fTempo = 0.0f, fPitch = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::GetSoundProperties ( *pSound, fSampleRate, fTempo, fPitch, bReversed ) )
            {
                lua_pushnumber ( luaVM, fSampleRate );
                lua_pushnumber ( luaVM, fTempo );
                lua_pushnumber ( luaVM, fPitch );
                lua_pushboolean ( luaVM, bReversed );
                return 4;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundFFTData ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    float* pData = NULL;
    int iLength = 0;
    int iBands = 0;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadNumber ( iLength );
    argStream.ReadNumber ( iBands, 0 );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            pData = CStaticFunctionDefinitions::GetSoundFFTData ( *pSound, iLength, iBands );
        }
        else if ( pPlayer )
        {
            pData = CStaticFunctionDefinitions::GetSoundFFTData ( *pPlayer, iLength, iBands );
        }
        if ( pData != NULL )
        {
            if ( iBands == 0 )
            {
                // Create a new table
                lua_newtable ( luaVM );
                for ( int i = 0; i <= iLength / 2;i++ )
                {
                    lua_pushnumber ( luaVM, i );
                    lua_pushnumber ( luaVM, pData[i] );
                    lua_settable ( luaVM, -3 );
                }
            }
            else
            {
                // Create a new table
                lua_newtable ( luaVM );
                for ( int i = 0; i <= iBands - 1;i++ )
                {
                    lua_pushnumber ( luaVM, i );
                    lua_pushnumber ( luaVM, pData[i] );
                    lua_settable ( luaVM, -3 );
                }
            }
            // Deallocate our data array here after it's used.
            delete [] pData;
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundWaveData ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    CClientPlayer* pPlayer = NULL;
    float* pData = NULL;
    int iLength = 0;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }
    argStream.ReadNumber ( iLength );

    if ( !argStream.HasErrors () )
    {
        if ( pSound != NULL )
        {
            pData = CStaticFunctionDefinitions::GetSoundWaveData ( *pSound, iLength );
        }
        else if ( pPlayer != NULL )
        {
            pData = CStaticFunctionDefinitions::GetSoundWaveData ( *pPlayer, iLength );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        if ( pData != NULL )
        {
            // Create a new table
            lua_newtable ( luaVM );
            for (int i = 0; i < iLength;i++)
            {
                lua_pushnumber ( luaVM, i );
                lua_pushnumber ( luaVM, pData[i] );
                lua_settable ( luaVM, -3 );
            }
            // Deallocate our data array here after it's used.
            delete [] pData;
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundLevelData ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    CClientPlayer* pPlayer = NULL;
    DWORD dwLeft = 0, dwRight = 0;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    if ( !argStream.HasErrors () )
    {
        if ( pSound != NULL && CStaticFunctionDefinitions::GetSoundLevelData ( *pSound, dwLeft, dwRight ) )
        {
            lua_pushnumber ( luaVM, dwLeft );
            lua_pushnumber ( luaVM, dwRight );
            return 2;
        }
        else if ( pPlayer != NULL && CStaticFunctionDefinitions::GetSoundLevelData ( *pPlayer, dwLeft, dwRight ) )
        {
            lua_pushnumber ( luaVM, dwLeft );
            lua_pushnumber ( luaVM, dwRight );
            return 2;
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundBPM ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    float fBPM = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetSoundBPM ( *pSound, fBPM ) )
        {
            lua_pushnumber ( luaVM, fBPM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetSoundPanEnabled ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    bool bEnabled = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundPanEnabled ( *pSound, bEnabled ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsSoundPanEnabled ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    bool bEnabled = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::IsSoundPanEnabled ( *pSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetSoundSpeed ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            float fSpeed = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundSpeed ( *pSound, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            float fSpeed = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundSpeed ( *pPlayer, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundMinDistance ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    float fDistance = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadNumber ( fDistance );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundMinDistance ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    float fDistance = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::GetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundMaxDistance ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    float fDistance = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadNumber ( fDistance );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundMaxDistance ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    float fDistance = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::GetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundMetaTags ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    SString strFormat = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadString ( strFormat, "" );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            SString strMetaTags = "";
            if ( strFormat != "" )
            {
                if ( CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, strFormat, strMetaTags ) )
                {
                    if ( !strMetaTags.empty() )
                    {
                        lua_pushstring ( luaVM, strMetaTags );
                        return 1;
                    }
                }
            }
            else
            {
                lua_newtable ( luaVM );
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%TITL", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "title" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%ARTI", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "artist" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%ALBM", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "album" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%GNRE", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "genre" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%YEAR", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "year" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%CMNT", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "comment" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%TRCK", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "track" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%COMP", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "composer" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%COPY", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "copyright" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%SUBT", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "subtitle" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%AART", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "album_artist" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "streamName", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "stream_name" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "streamTitle", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( luaVM, strMetaTags );
                    lua_setfield ( luaVM, -2, "stream_title" );
                }
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundEffectEnabled ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    SString strEffectName = "";
    bool bEnable = false;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }
    argStream.ReadString ( strEffectName );
    argStream.ReadBool ( bEnable, false );

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundEffectEnabled ( *pSound, strEffectName, bEnable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetSoundEffectEnabled ( *pPlayer, strEffectName, bEnable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundEffects ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = NULL;
    CClientSound* pSound = NULL;
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CClientSound > ( ) )
    {
        argStream.ReadUserData ( pSound );
    }
    else if ( argStream.NextIsUserDataOfType < CClientPlayer > ( ) )
    {
        argStream.ReadUserData ( pPlayer );
    }
    else
    {
        m_pScriptDebugging->LogBadPointer ( luaVM, "sound/player", 1 );
        lua_pushboolean ( luaVM, false );
        return false;
    }

    if ( !argStream.HasErrors() )
    {
        if ( pSound )
        {
            std::map < std::string, int > iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();
            lua_newtable ( luaVM );
            for ( std::map < std::string, int >::const_iterator iter = iFxEffects.begin(); iter != iFxEffects.end(); ++iter )
            {
                lua_pushboolean ( luaVM, pSound->IsFxEffectEnabled ( (*iter).second ) );
                lua_setfield ( luaVM, -2, (*iter).first.c_str () );
            }
            return 1;
        }
        else if ( pPlayer )
        {
            CClientPlayerVoice * pPlayerVoice = pPlayer->GetVoice ( );
            std::map < std::string, int > iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();
            lua_newtable ( luaVM );
            for ( std::map < std::string, int >::const_iterator iter = iFxEffects.begin(); iter != iFxEffects.end(); ++iter )
            {
                lua_pushboolean ( luaVM, pPlayerVoice->IsFxEffectEnabled ( (*iter).second ) );
                lua_setfield ( luaVM, -2, (*iter).first.c_str () );
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlaySoundFrontEnd ( lua_State* luaVM )
{
    CClientSound* pSound = NULL;
    CVector vecPosition;
    unsigned char ucSound = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucSound );

    if ( !argStream.HasErrors() )
    {
        if ( ucSound <= 101 )
        {
            if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( ucSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetAmbientSoundEnabled ( lua_State* luaVM )
{
    eAmbientSoundType eType; bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( eType, AMBIENT_SOUND_GENERAL );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAmbientSoundEnabled ( eType, bEnabled ) )
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


int CLuaFunctionDefs::IsAmbientSoundEnabled ( lua_State* luaVM )
{
    eAmbientSoundType eType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( eType );

    if ( !argStream.HasErrors () )
    {
        bool bResultEnabled;
        if ( CStaticFunctionDefinitions::IsAmbientSoundEnabled ( eType, bResultEnabled ) )
        {
            lua_pushboolean ( luaVM, bResultEnabled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetAmbientSounds ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetAmbientSounds () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetWorldSoundEnabled ( lua_State* luaVM )
{
//  setWorldSoundEnabled ( int group, [int index, ], bool enable )
    int group; int index = -1; bool bEnabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( group );
    if ( !argStream.NextIsBool () )
        argStream.ReadNumber ( index );
    argStream.ReadBool ( bEnabled );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWorldSoundEnabled ( group, index, bEnabled ) )
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


int CLuaFunctionDefs::IsWorldSoundEnabled ( lua_State* luaVM )
{
//  bool isWorldSoundEnabled ( int group, [int index] )
    int group; int index;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( group );
    argStream.ReadNumber ( index, -1 );

    if ( !argStream.HasErrors () )
    {
        bool bResultEnabled;
        if ( CStaticFunctionDefinitions::IsWorldSoundEnabled ( group, index, bResultEnabled ) )
        {
            lua_pushboolean ( luaVM, bResultEnabled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ResetWorldSounds ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetWorldSounds () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::PlaySFX ( lua_State* luaVM )
{
//  sound playSFX ( string audioContainer, int bankIndex, int audioIndex [, loop = false ] )
    eAudioLookupIndex containerIndex; int iBankIndex; int iAudioIndex; bool bLoop;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( containerIndex );
    argStream.ReadNumber ( iBankIndex );
    argStream.ReadNumber ( iAudioIndex );
    argStream.ReadBool ( bLoop, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientSound* pSound;
                if ( CStaticFunctionDefinitions::PlaySFX ( pResource, containerIndex, iBankIndex, iAudioIndex, bLoop, pSound ) )
                {
                    lua_pushelement ( luaVM, pSound );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetSoundPan ( lua_State* luaVM )
{
//  setSoundPan ( element theSound, float pan )
    CClientSound* pSound; float fPan;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );
    argStream.ReadNumber ( fPan );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::SetSoundPan ( *pSound, fPan ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetSoundPan ( lua_State* luaVM )
{
//  getSoundPan ( element theSound )
    CClientSound* pSound;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSound );

    if ( !argStream.HasErrors () )
    {
        if ( pSound )
        {
            float fPan = 0.0;
            if ( CStaticFunctionDefinitions::GetSoundPan ( *pSound, fPan ) )
            {
                lua_pushnumber ( luaVM, fPan );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}