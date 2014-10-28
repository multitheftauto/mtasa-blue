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

CBassManager::CBassManager ( )
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

    BASS_SetConfig ( BASS_CONFIG_NET_PREBUF, 0 );
    BASS_SetConfig ( BASS_CONFIG_NET_PLAYLIST, 1 ); // Allow playlists

    //m_strUserAgent = "MTA:SA Client - if you are reading this then dinosaurs have gained access to our source tree.";
    //BASS_SetConfigPtr ( BASS_CONFIG_NET_AGENT, (void*)*m_strUserAgent );
    
    // set our master sound volume if the cvar changed
    float fValue = 0.0f;

    if( CVARS_GET ( "mtavolume", fValue ) )
    {
        if ( fValue*10000 != BASS_GetConfig ( BASS_CONFIG_GVOL_STREAM ) )
            fValue = Max( 0.0f, Min( 1.0f, fValue ) );
    }
    else
    {
        fValue = 1.0f;
    }

    BASS_SetConfig( BASS_CONFIG_GVOL_STREAM, static_cast <DWORD> ( fValue * 10000 ) );
    BASS_SetConfig( BASS_CONFIG_GVOL_MUSIC, static_cast <DWORD> ( fValue * 10000 ) );

    m_pTestStream = NULL;

#ifdef TEST_URL
    m_pTestStream = new CBassAudio ( true, TEST_URL, true, false);

    m_pTestStream->BeginLoadingMedia ( );

    m_pTestStream->SetVolume ( 1.0f );
#endif
}


void CBassManager::SetUserAgent ( SString strConnectedServer )
{
    m_strUserAgent = SString( "MTA:SA Server %s - See http://mtasa.com/agent/", strConnectedServer );
    BASS_SetConfigPtr ( BASS_CONFIG_NET_AGENT, (void*)*m_strUserAgent );
}

void CBassManager::DoPulse ( )
{
    if ( m_pTestStream )
        m_pTestStream->DoPulse ( );
}