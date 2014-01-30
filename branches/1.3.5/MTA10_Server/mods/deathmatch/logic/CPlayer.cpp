/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayer.cpp
*  PURPOSE:     Player ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SimHeaders.h"

extern CGame * g_pGame;

CPlayer::CPlayer ( CPlayerManager* pPlayerManager, class CScriptDebugging* pScriptDebugging, const NetServerPlayerID& PlayerSocket ) : CPed ( NULL, NULL, NULL, 0 )
{
    CElementRefManager::AddElementRefs ( ELEMENT_REF_DEBUG ( this, "CPlayer" ), &m_pTeam, NULL );
    CElementRefManager::AddElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstBroadcastList" ), &m_lstBroadcastList );
    CElementRefManager::AddElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstIgnoredList" ), &m_lstIgnoredList );

    // Init
    m_pPlayerManager = pPlayerManager;
    m_pScriptDebugging = pScriptDebugging;
    m_PlayerSocket = PlayerSocket;
    m_UpdateNearListTimer.SetMaxIncrement ( 500, true );

    m_iType = CElement::PLAYER;
    SetTypeName ( "player" );
    m_bIsPlayer = true;
    m_bDoNotSendEntities = false;
    m_iGameVersion = 0;
    m_usMTAVersion = 0;
    m_usBitStreamVersion = 0;
    m_bIsMuted = false;
    m_lMoney = 0;
    m_bNametagColorOverridden = false;

    m_iStatus = STATUS_CONNECTED;

    m_fRotation = 0.0f;
    m_fAimDirection = 0.0f;
    m_ucDriveByDirection = 0;
    m_bAkimboArmUp = false;    

    m_VoiceState = VOICESTATE_IDLE;
    m_lstBroadcastList.push_back ( g_pGame->GetMapManager()->GetRootElement() );
    
    m_uiScriptDebugLevel = 0;

    m_ulTimeConnected = GetTime ();

    m_tNickChange = 0;

    m_pPlayerTextManager = new CPlayerTextManager ( this ); 

    m_PlayerAttackerID = INVALID_ELEMENT_ID;
    m_ucAttackWeapon = 0xFF;
    m_ucAttackBodyPart = 0xFF;
    m_llSetDamageInfoTime = 0;

    m_pTeam = NULL;

    m_pPad = new CPad ( this );

    m_bDebuggerVisible = false;

    m_uiWantedLevel = 0;

    m_bForcedScoreboard = false;
    m_bForcedMap = false;   

    m_pCamera = new CPlayerCamera ( this );

    m_pKeyBinds = new CKeyBinds ( this );

    m_bCursorShowing = false;

    m_szNametagText = NULL;
    m_ucNametagR = 0xFF;
    m_ucNametagG = 0xFF;
    m_ucNametagB = 0xFF;
    m_bNametagShowing = true;        

    m_ucBlurLevel = 36; // Default

    // Sync stuff
    m_bSyncingVelocity = false;
    m_uiPuresyncPackets = 0;

    m_uiWeaponIncorrectCount = 0;

    // Add us to the manager
    pPlayerManager->AddToList ( this );

    // DO NOT DEFAULT THIS TO THE CURRENT TIME OR YOU WILL BREAK EVERYTHING.
    // THIS IS USED BY LIGHT SYNC AND SHOULD NOT BE CHANGED>>> EVER.
    m_llLastPositionHasChanged = 0;

    CSimControl::AddSimPlayer ( this );

    m_pPlayerStatsPacket = new CPlayerStatsPacket ( );

    m_LastReceivedSyncTimer.SetMaxIncrement( 2000, true );
}


CPlayer::~CPlayer ( void )
{
    // Make sure the script debugger doesn't reference us
    SetScriptDebugLevel ( 0 );    

    if ( m_pCamera )
    {
        // Remove the camera from its targets FollowingCameras list
        if ( m_pCamera->GetTarget () )
            m_pCamera->GetTarget ()->m_FollowingCameras.remove ( m_pCamera );
        delete m_pCamera;
        m_pCamera = NULL;
    }    

    // Make sure nobody's syncing us
    RemoveAllSyncingVehicles ();
    RemoveAllSyncingPeds ();
    RemoveAllSyncingObjects ();

    // Delete the player text manager
    delete m_pPlayerTextManager;

    // Destroy our nick
    if ( m_szNametagText )
    {
        delete [] m_szNametagText;
        m_szNametagText = NULL;
    }

    SetTeam ( NULL, true );

    delete m_pPad;

    delete m_pKeyBinds;

    CSimControl::RemoveSimPlayer ( this );

    // Unparent us (CElement's unparenting will crash because of the incomplete vtable at that point)
    m_bDoNotSendEntities = true;
    SetParentObject ( NULL );

    // Do this
    if ( m_pJackingVehicle )
    {
        if ( m_uiVehicleAction == VEHICLEACTION_JACKING )
        {
            CPed * pOccupant = m_pJackingVehicle->GetOccupant ( 0 );
            if ( pOccupant )
            {
                m_pJackingVehicle->SetOccupant ( NULL, 0 );
                pOccupant->SetOccupiedVehicle ( NULL, 0 );
                pOccupant->SetVehicleAction ( VEHICLEACTION_NONE );
            }
        }
        if ( m_pJackingVehicle->GetJackingPlayer () == this )
            m_pJackingVehicle->SetJackingPlayer ( NULL );
    }

    CElementRefManager::RemoveElementRefs ( ELEMENT_REF_DEBUG ( this, "CPlayer" ), &m_pTeam, NULL );
    CElementRefManager::RemoveElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstBroadcastList" ), &m_lstBroadcastList );
    CElementRefManager::RemoveElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstIgnoredList" ), &m_lstIgnoredList );
    
    delete m_pPlayerStatsPacket;

    // Unlink from manager
    Unlink ();
}


void CPlayer::Unlink ( void )
{
    // Remove us from the player manager
    m_pPlayerManager->RemoveFromList ( this );

    // Remove us from all and any PerPlayerEntity list
    CPerPlayerEntity::StaticOnPlayerDelete( this );
}


void CPlayer::DoPulse ( void )
{
    if ( GetStatus () == STATUS_JOINED )
    {
        m_pPlayerTextManager->Process ();

        // Do dist update if too long since last one
        if ( m_UpdateNearListTimer.Get () > (uint)g_TickRateSettings.iNearListUpdate + 300 )
            MaybeUpdateOthersNearList ();
    }
}


void CPlayer::SetNick ( const char* szNick )
{
    if ( !m_strNick.empty () && m_strNick != szNick  )
    {
        // If changing, add the new name to the whowas list
        g_pGame->GetConsole ()->GetWhoWas ()->Add ( szNick, inet_addr ( GetSourceIP() ), GetSerial (), GetPlayerVersion (), GetAccount ()->GetName () );
    }

    m_strNick.AssignLeft ( szNick, MAX_NICK_LENGTH );
}

const char* CPlayer::GetSourceIP ( void )
{
    if ( m_strIP.empty () )
    {
        char szIP [22];
        unsigned short usPort;
        g_pNetServer->GetPlayerIP ( m_PlayerSocket, szIP, &usPort );
        m_strIP = szIP;
    }
    return m_strIP;
}

uint CPlayer::Send ( const CPacket& Packet )
{
    if ( !CNetBufferWatchDog::CanSendPacket ( Packet.GetPacketID () ) )
        return 0; 

    // Use the flags to determine how to send it
    NetServerPacketReliability Reliability;
    unsigned long ulFlags = Packet.GetFlags ();
    if ( ulFlags & PACKET_RELIABLE )
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_RELIABLE_ORDERED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_RELIABLE;
        }
    }
    else
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE_SEQUENCED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE;
        }
    }
    NetServerPacketPriority packetPriority = PACKET_PRIORITY_MEDIUM;
    if ( ulFlags & PACKET_HIGH_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_HIGH;
    }
    else if ( ulFlags & PACKET_LOW_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_LOW;
    }

    uint uiBitsSent = 0;
    // Allocate a bitstream for it
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( GetBitStreamVersion () );
    if ( pBitStream )
    {
        // Write the content to it and send it
        if ( Packet.Write ( *pBitStream ) )
        {
            uiBitsSent = pBitStream->GetNumberOfBitsUsed ();
            g_pGame->SendPacket ( Packet.GetPacketID (), m_PlayerSocket, pBitStream, FALSE, packetPriority, Reliability, Packet.GetPacketOrdering() );
        }

        // Destroy the bitstream
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
    return uiBitsSent;
}


void CPlayer::SendEcho ( const char* szEcho )
{
    Send ( CChatEchoPacket ( szEcho, CHATCOLOR_MESSAGE ) );
}


void CPlayer::SendConsole ( const char* szEcho )
{
    Send ( CConsoleEchoPacket ( szEcho ) );
}


void CPlayer::AddSyncingVehicle ( CVehicle* pVehicle )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pVehicle->SetSyncer ( this );
        bAlreadyIn = false;

        // Add it to our list
        m_SyncingVehicles.push_back ( pVehicle );
    }
}


void CPlayer::RemoveSyncingVehicle ( CVehicle* pVehicle )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pVehicle->SetSyncer ( NULL );
        bAlreadyIn = false;

        // Remove it from our list
        m_SyncingVehicles.remove ( pVehicle );
    }
}


void CPlayer::RemoveAllSyncingVehicles ( void )
{
    // Unreference us from all
    list < CVehicle* > ::const_iterator iter = m_SyncingVehicles.begin ();
    for ( ; iter != m_SyncingVehicles.end (); iter++ )
    {
        (*iter)->m_pSyncer = NULL;
    }
}


void CPlayer::AddSyncingPed ( CPed* pPed )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pPed->SetSyncer ( this );
        bAlreadyIn = false;

        // Add it to our list
        m_SyncingPeds.push_back ( pPed );
    }
}


void CPlayer::RemoveSyncingPed ( CPed* pPed )
{
    // Prevent a recursive call loop when setting vehicle's syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the vehicle
        bAlreadyIn = true;
        pPed->SetSyncer ( NULL );
        bAlreadyIn = false;

        // Remove it from our list
        m_SyncingPeds.remove ( pPed );
    }
}


void CPlayer::RemoveAllSyncingPeds ( void )
{
    // Unreference us from all
    list < CPed* > ::const_iterator iter = m_SyncingPeds.begin ();
    for ( ; iter != m_SyncingPeds.end (); iter++ )
    {
        (*iter)->m_pSyncer = NULL;
    }
}


void CPlayer::AddSyncingObject ( CObject* pObject )
{
    // Prevent a recursive call loop when setting a syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the object
        bAlreadyIn = true;
        pObject->SetSyncer ( this );
        bAlreadyIn = false;

        // Add it to our list
        m_SyncingObjects.push_back ( pObject );
    }
}


void CPlayer::RemoveSyncingObject ( CObject* pObject )
{
    // Prevent a recursive call loop when setting a syncer
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        // Update the object
        bAlreadyIn = true;
        pObject->SetSyncer ( NULL );
        bAlreadyIn = false;

        // Remove it from our list
        m_SyncingObjects.remove ( pObject );
    }
}


void CPlayer::RemoveAllSyncingObjects ( void )
{
    // Unreference us from all
    list < CObject* > ::const_iterator iter = m_SyncingObjects.begin ();
    for ( ; iter != m_SyncingObjects.end (); iter++ )
    {
        (*iter)->m_pSyncer = NULL;
    }
}


bool CPlayer::SetScriptDebugLevel ( unsigned int uiLevel )
{
    return m_pScriptDebugging->AddPlayer ( *this, uiLevel );
}


void CPlayer::SetDamageInfo ( ElementID ElementID, unsigned char ucWeapon, unsigned char ucBodyPart )
{
    m_PlayerAttackerID = ElementID;
    m_ucAttackWeapon = ucWeapon;
    m_ucAttackBodyPart = ucBodyPart;
    m_llSetDamageInfoTime = GetTickCount64_ ();
}


void CPlayer::ValidateDamageInfo ( void )
{
    if ( m_llSetDamageInfoTime + 100 < GetTickCount64_ () )
    {
        // Reset if data is too old
        m_PlayerAttackerID = INVALID_ELEMENT_ID;
        m_ucAttackWeapon = 0xFF;
        m_ucAttackBodyPart = 0xFF;
    }
}


ElementID CPlayer::GetPlayerAttacker ( void )
{
    ValidateDamageInfo ();
    return m_PlayerAttackerID;
}


unsigned char CPlayer::GetAttackWeapon ( void )
{
    ValidateDamageInfo ();
    return m_ucAttackWeapon;
}


unsigned char CPlayer::GetAttackBodyPart ( void )
{
    ValidateDamageInfo ();
    return m_ucAttackBodyPart;
}


void CPlayer::SetTeam ( CTeam* pTeam, bool bChangeTeam )
{
    if ( pTeam == m_pTeam ) return;

    if ( m_pTeam && bChangeTeam )
        m_pTeam->RemovePlayer ( this, false );

    m_pTeam = pTeam;
    if ( m_pTeam && bChangeTeam )
        m_pTeam->AddPlayer ( this, false );
}


void CPlayer::Reset ( void )
{
    //Called when resetMapInfo is called to reset per player information that is reset in the clientside implimentation of resetMapInfo. This stops our functions clientside and serverside possibly returning different results.
    memset ( &m_fStats[0], 0, sizeof ( m_fStats ) );
    m_pPlayerStatsPacket->Clear ( );
    SetPlayerStat ( 24, 569.0f );           // default max_health

    m_pClothes->DefaultClothes ();    
    m_bHasJetPack = false;

    // Removed
    //m_bForcedScoreboard = false;
    m_bForcedMap = false;
    m_ucInterior = 0;
    m_usDimension = 0;
    //m_pKeyBinds->Clear ();
    m_bCursorShowing = false;

    // Added
    m_ucFightingStyle = 15; // STYLE_GRAB_KICK

    SetNametagText ( NULL );
    m_ucAlpha = 255;

    // Make sure the nametag color is no longer overridden
    RemoveNametagOverrideColor ();

    m_bNametagShowing = true;
    m_usModel = 0;
}


void CPlayer::SetNametagText ( const char* szText )
{
    if ( m_szNametagText )
    {
        delete [] m_szNametagText;
        m_szNametagText = NULL;
    }
    if ( szText )
    {
        m_szNametagText = new char [ strlen ( szText ) + 1 ];
        strcpy ( m_szNametagText, szText );
    }
}


void CPlayer::GetNametagColor ( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB )
{
    // Is the nametag color overridden?
    if ( m_bNametagColorOverridden )
    {
        ucR = m_ucNametagR;
        ucG = m_ucNametagG;
        ucB = m_ucNametagB;
    }
    else
    {
        // Is the player on a team?
        if ( m_pTeam )
        {
            // Use the team color
            m_pTeam->GetColor ( ucR, ucG, ucB );
        }
        else
        {
            // Default white color
            ucR = 255;
            ucG = 255;
            ucB = 255;
        }
    }
}


void CPlayer::SetNametagOverrideColor ( unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    m_ucNametagR = ucR;
    m_ucNametagG = ucG;
    m_ucNametagB = ucB;
    m_bNametagColorOverridden = true;
}


void CPlayer::RemoveNametagOverrideColor ( void )
{
    m_ucNametagR = 255;
    m_ucNametagG = 255;
    m_ucNametagB = 255;
    m_bNametagColorOverridden = false;
}


// Is it time to send a pure sync to every other player ?
bool CPlayer::IsTimeForPuresyncFar ( void )
{
    long long llTime = GetModuleTickCount64 ();
    if ( llTime > m_llNextFarPuresyncTime )
    {
        int iSlowSyncRate = g_pBandwidthSettings->ZoneUpdateIntervals [ ZONE3 ];
        m_llNextFarPuresyncTime = llTime + iSlowSyncRate;
        m_llNextFarPuresyncTime += rand () % ( 1 + iSlowSyncRate / 10 );   // Extra bit to help distribute the load

        // No far sync if light sync is enabled
        if ( g_pBandwidthSettings->bLightSyncEnabled )
        {
            // Add stats
            // Record all far sync bytes/packets that would have been sent/skipped as skipped
            int iNumPackets = m_FarPlayerList.size ();
            int iNumSkipped = ( iNumPackets * iSlowSyncRate - iNumPackets * 1000 ) / 1000;
            g_pStats->lightsync.llSyncPacketsSkipped += iNumPackets;
            g_pStats->lightsync.llSyncBytesSkipped += iNumPackets * GetApproxPuresyncPacketSize ();
            g_pStats->lightsync.llSyncPacketsSkipped += iNumSkipped;
            g_pStats->lightsync.llSyncBytesSkipped += iNumSkipped * GetApproxPuresyncPacketSize ();
            return false;   // No far sync if light sync is enabled
        }

        // Add stats
        int iNumPackets = m_FarPlayerList.size ();
        int iNumSkipped = ( iNumPackets * iSlowSyncRate - iNumPackets * 1000 ) / 1000;
        g_pStats->puresync.llSentPacketsByZone [ ZONE3 ] += iNumPackets;
        g_pStats->puresync.llSentBytesByZone [ ZONE3 ] += iNumPackets * GetApproxPuresyncPacketSize ();
        g_pStats->puresync.llSkippedPacketsByZone [ ZONE3 ] += iNumSkipped;
        g_pStats->puresync.llSkippedBytesByZone [ ZONE3 ] += iNumSkipped * GetApproxPuresyncPacketSize ();
        return true;
    }
    return false;
}


// Note: The return value must be consumed before m_AnnounceValues is next modified
const std::string& CPlayer::GetAnnounceValue ( const string& strKey ) const
{
    std::map < string, string > ::const_iterator it = m_AnnounceValues.find ( strKey );
    if ( it != m_AnnounceValues.end () )
        return it->second;
    static std::string strDefault;
    return strDefault;
}


void CPlayer::SetAnnounceValue ( const string& strKey, const string& strValue )
{
    if ( strValue.length () > MAX_ANNOUNCE_VALUE_LENGTH )
        m_AnnounceValues [ strKey ] = strValue.substr ( 0, MAX_ANNOUNCE_VALUE_LENGTH );
    else
        m_AnnounceValues [ strKey ] = strValue;
}


void CPlayer::SetWeaponCorrect ( bool bWeaponCorrect )
{
    if ( bWeaponCorrect )
        m_uiWeaponIncorrectCount = 0;
    else
        m_uiWeaponIncorrectCount++;
}


bool CPlayer::GetWeaponCorrect ( void )
{
    return m_uiWeaponIncorrectCount == 0;
}


// Check if other player should be in this players near list
// i.e Should pure/key sync be sent to the other because he can observe us
bool CPlayer::ShouldPlayerBeInNearList ( CPlayer* pOther )
{
    if ( m_usDimension != pOther->GetDimension () )
        return false;

    const CVector& vecOtherPlayerPosition = pOther->GetPosition ();
    CVector vecOtherCameraPosition;
    pOther->GetCamera ()->GetPosition ( vecOtherCameraPosition );

    const CVector& vecPlayerPosition = GetPosition ();

    // Check within distance
    if ( ( vecOtherPlayerPosition - vecPlayerPosition ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER ||
         ( vecOtherCameraPosition - vecPlayerPosition ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER )
    {
        return true;
    }


    // Also check the other way if Lightsync is enabled
    if ( g_pBandwidthSettings->bLightSyncEnabled )
    {
        CVector vecCameraPosition;
        GetCamera ()->GetPosition ( vecCameraPosition );

        if ( ( vecPlayerPosition - vecOtherPlayerPosition ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER ||
             ( vecCameraPosition - vecOtherPlayerPosition ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER )
        {
            return true;
        }
    }

    return false;
}


void CPlayer::MaybeUpdateOthersNearList ( void )
{
    // If too long since last update
    if ( m_UpdateNearListTimer.Get () > (uint)g_TickRateSettings.iNearListUpdate * 9 / 10 )
    {
        CLOCK( "RelayPlayerPuresync", "UpdateNearList_Timer" );
        UpdateOthersNearList ();
        UNCLOCK( "RelayPlayerPuresync", "UpdateNearList_Timer" );
    }
    else
    // or player has moved too far
    if ( ( m_vecUpdateNearLastPosition - GetPosition () ).LengthSquared () > MOVEMENT_UPDATE_THRESH * MOVEMENT_UPDATE_THRESH )
    {
        CLOCK( "RelayPlayerPuresync", "UpdateNearList_Movement" );
        UpdateOthersNearList ();
        UNCLOCK( "RelayPlayerPuresync", "UpdateNearList_Movement" );
    }
}


// Put this player in other players nearlist if this player can observe them in some way
void CPlayer::UpdateOthersNearList ( void )
{
    m_UpdateNearListTimer.Reset ();

    // Get the two positions to check
    const CVector& vecPlayerPosition = GetPosition ();
    CVector vecCameraPosition;
    GetCamera ()->GetPosition ( vecCameraPosition );

    m_vecUpdateNearLastPosition = vecPlayerPosition;

    // Fill resultNearBoth with rough list of nearby players
    CElementResult resultNearBoth;
    {
        // Calculate distance from player to his camera. (Note as spatial database is 2D, we can use the 2D distance here)
        const float fCameraDistance = DistanceBetweenPoints2D ( vecCameraPosition, vecPlayerPosition );
        if ( fCameraDistance < 40.f )
        {
            //
            // If player near his camera (which is the usual case), we can do optimized things
            //

            // Do one query with a slightly bigger sphere
            const CVector vecAvgPos = ( vecCameraPosition + vecPlayerPosition ) * 0.5f;
            GetSpatialDatabase()->SphereQuery ( resultNearBoth, CSphere ( vecAvgPos, DISTANCE_FOR_NEAR_VIEWER + fCameraDistance * 0.5f ) );

        }
        else
        {
            //
            // Bit more complicated if camera is not near player
            //

            // Perform queries on spatial database
            CElementResult resultNearCamera;
            GetSpatialDatabase()->SphereQuery ( resultNearCamera, CSphere ( vecCameraPosition, DISTANCE_FOR_NEAR_VIEWER ) );

            CElementResult resultNearPlayer;
            GetSpatialDatabase()->SphereQuery ( resultNearPlayer, CSphere ( vecPlayerPosition, DISTANCE_FOR_NEAR_VIEWER ) );

            std::set < CPlayer* > mergedList;

            // Merge
            for ( CElementResult::const_iterator it = resultNearCamera.begin () ; it != resultNearCamera.end (); ++it )
                if ( (*it)->GetType () == CElement::PLAYER )
                    mergedList.insert ( (CPlayer*)*it );

            for ( CElementResult::const_iterator it = resultNearPlayer.begin () ; it != resultNearPlayer.end (); ++it )
                if ( (*it)->GetType () == CElement::PLAYER )
                    mergedList.insert ( (CPlayer*)*it );

            // Copy to resultNearBoth
            for ( std::set < CPlayer* > ::iterator it = mergedList.begin (); it != mergedList.end (); ++it )
                resultNearBoth.push_back ( *it );
        }
    }

    // Accurately check distance to other players, and put this player in their near list
    for ( CElementResult::const_iterator it = resultNearBoth.begin () ; it != resultNearBoth.end (); ++it )
    {
        if ( (*it)->GetType () == CElement::PLAYER )
        {
            CPlayer* pOtherPlayer = (CPlayer*)*it;
            if ( pOtherPlayer != this )
            {
                const CVector& vecOtherPlayerPos = pOtherPlayer->GetPosition ();

                // Check distance is accurate
                if ( ( vecPlayerPosition - vecOtherPlayerPos ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER ||
                     ( vecCameraPosition - vecOtherPlayerPos ).LengthSquared () < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER )
                {
                    // Check dimension matches
                    if ( m_usDimension == pOtherPlayer->GetDimension () )
                    {
                        pOtherPlayer->RefreshNearPlayer ( this );

                        // Lightsync needs it the other way round
                        if ( g_pBandwidthSettings->bLightSyncEnabled )
                            this->RefreshNearPlayer ( pOtherPlayer );
                    }
                }
            }
        }
    }
}


void CPlayer::SetVoiceBroadcastTo( CElement* pElement )
{
    m_lstBroadcastList.clear();
    if ( pElement )
        m_lstBroadcastList.push_back ( pElement );
}

void CPlayer::SetVoiceBroadcastTo( const std::list < CElement* >& lstElements )
{
    m_lstBroadcastList = lstElements;
}

void CPlayer::SetVoiceIgnoredElement( CElement* pElement )
{
    m_lstIgnoredList.clear();
    if ( pElement )
        m_lstIgnoredList.push_back ( pElement );
}

void CPlayer::SetVoiceIgnoredList( const std::list < CElement* >& lstElements )
{
    m_lstIgnoredList = lstElements;
}

bool CPlayer::IsPlayerIgnoringElement( CElement* pElement )
{
    // For each ignored element
    for ( list < CElement* > ::const_iterator iter = m_lstIgnoredList.begin () ; iter != m_lstIgnoredList.end () ; ++iter )
    {
        CElement* pIgnoredElement = *iter;
        if ( IS_TEAM ( pIgnoredElement ) )
        {
            // Check team
            CTeam* pTeam = static_cast < CTeam* > ( pIgnoredElement );
            // If the broadcast-to player is in the ignored team
            list < CPlayer* > ::const_iterator iter = pTeam->PlayersBegin ();
            for ( ; iter != pTeam->PlayersEnd (); iter++ )
            {
                if ( *iter == pElement )
                    return true;
            }
        }
        else if ( IS_PLAYER( pIgnoredElement ) )
        {
            // Check player
            if ( pIgnoredElement == pElement )
                return true;
        }
        else
        {
            // Check element decendants
            if ( pIgnoredElement->IsMyChild ( pElement , true ) )
                return true;
        }
    }
    return false;
}


//
// Save rough camera position and rotation for later
//
void CPlayer::SetCameraOrientation ( const CVector& vecPosition, const CVector& vecFwd )
{
    m_vecCamPosition = vecPosition;
    m_vecCamFwd = vecFwd;
}


//
// Ensure other player stays in the near list
//
void CPlayer::RefreshNearPlayer ( CPlayer* pOther )
{
    SViewerInfo* pInfo = MapFind ( m_NearPlayerList, pOther );
    if ( !pInfo )
    {
        // Move from far list
        MovePlayerToNearList ( pOther );
        pInfo = MapFind ( m_NearPlayerList, pOther );
    }
    pInfo->iMoveToFarCountDown = 5;
}


void CPlayer::AddPlayerToDistLists ( CPlayer* pOther )
{
    dassert ( !MapContains ( m_NearPlayerList, pOther ) && !MapContains ( m_FarPlayerList, pOther ) );
    MapSet ( m_NearPlayerList, pOther, SViewerInfo () );
}

void CPlayer::RemovePlayerFromDistLists ( CPlayer* pOther )
{
    dassert ( MapContains ( m_NearPlayerList, pOther ) || MapContains ( m_FarPlayerList, pOther ) );

#ifdef MTA_DEBUG
    SViewerInfo info;
    SViewerInfo* pInfo = MapFind ( m_NearPlayerList, pOther );
    if ( pInfo )
        info = *pInfo;
    dassert ( MapContains ( m_PureSyncSimSendList, pOther ) == info.bInPureSyncSimSendList );
#endif
    MapRemove ( m_PureSyncSimSendList, pOther );
    m_bPureSyncSimSendListDirty = true;

    MapRemove ( m_NearPlayerList, pOther );
    MapRemove ( m_FarPlayerList, pOther );
}

void CPlayer::MovePlayerToNearList ( CPlayer* pOther )
{
    OutputDebugLine ( SString ( "[Sync] ++ %s: Move %s to nearlist", GetNick (), pOther->GetNick () ) );

    dassert ( !MapContains ( m_NearPlayerList, pOther ) && MapContains ( m_FarPlayerList, pOther ) );
    SViewerInfo* pInfo = MapFind ( m_FarPlayerList, pOther );
    MapSet ( m_NearPlayerList, pOther, *pInfo );
    MapRemove ( m_FarPlayerList, pOther );
}

// Assumes CSimControl::UpdatePuresyncSimPlayer is called soon after this function
void CPlayer::MovePlayerToFarList ( CPlayer* pOther )
{
    OutputDebugLine ( SString ( "[Sync] -- %s: Move %s to farlist", GetNick (), pOther->GetNick () ) );

    dassert ( MapContains ( m_NearPlayerList, pOther ) && !MapContains ( m_FarPlayerList, pOther ) );
    SViewerInfo* pInfo = MapFind ( m_NearPlayerList, pOther );

#ifdef MTA_DEBUG
    dassert ( MapContains ( m_PureSyncSimSendList, pOther ) == pInfo->bInPureSyncSimSendList );
#endif
    if ( pInfo->bInPureSyncSimSendList )
    {
        MapRemove ( m_PureSyncSimSendList, pOther );
        m_bPureSyncSimSendListDirty = true;
        pInfo->bInPureSyncSimSendList = false;
    }

    MapSet ( m_FarPlayerList, pOther, *pInfo );
    MapRemove ( m_NearPlayerList, pOther );
}


//
// Dynamically increase the interval between near sync updates depending on stuffs
//
// Called by player who is in 'others' near list
// i.e. 'other' is trying to figure out if should send sync to 'this'
// i.e. Can 'this' see 'other'
//
bool CPlayer::IsTimeToReceivePuresyncNearFrom ( CPlayer* pOther, SViewerInfo& nearInfo )
{
    // Get correct camera position when dead
    if ( m_bIsDead )
        GetCamera ()->GetPosition ( m_vecCamPosition );

    int iZone = GetPuresyncZone ( pOther );
    nearInfo.iZone = iZone;

    int iUpdateInterval = g_pBandwidthSettings->ZoneUpdateIntervals [ iZone ];

#if MTA_DEBUG
    if ( m_iLastPuresyncZoneDebug != iZone )
    {
        // Calc direction from our camera to the other player
        const CVector& vecOtherPosition = pOther->GetPosition ();
        CVector vecDirToOther = pOther->GetPosition () - m_vecCamPosition;

        // Get distance
        float fDistSq = vecDirToOther.LengthSquared ();

        // Get angle between camera direction and direction to other
        vecDirToOther.Normalize ();
        float fDot = m_vecCamFwd.DotProduct ( &vecDirToOther );
        //SetDebugTagHidden ( "Sync", false );
        OutputDebugLine ( SString ( "[Sync] Dist:%1.0f  Dot:%0.3f  %s SyncTo %s zone changing: %d -> %d [Interval:%d] CamPos:%1.0f,%1.0f,%1.0f  CamFwd:%1.2f,%1.2f,%1.2f "
                ,sqrtf ( fDistSq )
                ,fDot
                ,pOther->GetNick ()
                ,GetNick ()
                ,m_iLastPuresyncZoneDebug
                ,iZone
                ,iUpdateInterval
                ,m_vecCamPosition.fX
                ,m_vecCamPosition.fY
                ,m_vecCamPosition.fZ
                ,m_vecCamFwd.fX
                ,m_vecCamFwd.fY
                ,m_vecCamFwd.fZ
            ) );

        m_iLastPuresyncZoneDebug = iZone;
    }
#endif

    long long llTimeNow = GetModuleTickCount64 ();
    long long llNextUpdateTime = nearInfo.llLastUpdateTime + iUpdateInterval;

    if ( llNextUpdateTime > llTimeNow )
    {
        g_pStats->puresync.llSkippedPacketsByZone[ iZone ]++;
        g_pStats->puresync.llSkippedBytesByZone[ iZone ] += GetApproxPuresyncPacketSize ();
        return false;
    }

    nearInfo.llLastUpdateTime = llTimeNow;

    g_pStats->puresync.llSentPacketsByZone[ iZone ]++;
    g_pStats->puresync.llSentBytesByZone[ iZone ] += GetApproxPuresyncPacketSize ();
    return true;
}


//
// Get the size pure sync packet will be for stats only
//
int CPlayer::GetApproxPuresyncPacketSize ( void )
{
    // vehicle passenger=15/driver=52, ped with weapon=34/no weapon=30
    return m_pVehicle ? ( m_uiVehicleSeat ? 15 : 52 ) : ( m_ucWeaponSlot ? 34 : 30 );
}


//
// Deduce what zone the other player is in
//
// Called by player who is in 'others' near list
// i.e. 'other' is trying to figure out if should send sync to 'this'
// i.e. Can 'this' see 'other'
//
int CPlayer::GetPuresyncZone ( CPlayer* pOther )
{
    int iZone = 0;

    // Calc direction from our camera to the other player
    const CVector& vecOtherPosition = pOther->GetPosition ();
    CVector vecDirToOther = vecOtherPosition - m_vecCamPosition;

    // See if in distance zone 0
    float fDistSq = vecDirToOther.LengthSquared ();
    if ( fDistSq < g_pBandwidthSettings->fZone0RadiusSq )
    {
        iZone = 0;
    }
    else
    {
        // Get angle between camera direction and direction to other
        vecDirToOther.Normalize ();
        float fDot = m_vecCamFwd.DotProduct ( &vecDirToOther );
        //  1=0 deg   0=90 deg  -1=180 deg
        if ( fDot > g_pBandwidthSettings->fZone1Dot )
        {
            iZone = 0;
        }
        else
        if ( fDot > g_pBandwidthSettings->fZone2Dot )
        {
            iZone = 1;
        }
        else
            iZone = 2;
    }


    // See if zone could be lowered
    if ( g_pBandwidthSettings->iMaxZoneIfOtherCanSee < iZone )
    {
        // Test if other can see us
        const CVector& vecOtherCamPosition = pOther->GetCamPosition ();
        const CVector& vecOtherCamFwd = pOther->GetCamFwd ();

        // Calc direction from other camera to our player
        CVector vecDirToHere = m_vecPosition - vecOtherCamPosition;

        // Get angle between camera direction and direction to here
        vecDirToHere.Normalize ();
        float fDot = vecOtherCamFwd.DotProduct ( &vecDirToHere );
        //  1=0 deg   0=90 deg  -1=180 deg
        if ( fDot > 0.643 ) // 100 deg fov  [cos ( DEG2RAD( 100 ) * 0.5f )]
            iZone = g_pBandwidthSettings->iMaxZoneIfOtherCanSee;
    }

    return iZone;
}

//
// Here to add player specific information to SetPosition
// - Light sync: Added m_bPositionHasChanged so ls knows the last synced values
//
void CPlayer::SetPosition ( const CVector &vecPosition )
{
    if ( ( vecPosition - m_vecPosition ).Length() > 0.001f )
    {
        // Light Sync
        MarkPositionAsChanged ( );
    }
    CElement::SetPosition ( vecPosition );
}

void CPlayer::SetPlayerStat ( unsigned short usStat, float fValue )
{
    m_pPlayerStatsPacket->Add( usStat, fValue );
    CPed::SetPlayerStat( usStat, fValue );
}

void CPlayer::SetJackingVehicle ( CVehicle* pVehicle )
{
    if ( pVehicle == m_pJackingVehicle )
        return;

    // Remove old
    if ( m_pJackingVehicle )
    {
        CVehicle* pPrev = m_pJackingVehicle;
        m_pJackingVehicle = NULL;
        pPrev->SetJackingPlayer ( NULL );
    }

    // Set new
    m_pJackingVehicle = pVehicle;

    if ( m_pJackingVehicle )
        m_pJackingVehicle->SetJackingPlayer ( this );
}

// Calculate weapon range using efficient stuffs
float CPlayer::GetWeaponRangeFromSlot( uint uiSlot )
{
    eWeaponType eWeapon = static_cast < eWeaponType > ( GetWeaponType ( uiSlot ) );
    float fSkill = GetPlayerStat ( CWeaponStatManager::GetSkillStatIndex ( eWeapon ) );

    if ( fSkill != m_fWeaponRangeLastSkill || eWeapon != m_eWeaponRangeLastWeapon || CWeaponStat::GetAllWeaponStatsRevision() != m_uiWeaponRangeLastStatsRevision )
    {
        m_fWeaponRangeLastSkill = fSkill;
        m_eWeaponRangeLastWeapon = eWeapon;
        m_uiWeaponRangeLastStatsRevision = CWeaponStat::GetAllWeaponStatsRevision();
        m_fWeaponRangeLast = g_pGame->GetWeaponStatManager ( )->GetWeaponRangeFromSkillLevel ( eWeapon, fSkill );       
    }
    return m_fWeaponRangeLast;
}

void CPlayer::SetPlayerVersion ( const SString& strPlayerVersion )
{
    m_strPlayerVersion = strPlayerVersion;
}


/////////////////////////////////////////////////////////////////
// For NearList/FarList hash maps
CPlayer* GetEmptyMapKey ( CPlayer** )
{
    return (CPlayer*)1;
}

CPlayer* GetDeletedMapKey ( CPlayer** )
{
    return (CPlayer*)2;
}


/////////////////////////////////////////////////////////////////
//
// CPlayerBitStream::CPlayerBitStream
//
//
/////////////////////////////////////////////////////////////////
CPlayerBitStream::CPlayerBitStream( CPlayer* pPlayer )
{
    pBitStream = g_pNetServer->AllocateNetServerBitStream ( pPlayer->GetBitStreamVersion() );
}

