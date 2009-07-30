/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadio.cpp
*  PURPOSE:     Radio class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include "StdInc.h"

#define CUSTOM_RADIO_COLOR 0xFF00FFFF
#define DEFAULT_RADIO_COLOR // TODO =D
#define DEFAULT_RADIO_DROP_COLOR 0xFF000000
#define RADIO_POS_X 0.5f
#define RADIO_POS_Y 0.05f
#define RADIO_SCALE_X 0.8f
#define RADIO_SCALE_Y 1.2f

CClientRadio::CClientRadio ( CClientManager * pManager )
{
    m_pManager = pManager;
    m_Channel = CHANNEL_PLAYBACK_FM;
    m_bIsPlaying = false;
    m_ulChangeTick = 0;
}


void CClientRadio::DoPulse ( void )
{
    CClientPlayer * pLocalPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();
    if ( pLocalPlayer )
    {
        CClientVehicle * pVehicle = pLocalPlayer->GetRealOccupiedVehicle ();
        if ( pVehicle )
        {
            if ( !IsPlaying () ) Start ();
        }
        else if ( IsPlaying () ) Stop ();
    }
}


void CClientRadio::Render ( void )
{    
    // Are we live n kickin?
    if ( IsPlaying () )
    {
        // Only visible for 2.5secs after turning on or changing station
        unsigned long ulTick = GetTickCount ();
        if ( m_ulChangeTick && ulTick <= ( m_ulChangeTick + 2500 ) )
        {
            char * trackName = g_pGame->GetAERadioTrackManager ()->GetRadioStationName ( ( BYTE ) m_Channel );
            if ( trackName )
            {
                // Position and scale correction for resolution
                CVector2D vecRes = g_pCore->GetGUI ()->GetResolution ();
                float fX = RADIO_POS_X * vecRes.fX, fY = RADIO_POS_Y * vecRes.fY;
                float fScaleX = RADIO_SCALE_X * ( vecRes.fX / 800.0f ), fScaleY = RADIO_SCALE_Y * ( vecRes.fY / 600.0f );

                CFont * pFont = g_pGame->GetFont ();
                pFont->SetFontStyle ( 2 );
                pFont->SetJustify ( 0 );
                pFont->SetBackground ( 0, 0 );
                pFont->SetScale ( fScaleX, fScaleY );
                pFont->SetProportional ( 1 );
                pFont->SetOrientation ( 0 );
                pFont->SetRightJustifyWrap ( 0 );
                pFont->SetEdge ( 1 );
                pFont->SetDropColor ( DEFAULT_RADIO_DROP_COLOR );
                pFont->SetColor ( CUSTOM_RADIO_COLOR );
                pFont->PrintString ( fX, fY, trackName );
                pFont->DrawFonts ();
            }
        }
    }
}


bool CClientRadio::IsPlaying ( void )
{
    return m_bIsPlaying;
}


void CClientRadio::Start ( void )
{    
    g_pGame->GetAudio ()->StartRadio ( ( unsigned int ) m_Channel );
    m_bIsPlaying = true;
    m_ulChangeTick = GetTickCount ();
}


void CClientRadio::Stop ( void )
{   
    g_pGame->GetAudio ()->StopRadio ();
    m_bIsPlaying = false;
}


void CClientRadio::SetChannel ( eRadioChannel channel )
{
    m_Channel = channel;       
    if ( m_bIsPlaying )
    {
        g_pGame->GetAudio ()->StartRadio ( m_Channel );
        m_ulChangeTick = GetTickCount ();
    }
}


char * CClientRadio::GetChannelName ( void )
{
    return g_pGame->GetAERadioTrackManager ()->GetRadioStationName ( ( BYTE ) m_Channel );
}


void CClientRadio::NextChannel ( void )
{
    if ( m_Channel == CHANNEL_RADIO_OFF ) m_Channel = CHANNEL_PLAYBACK_FM;
    else m_Channel = eRadioChannel ( m_Channel + 1 );
    SetChannel ( m_Channel );
}


void CClientRadio::PreviousChannel ( void )
{
    if ( m_Channel == CHANNEL_PLAYBACK_FM ) m_Channel = CHANNEL_RADIO_OFF;
    else m_Channel = eRadioChannel ( m_Channel - 1 );
    SetChannel ( m_Channel );
}