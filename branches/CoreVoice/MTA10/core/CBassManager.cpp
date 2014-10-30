/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CBassManager.cpp
*  PURPOSE:     core based manager class
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#include "StdInc.h"

CBassManager::CBassManager ( void )
{
    // Initialize BASS audio library
    if (!BASS_Init ( -1,44100,NULL,NULL,NULL ))
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in Init", BASS_ErrorGetCode() );
    
    // Load the Plugins
    if (!BASS_PluginLoad ( "basswma.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad WMA", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bassflac.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad FLAC", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bassmidi.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad MIDI", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bass_aac.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole()->Printf ( "BASS ERROR %d in PluginLoad AAC", BASS_ErrorGetCode() );
    if (!BASS_PluginLoad ( "bass_ac3.dll", 0 ) && BASS_ErrorGetCode () != BASS_ERROR_ALREADY)
        g_pCore->GetConsole ( )->Printf ( "BASS ERROR %d in PluginLoad AC3", BASS_ErrorGetCode ( ) );
    if (!BASS_PluginLoad ( "bassopus.dll", 0 ) && BASS_ErrorGetCode ( ) != BASS_ERROR_ALREADY )
        g_pCore->GetConsole ( )->Printf ( "BASS ERROR %d in PluginLoad OPUS", BASS_ErrorGetCode ( ) );

    // set our configuration options
    BASS_SetConfig ( BASS_CONFIG_NET_PREBUF, 0 );
    BASS_SetConfig ( BASS_CONFIG_NET_PLAYLIST, 1 ); // Allow playlists

    // set our user agent
    SetUserAgent ( "" );
    
    // set our master sound volume if the cvar changed
    float fValue = 0.0f;

    // calculate our global volume
    if( CVARS_GET ( "mtavolume", fValue ) )
    {
        if ( fValue*10000 != BASS_GetConfig ( BASS_CONFIG_GVOL_STREAM ) )
            fValue = Max( 0.0f, Min( 1.0f, fValue ) );
    }
    else
    {
        fValue = 1.0f;
    }

    // set our global volume
    BASS_SetConfig( BASS_CONFIG_GVOL_STREAM, static_cast <DWORD> ( fValue * 10000 ) );
    BASS_SetConfig( BASS_CONFIG_GVOL_MUSIC, static_cast <DWORD> ( fValue * 10000 ) );

    // load our credits stream
    m_pCreditsStream = new CBassAudio ( false, CalcMTASAPath ( "MTA\\credits.ogg" ), true, false);

    // load the underlying media
    m_pCreditsStream->BeginLoadingMedia ( );

    // pause it
    m_pCreditsStream->SetPaused ( true );
}


void CBassManager::SetUserAgent ( SString strConnectedServer )
{
    if ( strConnectedServer.empty ( ) )
    {
        // set our proper user agent
        m_strUserAgent = "MTA:SA Client - if you are reading this then dinosaurs have gained access to our source tree.";
    }
    else
    {
        // set our proper user agent
        m_strUserAgent = SString( "MTA:SA Server %s - See http://mtasa.com/agent/", strConnectedServer.c_str ( ) );
    }
    // update the BASS user agent
    BASS_SetConfigPtr ( BASS_CONFIG_NET_AGENT, (void*)*m_strUserAgent );
}

void CBassManager::DoPulse ( void )
{
    // if the credits stream is valid
    if ( m_pCreditsStream )
        // pulse the credits stream
        m_pCreditsStream->DoPulse ( );
}

void CBassManager::SetCreditsMusicPaused ( bool bPaused )
{
    // if the credits stream is valid
    if ( m_pCreditsStream )
    {
        // set us paused and restart
        m_pCreditsStream->SetPaused ( bPaused );
        m_pCreditsStream->SetPlayPosition ( 0.0f );
    }
}

bool CBassManager::IsCreditsMusicPaused ( void )
{
    // if the credits stream is valid
    if ( m_pCreditsStream )
    {
        // return if we are paused
        return m_pCreditsStream->IsPaused ( );
    }
    // technically nothing is playing right.... right?
    return false;
}

void CBassManager::SetCreditsOpen ( bool bOpen )
{
    // credits paused == true and bOpen == true - pass/play music
    // credits paused == false and bOpen == false - pass/pause music
    if ( IsCreditsMusicPaused ( ) == bOpen )
    {
        // get our paused state
        bool bPaused = true;
        CVARS_GET ( "CreditsMusicPaused", bPaused );

        // make sure we are allowed to update the credits music state
        if ( bPaused )
        {
            // update the credits music state
            SetCreditsMusicPaused ( !bOpen );
        }
    }
}