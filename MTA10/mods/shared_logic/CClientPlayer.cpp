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
int g_iDamageEventLimit = -1;
extern float g_fApplyDamageLastAmount;
extern CClientPed* g_pApplyDamageLastDamagedPed;

CClientPlayer::CClientPlayer ( CClientManager* pManager, ElementID ID, bool bIsLocalPlayer ) : ClassInit ( this ), CClientPed ( pManager, 0, ID, bIsLocalPlayer )
{
    // Initialize
    m_pManager = pManager;
    m_bIsLocalPlayer = bIsLocalPlayer;

    SetTypeName ( "player" );

    m_bNametagColorOverridden = false;
    m_uiPing = 0;

    m_usLatency = 0;

    m_uiPlayerSyncCount = 0;
    m_uiKeySyncCount = 0;
    m_uiVehicleSyncCount = 0;

    m_ulLastPuresyncTime = 0;
    m_bHasConnectionTrouble = false;    

    m_bForce = false;
    m_bDoExtrapolatingAim = false;

    m_bNetworkDead = false;

    m_voice = NULL;

    // If it's the local player, add the model
    if ( m_bIsLocalPlayer )
    {
        // Set us as the local player in the player manager
        if ( !pManager->GetPlayerManager ()->GetLocalPlayer () )
        {
            pManager->GetPlayerManager ()->SetLocalPlayer ( this );
        }

        CClientPlayer * pLocalPlayer = this;
        // Enable voice playback for local player
        if ( pLocalPlayer->GetVoice() == NULL )
        {
            //If voice is enabled
            CClientPlayerVoice * pVoice = new CClientPlayerVoice ( this, g_pClientGame->GetVoiceRecorder() );
            pLocalPlayer->SetPlayerVoice ( pVoice );
        }
    }
    else
    {
        // Enable weapon processing for players
        m_remoteDataStorage->SetProcessPlayerWeapon ( true );

        // Enable voice playback for remote players
        if ( g_pClientGame->GetVoiceRecorder()->IsEnabled() ) //If voice is enabled
            m_voice = new CClientPlayerVoice ( this, g_pClientGame->GetVoiceRecorder() );
    }

    // Set all our default stats
    m_pTeam = NULL;

    m_bNametagShowing = true;
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_ulLastNametagShow = 0;
    SetNametagText ( m_strNick );

    // Add us to the player list
    m_pManager->GetPlayerManager ()->AddToList ( this );

#ifdef MTA_DEBUG
    m_bShowingWepdata = false;
#endif
    m_LastPuresyncType = PURESYNC_TYPE_NONE;
}


CClientPlayer::~CClientPlayer ( void )
{
    // Remove us from the team
    if ( m_pTeam )
        m_pTeam->RemovePlayer ( this );

    // Remove us from the player list
    Unlink ();

    if ( m_voice )
        delete m_voice;
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
        m_strNick.AssignLeft ( szNick, MAX_PLAYER_NICK_LENGTH );

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
    RebuildModel ();

    // Nametag
    SetNametagText ( m_strNick );
    m_bNametagShowing = true;

    // Otherwize default to white
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_bNametagColorOverridden = false;

    SetAlpha ( 255 );
}


void CClientPlayer::SetNametagText ( const char * szText )
{
    if ( szText )
    {
        m_strNametag = szText;
    }
}


void CClientPlayer::DischargeWeapon ( eWeaponType weaponType, const CVector& vecStart, const CVector& vecEnd, float fBackupDamage, uchar ucBackupHitZone, CClientPlayer* pBackupDamagedPlayer )
{
    if ( m_pPlayerPed )
    {
        g_pApplyDamageLastDamagedPed = NULL;
        g_fApplyDamageLastAmount = 0;

        // Check weapon matches and is enabled for bullet sync
        if ( weaponType == GetCurrentWeaponType () &&
             g_pClientGame->GetWeaponTypeUsesBulletSync ( weaponType ) )
        {
            // Set bullet start and end points
            m_shotSyncData->m_vecRemoteBulletSyncStart = vecStart;
            m_shotSyncData->m_vecRemoteBulletSyncEnd = vecEnd;
            m_shotSyncData->m_bRemoteBulletSyncVectorsValid = true;

            g_iDamageEventLimit = 1;

            // Fixed #9038: bugged shotgun with bullet sync
            if ( weaponType == WEAPONTYPE_SHOTGUN || weaponType == WEAPONTYPE_SAWNOFF_SHOTGUN || weaponType == WEAPONTYPE_SPAS12_SHOTGUN )
            {
                if ( g_pClientGame->GetMiscGameSettings().bAllowShotgunDamageFix )
                    g_iDamageEventLimit = 2;
            }

            // Fire
            CWeapon* pWeapon = m_pPlayerPed->GetWeapon ( m_pPlayerPed->GetCurrentWeaponSlot () );
            pWeapon->FireBullet ( m_pPlayerPed, vecStart, vecEnd );
            g_iDamageEventLimit = -1;

            m_shotSyncData->m_bRemoteBulletSyncVectorsValid = false;
        }

        // Apply extra damage if player has bad network
        if ( pBackupDamagedPlayer && pBackupDamagedPlayer->GetGamePlayer() && pBackupDamagedPlayer->GetWasRecentlyInNetworkInterruption( 1000 ) )
        {
            // Subtract any damage that did get applied during FireBullet
            if ( pBackupDamagedPlayer == g_pApplyDamageLastDamagedPed )
                fBackupDamage -= g_fApplyDamageLastAmount;

            if ( fBackupDamage > 0 )
            {
                // Apply left over damage like what the game would:
                //      CClientPlayer has pre damage health/armor
                //      CPlayerPed has post damage health/armor

                float fPreviousHealth = pBackupDamagedPlayer->m_fHealth;
                float fPreviousArmor = pBackupDamagedPlayer->m_fArmor;

                // Calculate how much damage should be applied to health/armor
                float fArmorDamage = Min( fBackupDamage, pBackupDamagedPlayer->m_fArmor ); 
                float fHealthDamage = Min( fBackupDamage - fArmorDamage, pBackupDamagedPlayer->m_fHealth );

                float fNewArmor = pBackupDamagedPlayer->m_fArmor - fArmorDamage;
                float fNewHealth = pBackupDamagedPlayer->m_fHealth - fHealthDamage;

                // Ensure CPlayerPed has post damage health/armor
                pBackupDamagedPlayer->GetGamePlayer()->SetHealth( fNewHealth );
                pBackupDamagedPlayer->GetGamePlayer()->SetArmor( fNewArmor );

                g_pClientGame->ApplyPedDamageFromGame( weaponType, fBackupDamage, ucBackupHitZone, pBackupDamagedPlayer, this, NULL );

                SString strMessage( "Applied %0.2f damage to %s (from %s) due to network interruption", fBackupDamage, pBackupDamagedPlayer->GetNick(), GetNick() );
                g_pClientGame->TellServerSomethingImportant( 1010, strMessage );
            }
        }
    }
}

// Version info about a remote player
void CClientPlayer::SetRemoteVersionInfo ( ushort usBitstreamVersion, uint uiBuildNumber )
{
    m_usRemoteBitstreamVersion = usBitstreamVersion;
    m_uiRemoteBuildNumber = uiBuildNumber;
}

ushort CClientPlayer::GetRemoteBitstreamVersion ( void )
{
    return m_usRemoteBitstreamVersion;
}

uint CClientPlayer::GetRemoteBuildNumber ( void )
{
    return m_uiRemoteBuildNumber;
}

bool CClientPlayer::GetWasRecentlyInNetworkInterruption ( uint uiMaxTicksAgo )
{
    if ( m_bInNetworkInterruption )
        return true;
    return m_TimeSinceNetworkInterruptionEnded.Get() < uiMaxTicksAgo;
}

void CClientPlayer::SetIsInNetworkInterruption ( bool bBegan )
{
    m_bInNetworkInterruption = bBegan;
    if ( !bBegan )
        m_TimeSinceNetworkInterruptionEnded.Reset();
}
