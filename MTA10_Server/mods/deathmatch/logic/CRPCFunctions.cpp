/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRPCFunctions.cpp
*  PURPOSE:     Remote procedure call functionality class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CRPCFunctions * g_pRPCFunctions = NULL;
extern CGame * g_pGame;

CPlayer * CRPCFunctions::m_pSourcePlayer;

static CPlayerManager * m_pPlayerManager;

CRPCFunctions::CRPCFunctions ( void )
{
    g_pRPCFunctions = this;

    m_pPlayerManager = g_pGame->GetPlayerManager ();
    AddHandlers ();
}


CRPCFunctions::~CRPCFunctions ( void )
{
    vector < SRPCHandler * > ::iterator iter = m_RPCHandlers.begin ();
    for ( ; iter != m_RPCHandlers.end () ; iter++ )
    {
        delete *iter;
    }
    m_RPCHandlers.clear ();

    g_pRPCFunctions = NULL;
}


void CRPCFunctions::AddHandlers ( void )
{
    AddHandler ( PLAYER_INGAME_NOTICE, PlayerInGameNotice );
    AddHandler ( PLAYER_TARGET, PlayerTarget );
    AddHandler ( PLAYER_WEAPON, PlayerWeapon );
    AddHandler ( KEY_BIND, KeyBind );
    AddHandler ( CURSOR_EVENT, CursorEvent );
    AddHandler ( REQUEST_STEALTH_KILL, RequestStealthKill );
}


void CRPCFunctions::AddHandler ( unsigned char ucID, pfnRPCHandler Callback )
{
    SRPCHandler * pHandler = new SRPCHandler;
    pHandler->ID = ucID;
    pHandler->Callback = Callback;
    g_pRPCFunctions->m_RPCHandlers.push_back ( pHandler );
}


void CRPCFunctions::ProcessPacket ( NetServerPlayerID& Socket, NetBitStreamInterface& bitStream )
{
    m_pSourcePlayer = m_pPlayerManager->Get ( Socket );
    if ( m_pSourcePlayer && !m_pSourcePlayer->IsBeingDeleted () )
    {
        unsigned char ucFunctionID = 255;
        bitStream.Read ( ucFunctionID );

        SRPCHandler * pHandler;
        vector < SRPCHandler * > ::iterator iter = m_RPCHandlers.begin ();
        for ( ; iter != m_RPCHandlers.end () ; iter++ )
        {
            pHandler = *iter;
            if ( pHandler->ID == ucFunctionID )
            {
                (pHandler->Callback) ( bitStream );
                break;
            }
        }
    }
}


void CRPCFunctions::PlayerInGameNotice ( NetBitStreamInterface & bitStream )
{
    // Already ingame? Protocol error
    if ( m_pSourcePlayer->IsIngame () )
    {
        DisconnectPlayer ( g_pGame, *m_pSourcePlayer, "Protocol error: Already ingame" );
    }
    else
    {
        // Join him to the game
        g_pGame->JoinPlayer ( *m_pSourcePlayer );
    }
}


void CRPCFunctions::PlayerTarget ( NetBitStreamInterface & bitStream )
{
    if ( m_pSourcePlayer->IsJoined () )
    {
        ElementID TargetID;
        bitStream.ReadCompressed ( TargetID );

        CElement* pTarget = NULL;
        if ( TargetID != INVALID_ELEMENT_ID ) pTarget = CElementIDs::GetElement ( TargetID );
        m_pSourcePlayer->SetTargetedElement ( pTarget );

        // Call our script event
        CLuaArguments Arguments;
        if ( pTarget ) Arguments.PushElement ( pTarget );
        else Arguments.PushBoolean ( false );

        m_pSourcePlayer->CallEvent ( "onPlayerTarget", Arguments );
    }
}


void CRPCFunctions::PlayerWeapon ( NetBitStreamInterface & bitStream )
{
    if ( m_pSourcePlayer->IsJoined () && m_pSourcePlayer->IsSpawned () )
    {
        SWeaponSlotSync slot;
        bitStream.Read ( &slot );
        unsigned int uiSlot = slot.data.uiSlot;

        m_pSourcePlayer->SetWeaponSlot ( uiSlot );
        CWeapon* pWeapon = m_pSourcePlayer->GetWeapon ( uiSlot );

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            if ( pWeapon )
            {
                SWeaponAmmoSync ammo ( pWeapon->ucType, true, true );
                bitStream.Read ( &ammo );

                pWeapon->usAmmo = ammo.data.usTotalAmmo;
                pWeapon->usAmmoInClip = ammo.data.usAmmoInClip;
            }
        }
        else if ( pWeapon )
        {
            pWeapon->usAmmo = 1;
            pWeapon->usAmmoInClip = 1;
        }
    }
}


void CRPCFunctions::KeyBind ( NetBitStreamInterface & bitStream )
{
    unsigned char ucType;
    bool bHitState;
    if ( bitStream.ReadBit () == true )
        ucType = 1;
    else
        ucType = 0;
    bitStream.ReadBit ( bHitState );

    unsigned char ucKeyLength = bitStream.GetNumberOfUnreadBits () >> 3;
    if ( ucKeyLength < 256 )
    {
        char szKey [ 256 ];
        bitStream.Read ( szKey, ucKeyLength );
        szKey [ ucKeyLength ] = 0;

        m_pSourcePlayer->GetKeyBinds ()->ProcessKey ( szKey, bHitState, ( eKeyBindType ) ucType );
    }
}


void CRPCFunctions::CursorEvent ( NetBitStreamInterface & bitStream )
{
    SMouseButtonSync button;
    unsigned char ucButton;

    CVector2D vecCursorPosition;
    unsigned short usX;
    unsigned short usY;

    SPositionSync position ( false );
    CVector vecPosition;

    bool bHasCollisionElement;
    ElementID elementID;

    if ( bitStream.Read ( &button ) &&
         bitStream.ReadCompressed ( usX ) &&
         bitStream.ReadCompressed ( usY ) &&
         bitStream.Read ( &position ) &&
         bitStream.ReadBit ( bHasCollisionElement ) &&
         ( !bHasCollisionElement || bitStream.ReadCompressed ( elementID ) ) )
    {
        ucButton = button.data.ucButton;
        vecCursorPosition.fX = static_cast < float > ( usX );
        vecCursorPosition.fY = static_cast < float > ( usY );
        vecPosition = position.data.vecPosition;
        if ( !bHasCollisionElement )
            elementID = INVALID_ELEMENT_ID;
    }
    else
        return;

    if ( m_pSourcePlayer->IsJoined () )
    {
        if ( m_pSourcePlayer->IsCursorShowing () )
        {
            // Get the button and state
            const char* szButton = NULL;
            const char* szState = NULL;
            switch ( ucButton )
            {
                case 0: szButton = "left"; szState = "down";
                    break;
                case 1: szButton = "left"; szState = "up";
                    break;
                case 2: szButton = "middle"; szState = "down";
                    break;
                case 3: szButton = "middle"; szState = "up";
                    break;
                case 4: szButton = "right"; szState = "down";
                    break;
                case 5: szButton = "right"; szState = "up";
                    break;
            }
            if ( szButton && szState )
            {
                CElement* pElement = CElementIDs::GetElement ( elementID );
                if ( pElement )
                {
                    // Call the onElementClicked event
                    CLuaArguments Arguments;
                    Arguments.PushString ( szButton );
                    Arguments.PushString ( szState );
                    Arguments.PushElement ( m_pSourcePlayer );
                    Arguments.PushNumber ( vecPosition.fX );
                    Arguments.PushNumber ( vecPosition.fY );
                    Arguments.PushNumber ( vecPosition.fZ );
                    pElement->CallEvent ( "onElementClicked", Arguments );
                }
                // Call the onPlayerClick event
                CLuaArguments Arguments;
                Arguments.PushString ( szButton );
                Arguments.PushString ( szState );
                if ( pElement )
                    Arguments.PushElement ( pElement );
                else
                    Arguments.PushNil ();
                Arguments.PushNumber ( vecPosition.fX );
                Arguments.PushNumber ( vecPosition.fY );
                Arguments.PushNumber ( vecPosition.fZ );
                Arguments.PushNumber ( vecCursorPosition.fX );
                Arguments.PushNumber ( vecCursorPosition.fY );
                m_pSourcePlayer->CallEvent ( "onPlayerClick", Arguments );

                // TODO: iterate server-side element managers for the click events, eg: colshapes
            }
        }
    }
}


void CRPCFunctions::RequestStealthKill ( NetBitStreamInterface & bitStream )
{
    ElementID ID;
    bitStream.ReadCompressed ( ID );
    CElement * pElement = CElementIDs::GetElement ( ID );
    if ( pElement )
    {
        int elementType = pElement->GetType ();
        if ( elementType == CElement::PLAYER || elementType == CElement::PED )
        {
            CPed * pTarget = static_cast < CPed * > ( pElement );

            // Are they both alive?
            if ( !m_pSourcePlayer->IsDead () && !pTarget->IsDead () )
            {
                // Are they close enough?
                if ( DistanceBetweenPoints3D ( m_pSourcePlayer->GetPosition (), pTarget->GetPosition () ) <= 2.0f )
                {
                    // Start the stealth kill
                    CStaticFunctionDefinitions::KillPed ( pTarget, m_pSourcePlayer, 4 /*WEAPONTYPE_KNIFE*/, 9/*BODYPART_HEAD*/, true );
                }
            }
        }
    }
}
