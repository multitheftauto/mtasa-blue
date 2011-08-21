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

    m_iType = CElement::PLAYER;
    SetTypeName ( "player" );
    m_bIsPlayer = true;
    m_bDoNotSendEntities = false;
    m_szNick [0] = 0;
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

    m_ucLoginAttempts = 0;

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

    m_llNextFarSyncTime = 0;

    // Sync stuff
    m_bSyncingVelocity = false;
    m_uiPuresyncPackets = 0;

    m_ulLastReceivedSyncTime = 0;

    m_uiWeaponIncorrectCount = 0;

    m_llNearListUpdateTime = 0;

    // Add us to the manager
    pPlayerManager->AddToList ( this );
    m_iLastZoneDebug = 0;
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

    // Unlink from manager
    Unlink ();

    // Unparent us (CElement's unparenting will crash because of the incomplete vtable at that point)
    m_bDoNotSendEntities = true;
    SetParentObject ( NULL );

    CElementRefManager::RemoveElementRefs ( ELEMENT_REF_DEBUG ( this, "CPlayer" ), &m_pTeam, NULL );
    CElementRefManager::RemoveElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstBroadcastList" ), &m_lstBroadcastList );
    CElementRefManager::RemoveElementListRef ( ELEMENT_REF_DEBUG ( this, "CPlayer m_lstIgnoredList" ), &m_lstIgnoredList );
}


void CPlayer::DoPulse ( void )
{
    if ( GetStatus () == STATUS_JOINED )
    {
        m_pPlayerTextManager->Process ();

        if ( GetTickCount64_ () > m_llNearListUpdateTime + 300 )
            UpdateOthersNearList ();
    }
}


void CPlayer::Unlink ( void )
{
    // Remove us from the player manager
    m_pPlayerManager->RemoveFromList ( this );
}


void CPlayer::SetNick ( const char* szNick )
{
    if ( strlen ( m_szNick ) > 0 && strcmp ( m_szNick, szNick ) != 0 )
    {
        // If changing, add the new name to the whowas list
        char szIP [22];
        g_pGame->GetConsole ()->GetWhoWas ()->Add ( szNick, inet_addr ( GetSourceIP( szIP ) ), GetSerial (), GetPlayerVersion () );
    }

    assert ( sizeof ( m_szNick ) == MAX_NICK_LENGTH + 1 );
    // Copy the nick to us
    STRNCPY ( m_szNick, szNick, MAX_NICK_LENGTH + 1 );
}


char* CPlayer::GetSourceIP ( char* pBuffer )
{
    // Grab the player IP
    char szIP [22];
    unsigned short usPort;
    g_pNetServer->GetPlayerIP ( m_PlayerSocket, szIP, &usPort );

    // Copy the buffer and return a pointer to it
    strcpy ( pBuffer, szIP );
    return pBuffer;
}

// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
void CPlayer::Send ( const CPacket& Packet )
{
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

    // Allocate a bitstream for it
    NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( GetBitStreamVersion () );
    if ( pBitStream )
    {
        // Write the content to it and send it
        if ( Packet.Write ( *pBitStream ) )
        {
            g_pNetServer->SendPacket ( Packet.GetPacketID (), m_PlayerSocket, pBitStream, FALSE, packetPriority, Reliability, PACKET_ORDERING_GAME );
        }

        // Destroy the bitstream
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
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
        if ( !m_SyncingVehicles.empty() ) m_SyncingVehicles.remove ( pVehicle );
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
        if ( !m_SyncingPeds.empty() ) m_SyncingPeds.remove ( pPed );
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
        if ( !m_SyncingObjects.empty() ) m_SyncingObjects.remove ( pObject );
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
    memset ( m_fStats, 0, sizeof ( m_fStats ) );
    m_fStats [ 24 ] = 569.0f;           // default max_health
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
bool CPlayer::IsTimeForFarSync ( void )
{
    long long llTime = GetTickCount64_ ();
    if ( llTime > m_llNextFarSyncTime )
    {
        int iSlowSyncRate = g_pBandwidthSettings->ZoneUpdateIntervals [ ZONE3 ];
        m_llNextFarSyncTime = llTime + iSlowSyncRate;
        m_llNextFarSyncTime += rand () % ( 1 + iSlowSyncRate / 10 );   // Extra bit to help distribute the load

        // Calc stats
        int iNumPackets = m_FarPlayerList.size ();
        int iNumSkipped = ( iNumPackets * iSlowSyncRate - iNumPackets * 1000 ) / 1000;
        g_pStats->puresync.uiSentPacketsByZone [ ZONE3 ] += iNumPackets;
        g_pStats->puresync.uiSkippedPacketsByZone [ ZONE3 ] += iNumSkipped;
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

// Put this player in other players nearlist, if errm... the player is near, enough
void CPlayer::UpdateOthersNearList ( void )
{
    m_llNearListUpdateTime = GetTickCount64_ ();

    // Get the two positions to check
    const CVector& vecPlayerPosition = GetPosition ();
    CVector vecCameraPosition;
    GetCamera ()->GetPosition ( vecCameraPosition );

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
            GetSpatialDatabase()->SphereQuery ( resultNearBoth, CSphere ( vecAvgPos, DISTANCE_FOR_SLOW_SYNCRATE + fCameraDistance * 0.5f ) );

        }
        else
        {
            //
            // Bit more complicated if camera is not near player
            //

            // Perform queries on spatial database
            CElementResult resultNearCamera;
            GetSpatialDatabase()->SphereQuery ( resultNearCamera, CSphere ( vecCameraPosition, DISTANCE_FOR_SLOW_SYNCRATE ) );

            CElementResult resultNearPlayer;
            GetSpatialDatabase()->SphereQuery ( resultNearPlayer, CSphere ( vecPlayerPosition, DISTANCE_FOR_SLOW_SYNCRATE ) );

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
                if ( ( vecPlayerPosition - vecOtherPlayerPos ).LengthSquared () < DISTANCE_FOR_SLOW_SYNCRATE * DISTANCE_FOR_SLOW_SYNCRATE ||
                     ( vecCameraPosition - vecOtherPlayerPos ).LengthSquared () < DISTANCE_FOR_SLOW_SYNCRATE * DISTANCE_FOR_SLOW_SYNCRATE )
                {
                    // Check dimension matches
                    if ( m_usDimension == pOtherPlayer->GetDimension () )
                        pOtherPlayer->RefreshNearPlayer ( this );
                }
            }
        }
    }
}

void CPlayer::SetVoiceBroadcastTo( CElement* pElement )
{
    m_lstBroadcastList.clear();
    m_lstBroadcastList.push_back ( pElement );
}

void CPlayer::SetVoiceBroadcastTo( const std::list < CElement* >& lstElements )
{
    m_lstBroadcastList = lstElements;
}

void CPlayer::SetVoiceIgnoredElement( CElement* pElement )
{
    m_lstIgnoredList.clear();
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
    SNearInfo* pInfo = MapFind ( m_NearPlayerList, pOther );
    if ( !pInfo )
    {
        // Move from far list
        MovePlayerToNearList ( pOther );
        pInfo = MapFind ( m_NearPlayerList, pOther );
    }
    pInfo->iCount = 5;
}


void CPlayer::AddPlayerToDistLists ( CPlayer* pOther )
{
    dassert ( !MapContains ( m_NearPlayerList, pOther ) && !MapContains ( m_FarPlayerList, pOther ) );
    SNearInfo info = { 0, 0 };
    MapSet ( m_NearPlayerList, pOther, info );
}

void CPlayer::RemovePlayerFromDistLists ( CPlayer* pOther )
{
    dassert ( MapContains ( m_NearPlayerList, pOther ) || MapContains ( m_FarPlayerList, pOther ) );
    MapRemove ( m_NearPlayerList, pOther );
    MapRemove ( m_FarPlayerList, pOther );
}

void CPlayer::MovePlayerToNearList ( CPlayer* pOther )
{
    dassert ( !MapContains ( m_NearPlayerList, pOther ) && MapContains ( m_FarPlayerList, pOther ) );
    SNearInfo* pInfo = MapFind ( m_FarPlayerList, pOther );
    MapSet ( m_NearPlayerList, pOther, *pInfo );
    MapRemove ( m_FarPlayerList, pOther );
}

void CPlayer::MovePlayerToFarList ( CPlayer* pOther )
{
    dassert ( MapContains ( m_NearPlayerList, pOther ) && !MapContains ( m_FarPlayerList, pOther ) );
    SNearInfo* pInfo = MapFind ( m_NearPlayerList, pOther );
    MapSet ( m_FarPlayerList, pOther, *pInfo );
    MapRemove ( m_NearPlayerList, pOther );
}


//
// Dynamically increase the interval between near sync updates depending on stuffs
//
bool CPlayer::IsTimeToReceiveNearSyncFrom ( CPlayer* pOther, SNearInfo& nearInfo )
{
    int iZone = GetSyncZone ( pOther );

    int iUpdateInterval = g_pBandwidthSettings->ZoneUpdateIntervals [ iZone ];

#if MTA_DEBUG
    if ( m_iLastZoneDebug != iZone )
    {
        // Calc direction from our camera to the other player
        const CVector& vecOtherPosition = pOther->GetPosition ();
        CVector vecDirToOther = pOther->GetPosition () - m_vecCamPosition;

        // Get distance
        float fDistSq = vecDirToOther.LengthSquared ();

        // Get angle between camera direction and direction to other
        vecDirToOther.Normalize ();
        float fDot = m_vecCamFwd.DotProduct ( &vecDirToOther );

        OutputDebugLine ( SString ( "Dist:%1.0f  Dot:%0.3f  %s SyncTo %s zone changing: %d -> %d [Interval:%d] CamPos:%1.0f,%1.0f,%1.0f  CamFwd:%1.2f,%1.2f,%1.2f "
                ,sqrtf ( fDistSq )
                ,fDot
                ,pOther->GetNick ()
                ,GetNick ()
                ,m_iLastZoneDebug
                ,iZone
                ,iUpdateInterval
                ,m_vecCamPosition.fX
                ,m_vecCamPosition.fY
                ,m_vecCamPosition.fZ
                ,m_vecCamFwd.fX
                ,m_vecCamFwd.fY
                ,m_vecCamFwd.fZ
            ) );

        m_iLastZoneDebug = iZone;
    }
#endif

    long long llTimeNow = GetModuleTickCount64 ();
    long long llNextUpdateTime = nearInfo.llLastUpdateTime + iUpdateInterval;

    if ( llNextUpdateTime > llTimeNow )
    {
        g_pStats->puresync.uiSkippedPacketsByZone[ iZone ]++;
        return false;
    }

    nearInfo.llLastUpdateTime = llTimeNow;

    STATS_COUNTER_INC( puresync.uiSentPacketsByZone[ iZone ] );
    g_pStats->puresync.uiSentPacketsByZone[ iZone ]++;
    return true;
}


//
// Deduce what zone the other player is in
//
int CPlayer::GetSyncZone ( CPlayer* pOther )
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
