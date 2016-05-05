/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CElementRPCs.cpp
*  PURPOSE:     Element remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CElementRPCs.h"
#include "net/SyncStructures.h"

using std::list;

void CElementRPCs::LoadFunctions ( void )
{
    AddHandler ( SET_ELEMENT_PARENT,             SetElementParent,            "SetElementParent" );
    AddHandler ( SET_ELEMENT_DATA,               SetElementData,              "SetElementData" );
    AddHandler ( REMOVE_ELEMENT_DATA,            RemoveElementData,           "RemoveElementData" );
    AddHandler ( SET_ELEMENT_POSITION,           SetElementPosition,          "SetElementPosition" );
    AddHandler ( SET_ELEMENT_VELOCITY,           SetElementVelocity,          "SetElementVelocity" );
    AddHandler ( SET_ELEMENT_INTERIOR,           SetElementInterior,          "SetElementInterior" );
    AddHandler ( SET_ELEMENT_DIMENSION,          SetElementDimension,         "SetElementDimension" );
    AddHandler ( ATTACH_ELEMENTS,                AttachElements,              "AttachElements" );
    AddHandler ( DETACH_ELEMENTS,                DetachElements,              "DetachElements" );
    AddHandler ( SET_ELEMENT_ALPHA,              SetElementAlpha,             "SetElementAlpha" );
    AddHandler ( SET_ELEMENT_NAME,               SetElementName,              "SetElementName" );
    AddHandler ( SET_ELEMENT_HEALTH,             SetElementHealth,            "SetElementHealth" );
    AddHandler ( SET_ELEMENT_MODEL,              SetElementModel,             "SetElementModel" );
    AddHandler ( SET_ELEMENT_ATTACHED_OFFSETS,   SetElementAttachedOffsets,   "SetElementAttachedOffsets" );
    AddHandler ( SET_ELEMENT_DOUBLESIDED,        SetElementDoubleSided,       "SetElementDoubleSided" );
    AddHandler ( SET_ELEMENT_COLLISIONS_ENABLED, SetElementCollisionsEnabled, "SetElementCollisionsEnabled" );
    AddHandler ( SET_ELEMENT_FROZEN,             SetElementFrozen,            "SetElementFrozen" );
    AddHandler ( SET_LOW_LOD_ELEMENT,            SetLowLodElement,            "SetLowLodElement" );
    AddHandler ( FIRE_CUSTOM_WEAPON,             FireCustomWeapon,            "fireWeapon" );
    AddHandler ( SET_CUSTOM_WEAPON_STATE,        SetCustomWeaponState,        "setWeaponState" );
    AddHandler ( SET_CUSTOM_WEAPON_CLIP_AMMO,    SetCustomWeaponClipAmmo,     "setWeaponClipAmmo" );
    AddHandler ( SET_CUSTOM_WEAPON_AMMO,         SetCustomWeaponAmmo,         "setWeaponAmmo" );
    AddHandler ( SET_CUSTOM_WEAPON_TARGET,       SetCustomWeaponTarget,       "setWeaponTarget" );
    AddHandler ( RESET_CUSTOM_WEAPON_TARGET,     ResetCustomWeaponTarget,     "resetWeaponTarget" );
    AddHandler ( SET_CUSTOM_WEAPON_FLAGS,        SetCustomWeaponFlags,        "setWeaponFlags" );
    AddHandler ( SET_CUSTOM_WEAPON_FIRING_RATE,  SetCustomWeaponFiringRate,   "setWeaponFiringRate" );
    AddHandler ( RESET_CUSTOM_WEAPON_FIRING_RATE,ResetCustomWeaponFiringRate, "resetWeaponFiringRate" );
    AddHandler ( SET_WEAPON_OWNER,               SetWeaponOwner,              "setWeaponOwner" );
    AddHandler ( SET_CUSTOM_WEAPON_FLAGS,        SetWeaponConfig,             "setWeaponFlags" );
    AddHandler ( SET_PROPAGATE_CALLS_ENABLED,    SetCallPropagationEnabled,   "setCallPropagationEnabled" );
}

#define RUN_CHILDREN_SERVER( func ) \
    if ( pSource->CountChildren () && pSource->IsCallPropagationEnabled() ) \
    { \
        CElementListSnapshot* pList = pSource->GetChildrenListSnapshot(); \
        pList->AddRef();    /* Keep list alive during use */ \
        for ( CElementListSnapshot::const_iterator iter = pList->begin() ; iter != pList->end() ; iter++ ) \
            if ( !(*iter)->IsBeingDeleted() && !(*iter)->IsLocalEntity() ) \
                func; \
        pList->Release(); \
    }

void CElementRPCs::SetElementParent ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the entity id and parent id
    ElementID ParentID;
    if ( bitStream.Read ( ParentID ) )
    {
        CClientEntity* pParent = CElementIDs::GetElement ( ParentID );
        if ( pParent )
        {
            pSource->SetParent ( pParent );
        }
        else
        {
            // TODO: raise an error
        }
    }
    else
    {
        // TODO: raise an error
    }
}


void CElementRPCs::SetElementData ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned short usNameLength;
    if ( bitStream.ReadCompressed ( usNameLength ) )
    {
        // We should never receive an illegal name length from the server
        if ( usNameLength > MAX_CUSTOMDATA_NAME_LENGTH )
        {
            CLogger::ErrorPrintf ( "RPC SetElementData name length > MAX_CUSTOMDATA_NAME_LENGTH" );
            return;
        }
        SString strName;
        CLuaArgument Argument;
        if ( bitStream.ReadStringCharacters ( strName, usNameLength ) && Argument.ReadFromBitStream ( bitStream ) )
        {
            pSource->SetCustomData ( strName, Argument );
        }
    }
}


void CElementRPCs::RemoveElementData ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the name length
    unsigned short usNameLength;
    bool bRecursive;    // Unused
    if ( bitStream.ReadCompressed ( usNameLength ) )
    {
        SString strName;

        // Read out the name plus whether it's recursive or not
        if ( bitStream.ReadStringCharacters ( strName, usNameLength ) &&
             bitStream.ReadBit ( bRecursive ) )
        {
            // Remove that name
            pSource->DeleteCustomData ( strName );
        }
    }
}


void CElementRPCs::SetElementPosition ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the entity id and the position
    CVector vecPosition;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( vecPosition.fX ) &&
         bitStream.Read ( vecPosition.fY ) &&
         bitStream.Read ( vecPosition.fZ ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        // Update the sync context to the new one
        pSource->SetSyncTimeContext ( ucTimeContext );

        // If it's a player, use Teleport
        if ( pSource->GetType () == CCLIENTPLAYER )
        {
            unsigned char ucWarp = 1;
            bitStream.Read ( ucWarp );

            CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pSource );

            if ( ucWarp )
            {
                pPlayer->Teleport ( vecPosition );
                pPlayer->ResetInterpolation ();
            }
            else
            {
                pPlayer->SetPosition ( vecPosition );
            }

            // If local player, reset return position (so we can't warp back if connection fails)
            if ( pPlayer->IsLocalPlayer () )
            {
                m_pClientGame->GetNetAPI ()->ResetReturnPosition ();
            }
        }
        else if ( pSource->GetType () == CCLIENTVEHICLE ) 
        {
            CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pSource );
            pVehicle->RemoveTargetPosition ();
            pVehicle->SetPosition ( vecPosition );
        }
        else
        {
            // Set its position
            pSource->SetPosition ( vecPosition );
        }
    }
}


void CElementRPCs::SetElementVelocity ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out the entity id and the speed
    CVector vecVelocity;
    if ( bitStream.Read ( vecVelocity.fX ) &&
         bitStream.Read ( vecVelocity.fY ) &&
         bitStream.Read ( vecVelocity.fZ ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed* > ( pSource );

                pPed->SetMoveSpeed ( vecVelocity );
                pPed->ResetInterpolation ();

                // If local player, reset return position (so we can't warp back if connection fails)
                if ( pPed->IsLocalPlayer () )
                {
                    m_pClientGame->GetNetAPI ()->ResetReturnPosition ();
                }
                break;
            }
            case CCLIENTVEHICLE:
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pSource );                    
                pVehicle->SetMoveSpeed ( vecVelocity );

                break;
            }
            case CCLIENTOBJECT:
            case CCLIENTWEAPON:
            {
                CClientObject * pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetMoveSpeed ( vecVelocity );
                
                break;
            }
        }
    }
}


void CElementRPCs::SetElementInterior ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucInterior, ucSetPosition;
    if ( bitStream.Read ( ucInterior ) && bitStream.Read ( ucSetPosition ) )
    {
        pSource->SetInterior ( ucInterior );

        if ( ucSetPosition == 1 )
        {
            CVector vecPosition;
            if ( bitStream.Read ( vecPosition.fX ) &&
                 bitStream.Read ( vecPosition.fY ) &&
                 bitStream.Read ( vecPosition.fZ ) )
            {
                pSource->SetPosition ( vecPosition );
            }
        }
    }
}


void CElementRPCs::SetElementDimension ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned short usDimension;
    if ( bitStream.Read ( usDimension ) )
    {
        if ( pSource->GetType () == CCLIENTTEAM )
        {
            CClientTeam* pTeam = static_cast < CClientTeam* > ( pSource );
            list < CClientPlayer* > ::const_iterator iter = pTeam->IterBegin ();
            for ( ; iter != pTeam->IterEnd () ; iter++ )
            {
                CClientPlayer* pPlayer = *iter;
                if ( pPlayer->IsLocalPlayer () )
                {
                    // Update all of our streamers/managers to the local player's dimension
                    m_pClientGame->SetAllDimensions ( usDimension );
                }

                pPlayer->SetDimension ( usDimension );
            }
        }
        else
        {
            if ( pSource->GetType () == CCLIENTPLAYER )
            {
                CClientPlayer* pPlayer = static_cast < CClientPlayer* > ( pSource );
                if ( pPlayer->IsLocalPlayer () )
                {
                    // Update all of our streamers/managers to the local player's dimension
                    m_pClientGame->SetAllDimensions ( usDimension );
                }         
            }
            
            pSource->SetDimension ( usDimension );
        }
    }
}


void CElementRPCs::AttachElements ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    ElementID usAttachedToID;
    CVector vecPosition, vecRotation;
    if ( bitStream.Read ( usAttachedToID ) &&
        bitStream.Read ( vecPosition.fX ) &&
        bitStream.Read ( vecPosition.fY ) &&
        bitStream.Read ( vecPosition.fZ ) &&
        bitStream.Read ( vecRotation.fX ) &&
        bitStream.Read ( vecRotation.fY ) &&
        bitStream.Read ( vecRotation.fZ ) )
    {
        CClientEntity* pAttachedToEntity = CElementIDs::GetElement ( usAttachedToID );
        if ( pAttachedToEntity )
        {
            pSource->SetAttachedOffsets ( vecPosition, vecRotation );
            pSource->AttachTo ( pAttachedToEntity );
        }
    }
}


void CElementRPCs::DetachElements ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucTimeContext;
    if ( bitStream.Read ( ucTimeContext ) )
    {
        pSource->SetSyncTimeContext ( ucTimeContext );
        pSource->AttachTo ( NULL );

        CVector vecPosition;
        if ( bitStream.Read ( vecPosition.fX ) &&
             bitStream.Read ( vecPosition.fY ) &&
             bitStream.Read ( vecPosition.fZ ) )
        {
            pSource->SetPosition ( vecPosition );
        }
    }
}


void CElementRPCs::SetElementAlpha ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned char ucAlpha;
    if ( bitStream.Read ( ucAlpha ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed* > ( pSource );
                pPed->SetAlpha ( ucAlpha );
                break;
            }                
            case CCLIENTVEHICLE:
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pSource );
                pVehicle->SetAlpha ( ucAlpha );
                break;
            }
            case CCLIENTOBJECT:
            case CCLIENTWEAPON:
            {
                CClientObject * pObject = static_cast < CClientObject* > ( pSource );
                pObject->SetAlpha ( ucAlpha );
                break;
            }
            default: break;
        }
    }
}


void CElementRPCs::SetElementDoubleSided ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bDoubleSided;
    if ( bitStream.ReadBit ( bDoubleSided ) )
    {
        pSource->SetDoubleSided ( bDoubleSided );
    }
}


void CElementRPCs::SetElementName ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SString strName;
    if ( bitStream.ReadString ( strName ) )
        pSource->SetName ( strName );
}


void CElementRPCs::SetElementHealth ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    float fHealth;
    unsigned char ucTimeContext;
    if ( bitStream.Read ( fHealth ) &&
         bitStream.Read ( ucTimeContext ) )
    {
        pSource->SetSyncTimeContext ( ucTimeContext );

        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed * > ( pSource );
                if ( pPed->IsHealthLocked() )
                    pPed->LockHealth ( fHealth );
                else
                    pPed->SetHealth ( fHealth );
                break;
            }

            case CCLIENTVEHICLE:
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pSource );
                pVehicle->SetHealth ( fHealth );
                break;
            }

            case CCLIENTOBJECT:
            case CCLIENTWEAPON:
            {
                CClientObject* pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetHealth ( fHealth );
                break;
            }
        }
    }
}


void CElementRPCs::SetElementModel ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    unsigned short usModel;
    if ( bitStream.Read ( usModel ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed * > ( pSource );
                pPed->SetModel ( usModel );
                break;
            }

            case CCLIENTVEHICLE:
            {
                uchar ucVariant = 255, ucVariant2 = 255;
                if ( bitStream.GetNumberOfUnreadBits () >= sizeof ( ucVariant ) + sizeof ( ucVariant2 ) )
                {
                    bitStream.Read ( ucVariant );
                    bitStream.Read ( ucVariant2 );
                }
                CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pSource );
                pVehicle->SetModelBlocking ( usModel, ucVariant, ucVariant2 );
                break;
            }

            case CCLIENTOBJECT:
            case CCLIENTWEAPON:
            {
                CClientObject* pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetModel ( usModel );
                break;
            }
        }
    }
}


void CElementRPCs::SetElementAttachedOffsets ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SPositionSync position ( true );
    SRotationRadiansSync rotation ( true );
    if ( position.Read ( bitStream ) && rotation.Read ( bitStream ) )
    {
        pSource->SetAttachedOffsets ( position.data.vecPosition, rotation.data.vecRotation );
    }
}

void CElementRPCs::SetElementCollisionsEnabled ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bEnable;

    if ( bitStream.ReadBit ( bEnable ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed * > ( pSource );
                pPed->SetUsesCollision ( bEnable );
                break;
            }

            case CCLIENTVEHICLE:
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pSource );
                pVehicle->SetCollisionEnabled ( bEnable );
                break;
            }

            case CCLIENTOBJECT:
            {
                CClientObject* pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetCollisionEnabled ( bEnable );
                break;
            }
        }
    }
}

void CElementRPCs::SetElementFrozen ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    bool bFrozen;

    if ( bitStream.ReadBit ( bFrozen ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTPED:
            case CCLIENTPLAYER:
            {
                CClientPed* pPed = static_cast < CClientPed * > ( pSource );
                pPed->SetFrozen ( bFrozen );
                break;
            }

            case CCLIENTVEHICLE:
            {
                CClientVehicle* pVehicle = static_cast < CClientVehicle * > ( pSource );
                pVehicle->SetFrozen ( bFrozen );
                break;
            }

            case CCLIENTOBJECT:
            {
                CClientObject* pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetStatic ( bFrozen );
                break;
            }
        }
    }
}


void CElementRPCs::SetLowLodElement ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    ElementID LowLodObjectID;
    if ( bitStream.Read ( LowLodObjectID ) )
    {
        switch ( pSource->GetType () )
        {
            case CCLIENTOBJECT:
            {
                CClientObject* pLowLodObject = DynamicCast < CClientObject > ( CElementIDs::GetElement ( LowLodObjectID ) );
                CClientObject* pObject = static_cast < CClientObject * > ( pSource );
                pObject->SetLowLodObject ( pLowLodObject );
                break;
            }
        }
    }
}

void CElementRPCs::FireCustomWeapon ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->Fire ( true );
    }
}

void CElementRPCs::SetCustomWeaponState ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    char cWeaponState = 0;
    if ( bitStream.Read ( cWeaponState ) &&
        pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->SetWeaponState ( (eWeaponState) cWeaponState );
    }
}

void CElementRPCs::SetCustomWeaponClipAmmo ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    int iAmmo = 0;
    if ( bitStream.Read ( iAmmo ) &&
        pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->SetClipAmmo ( iAmmo );
    }
}

void CElementRPCs::SetCustomWeaponAmmo ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    int iAmmo = 0;
    if ( bitStream.Read ( iAmmo ) &&
        pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->SetAmmo ( iAmmo );
    }
}

void CElementRPCs::SetCustomWeaponTarget ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    ElementID elementID = INVALID_ELEMENT_ID;
    char cTargetBone = 0;
    bool bVector = false;
    CVector vecTarget;

    if ( bitStream.ReadBit ( bVector ) &&
        pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        if ( bVector )
        {
            if ( bitStream.ReadVector ( vecTarget.fX, vecTarget.fY, vecTarget.fZ ) )
            {
                pWeapon->SetWeaponTarget ( vecTarget );
            }
        }
        else
        {
            if ( bitStream.Read ( elementID ) && 
                bitStream.Read ( cTargetBone ) )
            {
                pWeapon->SetWeaponTarget ( CElementIDs::GetElement( elementID ), cTargetBone );
            }
        }
    }
}

void CElementRPCs::ResetCustomWeaponTarget ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->ResetWeaponTarget ( );
    }
}

void CElementRPCs::SetCustomWeaponFlags ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->Fire ( );
    }
}

void CElementRPCs::SetCustomWeaponFiringRate ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    int iFiringRate = 0;
    if ( bitStream.Read ( iFiringRate ) &&
        pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->SetWeaponFireTime ( iFiringRate );
    }
}

void CElementRPCs::ResetCustomWeaponFiringRate ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        pWeapon->ResetWeaponFireTime ( );
    }
}

void CElementRPCs::SetWeaponOwner ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        ElementID PlayerID;
        if ( bitStream.Read ( PlayerID ) )
        {
            CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
            CClientPlayer * pPlayer = NULL;
            if ( PlayerID != INVALID_ELEMENT_ID )
            {
                pPlayer = DynamicCast < CClientPlayer > ( CElementIDs::GetElement ( PlayerID ) );
            }
            else
                pPlayer = NULL;

            pWeapon->SetOwner ( pPlayer );
        }
    }
}

void CElementRPCs::SetWeaponConfig ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    if ( pSource->GetType() == CCLIENTWEAPON )
    {
        CClientWeapon * pWeapon = static_cast < CClientWeapon * > ( pSource );
        SWeaponConfiguration weaponConfig;

        if ( bitStream.ReadBit ( weaponConfig.bDisableWeaponModel ) &&
            bitStream.ReadBit ( weaponConfig.bInstantReload ) &&
            bitStream.ReadBit ( weaponConfig.bShootIfTargetBlocked ) &&
            bitStream.ReadBit ( weaponConfig.bShootIfTargetOutOfRange ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckBuildings ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckCarTires ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckDummies ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckObjects ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckPeds ) &&
            bitStream.ReadBit ( weaponConfig.flags.bCheckVehicles ) &&
            bitStream.ReadBit ( weaponConfig.flags.bIgnoreSomeObjectsForCamera ) &&
            bitStream.ReadBit ( weaponConfig.flags.bSeeThroughStuff ) &&
            bitStream.ReadBit ( weaponConfig.flags.bShootThroughStuff ) )
        {
            pWeapon->SetFlags ( weaponConfig );
        }
    }
}

void CElementRPCs::SetCallPropagationEnabled ( CClientEntity * pSource, NetBitStreamInterface& bitStream )
{
    bool bEnabled;
    if ( bitStream.ReadBit ( bEnabled ) )
    {
        pSource->SetCallPropagationEnabled ( bEnabled );
    }
}
