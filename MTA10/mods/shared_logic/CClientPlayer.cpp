/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayer.cpp
*  PURPOSE:     Player entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include <StdInc.h>

CClientPlayer::CClientPlayer ( CClientManager* pManager, ElementID ID, bool bIsLocalPlayer ) : ClassInit ( this ), CClientPed ( pManager, 0, ID, bIsLocalPlayer )
{
    // Initialize
    m_pManager = pManager;
    m_bIsLocalPlayer = bIsLocalPlayer;

    SetTypeName ( "player" );

    m_bNametagColorOverridden = false;
    m_uiPing = 0;

    m_szNick [ 0 ] = '\0';

    m_usLatency = 0;

    m_uiPlayerSyncCount = 0;
    m_uiKeySyncCount = 0;
    m_uiVehicleSyncCount = 0;

    m_ulLastPuresyncTime = 0;
    m_bHasConnectionTrouble = false;    

    m_bForce = false;
    m_bDoExtrapolatingAim = false;

    m_bNetworkDead = false;

    // If it's the local player, add the model
    if ( m_bIsLocalPlayer )
    {
        // Set us as the local player in the player manager
        if ( !pManager->GetPlayerManager ()->GetLocalPlayer () )
        {
            pManager->GetPlayerManager ()->SetLocalPlayer ( this );
        }
    }
    else
    {
        // Enable weapon processing for players
        m_remoteDataStorage->SetProcessPlayerWeapon ( true );
    }

    // Set all our default stats
    m_pTeam = NULL;

    m_bNametagShowing = true;
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_ulLastNametagShow = 0;
    SetNametagText ( m_szNick );
    
    // Create the static icon (defaults to a warning icon for network trouble)
    m_pStatusIcon = g_pCore->GetGUI ()->CreateStaticImage ();
    m_pStatusIcon->SetSize ( CVector2D ( 16, 16 ) );
    m_pStatusIcon->SetVisible ( false );

    // Could a connection trouble texture be loaded? Load it into the status icon.
    CGUITexture* pTexture = m_pManager->GetConnectionTroubleTexture ();
    if ( pTexture )
    {
        m_pStatusIcon->LoadFromTexture ( pTexture );
    }   

    // Add us to the player list
    m_pManager->GetPlayerManager ()->AddToList ( this );

#ifdef MTA_DEBUG
    m_bShowingWepdata = false;
#endif
}


CClientPlayer::~CClientPlayer ( void )
{
    // Remove us from the team
    if ( m_pTeam )
        m_pTeam->RemovePlayer ( this );

    // Remove us from the player list
    Unlink ();

    // Remove the icon
    if ( m_pStatusIcon )
    {
        m_pStatusIcon->SetVisible ( false );

        delete m_pStatusIcon;
        m_pStatusIcon = NULL;
    }
}


void CClientPlayer::Unlink ( void )
{
    m_pManager->GetPlayerManager ()->RemoveFromList ( this );
}


void CClientPlayer::UpdateAimPosition ( const CVector &vecAim )
{ 
    m_vecOldAim = m_vecCurrentAim;
    m_vecCurrentAim = vecAim;
    m_ulOldAimTime =  m_ulCurrentAimTime;
    m_ulCurrentAimTime = GetTickCount32();
    m_vecAimSpeed = GetExtrapolatedSpeed( m_vecOldAim, m_ulOldAimTime, m_vecCurrentAim, m_ulCurrentAimTime );
}


void CClientPlayer::SetNick ( const char* szNick )
{
    // Valid pointer?
    if ( szNick )
    {
        strncpy ( m_szNick, szNick, MAX_PLAYER_NICK_LENGTH );
        m_szNick [ MAX_PLAYER_NICK_LENGTH ] = '\0';

        if ( m_strNametag.empty () )
            m_strNametag = szNick;
    }
}


void CClientPlayer::GetNametagColor ( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB )
{
    // If the nametag color is overridden, use the override color
    if ( m_bNametagColorOverridden )
    {
        ucR = m_ucNametagColorR;
        ucG = m_ucNametagColorG;
        ucB = m_ucNametagColorB;
    }
    else
    {
        // Otherwize take the color from the team if any
        if ( m_pTeam )
        {
            m_pTeam->GetColor ( ucR, ucG, ucB );
        }
        else
        {
            // No team and no override? Default color is white
            ucR = 255;
            ucG = 255;
            ucB = 255;
        }
    }
}


void CClientPlayer::SetNametagOverrideColor ( unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    m_ucNametagColorR = ucR;
    m_ucNametagColorG = ucG;
    m_ucNametagColorB = ucB;
    m_bNametagColorOverridden = true;
}


void CClientPlayer::RemoveNametagOverrideColor ( void )
{
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_bNametagColorOverridden = false;
}


void CClientPlayer::SetTeam ( CClientTeam* pTeam, bool bChangeTeam )
{
    if ( pTeam == m_pTeam ) return;

    if ( m_pTeam && bChangeTeam )
        m_pTeam->RemovePlayer ( this, false );

    m_pTeam = pTeam;
    if ( m_pTeam && bChangeTeam )
        m_pTeam->AddPlayer ( this, false );
}


bool CClientPlayer::IsOnMyTeam ( CClientPlayer* pPlayer )
{
    if ( m_pTeam )
    {
        if ( m_pTeam == pPlayer->GetTeam () )
            return true;
    }

    return false;
}


void CClientPlayer::Reset ( void )
{
    // stats
    for ( unsigned short us = 0 ; us <= NUM_PLAYER_STATS ; us++ )
    {
        if ( us == MAX_HEALTH )
        {
            SetStat ( us, 569.0f );
        }
        else
        {
            SetStat ( us, 0.0f );
        }
    }

    // model
    SetModel ( 0 );

    // clothes        
    GetClothes ()->DefaultClothes ( false );

    // jetpack
    SetHasJetPack ( false );

    // fight-style
    SetFightingStyle ( STYLE_GRAB_KICK );

    // rebuild
    RebuildModel ( true );

    // Nametag
    SetNametagText ( m_szNick );
    m_bNametagShowing = true;

    // Otherwize default to white
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_bNametagColorOverridden = false;

    SetAlpha ( 255 );

    if ( m_pStatusIcon )
    {
        m_pStatusIcon->SetVisible ( false );
    }
}


void CClientPlayer::SetNametagText ( const char * szText )
{
    if ( szText )
    {
        m_strNametag = szText;
    }
}
