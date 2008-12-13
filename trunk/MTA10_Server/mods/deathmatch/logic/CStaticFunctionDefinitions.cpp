/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CStaticFunctionDefinitions.cpp
*  PURPOSE:     Lua static function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

static CLuaManager*                                 m_pLuaManager;
static CColManager*                                 m_pColManager;
static CPickupManager *                             m_pPickupManager;
static CPlayerManager *                             m_pPlayerManager;
static CVehicleManager *                            m_pVehicleManager;
static CObjectManager *                             m_pObjectManager;
static CMarkerManager*                              m_pMarkerManager;
static CMapManager*                                 m_pMapManager;
static CBlipManager*                                m_pBlipManager;
static CRadarAreaManager*                           m_pRadarAreaManager;
static CTeamManager*                                m_pTeamManager;
static CClock*                                      m_pClock;
static CEvents*                                     m_pEvents;
static CElementDeleter*                             m_pElementDeleter;
static CMainConfig*                                 m_pMainConfig;
static CRegistry*                                   m_pRegistry;
static CAccountManager*                             m_pAccountManager;
static CBanManager*                                 m_pBanManager;
static CPedManager*                                 m_pPedManager;

// Used to run a function on all the children of the elements too
#define RUN_CHILDREN list<CElement*>::const_iterator iter=pElement->IterBegin();for(;iter!=pElement->IterEnd();iter++)
#define RUN_CHILDREN_BACKWARDS list<CElement*>::const_reverse_iterator iter=pElement->IterReverseBegin();for(;iter!=pElement->IterReverseEnd();iter++)

CStaticFunctionDefinitions::CStaticFunctionDefinitions ( CGame * pGame )
{
    m_pLuaManager = pGame->GetLuaManager ();
    m_pColManager = pGame->GetColManager ();
    m_pPickupManager = pGame->GetPickupManager ();
    m_pPlayerManager = pGame->GetPlayerManager ();
    m_pVehicleManager = pGame->GetVehicleManager ();
    m_pObjectManager = pGame->GetObjectManager ();
    m_pMarkerManager = pGame->GetMarkerManager ();
    m_pMapManager = pGame->GetMapManager ();
    m_pBlipManager = pGame->GetBlipManager ();
    m_pRadarAreaManager = pGame->GetRadarAreaManager ();
    m_pTeamManager = pGame->GetTeamManager ();
    m_pClock = pGame->GetClock ();
    m_pEvents = pGame->GetEvents ();
    m_pElementDeleter = pGame->GetElementDeleter ();
    m_pMainConfig = pGame->GetConfig ();
    m_pRegistry = pGame->GetRegistry ();
    m_pAccountManager = pGame->GetAccountManager ();
    m_pBanManager = pGame->GetBanManager ();
    m_pPedManager = pGame->GetPedManager ();
}


CStaticFunctionDefinitions::~CStaticFunctionDefinitions ( void )
{
}


bool CStaticFunctionDefinitions::AddEvent ( CLuaMain* pLuaMain, const char* szName, const char* szArguments, bool bAllowRemoteTrigger )
{
    assert ( pLuaMain );
    assert ( szName );
    assert ( szArguments );

    // Valid name?
    if ( strlen ( szName ) > 0 )
    {
        // Add our event to CEvents
        return m_pEvents->AddEvent ( szName, szArguments, pLuaMain, bAllowRemoteTrigger );
    }

    return false;
}


bool CStaticFunctionDefinitions::AddEventHandler ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, int iLuaFunction, bool bPropagated )
{
    assert ( pLuaMain );
    assert ( szName );
    assert ( pElement );

    // We got an event with that name?
    if ( m_pEvents->Exists ( szName ) )
    {
        // Add the event handler
        if ( pElement->AddEvent ( pLuaMain, szName, iLuaFunction, bPropagated ) )
            return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveEventHandler ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, int iLuaFunction )
{
    assert ( pLuaMain );
    assert ( szName );
    assert ( pElement );

    // We got an event and handler with that name?
    if ( m_pEvents->Exists ( szName ) )
    {
        if ( pElement->DeleteEvent ( pLuaMain, szName, iLuaFunction ) )
        {
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::TriggerEvent ( const char* szName, CElement* pElement, const CLuaArguments& Arguments, bool & bWasCanceled )
{
    // There is such event?
    if ( m_pEvents->Exists ( szName ) )
    {
        // Call the event
        pElement->CallEvent ( szName, Arguments );
        bWasCanceled = m_pEvents->WasEventCancelled ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::TriggerClientEvent ( CElement* pElement, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments )
{
    assert ( pElement );
    assert ( szName );    
    assert ( pCallWithElement );
    RUN_CHILDREN TriggerClientEvent ( *iter, szName, pCallWithElement, Arguments );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        CLuaEventPacket Packet ( szName, pCallWithElement->GetID (), Arguments );
        pPlayer->Send ( Packet );
    }

    return true;
}      


bool CStaticFunctionDefinitions::CancelEvent ( bool bCancel, const char* szReason )
{
	if ( szReason )
	{
		m_pEvents->CancelEvent ( bCancel, (char*)szReason );
	}
	else
		m_pEvents->CancelEvent ( bCancel );

    return true;
}


bool CStaticFunctionDefinitions::GetCancelReason ( char* szReason )
{
	szReason = (char*)m_pEvents->GetLastError();
	if ( szReason )
	{
		return true;
	}
    return false;
}


bool CStaticFunctionDefinitions::WasEventCancelled ( void )
{
    return m_pEvents->WasEventCancelled ();
}


CDummy* CStaticFunctionDefinitions::CreateElement ( CResource* pResource, const char* szTypeName, const char* szID )
{
    assert ( szTypeName );
    assert ( szID );

    // Long enough typename and not an internal one?
    if ( strlen ( szTypeName ) > 0 && CElement::GetTypeID ( szTypeName ) == CElement::UNKNOWN )
    {
        // Create the element.
		CDummy* pDummy = new CDummy ( g_pGame->GetGroups (), pResource->GetDynamicElementRoot() );

		// Set the ID
		pDummy->SetName ( szID );

		// Set the type name
        pDummy->SetTypeName ( szTypeName );

		if ( pResource->HasStarted() )
		{
			CEntityAddPacket Packet;
			Packet.Add ( pDummy );
			m_pPlayerManager->BroadcastOnlyJoined ( Packet );
		}

        return pDummy;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::DestroyElement ( CElement* pElement )
{
    // Run us on all its children
    list < CElement* > ::const_iterator iter = pElement->IterBegin ();
    while ( iter != pElement->IterEnd () )
    {
        if ( DestroyElement ( *iter ) )
            iter = pElement->IterBegin ();
        else
            ++iter;
    }

    if ( pElement->IsBeingDeleted () )
        return false;

    // We can't destroy the root or a player/remote client/console
    int iType = pElement->GetType ();
    if ( pElement == m_pMapManager->GetRootElement () || 
         iType == CElement::PLAYER ||
         iType == CElement::CONSOLE ||
         g_pGame->GetResourceManager()->IsAResourceElement ( pElement ) )
    {
        return false;
    }

    // Tell everyone to destroy it if this is not a per-player entity
    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        // Unsync it (will destroy it for those that know about it)
        CPerPlayerEntity* pEntity = static_cast < CPerPlayerEntity* > ( pElement );
        pEntity->Sync ( false );
    }

    // Tell everyone to destroy it
    CEntityRemovePacket Packet;
    Packet.Add ( pElement );
    m_pPlayerManager->BroadcastOnlyJoined ( Packet );

    // Delete it
    m_pElementDeleter->Delete ( pElement );
    return true;
}


CElement* CStaticFunctionDefinitions::CloneElement ( CResource* pResource, CElement* pElement, const CVector& vecPosition, bool bCloneChildren )
{
    // TODO: Element types should be able to copy themselves... Some virtual copy constructor
    //       or something so it can be done at CElement level so we can be type independant.
    //       This code is ugly and does not support every option/element atm and won't in the future.

    // TODO: per-player entity stuff ( visibility )
    assert ( pElement );

    // Supposed to clone its children aswell?
    if ( bCloneChildren )
    {
        // Copy the current children list (prevents a continuous loop)
        list < CElement* > currentChildren = pElement->GetChildrenList ();

        // Loop through the children list doing this (cloning elements)
        list < CElement * > ::iterator iter = currentChildren.begin ();
        for ( ; iter != currentChildren.end (); iter++ )
        {
            CloneElement ( pResource, *iter, vecPosition, true );
        }
    }

    CElement* pParent = pElement->GetParentEntity ();
    if ( pParent )
    {
        int iParentType = pParent->GetType ();
        switch ( iParentType )
        {
            case CElement::DUMMY:
            case CElement::VEHICLE:
            case CElement::OBJECT:
            case CElement::MARKER:
            case CElement::BLIP:
            case CElement::PICKUP:
            case CElement::RADAR_AREA:
            case CElement::PATH_NODE:
                break;
            default:
                return NULL;
        }
    }

    int iType = pElement->GetType ();
    CVector vecClonedPosition = pElement->GetPosition ();
    CElement* pNewElement = NULL;
    bool bAddEntity = true;
    switch ( iType )
    {
        case CElement::DUMMY:
        {
            // Can't clone the root
            if ( pElement != m_pMapManager->GetRootElement () )
            {
                CDummy* pDummy = static_cast < CDummy* > ( pElement );
                
                pNewElement = new CDummy ( g_pGame->GetGroups (), pElement->GetParentEntity () );
                pNewElement->SetName ( pDummy->GetName () );
                pNewElement->SetTypeName ( pDummy->GetTypeName () );
            }
            break;
        }

        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            
            CVehicle* pTemp = m_pVehicleManager->Create ( pVehicle->GetModel (), pElement->GetParentEntity () );
            if ( pTemp )
            {
                CVector vecRotationDegrees;
                pVehicle->GetRotationDegrees ( vecRotationDegrees );
                pTemp->SetRotationDegrees ( vecRotationDegrees );

                pTemp->SetHealth ( pVehicle->GetHealth () );
                pTemp->SetColor ( pVehicle->GetColor () );
                pTemp->SetUpgrades ( pVehicle->GetUpgrades () );
                memcpy ( pTemp->m_ucDoorStates, pVehicle->m_ucDoorStates, MAX_DOORS );
                memcpy ( pTemp->m_ucWheelStates, pVehicle->m_ucWheelStates, MAX_WHEELS );
                memcpy ( pTemp->m_ucPanelStates, pVehicle->m_ucPanelStates, MAX_PANELS );
                memcpy ( pTemp->m_ucLightStates, pVehicle->m_ucLightStates, MAX_LIGHTS );

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );

            CObject* pTemp = m_pObjectManager->Create ( pElement->GetParentEntity () );
            if ( pTemp )
            {
                pTemp->SetRotation ( pObject->GetRotation () );
                pTemp->SetModel ( pObject->GetModel () );
                pTemp->SetParentObject ( pObject->GetParentEntity () );

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::MARKER:
        {
            CMarker* pMarker = static_cast < CMarker* > ( pElement );

            CMarker* pTemp = m_pMarkerManager->Create ( pElement->GetParentEntity () );
            if ( pTemp )
            {
                pTemp->SetMarkerType ( pMarker->GetMarkerType () );
                pTemp->SetColor ( pMarker->GetColor () );
                pTemp->SetSize ( pMarker->GetSize () );

				if ( pResource->HasStarted() )
					pTemp->Sync ( true );
                bAddEntity = false;

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::BLIP:
        {
            CBlip* pBlip = static_cast < CBlip* > ( pElement );

            CBlip* pTemp = m_pBlipManager->Create ( pElement->GetParentEntity () );
            if ( pTemp )
            {
                pTemp->m_ucIcon = pBlip->m_ucIcon;
                pTemp->m_ucSize = pBlip->m_ucSize;
                pTemp->SetColor ( pBlip->m_ucColorRed, pBlip->m_ucColorGreen,
                                  pBlip->m_ucColorBlue, pBlip->m_ucColorAlpha );

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::PICKUP:
        {
            CPickup* pPickup = static_cast < CPickup* > ( pElement );
            
            CPickup* pTemp = m_pPickupManager->Create ( pElement->GetParentEntity () );
            if ( pTemp )
            {
                pTemp->SetPickupType ( pPickup->GetPickupType () );
                pTemp->SetPickupType ( pPickup->GetPickupType () );
                pTemp->SetModel ( pPickup->GetModel () );
                pTemp->SetWeaponType ( pPickup->GetWeaponType() );
                pTemp->SetAmmo ( pPickup->GetAmmo() );
                pTemp->SetRespawnIntervals ( pPickup->GetRespawnIntervals() );

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::RADAR_AREA:
        {
            CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );

            CRadarArea* pTemp = m_pRadarAreaManager->Create ( pElement->GetParentEntity (), NULL );
            if ( pTemp )
            {
                pTemp->SetSize ( pRadarArea->GetSize () );
                pTemp->SetColor ( pRadarArea->GetColor () );

				if ( pResource->HasStarted() )
					pTemp->Sync ( true );
                bAddEntity = false;

                pNewElement = pTemp;
            }
            break;
        }
        case CElement::PED:
        {
            CPed * pPed = static_cast < CPed * > ( pElement );

            CPed * pTemp = m_pPedManager->Create ( pPed->GetModel (), pElement->GetParentEntity (), NULL );
            if ( pTemp )
            {
                pTemp->SetPosition ( pPed->GetPosition () );
                pTemp->SetRotation ( pPed->GetRotation () );
                pTemp->SetHealth ( pPed->GetHealth () );
                pTemp->SetArmor ( pPed->GetArmor () );
                pTemp->SetSyncable ( pTemp->IsSyncable () );

                pNewElement = pTemp;
            }
            break;
        }
    }

    if ( pNewElement )
    {
        CVector vecNewPosition = vecPosition;
        // If we're cloning children, use the given position as an offset
        if ( bCloneChildren )
            vecNewPosition += vecClonedPosition;

        pNewElement->SetPosition ( vecNewPosition );
        pNewElement->GetCustomDataPointer ()->Copy ( pElement->GetCustomDataPointer () );
        pNewElement->SetInterior ( pElement->GetInterior () );
        pNewElement->SetDimension ( pElement->GetDimension () );

        if ( bAddEntity )
        {
			if ( pResource->HasStarted() )
			{
				CEntityAddPacket Packet;
				Packet.Add ( pNewElement );
				m_pPlayerManager->BroadcastOnlyJoined ( Packet );
			}
        }
    }

    return pNewElement;
}


CElement* CStaticFunctionDefinitions::GetElementChild ( CElement* pElement, unsigned int uiIndex )
{
    assert ( pElement );

    // Grab it
    unsigned int uiCurrent = 0;
    list < CElement* > ::const_iterator iter = pElement->IterBegin ();
    for ( ; iter != pElement->IterEnd (); iter++ )
    {
        if ( uiIndex == uiCurrent++ )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


bool CStaticFunctionDefinitions::GetElementChildrenCount ( CElement* pElement, unsigned int& uiCount )
{
    assert ( pElement );

    uiCount = pElement->CountChildren ();
    return true;
}


CElement* CStaticFunctionDefinitions::GetElementByID ( const char* szID, unsigned int uiIndex )
{
    assert ( szID );

    // Search it from root and down
    return m_pMapManager->GetRootElement ()->FindChild ( szID, uiIndex, true );
}


CElement* CStaticFunctionDefinitions::GetElementByIndex ( const char* szType, unsigned int uiIndex )
{
    assert ( szType );

    // Search it from root and down
    return m_pMapManager->GetRootElement ()->FindChildByType ( szType, uiIndex, true );
}


CLuaArgument* CStaticFunctionDefinitions::GetElementData ( CElement* pElement, const char* szName, bool bInherit )
{
    assert ( pElement );
    assert ( szName );

    // Return its custom data
    return pElement->GetCustomData ( szName, bInherit );
}

CLuaArguments* CStaticFunctionDefinitions::GetAllElementData ( CElement* pElement, CLuaArguments * table )
{
    assert ( pElement );
    assert ( table );

    return pElement->GetAllCustomData ( table );
}


CElement* CStaticFunctionDefinitions::GetElementParent ( CElement* pElement )
{
    return pElement->GetParentEntity ();
}


bool CStaticFunctionDefinitions::GetElementInterior ( CElement* pElement, unsigned char& ucInterior )
{
    assert ( pElement );

    ucInterior = pElement->GetInterior ();
    return true;
}


bool CStaticFunctionDefinitions::IsElementWithinColShape ( CElement* pElement, CColShape* pColShape, bool& bWithin )
{
    assert ( pElement );
    assert ( pColShape );

    bWithin = pElement->CollisionExists ( pColShape );
    return true;
}

bool CStaticFunctionDefinitions::IsElementWithinMarker ( CElement* pElement, CMarker* pMarker, bool& bWithin )
{
    assert ( pElement );
    assert ( pMarker );

    bWithin = pElement->CollisionExists ( pMarker->GetColShape() );
    return true;
}




bool CStaticFunctionDefinitions::GetElementDimension ( CElement* pElement, unsigned short& usDimension )
{
    assert ( pElement );

    usDimension = pElement->GetDimension ();
    return true;
}


bool CStaticFunctionDefinitions::GetElementZoneName ( CElement* pElement, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly )
{
    assert ( pElement );
    assert ( szBuffer );
    assert ( uiBufferLength );

    CVector vecPosition = pElement->GetPosition ();

    const char* szZone = NULL;
    if ( bCitiesOnly )
        szZone = g_pGame->GetZoneNames ()->GetCityName ( vecPosition );
    else
        szZone = g_pGame->GetZoneNames ()->GetZoneName ( vecPosition );

    strncpy ( szBuffer, szZone, uiBufferLength );
    szBuffer [ uiBufferLength - 1 ] = 0;
    return true;
}


bool CStaticFunctionDefinitions::IsElementAttached ( CElement* pElement )
{
    assert ( pElement );

    CElement* pElementAttachedTo = pElement->GetAttachedToElement();

    if ( !pElementAttachedTo ) return false;

    assert ( pElementAttachedTo );

    if ( pElementAttachedTo->IsElementAttached ( pElement ) ) return true;

    return false;
}


CElement* CStaticFunctionDefinitions::GetElementAttachedTo ( CElement* pElement )
{
    assert ( pElement );

    CElement* pElementAttachedTo = pElement->GetAttachedToElement();

    if ( !pElementAttachedTo ) return NULL;

    assert ( pElementAttachedTo );

    if ( pElementAttachedTo->IsElementAttached ( pElement ) ) return pElementAttachedTo;

    return NULL;
}


CColShape* CStaticFunctionDefinitions::GetElementColShape ( CElement* pElement )
{
    assert ( pElement );

    CColShape* pColShape = NULL;
    switch ( pElement->GetType () )
    {
        case CElement::MARKER:
            pColShape = static_cast < CMarker* > ( pElement )->GetColShape ();
            break;
        case CElement::PICKUP:
            pColShape = static_cast < CPickup* > ( pElement )->GetColShape ();
            break;
    }
    return pColShape;
}


bool CStaticFunctionDefinitions::GetElementAlpha ( CElement* pElement, unsigned char& ucAlpha )
{
    assert ( pElement );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            ucAlpha = pPed->GetAlpha ();
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            ucAlpha = pVehicle->GetAlpha ();
            break;
        }
        case CElement::OBJECT:
        {
            CObject * pObject = static_cast < CObject* > ( pElement );
            ucAlpha = pObject->GetAlpha ();
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetElementHealth ( CElement* pElement, float& fHealth )
{
    assert ( pElement );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            fHealth = pPed->GetHealth ();
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            fHealth = pVehicle->GetHealth ();
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetElementModel ( CElement* pElement, unsigned short & usModel )
{
    assert ( pElement );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            usModel = pPed->GetModel ();
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            usModel = pVehicle->GetModel ();
            break;
        }
        case CElement::OBJECT:
        {
            CObject * pObject = static_cast < CObject * > ( pElement );
            usModel = pObject->GetModel ();
            break;
        }
		case CElement::PICKUP:
		{
			CPickup* pPickup = static_cast < CPickup* > ( pElement );
			usModel = pPickup->GetModel ();
			break;
		}
        default: return false;
    }

    return true;
}


CElement* CStaticFunctionDefinitions::GetElementSyncer ( CElement* pElement )
{
    assert ( pElement );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            return static_cast < CElement* > ( pPed->GetSyncer() );
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            return static_cast < CElement* > ( pVehicle->GetSyncer() );
            break;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::IsElementInWater ( CElement* pElement, bool & bInWater )
{
    assert ( pElement );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            bInWater = pPed->IsInWater ();
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            bInWater = pVehicle->IsInWater ();
            break;
        }
        default: return false;
    }

    return true;
}
            

bool CStaticFunctionDefinitions::SetElementID ( CElement* pElement, const char* szID )
{
    assert ( pElement );
    assert ( szID );

    // Set the new ID
    pElement->SetName ( szID );

	// Tell the clients of the name change
	unsigned short usIDLength = static_cast < unsigned short > ( strlen ( szID ) );
	CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( usIDLength );
    BitStream.pBitStream->Write ( szID, usIDLength );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_NAME, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementData ( CElement* pElement, char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain, bool bSynchronize )
{
    assert ( pElement );
    assert ( szName );

    if ( strlen ( szName ) > MAX_CUSTOMDATA_NAME_LENGTH )
        szName [ MAX_CUSTOMDATA_NAME_LENGTH ] = 0;

    CLuaArgument * pCurrentVariable = pElement->GetCustomData ( szName, false );
    if ( !pCurrentVariable || *pCurrentVariable != Variable )
    {
        // Set its custom data
        pElement->SetCustomData ( szName, Variable, pLuaMain );

        if ( bSynchronize )
        {
            // Tell our clients to update their data
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pElement->GetID () );
            BitStream.pBitStream->Write ( usNameLength );
            BitStream.pBitStream->Write ( szName, usNameLength );
            Variable.WriteToBitStream ( *BitStream.pBitStream );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_DATA, *BitStream.pBitStream ) );
        }
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::RemoveElementData ( CElement* pElement, const char* szName )
{
    assert ( pElement );
    assert ( szName );

    // Make sure the name isn't too long
    char szShortName [MAX_CUSTOMDATA_NAME_LENGTH + 1];
    szShortName [MAX_CUSTOMDATA_NAME_LENGTH] = 0;
    strncpy ( szShortName, szName, MAX_CUSTOMDATA_NAME_LENGTH );

    // Set its custom data
    if ( pElement->DeleteCustomData ( szName, false ) )
    {
        // Tell our clients to update their data
        unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szShortName ) );
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pElement->GetID () );
        BitStream.pBitStream->Write ( usNameLength );
        BitStream.pBitStream->Write ( szShortName, usNameLength );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( 0 ) ); // Not recursive
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_ELEMENT_DATA, *BitStream.pBitStream ) );
        return true;
    }

    // Failed
    return false;
}


bool CStaticFunctionDefinitions::SetElementParent ( CElement* pElement, CElement* pParent )
{
    assert ( pElement );
    assert ( pParent );

    CElement * pTemp = pParent;
    CElement * pRoot = m_pMapManager->GetRootElement();
    bool bValidParent = false;
    while ( pTemp != pRoot )
    {
        const char * szTypeName = pTemp->GetTypeName ().c_str ();
        if ( szTypeName && strcmp(szTypeName, "map") == 0 )
        {
            bValidParent = true; // parents must be a map
            break;
        }

        pTemp = pTemp->GetParentEntity();
    }

    // Make sure the new parent isn't the element and isn't a child of the entity
    if ( bValidParent && pElement != pParent && !pElement->IsMyChild ( pParent, true ) )
    {
        // Set the new parent
        pElement->SetParentObject ( pParent );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pElement->GetID () );
        BitStream.pBitStream->Write ( pParent->GetID () );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_PARENT, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetElementPosition ( CElement* pElement, CVector& vecPosition )
{
    assert ( pElement );

    vecPosition = pElement->GetPosition ();
    return true;
}


bool CStaticFunctionDefinitions::GetElementVelocity ( CElement* pElement, CVector& vecVelocity )
{
    assert ( pElement );

    int iType = pElement->GetType ();
    switch ( iType )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            pPed->GetVelocity ( vecVelocity );
            
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            vecVelocity = pVehicle->GetVelocity ();
            
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementPosition ( CElement* pElement, const CVector& vecPosition )
{
    assert ( pElement );
    RUN_CHILDREN SetElementPosition ( *iter, vecPosition );

    // Update our position for that entity. 
    pElement->SetPosition ( vecPosition );

    if ( pElement->GetType () != CElement::COLSHAPE )
    {
        // Run colpoint checks
        m_pColManager->DoHitDetection ( pElement->GetLastPosition (), pElement->GetPosition (), 0.0f, pElement );  
    }

    // Construct the set position packet
    CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( vecPosition.fX );
    BitStream.pBitStream->Write ( vecPosition.fY );
    BitStream.pBitStream->Write ( vecPosition.fZ );
    BitStream.pBitStream->Write ( pElement->GenerateSyncTimeContext () );

    // Tell only the relavant clients about this elements new position
    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        m_pPlayerManager->Broadcast ( CLuaPacket ( SET_ELEMENT_POSITION, *BitStream.pBitStream ), static_cast < CPerPlayerEntity * > ( pElement )->GetPlayersList () );
    }
    // Tell all clients about its new position
    else
    {        
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_POSITION, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementVelocity ( CElement* pElement, const CVector& vecVelocity )
{
    assert ( pElement );
    RUN_CHILDREN SetElementVelocity ( *iter, vecVelocity );

    int iType = pElement->GetType ();
    switch ( iType )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            pPed->SetVelocity ( vecVelocity );
            
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetVelocity ( vecVelocity );
            
            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            //pObject->SetVelocity ( vecVelocity );
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( vecVelocity.fX );
    BitStream.pBitStream->Write ( vecVelocity.fY );
    BitStream.pBitStream->Write ( vecVelocity.fZ );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_VELOCITY, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementVisibleTo ( CElement* pElement, CElement* pReference, bool bVisible )
{
    RUN_CHILDREN SetElementVisibleTo ( *iter, pReference, bVisible );

    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        CPerPlayerEntity* pEntity = static_cast < CPerPlayerEntity* > ( pElement );
        if ( bVisible )
			pEntity->AddVisibleToReference ( pReference );
		else
			pEntity->RemoveVisibleToReference ( pReference );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementInterior ( CElement* pElement, unsigned char ucInterior, bool bSetPosition, CVector& vecPosition )
{
    assert ( pElement );
    RUN_CHILDREN SetElementInterior ( *iter, ucInterior, bSetPosition, vecPosition );

    if ( ucInterior != pElement->GetInterior () )
    {
        pElement->SetInterior ( ucInterior );

        // Tell everyone
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pElement->GetID () );
        BitStream.pBitStream->Write ( ucInterior );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bSetPosition ) ? 1 : 0 ) );
        if ( bSetPosition )
        {
            BitStream.pBitStream->Write ( vecPosition.fX );
            BitStream.pBitStream->Write ( vecPosition.fY );
            BitStream.pBitStream->Write ( vecPosition.fZ );
        }
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_INTERIOR, *BitStream.pBitStream ) );
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementDimension ( CElement* pElement, unsigned short usDimension )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {        
        // Client side elements
        case CElement::TEAM:
        {
            CTeam* pTeam = static_cast < CTeam* > ( pElement );
            list < CPlayer* > ::const_iterator iter = pTeam->PlayersBegin ();
            for ( ; iter != pTeam->PlayersEnd () ; iter++ )
            {
                if ( (*iter)->IsSpawned () )
                {
                    (*iter)->SetDimension ( usDimension );
                }
            }
            break;
        }
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( !pPed->IsSpawned () )
            {
                return false;
            }
        }
        case CElement::COLSHAPE:
        case CElement::DUMMY:        
        case CElement::VEHICLE:
        case CElement::OBJECT:
        case CElement::MARKER:
        case CElement::BLIP:
        case CElement::PICKUP:
        case CElement::RADAR_AREA:
        case CElement::WORLD_MESH:
        {
            pElement->SetDimension ( usDimension );

            CBitStream bitStream;
            bitStream.pBitStream->Write ( pElement->GetID () );
            bitStream.pBitStream->Write ( usDimension );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_DIMENSION, *bitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AttachElements ( CElement* pElement, CElement* pAttachedToElement, CVector& vecPosition, CVector& vecRotation )
{
    assert ( pElement );
    assert ( pAttachedToElement );
    RUN_CHILDREN AttachElements ( *iter, pAttachedToElement, vecPosition, vecRotation );

    // Are they the same element?
    if ( pElement != pAttachedToElement )
    {
        // Make sure they aren't already attached to eachother in reverse
        if ( pAttachedToElement->GetAttachedToElement () != pElement )
        {
            if ( pElement->IsAttachToable () && pAttachedToElement->IsAttachable () )
            {
                ConvertDegreesToRadians ( vecRotation );
                pElement->AttachTo ( pAttachedToElement );
                pElement->SetAttachedOffsets ( vecPosition, vecRotation );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( pElement->GetID () );
                BitStream.pBitStream->Write ( pAttachedToElement->GetID () );
                BitStream.pBitStream->Write ( vecPosition.fX );
                BitStream.pBitStream->Write ( vecPosition.fY );
                BitStream.pBitStream->Write ( vecPosition.fZ );
                BitStream.pBitStream->Write ( vecRotation.fX );
                BitStream.pBitStream->Write ( vecRotation.fY );
                BitStream.pBitStream->Write ( vecRotation.fZ );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( ATTACH_ELEMENTS, *BitStream.pBitStream ) );

                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::DetachElements ( CElement* pElement, CElement* pAttachedToElement )
{
    assert ( pElement );
    RUN_CHILDREN DetachElements ( *iter, pAttachedToElement );

    CElement* pActualAttachedToElement = pElement->GetAttachedToElement ();
    if ( pActualAttachedToElement )
    {
        if ( pAttachedToElement == NULL || pActualAttachedToElement == pAttachedToElement )
        {
            // Detach it. Also generate a new time context to prevent sync screwup from
            // old packes arriving.
			CVector vecPosition = pElement->GetPosition ();
			pElement->AttachTo ( NULL );
            pElement->GenerateSyncTimeContext ();

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pElement->GetID () );
            BitStream.pBitStream->Write ( pElement->GetSyncTimeContext () );
            BitStream.pBitStream->Write ( vecPosition.fX );
            BitStream.pBitStream->Write ( vecPosition.fY );
            BitStream.pBitStream->Write ( vecPosition.fZ );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( DETACH_ELEMENTS, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementAlpha ( CElement* pElement, unsigned char ucAlpha )
{
    assert ( pElement );
    RUN_CHILDREN SetElementAlpha ( *iter, ucAlpha );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            pPed->SetAlpha ( ucAlpha );
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetAlpha ( ucAlpha );
            break;
        }
        case CElement::OBJECT:
        {
            CObject * pObject = static_cast < CObject* > ( pElement );
            pObject->SetAlpha ( ucAlpha );
            break;
        }
        case CElement::MARKER:
        {
            CMarker * pMarker = static_cast < CMarker* > ( pElement );
            unsigned char R, G, B, A;
            pMarker->GetColor ( R, G, B, A );
            pMarker->SetColor ( R, G, B, ucAlpha );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( ucAlpha );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_ALPHA, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementHealth ( CElement* pElement, float fHealth )
{
    assert ( pElement );
    RUN_CHILDREN SetElementHealth ( *iter, fHealth );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->IsSpawned () )
            {
                // Limit their max health to what the stat says
                float fMaxHealth = pPed->GetMaxHealth ();
                if ( fHealth > fMaxHealth )
                    fHealth = fMaxHealth;

                // This makes sure the health is set to what will get reported
                unsigned char ucHealth = static_cast < unsigned char > ( fHealth * 1.25f );
                fHealth = static_cast < float > ( ucHealth ) / 1.25f;
                pPed->SetHealth ( fHealth );
                pPed->SetHealthChangeTime ( GetTickCount () );
            }
            else
                return false;
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetHealth ( fHealth );
            pVehicle->SetHealthChangeTime ( GetTickCount () );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( fHealth );
    BitStream.pBitStream->Write ( pElement->GenerateSyncTimeContext () );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_HEALTH, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementModel ( CElement* pElement, unsigned short usModel )
{
    assert ( pElement );
    RUN_CHILDREN SetElementModel ( *iter, usModel );
    
    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed * pPed = static_cast < CPed* > ( pElement );
            if ( pPed->GetModel () == usModel ) return false;
            if ( !CPlayerManager::IsValidPlayerModel ( usModel ) ) return false;
            pPed->SetModel ( usModel );
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            if ( pVehicle->GetModel () == usModel ) return false;
            if ( !CVehicleManager::IsValidModel ( usModel ) ) return false;
            pVehicle->SetModel ( usModel );

            // Check for any passengers above the max seat list
            unsigned char ucMaxPassengers = pVehicle->GetMaxPassengers ();
            CLogger::DebugPrintf ( "Max passengers = %u\n", ucMaxPassengers );
            unsigned char i = 0;
            for ( ; i < MAX_VEHICLE_SEATS; i++ )
            {
                // Got a player in this seat and is it bigger than the supported amount
                // of seats in this new vehicle
                CPed* pPed = pVehicle->GetOccupant ( i );
                if ( pPed && IS_PLAYER ( pPed ) && ( i > ucMaxPassengers ) )
                {
                    // Throw him out
                    // TODO: Maybe relocate him in the future. Find a free seat if available and put him in it.
                    RemovePedFromVehicle ( pPed );
                }
            }
            break;
        }
        case CElement::OBJECT:
        {
            CObject * pObject = static_cast < CObject* > ( pElement );
            if ( pObject->GetModel () == usModel ) return false;
            if ( !CObjectManager::IsValidModel ( usModel ) ) return false;
            pObject->SetModel ( usModel );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pElement->GetID () );
    BitStream.pBitStream->Write ( usModel );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ELEMENT_MODEL, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::ClearElementVisibleTo ( CElement* pElement )
{
    RUN_CHILDREN ClearElementVisibleTo ( *iter );

    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        CPerPlayerEntity* pEntity = static_cast < CPerPlayerEntity* > ( pElement );
        pEntity->ClearVisibleToReferences ();
    }

    return true;
}


bool CStaticFunctionDefinitions::GetClientName ( CElement* pElement, char* szNick )
{
    assert ( pElement );
    assert ( szNick );

    switch ( pElement->GetType () )
    {
        case CElement::PLAYER:
        {
            strncpy ( szNick, static_cast < CPlayer* > ( pElement )->GetNick (), 25 );
            break;
        }
        case CElement::CONSOLE:
        {
            strcpy ( szNick, "Console" );
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetClientIP ( CElement* pElement, char* szIP )
{
    assert ( pElement );
    assert ( szIP );

    switch ( pElement->GetType () )
    {
        case CElement::PLAYER:
        {
            static_cast < CPlayer* > ( pElement )->GetSourceIP ( szIP );
            break;
        }
        case CElement::CONSOLE:
        {
            strcpy ( szIP, "127.0.0.1" );
            break;
        }
        default: return false;
    }

    return true;
}


CAccount* CStaticFunctionDefinitions::GetClientAccount ( CElement* pElement )
{
    assert ( pElement );

    CClient* pClient = pElement->GetClient ();
    if ( pClient )
    {
        return pClient->GetAccount ();
    }
    return NULL;
}


bool CStaticFunctionDefinitions::SetClientName ( CElement* pElement, const char* szName )
{
    assert ( pElement );
    assert ( szName );

    CClient* pClient = pElement->GetClient ();
    if ( pClient )
    {
        if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );
            // Check its validity
            if ( IsNickValid ( szName ) )
            {
                // Verify the length
                size_t sizeNewNick = strlen ( szName );
                if ( sizeNewNick >= MIN_NICK_LENGTH && sizeNewNick <= MAX_NICK_LENGTH )
                {
                    // Does the nickname differ from the previous nickname?
                    const char* szNick = pPlayer->GetNick ();
                    if ( szNick == NULL || strcmp ( szName, szNick ) != 0 )
                    {
                        // Check that it doesn't already exist, or if it matches our current nick case-independantly (means we changed to the same nick but in a different case)
                        if ( ( szNick && stricmp ( szNick, szName ) == 0 ) || m_pPlayerManager->Get ( szName ) == NULL )
                        {
                            // Tell the console
                            CLogger::LogPrintf ( "NICK: %s is now known as %s\n", szNick, szName );

                            // Change the nick
                            pPlayer->SetNick ( szName );

                            // Tell all ingame players about the nick change
                            CPlayerChangeNickPacket Packet ( szName );
                            Packet.SetSourceElement ( pPlayer );
                            m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}


CPed* CStaticFunctionDefinitions::CreatePed ( CResource* pResource, unsigned short usModel, const CVector& vecPosition, float fRotation, bool bSynced )
{
    if ( CPlayerManager::IsValidPlayerModel ( usModel ) )
    {
        CPed * pPed = m_pPedManager->Create ( usModel, pResource->GetDynamicElementRoot () );
        if ( pPed )
        {
            pPed->SetPosition ( vecPosition );
            pPed->SetIsDead ( false );
            pPed->SetSpawned ( true );
            pPed->SetHealth ( 100.0f );
            pPed->SetSyncable ( bSynced );

			pPed->SetRotation ( fRotation );

            CEntityAddPacket Packet;
            Packet.Add ( pPed );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
            return pPed;
        }
    }

    return NULL;
}


unsigned int CStaticFunctionDefinitions::GetPlayerCount ( void )
{
    return m_pPlayerManager->CountJoined ();
}


bool CStaticFunctionDefinitions::GetPlayerAmmoInClip ( CPlayer* pPlayer, unsigned short& usAmmo )
{
    assert ( pPlayer );

    usAmmo = pPlayer->GetWeaponAmmoInClip ( pPlayer->GetWeaponSlot () );
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerTotalAmmo ( CPlayer* pPlayer, unsigned short& usAmmo )
{
    assert ( pPlayer );

    usAmmo = pPlayer->GetWeaponTotalAmmo ( pPlayer->GetWeaponSlot () );
    return true;
}


bool CStaticFunctionDefinitions::SetPlayerAmmo ( CElement* pElement, unsigned char ucSlot, unsigned short usAmmo )
{
    assert ( pElement );
    CPlayer* pPlayer = NULL;
    if ( IS_PLAYER ( pElement ) )
		pPlayer = static_cast < CPlayer* > ( pElement );
    if ( !pPlayer )
        return false;
    unsigned char ucWeaponID = pPlayer->GetWeapon ( ucSlot )->ucType;
    
	RUN_CHILDREN SetWeaponAmmo ( *iter, ucWeaponID, usAmmo );

    if ( pPlayer->IsSpawned () )
    {
        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucWeaponID );
        BitStream.pBitStream->Write ( usAmmo );
        pPlayer->Send ( CLuaPacket ( SET_WEAPON_AMMO, *BitStream.pBitStream ) );

        return true;
    }

	return false;
}


bool CStaticFunctionDefinitions::GetPlayerWeapon ( CPlayer* pPlayer, unsigned char& ucWeapon )
{
    assert ( pPlayer );

    ucWeapon = pPlayer->GetWeaponType ( pPlayer->GetWeaponSlot () );
    return true;
}


CPlayer* CStaticFunctionDefinitions::GetPlayerFromNick ( const char* szNick )
{
    assert ( szNick );

    return m_pPlayerManager->Get ( szNick );
}


CVehicle* CStaticFunctionDefinitions::GetPlayerOccupiedVehicle ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    return pPlayer->GetOccupiedVehicle ();
}


bool CStaticFunctionDefinitions::GetPlayerOccupiedVehicleSeat ( CPlayer* pPlayer, unsigned int& uiSeat )
{
    assert ( pPlayer );

    if ( pPlayer->GetOccupiedVehicle () )
    {
        uiSeat = pPlayer->GetOccupiedVehicleSeat ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetPlayerPing ( CPlayer* pPlayer, unsigned int& uiPing )
{
    assert ( pPlayer );

    uiPing = pPlayer->GetPing ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerSourceIP ( CPlayer* pPlayer, char* szIP )
{
    assert ( pPlayer );
    assert ( szIP );

    pPlayer->GetSourceIP ( szIP );
    return true;
}


CPlayer* CStaticFunctionDefinitions::GetRandomPlayer ( void )
{
    unsigned int uiJoinedCount = m_pPlayerManager->CountJoined ();
    if ( uiJoinedCount > 0 )
    {
        // Grab a random number between 0 and the joined count
        unsigned int uiRandom = GetRandom ( 0, uiJoinedCount - 1 );

        // Grab the player with that index
        unsigned int i = 0;
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            // This is the given index?
            if ( i++ == uiRandom )
            {
                return *iter;
            }
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::IsPlayerMuted ( CPlayer* pPlayer, bool& bMuted )
{
    assert ( pPlayer );

    bMuted = pPlayer->IsMuted ();
    return true;
}


CTeam* CStaticFunctionDefinitions::GetPlayerTeam ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    return pPlayer->GetTeam ();
}


bool CStaticFunctionDefinitions::CanPlayerUseFunction ( CPlayer* pPlayer, char* szFunction, bool& bCanUse )
{
    assert ( pPlayer );
    assert ( szFunction );

    CConsoleCommand* pConsoleCommand = g_pGame->GetConsole ()->GetCommand ( szFunction );
    if ( pConsoleCommand )
    {
        bCanUse = g_pGame->GetACLManager()->CanObjectUseRight ( pPlayer->GetNick (), CAccessControlListGroupObject::OBJECT_TYPE_USER, szFunction, CAccessControlListRight::RIGHT_TYPE_FUNCTION, false );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::IsPlayerDebuggerVisible ( CPlayer* pPlayer, bool& bVisible )
{
    assert ( pPlayer );

    bVisible = pPlayer->IsDebuggerVisible ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerWantedLevel ( CPlayer* pPlayer, unsigned int& uiWantedLevel )
{
    assert ( pPlayer );

    uiWantedLevel = pPlayer->GetWantedLevel();
    return true;
}


bool CStaticFunctionDefinitions::IsPlayerScoreboardForced ( CPlayer* pPlayer, bool& bForced )
{
    assert ( pPlayer );

    bForced = pPlayer->GetForcedScoreboard ();
    return true;
}


bool CStaticFunctionDefinitions::IsPlayerMapForced ( CPlayer* pPlayer, bool& bForced )
{
    assert ( pPlayer );

    bForced = pPlayer->GetForcedMap ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerNametagText ( CPlayer* pPlayer, char* szBuffer, unsigned int uiBufferLength )
{
    assert ( pPlayer );
    assert ( szBuffer );
    assert ( uiBufferLength );

    const char* szNametagText = pPlayer->GetNametagText ();
    if ( szNametagText == NULL )
        szNametagText = pPlayer->GetNick ();

    if ( szNametagText )
    {
        strncpy ( szBuffer, szNametagText, uiBufferLength );
        szBuffer [ uiBufferLength - 1 ] = 0;
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetPlayerNametagColor ( CPlayer* pPlayer, unsigned char& ucR, unsigned char& ucG, unsigned char& ucB )
{
    assert ( pPlayer );

    pPlayer->GetNametagColor ( ucR, ucG, ucB );
    return true;
}


bool CStaticFunctionDefinitions::IsPlayerNametagShowing ( CPlayer* pPlayer, bool& bShowing )
{
    assert ( pPlayer );

    bShowing = pPlayer->IsNametagShowing ();
    return true;
}


const std::string& CStaticFunctionDefinitions::GetPlayerSerial ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    return pPlayer->GetSerial ();
}


const std::string& CStaticFunctionDefinitions::GetPlayerUserName ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    return pPlayer->GetSerialUser ();
}


const std::string& CStaticFunctionDefinitions::GetPlayerCommunityID ( CPlayer* pPlayer )
{
    assert ( pPlayer );

	return pPlayer->GetCommunityID ();
}


bool CStaticFunctionDefinitions::GetPlayerBlurLevel ( CPlayer* pPlayer, unsigned char& ucLevel )
{
    assert ( pPlayer );

    ucLevel = pPlayer->GetBlurLevel ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerMoney ( CPlayer* pPlayer, long& lMoney )
{
    assert ( pPlayer );

    lMoney = pPlayer->GetMoney ();
    return true;
}


// ***************** PLAYER SET FUNCS **************** //
bool CStaticFunctionDefinitions::SetPlayerMoney ( CElement* pElement, long lMoney )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerMoney ( *iter, lMoney );

    // Exists?
    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Is it above 99999999? Limit it to it
        if ( lMoney > 99999999 ) lMoney = 99999999;
        // Is it below -99999999?
        else if ( lMoney < -99999999 ) lMoney = -99999999;

        // Tell him his new money
        CBitStream BitStream;
        BitStream.pBitStream->Write ( lMoney );
        pPlayer->Send ( CLuaPacket ( SET_PLAYER_MONEY, *BitStream.pBitStream ) );

        // Set the money and return true
        pPlayer->SetMoney ( lMoney );
    }

    return true;
}


bool CStaticFunctionDefinitions::GivePlayerMoney ( CElement* pElement, long lMoney )
{
    assert ( pElement );
    RUN_CHILDREN GivePlayerMoney ( *iter, lMoney );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Is it above 99999999? Limit it to it
        if ( lMoney > 99999999 ) lMoney = 99999999;
        // Is it below -99999999?
        else if ( lMoney < -99999999 ) lMoney = -99999999;

        // Calculate his new money, if it exceeds 8 digits, set it to 99999999
        long lNewMoney = pPlayer->GetMoney () + lMoney;
        if ( lNewMoney > 99999999 ) lNewMoney = 99999999;
        else if ( lNewMoney < -99999999 ) lNewMoney = -99999999;

        // Tell him his new money
        CBitStream BitStream;
        BitStream.pBitStream->Write ( lNewMoney );
        pPlayer->Send ( CLuaPacket ( SET_PLAYER_MONEY, *BitStream.pBitStream ) );

        // Set the money and return true
        pPlayer->SetMoney ( lNewMoney );
    }

    return true;
}


bool CStaticFunctionDefinitions::TakePlayerMoney ( CElement* pElement, long lMoney )
{
    assert ( pElement );
    RUN_CHILDREN TakePlayerMoney ( *iter, lMoney );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Take his new money.
        long lNewMoney = pPlayer->GetMoney () - lMoney;

        // Tell him his new money
        CBitStream BitStream;
        BitStream.pBitStream->Write ( lNewMoney );
        pPlayer->Send ( CLuaPacket ( SET_PLAYER_MONEY, *BitStream.pBitStream ) );

        // Set the money and return true
        pPlayer->SetMoney ( lNewMoney );
    }

    return true;
}


bool CStaticFunctionDefinitions::SpawnPlayer ( CPlayer* pPlayer, const CVector& vecPosition, float fRotation, unsigned long ulSkin, unsigned char ucInterior, unsigned short usDimension, CTeam* pTeam )
{
    // Valid model?
    unsigned short usSkin = static_cast < unsigned short > ( ulSkin );
    if ( CPlayerManager::IsValidPlayerModel ( usSkin ) )
    {
        // Spawn him
        m_pMapManager->SpawnPlayer ( *pPlayer, vecPosition, ConvertDegreesToRadians ( fRotation ), usSkin, ucInterior, usDimension, pTeam );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::ShowPlayerHudComponent ( CElement* pElement, unsigned char ucComponent, bool bShow )
{
    assert ( pElement );

    RUN_CHILDREN ShowPlayerHudComponent ( *iter, ucComponent, bShow );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        
        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucComponent );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShow ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( SHOW_PLAYER_HUD_COMPONENT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPlayerDebuggerVisible ( CElement* pElement, bool bVisible )
{
    // * Not used by scripts

    assert ( pElement );
    RUN_CHILDREN SetPlayerDebuggerVisible ( *iter, bVisible );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bVisible ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( TOGGLE_DEBUGGER, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPlayerWantedLevel ( CElement* pElement, unsigned int iLevel )
{
    assert ( pElement );

    // Make sure the health is above 0
    if ( iLevel >= 0 && iLevel <= 6 )
    {
        RUN_CHILDREN SetPlayerWantedLevel ( *iter, iLevel );

        if ( IS_PLAYER ( pElement ) )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
            pPlayer->SetWantedLevel ( iLevel );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( (unsigned char)iLevel );
            pPlayer->Send ( CLuaPacket ( SET_WANTED_LEVEL, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::ForcePlayerMap ( CElement* pElement, bool bVisible )
{
    assert ( pElement );
    RUN_CHILDREN ForcePlayerMap ( *iter, bVisible );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        if ( pPlayer->GetForcedMap () != bVisible )
        {
            pPlayer->SetForcedMap ( bVisible );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bVisible ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( FORCE_PLAYER_MAP, *BitStream.pBitStream ) );

            return true;
        }
    }
    
    return false;
}


bool CStaticFunctionDefinitions::SetPlayerNametagText ( CElement* pElement, const char* szText )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerNametagText ( *iter, szText );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        char* szNametagText = pPlayer->GetNametagText ();
        if ( ( szText && ( szNametagText == NULL || strcmp ( szNametagText, szText ) ) ) ||
             ( szText == NULL && szNametagText ) )
        {
            if ( szText == NULL || IsNickValid ( szText ) )
            {
                pPlayer->SetNametagText ( szText );

                unsigned short usTextLength = 0;
                if ( szText )
                    usTextLength = static_cast < unsigned short > ( strlen ( szText ) );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPlayer->GetID () );
                BitStream.pBitStream->Write ( usTextLength );
                if ( usTextLength > 0 )
                    BitStream.pBitStream->Write ( const_cast < char* > ( szText ), usTextLength );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PLAYER_NAMETAG_TEXT, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPlayerNametagColor ( CElement* pElement, bool bRemoveOverride, unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerNametagColor ( *iter, bRemoveOverride, ucR, ucG, ucB );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Are we removing the override?
        if ( bRemoveOverride )
        {
            // Was it even overridden?
            if ( pPlayer->IsNametagColorOverridden () )
            {
                // Send a packet
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPlayer->GetID () );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_PLAYER_NAMETAG_COLOR, *BitStream.pBitStream ) );

                return true;
            }
        }
        else
        {
            // Grab the current nametag color
            unsigned char ucCurrentR, ucCurrentG, ucCurrentB;
            pPlayer->GetNametagColor ( ucCurrentR, ucCurrentG, ucCurrentB );
        
            // We don't have an override OR it's different than current color?
            if ( pPlayer->IsNametagColorOverridden () ||
                 ucR != ucCurrentR || ucG != ucCurrentG || ucB != ucCurrentB )
            {
                // Set the new override color
                pPlayer->SetNametagOverrideColor ( ucR, ucG, ucB );

                // Tell the players
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPlayer->GetID () );
                BitStream.pBitStream->Write ( ucR );
                BitStream.pBitStream->Write ( ucG );
                BitStream.pBitStream->Write ( ucB );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PLAYER_NAMETAG_COLOR, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPlayerNametagShowing ( CElement* pElement, bool bShowing )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerNametagShowing ( *iter, bShowing );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        if ( pPlayer->IsNametagShowing () != bShowing )
        {
            pPlayer->SetNametagShowing ( bShowing );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPlayer->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShowing ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PLAYER_NAMETAG_SHOWING, *BitStream.pBitStream ) );
            
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPlayerMuted ( CElement* pElement, bool bMuted )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerMuted ( *iter, bMuted );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
		pPlayer->SetMuted ( bMuted );
		return true;
	}
	return false;
}


bool CStaticFunctionDefinitions::SetPlayerBlurLevel ( CElement* pElement, unsigned char ucLevel )
{
    assert ( pElement );
    RUN_CHILDREN SetPlayerBlurLevel ( *iter, ucLevel );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
		pPlayer->SetBlurLevel ( ucLevel );

        CBitStream bitStream;
        bitStream.pBitStream->Write ( ucLevel );
        pPlayer->Send ( CLuaPacket ( SET_BLUR_LEVEL, *bitStream.pBitStream ) );

		return true;
	}
	return false;
}


bool CStaticFunctionDefinitions::RedirectPlayer ( CElement* pElement, const char* szHost, unsigned short usPort, const char* szPassword )
{
    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        unsigned char ucHostLength = static_cast < unsigned char > ( strlen ( szHost ) );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucHostLength );
        BitStream.pBitStream->Write ( const_cast < char* > ( szHost ), ucHostLength );
        BitStream.pBitStream->Write ( usPort );
        if ( szPassword )
        {
            unsigned char ucPasswordLength = static_cast < unsigned short > ( strlen ( szPassword ) );
            BitStream.pBitStream->Write ( ucPasswordLength );
            BitStream.pBitStream->Write ( const_cast < char* > ( szPassword ), ucPasswordLength );
        }
        pPlayer->Send ( CLuaPacket ( FORCE_RECONNECT, *BitStream.pBitStream ) );

		return true;
	}
	return false;
}


// ***************** PED GET FUNCS ***************** //
bool CStaticFunctionDefinitions::GetPedArmor ( CPed* pPed, float& fArmor )
{
    assert ( pPed );

    fArmor = pPed->GetArmor ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedRotation ( CPed* pPed, float& fRotation )
{
    assert ( pPed );

    fRotation = ConvertRadiansToDegrees ( pPed->GetRotation () );
    return true;
}


bool CStaticFunctionDefinitions::IsPedDead ( CPed* pPed, bool& bDead )
{
    assert ( pPed );

    bDead = pPed->IsDead ();
    return true;
}


bool CStaticFunctionDefinitions::IsPedDucked ( CPed* pPed, bool& bDucked )
{
    assert ( pPed );

    bDucked = pPed->IsDucked ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedStat ( CPed* pPed, unsigned short usStat, float& fValue )
{
	assert ( pPed );

    // Check the stat
    if ( usStat < NUM_PLAYER_STATS )
    {
		fValue = pPed->GetPlayerStat ( usStat );

		return true;
    }

    return false;
}


CElement* CStaticFunctionDefinitions::GetPedTarget ( CPed* pPed )
{
	assert ( pPed );

	return pPed->GetTargetedElement ();
}


bool CStaticFunctionDefinitions::GetPedClothes ( CPed* pPed, unsigned char ucType, char* szTextureReturn, char* szModelReturn )
{
    assert ( pPed );

    SPlayerClothing* pClothing = pPed->GetClothes ()->GetClothing ( ucType );
    if ( pClothing )
    {
        if ( szTextureReturn )
            strcpy ( szTextureReturn, pClothing->szTexture );
        if ( szModelReturn )
            strcpy ( szModelReturn, pClothing->szModel );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::DoesPedHaveJetPack ( CPed* pPed, bool& bHasJetPack )
{
    assert ( pPed );

    bHasJetPack = pPed->HasJetPack ();
    return true;
}


bool CStaticFunctionDefinitions::IsPedOnGround ( CPed* pPed, bool& bOnGround )
{
    assert ( pPed );

    bOnGround = pPed->IsOnGround ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedFightingStyle ( CPed* pPed, unsigned char& ucStyle )
{
    assert ( pPed );

    ucStyle = pPed->GetFightingStyle ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedMoveAnim ( CPed* pPed, unsigned int& iMoveAnim )
{
    assert ( pPed );

    iMoveAnim = pPed->GetMoveAnim ();
    return true;
}


bool CStaticFunctionDefinitions::GetPedGravity ( CPed* pPed, float& fGravity )
{
    assert ( pPed );

    fGravity = pPed->GetGravity ();
    return true;
}


CElement* CStaticFunctionDefinitions::GetPedContactElement ( CPed* pPed )
{
    assert ( pPed );

    return pPed->GetContactElement ();
}


bool CStaticFunctionDefinitions::GetPedWeaponSlot ( CPed* pPed, unsigned char& ucWeaponSlot )
{
    assert ( pPed );

    ucWeaponSlot = pPed->GetWeaponSlot ();
    return true;
}


bool CStaticFunctionDefinitions::IsPedDoingGangDriveby ( CPed * pPed, bool & bDoingGangDriveby )
{
    assert ( pPed );

    bDoingGangDriveby = pPed->IsDoingGangDriveby ();
    return true;
}

bool CStaticFunctionDefinitions::IsPedOnFire ( CPed * pPed, bool & bIsOnFire )
{
    assert ( pPed );

    bIsOnFire = pPed->IsOnFire ();
    return true;
}

bool CStaticFunctionDefinitions::IsPedHeadless ( CPed * pPed, bool & bIsHeadless )
{
    assert ( pPed );

    bIsHeadless = pPed->IsHeadless ();
    return true;
}

// ************** PED SET FUNCS ************** //
bool CStaticFunctionDefinitions::SetPedArmor ( CElement* pElement, float fArmor )
{
    assert ( pElement );

    // Make sure it's above 0
    if ( fArmor >= 0.0f )
    {
        RUN_CHILDREN SetPedArmor ( *iter, fArmor );

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->IsSpawned () )
            {
                // Limit it to 100.0
                if ( fArmor > 100.0f )
                    fArmor = 100.0f;

                pPed->SetArmor ( fArmor );
                pPed->SetArmorChangeTime ( GetTickCount () );

                unsigned char ucArmor = static_cast < unsigned char > ( fArmor * 1.25 );

                // Tell everyone
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( ucArmor );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_ARMOR, *BitStream.pBitStream ) );
            }
        }

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::KillPed ( CElement* pElement, CElement* pKiller, unsigned char ucKillerWeapon, unsigned char ucBodyPart, bool bStealth )
{
    assert ( pElement );
    RUN_CHILDREN KillPed ( *iter, pKiller, ucKillerWeapon, ucBodyPart );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );

        // Is the ped alive?
        if ( !pPed->IsDead () && pPed->IsSpawned () )
        {           
            // Remove him from any occupied vehicle
            pPed->SetVehicleAction ( CPed::VEHICLEACTION_NONE );
            CVehicle* pVehicle = pPed->GetOccupiedVehicle ();
            if ( pVehicle )
            {
                pVehicle->SetOccupant ( NULL, pPed->GetOccupiedVehicleSeat () );
                pPed->SetOccupiedVehicle ( NULL, 0 );
            }

            // Update the ped
            pPed->SetSpawned ( false ); 
            pPed->SetIsDead ( true );
            pPed->SetHealth ( 0.0f );
            pPed->SetArmor ( 0.0f );            

            ElementID KillerID = INVALID_ELEMENT_ID;
            if ( pKiller && IS_PED ( pKiller ) )
            {
                CPed* pPedKiller = static_cast < CPed* > ( pKiller );
                KillerID = pPedKiller->GetID ();
            }
            // Tell everyone to kill this player
            CPlayerWastedPacket WastedPacket ( pPed, pKiller, ucKillerWeapon, ucBodyPart, bStealth );
            m_pPlayerManager->BroadcastOnlyJoined ( WastedPacket );

            // Tell our scripts the ped has died
            CLuaArguments Arguments;
            Arguments.PushNumber ( pPed->GetWeaponAmmoInClip () );
            if ( pKiller ) Arguments.PushElement ( pKiller );
            else Arguments.PushBoolean ( false );
            if ( ucKillerWeapon != 0xFF ) Arguments.PushNumber ( ucKillerWeapon );
            else Arguments.PushBoolean ( false );
            if ( ucBodyPart != 0xFF ) Arguments.PushNumber ( ucBodyPart );
            else Arguments.PushBoolean ( false );
            Arguments.PushBoolean ( bStealth );
            // TODO: change to onPedWasted
            pPed->CallEvent ( "onPlayerWasted", Arguments );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedRotation ( CElement* pElement, float fRotation )
{
    assert ( pElement );
    RUN_CHILDREN SetPedRotation ( *iter, fRotation );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
            // Set his new rotation
            float fRadians = ConvertDegreesToRadians ( fRotation );
            pPed->SetRotation ( fRadians );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
            BitStream.pBitStream->Write ( fRadians );
            BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_ROTATION, *BitStream.pBitStream ) );
        }
    }

    return true;
}


bool CStaticFunctionDefinitions::SetPedStat ( CElement* pElement, unsigned short usStat, float fValue )
{
	assert ( pElement );

    // Check the stat
    if ( usStat < NUM_PLAYER_STATS && fValue >= 0.0f && fValue <= 1000.0f )
    {        
        RUN_CHILDREN SetPedStat ( *iter, usStat, fValue );

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );

            // Dont let them set visual stats if they dont have the CJ model
            if ( ( usStat != 21 /* FAT */ && usStat != 23 /* BODY_MUSCLE */ ) || pPed->GetModel () == 0 )
            {
			    // Save the stat
			    pPed->SetPlayerStat ( usStat, fValue );
    			
			    // Notify everyone
			    CPlayerStatsPacket Packet;
			    Packet.SetSourceElement ( pPed );
			    Packet.Add ( usStat, fValue );
			    m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                return true;
            }
        }        
    }

    return false;
}


bool CStaticFunctionDefinitions::AddPedClothes ( CElement* pElement, char* szTexture, char* szModel, unsigned char ucType )
{
    assert ( pElement );
    assert ( szTexture );
    assert ( szModel );

    if ( CPlayerClothes::IsValidClothing ( szTexture, szModel, ucType ) )
    {
        RUN_CHILDREN ( *iter, szTexture, szModel, ucType );

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );

            if ( pPed->GetModel () == 0 )
            {             
                pPed->GetClothes ()->AddClothes ( szTexture, szModel, ucType );

                CPlayerClothesPacket Packet;
                Packet.SetSourceElement ( pPed );
                Packet.Add ( szTexture, szModel, ucType );
                m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::RemovePedClothes ( CElement* pElement, unsigned char ucType, char* szTexture, char* szModel )
{
    assert ( pElement );

    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        RUN_CHILDREN RemovePedClothes ( *iter, ucType, szTexture, szModel );

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );

            SPlayerClothing* pClothing = pPed->GetClothes ()->GetClothing ( ucType );
            if ( pClothing )
            {
                if ( ( !szTexture || !stricmp ( pClothing->szTexture, szTexture ) ) &&
                     ( !szModel || !stricmp ( pClothing->szModel, szModel ) ) )
                {
                    pPed->GetClothes ()->RemoveClothes ( ucType );

                    CBitStream BitStream;
                    BitStream.pBitStream->Write ( pPed->GetID () );
                    BitStream.pBitStream->Write ( ucType );
                    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_PED_CLOTHES, *BitStream.pBitStream ) );

                    return true;
                }
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GivePedJetPack ( CElement* pElement )
{
    assert ( pElement );

    RUN_CHILDREN GivePedJetPack ( *iter );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () && !pPed->GetOccupiedVehicle () && !pPed->HasJetPack () )
        {
            pPed->SetHasJetPack ( true );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );            
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( GIVE_PED_JETPACK, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::RemovePedJetPack ( CElement* pElement )
{
    assert ( pElement );

    RUN_CHILDREN RemovePedJetPack ( *iter );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () && pPed->HasJetPack () )
        {
            pPed->SetHasJetPack ( false );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_PED_JETPACK, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedFightingStyle ( CElement* pElement, unsigned char ucStyle )
{
    assert ( pElement );
    RUN_CHILDREN SetPedFightingStyle ( *iter, ucStyle );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->GetFightingStyle () != ucStyle )
        {
            // valid style?
            if ( ucStyle >= 4 && ucStyle <= 16 )
            {
                pPed->SetFightingStyle ( ucStyle );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( ucStyle );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_FIGHTING_STYLE, *BitStream.pBitStream ) );
            
                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedMoveAnim ( CElement* pElement, unsigned int iMoveAnim )
{
    assert ( pElement );
    RUN_CHILDREN SetPedMoveAnim ( *iter, iMoveAnim );

    if ( IS_PED ( pElement ) )
    {
        if ( ( iMoveAnim >= 54 && iMoveAnim <= 70 ) ||
             ( iMoveAnim >= 118 && iMoveAnim <= 138 ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->GetMoveAnim () != iMoveAnim )
            {
                pPed->SetMoveAnim ( iMoveAnim );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( iMoveAnim );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_MOVE_ANIM, *BitStream.pBitStream ) );
            
                return true;
            }
        }
    }
    return false;
}


#define MAX_GRAVITY 1.0f
#define MIN_GRAVITY 0.0f

bool CStaticFunctionDefinitions::SetPedGravity ( CElement* pElement, float fGravity )
{
    assert ( pElement );
    RUN_CHILDREN SetPedGravity ( *iter, fGravity );

    if ( IS_PED ( pElement ) )
    {
        if ( fGravity >= MIN_GRAVITY && fGravity <= MAX_GRAVITY )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->GetGravity () != fGravity )
            {
                pPed->SetGravity ( fGravity );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( fGravity );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_GRAVITY, *BitStream.pBitStream ) );
            }

            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedChoking ( CElement* pElement, bool bChoking )
{
    assert ( pElement );
    RUN_CHILDREN SetPedChoking ( *iter, bChoking );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
            // On foot?
            if ( !pPed->GetOccupiedVehicle () && pPed->GetVehicleAction () == CPed::VEHICLEACTION_NONE )
            {
                // Not already (not) choking?
                if ( bChoking != pPed->IsChoking () )
                {
                    pPed->SetChoking ( bChoking );

                    CBitStream bitStream;
                    bitStream.pBitStream->Write ( pPed->GetID () );
                    bitStream.pBitStream->Write ( ( unsigned char ) ( ( bChoking ) ? 1 : 0 ) );
                    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_CHOKING, *bitStream.pBitStream ) );

                    return true;
                }
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedWeaponSlot ( CElement * pElement, unsigned char ucWeaponSlot )
{
    assert ( pElement );
    RUN_CHILDREN SetPedWeaponSlot ( *iter, ucWeaponSlot );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
            BitStream.pBitStream->Write ( ucWeaponSlot );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEAPON_SLOT, *BitStream.pBitStream ) );
            
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::WarpPedIntoVehicle ( CPed* pPed, CVehicle* pVehicle, unsigned int uiSeat )
{
    assert ( pPed );
    assert ( pVehicle );

    // Valid seat id for that vehicle?
    if ( uiSeat <= pVehicle->GetMaxPassengers () )
    {
        if ( !pPed->IsDead () )
        {
            if ( pVehicle->GetHealth () > 0.0f )
            {            
                // Toss the previous player out of it if neccessary
                CPed* pPreviousOccupant = pVehicle->GetOccupant ( uiSeat );
                if ( pPreviousOccupant )
                {
                    // Remove him from the vehicle
                    RemovePedFromVehicle ( pPreviousOccupant );
                }

                // Jax: ::RemovePedFromVehicle caused a short delay between removing and entering,
                // which creates a buggy effect if we're just warping into a different seat

                // Is he already in a vehicle? Remove him from it
                CVehicle* pPreviousVehicle = pPed->GetOccupiedVehicle ();
                if ( pPreviousVehicle )
                {
                    // Remove him from the vehicle
                    pPreviousVehicle->SetOccupant ( NULL, pPed->GetOccupiedVehicleSeat () );
                }                

                // Put him in the new vehicle
                pPed->SetOccupiedVehicle ( pVehicle, uiSeat );
                pPed->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                // Tell all the players
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( pVehicle->GetID () );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( uiSeat ) );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( WARP_PED_INTO_VEHICLE, *BitStream.pBitStream ) );

                // Call the player->vehicle event
                CLuaArguments PlayerVehicleArguments;
                PlayerVehicleArguments.PushElement ( pVehicle );        // vehicle
                PlayerVehicleArguments.PushNumber ( uiSeat );            // seat
                if ( pPreviousOccupant )                    // jacked
                    PlayerVehicleArguments.PushElement ( pPreviousOccupant );
                else
                    PlayerVehicleArguments.PushBoolean ( false );
                // Leave onPlayerVehicleEnter for backwards compatibility
                pPed->CallEvent ( "onPlayerVehicleEnter", PlayerVehicleArguments );

                // Call the vehicle->player event
                CLuaArguments VehiclePlayerArguments;
                VehiclePlayerArguments.PushElement ( pPed );            // player
                VehiclePlayerArguments.PushNumber ( uiSeat );           // seat
                if ( pPreviousOccupant )                                // jacked
                    VehiclePlayerArguments.PushElement ( pPreviousOccupant );
                else
                    VehiclePlayerArguments.PushBoolean ( false );
                pVehicle->CallEvent ( "onVehicleEnter", VehiclePlayerArguments );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::RemovePedFromVehicle ( CElement* pElement )
{
    assert ( pElement );
    RUN_CHILDREN RemovePedFromVehicle ( *iter );

    // Verify the player and the vehicle pointer
    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );

        // Grab his occupied vehicle
        CVehicle* pVehicle = pPed->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            // Remove him from the vehicle
            pVehicle->SetOccupant ( NULL, pPed->GetOccupiedVehicleSeat () );
            pPed->SetOccupiedVehicle ( NULL, 0 );
            pPed->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
            BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_PED_FROM_VEHICLE, *BitStream.pBitStream ) );
        }
    }

    return true;
}


bool CStaticFunctionDefinitions::SetPedDoingGangDriveby ( CElement * pElement, bool bGangDriveby )
{
    assert ( pElement );
    RUN_CHILDREN SetPedDoingGangDriveby ( *iter, bGangDriveby );

    if ( IS_PED ( pElement ) )
    {
        CPed * pPed = static_cast < CPed * > ( pElement );
        // Are they spawned and stationary in a vehicle?        
        if ( pPed->IsSpawned () && pPed->GetOccupiedVehicle () && pPed->GetVehicleAction () == CPed::VEHICLEACTION_NONE )
        {
            if ( pPed->IsDoingGangDriveby () != bGangDriveby )
            {
                pPed->SetDoingGangDriveby ( bGangDriveby );

                // Tell the players
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( ( unsigned char ) ( ( bGangDriveby ) ? 1 : 0 ) );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_DOING_GANG_DRIVEBY, *BitStream.pBitStream ) );        
            
                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedAnimation ( CElement * pElement, const char * szBlockName, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInteruptable )
{
    assert ( pElement );
    RUN_CHILDREN SetPedAnimation ( *iter, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInteruptable );

    if ( IS_PED ( pElement ) )
    {
        CPed * pPed = static_cast < CPed * > ( pElement );
        if ( pPed->IsSpawned () )
        {
            // TODO: save their animation?

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () ); 
            if ( szBlockName && szAnimName )
            {
                unsigned char ucBlockSize = ( unsigned char ) strlen ( szBlockName );
                unsigned char ucAnimSize = ( unsigned char ) strlen ( szAnimName );
                
                BitStream.pBitStream->Write ( ucBlockSize );
                BitStream.pBitStream->Write ( szBlockName, ucBlockSize );
                BitStream.pBitStream->Write ( ucAnimSize );
                BitStream.pBitStream->Write ( szAnimName, ucAnimSize );
                BitStream.pBitStream->Write ( iTime );
                BitStream.pBitStream->Write ( ( unsigned char ) ( ( bLoop ) ? 1 : 0 ) );
                BitStream.pBitStream->Write ( ( unsigned char ) ( ( bUpdatePosition ) ? 1 : 0 ) );
                BitStream.pBitStream->Write ( ( unsigned char ) ( ( bInteruptable ) ? 1 : 0 ) );
            }
            else
            {
                // Inform them to kill the current animation instead
                BitStream.pBitStream->Write ( ( unsigned char ) 0 );
            }
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_ANIMATION, *BitStream.pBitStream ) );        
        
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedOnFire ( CElement * pElement, bool bIsOnFire )
{
    assert ( pElement );
    RUN_CHILDREN SetPedOnFire ( *iter, bIsOnFire );

    if ( IS_PED ( pElement ) )
    {
        CBitStream BitStream;
        CPed * pPed = static_cast < CPed * > ( pElement );

        pPed->SetOnFire( bIsOnFire );

        BitStream.pBitStream->Write ( pPed->GetID () );
        BitStream.pBitStream->Write ( ( unsigned char ) ( ( bIsOnFire ) ? 1 : 0 ) );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_ON_FIRE, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedHeadless ( CElement * pElement, bool bIsHeadless )
{
    assert ( pElement );
    RUN_CHILDREN SetPedOnFire ( *iter, bIsHeadless );

    if ( IS_PED ( pElement ) )
    {
        CBitStream BitStream;
        CPed * pPed = static_cast < CPed * > ( pElement );

        pPed->SetHeadless ( bIsHeadless );

        BitStream.pBitStream->Write ( pPed->GetID () );
        BitStream.pBitStream->Write ( ( unsigned char ) ( ( bIsHeadless ) ? 1 : 0 ) );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PED_HEADLESS, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetCameraMode ( CPlayer* pPlayer, char* szBuffer, size_t sizeBuffer )
{
    assert ( pPlayer );

    CPlayerCamera::GetMode ( pPlayer->GetCamera ()->GetMode (), szBuffer, sizeBuffer );
    return true;
}


bool CStaticFunctionDefinitions::GetCameraMatrix ( CPlayer * pPlayer, CVector & vecPosition, CVector & vecLookAt )
{
    assert ( pPlayer );

    CPlayerCamera * pCamera = pPlayer->GetCamera ();

    // Only allow this if we're in fixed mode?
    if ( pCamera->GetMode () == CAMERAMODE_FIXED )
    {
        pCamera->GetPosition ( vecPosition );
        pCamera->GetLookAt ( vecLookAt );
        return true;
    }
    return false;
}


CElement* CStaticFunctionDefinitions::GetCameraTarget ( CPlayer* pPlayer )
{
    assert ( pPlayer );
    return pPlayer->GetCamera ()->GetTarget ();
}


bool CStaticFunctionDefinitions::GetCameraInterior ( CPlayer * pPlayer, unsigned char & ucInterior )
{
    assert ( pPlayer );
    ucInterior = pPlayer->GetCamera ()->GetInterior ();
    return true;
}


bool CStaticFunctionDefinitions::SetCameraMatrix ( CElement* pElement, const CVector& vecPosition, const CVector& vecLookAt )
{
    assert ( pElement );
    RUN_CHILDREN SetCameraMatrix ( *iter, vecPosition, vecLookAt );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        CPlayerCamera * pCamera = pPlayer->GetCamera ();

        pCamera->SetMode ( CAMERAMODE_FIXED );
        pCamera->SetMatrix ( const_cast < CVector & > ( vecPosition ), const_cast < CVector & > ( vecLookAt ) );

        // Tell the player
        CBitStream BitStream;
        BitStream.pBitStream->Write ( vecPosition.fX );   
        BitStream.pBitStream->Write ( vecPosition.fY ); 
        BitStream.pBitStream->Write ( vecPosition.fZ );
        BitStream.pBitStream->Write ( vecLookAt.fX );   
        BitStream.pBitStream->Write ( vecLookAt.fY ); 
        BitStream.pBitStream->Write ( vecLookAt.fZ );
	    pPlayer->Send ( CLuaPacket ( SET_CAMERA_MATRIX, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetCameraTarget ( CElement* pElement, CElement* pTarget )
{
    assert ( pElement );
    RUN_CHILDREN SetCameraTarget ( *iter, pTarget );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        CPlayerCamera * pCamera = pPlayer->GetCamera ();

        // If we don't have a target, change it to the player
        if ( !pTarget ) pTarget = pPlayer;
        
        // Make sure our target is a player element
        if ( pTarget->GetType () == CElement::PLAYER )
        {
            pCamera->SetMode ( CAMERAMODE_PLAYER );
            pCamera->SetTarget ( pTarget );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pTarget->GetID () );   
	        pPlayer->Send ( CLuaPacket ( SET_CAMERA_TARGET, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetCameraInterior ( CElement * pElement, unsigned char ucInterior )
{
    assert ( pElement );
    RUN_CHILDREN SetCameraInterior ( *iter, ucInterior );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        CPlayerCamera * pCamera = pPlayer->GetCamera ();

        if ( pCamera->GetInterior () != ucInterior )
        {
            pCamera->SetInterior ( ucInterior );
    
            CBitStream BitStream;
            BitStream.pBitStream->Write ( ucInterior );
            pPlayer->Send ( CLuaPacket ( SET_CAMERA_INTERIOR, *BitStream.pBitStream ) );

            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::FadeCamera ( CElement * pElement, bool bFadeIn, float fFadeTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    assert ( pElement );
    RUN_CHILDREN FadeCamera ( *iter, bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        pPlayer->SetCamFadedIn ( bFadeIn );

        unsigned char ucFadeIn = bFadeIn ? 1:0;

        if ( !bFadeIn )
            pPlayer->SetCamFadeColor ( ucRed, ucGreen, ucBlue );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucFadeIn );
        BitStream.pBitStream->Write ( fFadeTime );
        if ( !bFadeIn )
        {
            BitStream.pBitStream->Write ( ucRed );
            BitStream.pBitStream->Write ( ucGreen );
            BitStream.pBitStream->Write ( ucBlue );
        }
	    pPlayer->Send ( CLuaPacket ( FADE_CAMERA, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GiveWeapon ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, bool bSetAsCurrent )
{
    assert ( pElement );
    RUN_CHILDREN GiveWeapon ( *iter, ucWeaponID, usAmmo, bSetAsCurrent );

    if ( ucWeaponID == 0 || CPickupManager::IsValidWeaponID ( ucWeaponID ) )
    {
        if ( usAmmo > 9999 ) usAmmo = 9999;

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->IsSpawned () )
            {
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );
                BitStream.pBitStream->Write ( ucWeaponID );
                BitStream.pBitStream->Write ( usAmmo );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bSetAsCurrent ) ? 1 : 0 ) );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( GIVE_WEAPON, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::TakeWeapon ( CElement* pElement, unsigned char ucWeaponID )
{
    assert ( pElement );
    RUN_CHILDREN TakeWeapon ( *iter, ucWeaponID );

    if ( CPickupManager::IsValidWeaponID ( ucWeaponID ) )
    {
        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->IsSpawned () )
            {
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPed->GetID () );                
                BitStream.pBitStream->Write ( ucWeaponID );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( TAKE_WEAPON, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::TakeAllWeapons ( CElement* pElement )
{
    assert ( pElement );
    RUN_CHILDREN TakeAllWeapons ( *iter );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( TAKE_ALL_WEAPONS, *BitStream.pBitStream ) );

		    return true;
        }
    }

	return false;
}


bool CStaticFunctionDefinitions::GiveWeaponAmmo ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo )
{
	assert ( pElement );
	RUN_CHILDREN GiveWeaponAmmo ( *iter, ucWeaponID, usAmmo );

	if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
		    CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
		    BitStream.pBitStream->Write ( ucWeaponID );
            BitStream.pBitStream->Write ( usAmmo );
		    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( GIVE_WEAPON_AMMO, *BitStream.pBitStream ) );

		    return true;
        }
	}

	return false;
}


bool CStaticFunctionDefinitions::TakeWeaponAmmo ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo )
{
	assert ( pElement );
	RUN_CHILDREN TakeWeaponAmmo ( *iter, ucWeaponID, usAmmo );

	if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
		    CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
		    BitStream.pBitStream->Write ( ucWeaponID );
            BitStream.pBitStream->Write ( usAmmo );
		    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( TAKE_WEAPON_AMMO, *BitStream.pBitStream ) );

		    return true;
        }
	}

	return false;
}


bool CStaticFunctionDefinitions::SetWeaponAmmo ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo )
{
    assert ( pElement );
	RUN_CHILDREN SetWeaponAmmo ( *iter, ucWeaponID, usAmmo );

	if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
		    CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GetID () );
		    BitStream.pBitStream->Write ( ucWeaponID );
            BitStream.pBitStream->Write ( usAmmo );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEAPON_AMMO, *BitStream.pBitStream ) );

		    return true;
        }
	}

	return false;
}


CVehicle* CStaticFunctionDefinitions::CreateVehicle ( CResource* pResource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, char* szRegPlate )
{
    if ( CVehicleManager::IsValidModel ( usModel ) )
    {
        //CVehicle* pVehicle = m_pVehicleManager->Create ( usModel, m_pMapManager->GetRootElement () );
		CVehicle* pVehicle = m_pVehicleManager->Create ( usModel, pResource->GetDynamicElementRoot() );
        if ( pVehicle )
        {
            pVehicle->SetPosition ( vecPosition );
            pVehicle->SetRotationDegrees ( vecRotation );

            pVehicle->SetRespawnPosition ( vecPosition );
            pVehicle->SetRespawnRotationDegrees ( vecRotation );

            if ( szRegPlate )
            {
                pVehicle->SetRegPlate ( szRegPlate );
            }

			// Only sync if the resource has fully started
			if ( pResource->HasStarted() )
			{
				CEntityAddPacket Packet;
				Packet.Add ( pVehicle );
				m_pPlayerManager->BroadcastOnlyJoined ( Packet );
			}
            return pVehicle;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetVehicleColor ( CVehicle* pVehicle, unsigned char& ucColor1, unsigned char& ucColor2, unsigned char& ucColor3, unsigned char& ucColor4 )
{
    assert ( pVehicle );

    const CVehicleColor& Color = pVehicle->GetColor ();
    ucColor1 = Color.GetColor1 ();
    ucColor2 = Color.GetColor2 ();
    ucColor3 = Color.GetColor3 ();
    ucColor4 = Color.GetColor4 ();
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleModelFromName ( const char* szName, unsigned short& usID )
{
    assert ( szName );

    unsigned long ulID = CVehicleNames::GetVehicleModel ( szName );
    if ( ulID != 0 )
    {
        usID = static_cast < unsigned short > ( ulID );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleMaxPassengers ( CVehicle* pVehicle, unsigned char& ucMaxPassengers )
{
    assert ( pVehicle );

    ucMaxPassengers = pVehicle->GetMaxPassengers ();
    return ucMaxPassengers != 255;
}


bool CStaticFunctionDefinitions::GetVehicleName ( CVehicle* pVehicle, char* szName )
{
    assert ( pVehicle );
    assert ( szName );

    strncpy ( szName, CVehicleNames::GetVehicleName ( pVehicle->GetModel () ), 32 );
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleNameFromModel ( unsigned short usModel, char* szName )
{
    assert ( szName );

    strncpy ( szName, CVehicleNames::GetVehicleName ( usModel ), 32 );
    return true;
}


CPed* CStaticFunctionDefinitions::GetVehicleOccupant ( CVehicle* pVehicle, unsigned int uiSeat )
{
    assert ( pVehicle );
    return pVehicle->GetOccupant ( uiSeat );
}


CPed* CStaticFunctionDefinitions::GetVehicleController ( CVehicle* pVehicle )
{
    assert ( pVehicle );
    return pVehicle->GetController ();
}


bool CStaticFunctionDefinitions::GetVehicleRotation ( CVehicle* pVehicle, CVector& vecRotation )
{
    assert ( pVehicle );

    pVehicle->GetRotationDegrees ( vecRotation );
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleTurnVelocity ( CVehicle* pVehicle, CVector& vecTurnVelocity )
{
    assert ( pVehicle );

    vecTurnVelocity = pVehicle->GetTurnSpeed ();
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleTurretPosition ( CVehicle* pVehicle, CVector2D& vecPosition )
{
    assert ( pVehicle );

    vecPosition.fX = pVehicle->GetTurretPositionX ();
    vecPosition.fY = pVehicle->GetTurretPositionY ();
    return true;
}


bool CStaticFunctionDefinitions::IsVehicleLocked ( CVehicle* pVehicle, bool& bLocked )
{
    assert ( pVehicle );

    bLocked = pVehicle->IsLocked ();
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeOnSlot ( CVehicle* pVehicle, unsigned char ucSlot, unsigned short& usUpgrade )
{
	assert ( pVehicle );

	CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
	if ( pUpgrades )
	{
		usUpgrade = pUpgrades->GetSlotState ( ucSlot );

		return true;
	}

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned char ucSlot, char* szName )
{
	strncpy ( szName, CVehicleUpgrades::GetSlotName ( ucSlot ), 32 );
	return true;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned short usUpgrade, char* szName )
{
	unsigned char ucSlot;
	if ( CVehicleUpgrades::GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
	{
		strncpy ( szName, CVehicleUpgrades::GetSlotName ( ucSlot ), 32 );
		return true;
	}

	return false;
}    


bool CStaticFunctionDefinitions::GetVehicleDoorState ( CVehicle* pVehicle, unsigned char ucDoor, unsigned char& ucState )
{
	assert ( pVehicle );

	if ( ucDoor < MAX_DOORS )
	{
        ucState = pVehicle->m_ucDoorStates [ ucDoor ];

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::GetVehicleWheelStates ( CVehicle* pVehicle, unsigned char& ucFrontLeft, unsigned char& ucFrontRight, unsigned char& ucRearLeft, unsigned char& ucRearRight )
{
	assert ( pVehicle );

    ucFrontLeft = pVehicle->m_ucWheelStates [ 0 ];
    ucFrontRight = pVehicle->m_ucWheelStates [ 1 ];
    ucRearLeft = pVehicle->m_ucWheelStates [ 2 ];
    ucRearRight = pVehicle->m_ucWheelStates [ 3 ];
		
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleLightState ( CVehicle* pVehicle, unsigned char ucLight, unsigned char& ucState )
{
	assert ( pVehicle );

	if ( ucLight < 4 )
	{
        ucState = pVehicle->m_ucLightStates [ ucLight ];

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::GetVehiclePanelState ( CVehicle* pVehicle, unsigned char ucPanel, unsigned char& ucState )
{
	assert ( pVehicle );

	if ( ucPanel < 7 )
	{
        ucState = pVehicle->m_ucPanelStates [ ucPanel ];
		
		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::GetVehicleOverrideLights ( CVehicle* pVehicle, unsigned char& ucLights )
{
    assert ( pVehicle );

    ucLights = pVehicle->GetOverrideLights ();

    return true;
}


bool CStaticFunctionDefinitions::GetVehiclePaintjob ( CVehicle* pVehicle, unsigned char& ucPaintjob )
{
    assert ( pVehicle );

    ucPaintjob = pVehicle->GetPaintjob ();

    return true;
}


bool CStaticFunctionDefinitions::GetVehiclePlateText ( CVehicle* pVehicle, char* szPlateText )
{
    assert ( pVehicle );

    const char* szRegPlate = pVehicle->GetRegPlate ();
    strcpy ( szPlateText, szRegPlate );
    return true;
}


bool CStaticFunctionDefinitions::IsVehicleFuelTankExplodable ( CVehicle* pVehicle, bool& bExplodable )
{
    assert ( pVehicle );

    bExplodable = pVehicle->IsFuelTankExplodable ();

    return true;
}

bool CStaticFunctionDefinitions::IsVehicleFrozen ( CVehicle* pVehicle, bool& bFrozen )
{
    assert ( pVehicle );

    bFrozen = pVehicle->GetFrozen ();

    return true;
}


bool CStaticFunctionDefinitions::IsVehicleOnGround ( CVehicle* pVehicle, bool& bOnGround )
{
    assert ( pVehicle );

    bOnGround = pVehicle->IsOnGround ();
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleEngineState ( CVehicle * pVehicle, bool & bState )
{
    assert ( pVehicle );

    bState = pVehicle->IsEngineOn ();
    return true;
}


bool CStaticFunctionDefinitions::FixVehicle ( CElement* pElement )
{
    assert ( pElement );
    RUN_CHILDREN FixVehicle ( *iter );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Generate a new sync time context or it's likely to blow again
        // for those who are not syncing it being around it.
        pVehicle->GenerateSyncTimeContext ();

        // Repair it
        pVehicle->SetHealth ( DEFAULT_VEHICLE_HEALTH );
        pVehicle->GetInitialDoorStates ( pVehicle->m_ucDoorStates );
        memset ( pVehicle->m_ucWheelStates, 0, sizeof ( pVehicle->m_ucWheelStates ) );
        memset ( pVehicle->m_ucPanelStates, 0, sizeof ( pVehicle->m_ucPanelStates ) );
        memset ( pVehicle->m_ucLightStates, 0, sizeof ( pVehicle->m_ucLightStates ) );
        
        pVehicle->SetBlowTime ( 0 );
        
        // Tell everyone
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( pVehicle->GetSyncTimeContext () );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( FIX_VEHICLE, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::BlowVehicle ( CElement* pElement, bool bExplode )
{
    assert ( pElement );
    RUN_CHILDREN BlowVehicle ( *iter, bExplode );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Blow it up on our records. Also change the sync time context or this vehicle
        // is likely to blow up twice, call the events twice and all that if someone's
        // nearby and syncing it.
        pVehicle->SetHealth ( 0.0f );
        pVehicle->SetBlowTime ( ::GetTime () );
        pVehicle->GenerateSyncTimeContext ();

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( ( unsigned char ) ( ( bExplode ) ? 1 : 0 ) );
        BitStream.pBitStream->Write ( pVehicle->GetSyncTimeContext () );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( BLOW_VEHICLE, *BitStream.pBitStream ) );
 
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleColor ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleColor ( *iter, ucRed, ucGreen, ucBlue, ucAlpha );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetColor ( CVehicleColor ( ucRed, ucGreen, ucBlue, ucAlpha ) );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( pVehicle->GetColor ().GetColor () );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_COLOR, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleLandingGearDown ( CElement* pElement, bool bLandingGearDown )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleLandingGearDown ( *iter, bLandingGearDown );

    // Is this a vehicle?
    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Has landing gear and different than before?
        if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) &&
             bLandingGearDown != pVehicle->IsLandingGearDown () )
        {
            // Set the new state
            pVehicle->SetLandingGearDown ( bLandingGearDown );

            // Tell everyone
            unsigned char ucLandingGearDown = 0;
            if ( bLandingGearDown ) ucLandingGearDown = 1;

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( ucLandingGearDown );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_LANDING_GEAR_DOWN, *BitStream.pBitStream ) );
        }
    }

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleLocked ( CElement* pElement, bool bLocked )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleLocked ( *iter, bLocked );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        
		//if ( bLocked != pVehicle->IsLocked () )	// this lock check is redundant
        {
            // Set the new locked state
            pVehicle->SetLocked ( bLocked );

            // Tell all the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bLocked ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_LOCKED, *BitStream.pBitStream ) );
        
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( CElement* pElement, bool bDoorsUndamageable )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleDoorsUndamageable ( *iter, bDoorsUndamageable );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        if ( bDoorsUndamageable != pVehicle->AreDoorsUndamageable () )
        {
            // Set the new doors undamageable state
            pVehicle->SetDoorsUndamageable ( bDoorsUndamageable );

            // Tell all the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bDoorsUndamageable ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DOORS_UNDAMAGEABLE, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleRotation ( CElement* pElement, const CVector& vecRotation )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleRotation ( *iter, vecRotation );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Set it
        pVehicle->SetRotationDegrees ( vecRotation );

        // Tell all players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( vecRotation.fX );
        BitStream.pBitStream->Write ( vecRotation.fY );
        BitStream.pBitStream->Write ( vecRotation.fZ );
        BitStream.pBitStream->Write ( pVehicle->GenerateSyncTimeContext () );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_ROTATION, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleSirensOn ( CElement* pElement, bool bSirensOn )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleSirensOn ( *iter, bSirensOn );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Has Sirens and different state than before?
        if ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) &&
             bSirensOn != pVehicle->IsSirenActive () )
        {
            // Set the new state
            pVehicle->SetSirenActive ( bSirensOn );

            // Tell everyone
            unsigned char ucSirensOn = 0;
            if ( bSirensOn ) ucSirensOn = 1;

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( ucSirensOn );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_SIRENE_ON, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleTurnVelocity ( CElement* pElement, const CVector& vecTurnVelocity )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleTurnVelocity ( *iter, vecTurnVelocity );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetTurnSpeed ( vecTurnVelocity );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( vecTurnVelocity.fX );
        BitStream.pBitStream->Write ( vecTurnVelocity.fY );
        BitStream.pBitStream->Write ( vecTurnVelocity.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_TURNSPEED, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::AddVehicleUpgrade ( CElement* pElement, unsigned short usUpgrade )
{
	assert ( pElement );
    RUN_CHILDREN AddVehicleUpgrade ( *iter, usUpgrade );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
		if ( pUpgrades )
		{
            // Did we add it successfully?
			if ( pUpgrades->AddUpgrade ( usUpgrade ) )
            {
				CBitStream BitStream;
				BitStream.pBitStream->Write ( pVehicle->GetID () );
				BitStream.pBitStream->Write ( usUpgrade );
				m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( ADD_VEHICLE_UPGRADE, *BitStream.pBitStream ) );
				
				return true;
			}
		}
    }

    return false;
}


bool CStaticFunctionDefinitions::AddAllVehicleUpgrades ( CElement* pElement )
{
	assert ( pElement );
    RUN_CHILDREN AddAllVehicleUpgrades ( *iter );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
		if ( pUpgrades )
		{
            pUpgrades->AddAllUpgrades ();

			CBitStream BitStream;
			BitStream.pBitStream->Write ( pVehicle->GetID () );
			m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( ADD_ALL_VEHICLE_UPGRADES, *BitStream.pBitStream ) );
			
			return true;
		}
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveVehicleUpgrade ( CElement* pElement, unsigned short usUpgrade )
{
	assert ( pElement );
    RUN_CHILDREN RemoveVehicleUpgrade ( *iter, usUpgrade );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
		if ( pUpgrades )
		{
			if ( pUpgrades->HasUpgrade ( usUpgrade ) )
			{
				unsigned char ucSlot;
				if ( pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
				{
					pUpgrades->SetSlotState ( ucSlot, 0 );

					// Convert to unsigned char and send
					unsigned char ucUpgrade = ( usUpgrade - 1000 );
					CBitStream BitStream;
					BitStream.pBitStream->Write ( pVehicle->GetID () );
					BitStream.pBitStream->Write ( ucUpgrade );
					m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_VEHICLE_UPGRADE, *BitStream.pBitStream ) );
					
					return true;
				}
			}
		}
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDoorState ( CElement* pElement, unsigned char ucDoor, unsigned char ucState )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleDoorState ( *iter, ucDoor, ucState );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		if ( ucDoor < MAX_DOORS )
		{
            switch ( pVehicle->GetModel () )
            {        
                case VT_BFINJECT:
                case VT_RCBANDIT:
                case VT_CADDY:
                case VT_RCRAIDER:
                case VT_BAGGAGE:
                case VT_DOZER:
                case VT_FORKLIFT:
                case VT_TRACTOR:
                case VT_TIGER:
                case VT_BANDITO:
                case VT_KART:
                case VT_MOWER:
                case VT_RCCAM:
                case VT_RCGOBLIN:
                    return false;
            }

            if ( ucState != pVehicle->m_ucDoorStates [ ucDoor ] )
            {
                pVehicle->m_ucDoorStates [ ucDoor ] = ucState;

			    CBitStream BitStream;
			    BitStream.pBitStream->Write ( pVehicle->GetID () );
			    unsigned char ucObject = 0;
			    BitStream.pBitStream->Write ( ucObject );
			    BitStream.pBitStream->Write ( ucDoor );
			    BitStream.pBitStream->Write ( ucState );
			    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );
        
			    return true;
            }
		}
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleWheelStates ( CElement* pElement, unsigned char ucFrontLeft, unsigned char ucFrontRight, unsigned char ucRearLeft, unsigned char ucRearRight )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleWheelStates ( *iter, ucFrontLeft, ucFrontRight, ucRearLeft, ucRearRight );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // If atleast 1 wheel state is different
        if ( ( ucFrontLeft != -1 && ucFrontLeft != pVehicle->m_ucWheelStates [ FRONT_LEFT_WHEEL ] ) ||
             ( ucRearLeft != -1 && ucRearLeft != pVehicle->m_ucWheelStates [ REAR_LEFT_WHEEL ] ) ||
             ( ucFrontRight != -1 && ucFrontRight != pVehicle->m_ucWheelStates [ FRONT_RIGHT_WHEEL ] ) ||
             ( ucRearRight != -1 && ucRearRight != pVehicle->m_ucWheelStates [ REAR_RIGHT_WHEEL ] ) )
        {
            if ( ucFrontLeft != -1 ) pVehicle->m_ucWheelStates [ FRONT_LEFT_WHEEL ] = ucFrontLeft;
            if ( ucRearLeft != -1 ) pVehicle->m_ucWheelStates [ REAR_LEFT_WHEEL ] = ucRearLeft;
            if ( ucFrontRight != -1 ) pVehicle->m_ucWheelStates [ FRONT_RIGHT_WHEEL ] = ucFrontRight;
            if ( ucRearRight != -1 )  pVehicle->m_ucWheelStates [ REAR_RIGHT_WHEEL ] = ucRearRight;

			CBitStream BitStream;
			BitStream.pBitStream->Write ( pVehicle->GetID () );
			BitStream.pBitStream->Write ( ( char * ) pVehicle->m_ucWheelStates, MAX_WHEELS );
			m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_WHEEL_STATES, *BitStream.pBitStream ) );
    
			return true;
		}
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleLightState ( CElement* pElement, unsigned char ucLight, unsigned char ucState )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleLightState ( *iter, ucLight, ucState );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		if ( ucLight < MAX_LIGHTS )
		{		
            if ( ucState != pVehicle->m_ucLightStates [ ucLight ] )
            {
			    pVehicle->m_ucLightStates [ ucLight ] = ucState;

			    CBitStream BitStream;
			    BitStream.pBitStream->Write ( pVehicle->GetID () );
			    unsigned char ucObject = 2;
			    BitStream.pBitStream->Write ( ucObject );
			    BitStream.pBitStream->Write ( ucLight );
			    BitStream.pBitStream->Write ( ucState );
			    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );

			    return true;
            }
		}
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehiclePanelState ( CElement* pElement, unsigned char ucPanel, unsigned char ucState )
{
	assert ( pElement );
	RUN_CHILDREN SetVehiclePanelState ( *iter, ucPanel, ucState );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

		if ( ucPanel < MAX_PANELS )
		{
            if ( ucState != pVehicle->m_ucPanelStates [ ucPanel ] )
            {
                pVehicle->m_ucPanelStates [ ucPanel ] = ucState;

			    CBitStream BitStream;
			    BitStream.pBitStream->Write ( pVehicle->GetID () );
			    unsigned char ucObject = 3;
			    BitStream.pBitStream->Write ( ucObject );
			    BitStream.pBitStream->Write ( ucPanel );
			    BitStream.pBitStream->Write ( ucState );
			    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );
			    return true;
            }
		}
	}

	return false;
}


bool CStaticFunctionDefinitions::ToggleVehicleRespawn ( CElement* pElement, bool bRespawn )
{
	assert ( pElement );
	RUN_CHILDREN ToggleVehicleRespawn ( *iter, bRespawn );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetRespawnEnabled ( bRespawn );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleRespawnDelay ( CElement* pElement, unsigned long ulTime )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleRespawnDelay ( *iter, ulTime );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetRespawnTime ( ulTime );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay ( CElement* pElement, unsigned long ulTime )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleIdleRespawnDelay ( *iter, ulTime );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetIdleRespawnTime ( ulTime );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleRespawnPosition ( CElement* pElement, const CVector& vecPosition, const CVector& vecRotation )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleRespawnPosition ( *iter, vecPosition, vecRotation );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        pVehicle->SetRespawnPosition ( vecPosition );
        pVehicle->SetRespawnRotationDegrees ( vecRotation );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::ResetVehicleExplosionTime ( CElement* pElement )
{
	assert ( pElement );
	RUN_CHILDREN ResetVehicleExplosionTime ( *iter );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetBlowTime ( 0 );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::ResetVehicleIdleTime ( CElement* pElement )
{
	assert ( pElement );
	RUN_CHILDREN ResetVehicleIdleTime ( *iter );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetIdleTime ( 0 );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::SpawnVehicle ( CElement* pElement, const CVector& vecPosition, const CVector& vecRotation )
{
	assert ( pElement );
	RUN_CHILDREN SpawnVehicle ( *iter, vecPosition, vecRotation );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        pVehicle->SetBlowTime ( 0 );
        pVehicle->SetIdleTime ( 0 );
        pVehicle->GetInitialDoorStates ( pVehicle->m_ucDoorStates );
        memset ( pVehicle->m_ucWheelStates, 0, sizeof ( pVehicle->m_ucWheelStates ) );
        memset ( pVehicle->m_ucPanelStates, 0, sizeof ( pVehicle->m_ucPanelStates ) );
        memset ( pVehicle->m_ucLightStates, 0, sizeof ( pVehicle->m_ucLightStates ) );
        pVehicle->SetLandingGearDown ( true );
        pVehicle->SetAdjustableProperty ( 0 );

        // Move it to spawn
        pVehicle->SetPosition ( vecPosition );

        // Convert rotation to radians and set it
        pVehicle->SetRotationDegrees ( vecRotation );

        // Tell everyone to spawn it
        CVehicleSpawnPacket Packet;
        Packet.Add ( pVehicle );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::RespawnVehicle ( CElement* pElement )
{
	assert ( pElement );
	RUN_CHILDREN RespawnVehicle ( *iter );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        pVehicle->SetBlowTime ( 0 );
        pVehicle->SetIdleTime ( 0 );
        pVehicle->GetInitialDoorStates ( pVehicle->m_ucDoorStates );
        memset ( pVehicle->m_ucWheelStates, 0, sizeof ( pVehicle->m_ucWheelStates ) );
        memset ( pVehicle->m_ucPanelStates, 0, sizeof ( pVehicle->m_ucPanelStates ) );
        memset ( pVehicle->m_ucLightStates, 0, sizeof ( pVehicle->m_ucLightStates ) );
        pVehicle->SetLandingGearDown ( true );
        pVehicle->SetAdjustableProperty ( 0 );

        // Respawn it
        pVehicle->PutAtRespawnLocation ();

        // Call the respawn event?
        CLuaArguments Arguments;
        Arguments.PushBoolean ( false );
        pVehicle->CallEvent ( "onVehicleRespawn", Arguments );

        // Tell everyone to respawn it
        CVehicleSpawnPacket Packet;
        Packet.Add ( pVehicle );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );

		return true;
	}

	return false;
}


bool CStaticFunctionDefinitions::SetVehicleOverrideLights ( CElement* pElement, unsigned char ucLights )
{
	assert ( pElement );
	RUN_CHILDREN SetVehicleOverrideLights ( *iter, ucLights );

	if ( IS_VEHICLE ( pElement ) )
	{
		CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( pVehicle->GetOverrideLights () != ucLights )
        {
            pVehicle->SetOverrideLights ( ucLights );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( ucLights );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_OVERRIDE_LIGHTS, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::AttachTrailerToVehicle ( CVehicle* pVehicle, CVehicle* pTrailer )
{
    assert ( pVehicle );
    assert ( pTrailer );

    // Check if the vehicle already has a trailer
    if ( pVehicle->GetTowedVehicle () == NULL )
    {
        // ..and the trailer isnt attached to anything
        if ( pTrailer->GetTowedByVehicle () == NULL )
        {
            // Attach them
            pVehicle->SetTowedVehicle ( pTrailer );
            pTrailer->SetTowedByVehicle ( pVehicle );

            // Tell everyone to attach them
            CVehicleTrailerPacket AttachPacket ( pVehicle, pTrailer, true );
            m_pPlayerManager->BroadcastOnlyJoined ( AttachPacket );

            // Execute the attach trailer script function
            CLuaArguments Arguments;
            Arguments.PushElement ( pVehicle );
            bool bContinue = pTrailer->CallEvent ( "onTrailerAttach", Arguments );

            if ( !bContinue )
            {
                // Detach them
                CVehicleTrailerPacket DetachPacket ( pVehicle, pTrailer, false );
                m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );
            }

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::DetachTrailerFromVehicle ( CVehicle* pVehicle, CVehicle* pTrailer )
{
    assert ( pVehicle );
    if ( pTrailer )
        assert ( pTrailer );

    // Is there a trailer attached, and does it match this one
    CVehicle* pTempTrailer = pVehicle->GetTowedVehicle ();
    if ( pTempTrailer && ( pTrailer == NULL || pTempTrailer == pTrailer ) )
    {
        // Detach them
        pVehicle->SetTowedVehicle ( NULL );
        pTempTrailer->SetTowedByVehicle ( NULL );

        // Tell everyone to detach them
        CVehicleTrailerPacket DetachPacket ( pVehicle, pTempTrailer, false );
        m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );

        // Execute the detach trailer script function
        CLuaArguments Arguments;
        Arguments.PushElement ( pVehicle );
        pTempTrailer->CallEvent ( "onTrailerDetach", Arguments );
    
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleEngineState ( CElement* pElement, bool bState )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleEngineState ( *iter, bState );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bState ) ? 1 : 0 ) );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_ENGINE_STATE, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDirtLevel ( CElement* pElement, float fDirtLevel )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleDirtLevel ( *iter, fDirtLevel );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetID () );
        BitStream.pBitStream->Write ( fDirtLevel );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DIRT_LEVEL, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleDamageProof ( CElement* pElement, bool bDamageProof )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleDamageProof ( *iter, bDamageProof );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( pVehicle->m_bDamageProof != bDamageProof )
        {
            pVehicle->SetDamageProof ( bDamageProof );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bDamageProof ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_DAMAGE_PROOF, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehiclePaintjob ( CElement* pElement, unsigned char ucPaintjob )
{
    assert ( pElement );
    RUN_CHILDREN SetVehiclePaintjob ( *iter, ucPaintjob );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( ucPaintjob != pVehicle->GetPaintjob () && ucPaintjob <= 3 )
        {
            pVehicle->SetPaintjob ( ucPaintjob );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( ucPaintjob );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_PAINTJOB, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( CElement* pElement, bool bExplodable )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleFuelTankExplodable ( *iter, bExplodable );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( bExplodable != pVehicle->IsFuelTankExplodable () )
        {
            pVehicle->SetFuelTankExplodable ( bExplodable );

            CBitStream BitStream;
            BitStream.pBitStream->Write ( pVehicle->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bExplodable ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_FUEL_TANK_EXPLODABLE, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleFrozen ( CVehicle* pVehicle, bool bFrozen )
{
    assert ( pVehicle );

    pVehicle->SetFrozen ( bFrozen );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pVehicle->GetID () );
    if ( bFrozen )
        BitStream.pBitStream->Write ( ( unsigned char ) 0 );
    else
        BitStream.pBitStream->Write ( ( unsigned char ) 1 );

    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_VEHICLE_FROZEN, *BitStream.pBitStream ) );

    return true;
}


CMarker* CStaticFunctionDefinitions::CreateMarker ( CResource* pResource, const CVector& vecPosition, const char* szType, float fSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, CElement* pVisibleTo )
{
    assert ( szType );

    // Grab the type id
    unsigned char ucType = CMarkerManager::StringToType ( szType );
    if ( ucType != CMarker::TYPE_INVALID )
    {
        // Create the marker
        //CMarker* pMarker = m_pMarkers->Create ( m_pMapManager->GetRootElement () );
		CMarker* pMarker = m_pMarkerManager->Create ( pResource->GetDynamicElementRoot() );
        if ( pMarker )
        {
            // Set the properties
            pMarker->SetPosition ( vecPosition );
            pMarker->SetMarkerType ( ucType );
            pMarker->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
            pMarker->SetSize ( fSize );

            // Make him visible to the given element
            if ( pVisibleTo )
            {
                pMarker->AddVisibleToReference ( pVisibleTo );
            }

            // Tell everyone about it
			if ( pResource->HasStarted() )
				pMarker->Sync ( true );
            return pMarker;
        }
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetMarkerCount ( unsigned int& uiCount )
{
    uiCount = m_pMarkerManager->Count ();
    return true;
}


bool CStaticFunctionDefinitions::GetMarkerType ( CMarker* pMarker, char* szType )
{
    assert ( pMarker );

    return CMarkerManager::TypeToString ( pMarker->GetMarkerType (), szType );
}


bool CStaticFunctionDefinitions::GetMarkerSize ( CMarker* pMarker, float& fSize )
{
    assert ( pMarker );

    fSize = pMarker->GetSize ();
    return true;
}


bool CStaticFunctionDefinitions::GetMarkerColor ( CMarker* pMarker, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha )
{
    assert ( pMarker );

    ucRed = pMarker->GetColorRed ();
    ucGreen = pMarker->GetColorGreen ();
    ucBlue = pMarker->GetColorBlue ();
    ucAlpha = pMarker->GetColorAlpha ();
    return true;
}


bool CStaticFunctionDefinitions::GetMarkerTarget ( CMarker* pMarker, CVector& vecTarget )
{
    assert ( pMarker );

    if ( pMarker->HasTarget () )
    {
        vecTarget = pMarker->GetTarget ();
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetMarkerIcon ( CMarker* pMarker, char* szIcon )
{
    assert ( pMarker );

    return CMarkerManager::IconToString ( pMarker->GetIcon (), szIcon );
}


bool CStaticFunctionDefinitions::SetMarkerType ( CElement* pElement, const char* szType )
{
    assert ( pElement );
    assert ( szType );
    RUN_CHILDREN SetMarkerType ( *iter, szType );

    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        // Grab the marker type
        unsigned char ucType = CMarkerManager::StringToType ( szType );
        if ( ucType != CMarker::TYPE_INVALID )
        {
            // Set the new type
            CMarker* pMarker = static_cast < CMarker* > ( pElement );
            pMarker->SetMarkerType ( ucType );
            return true;
        }

        return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetMarkerSize ( CElement* pElement, float fSize )
{
    assert ( pElement );
    RUN_CHILDREN SetMarkerSize ( *iter, fSize );

    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        // Set the new size
        CMarker* pMarker = static_cast < CMarker* > ( pElement );
        pMarker->SetSize ( fSize );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetMarkerColor ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    assert ( pElement );
    RUN_CHILDREN SetMarkerColor ( *iter, ucRed, ucGreen, ucBlue, ucAlpha );

    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        // Set the new color
        CMarker* pMarker = static_cast < CMarker* > ( pElement );
        pMarker->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetMarkerTarget ( CElement* pElement, const CVector* pTarget )
{
    assert ( pElement );
    RUN_CHILDREN SetMarkerTarget ( *iter, pTarget );

    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        // Set the new target
        CMarker* pMarker = static_cast < CMarker* > ( pElement );
        pMarker->SetTarget ( pTarget );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMarkerIcon ( CElement* pElement, const char* szIcon )
{
    assert ( pElement );
    assert ( szIcon );
    RUN_CHILDREN SetMarkerIcon ( *iter, szIcon );
    
    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        unsigned char ucIcon = CMarkerManager::StringToIcon ( szIcon );
        if ( ucIcon != CMarker::ICON_INVALID )
        {
            // Set the new icon
            CMarker* pMarker = static_cast < CMarker* > ( pElement );
            if ( pMarker->GetIcon () != ucIcon )
            {
                pMarker->SetIcon ( ucIcon );

                return true;
            }
        }
    }

    return false;
}


CBlip* CStaticFunctionDefinitions::CreateBlip ( CResource* pResource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, short sOrdering, CElement* pVisibleTo )
{
    // Valid icon and size?
    if ( CBlipManager::IsValidIcon ( ucIcon ) && ucSize <= 25 )
    {
        // Create the blip as a child of the root item
        //CBlip* pBlip = m_pBlipManager->Create ( m_pMapManager->GetRootElement () );
		CBlip* pBlip = m_pBlipManager->Create ( pResource->GetDynamicElementRoot() );
        if ( pBlip )
        {
            // Set the given properties
            pBlip->SetPosition ( vecPosition );
            pBlip->m_ucIcon = ucIcon;
            pBlip->m_ucSize = ucSize;
            pBlip->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
            pBlip->m_sOrdering = sOrdering;

            // Make him visible to the given element
            if ( pVisibleTo )
            {
                pBlip->AddVisibleToReference ( pVisibleTo );
            }

            // Tell everyone about it
			if ( pResource->HasStarted() )
				pBlip->Sync ( true );
            return pBlip;
        }
    }

    return NULL;
}


CBlip* CStaticFunctionDefinitions::CreateBlipAttachedTo ( CResource* pResource, CElement* pElement, unsigned char ucIcon, unsigned char ucSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, short sOrdering, CElement* pVisibleTo )
{
    assert ( pElement );
    // Valid icon and size?
    if ( CBlipManager::IsValidIcon ( ucIcon ) && ucSize <= 25 )
    {
        CBlip* pBlip = m_pBlipManager->Create ( m_pMapManager->GetRootElement () );
        if ( pBlip )
        {           
            // Set the properties
            pBlip->m_ucIcon = ucIcon;
            pBlip->m_ucSize = ucSize;
            pBlip->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
            pBlip->m_sOrdering = sOrdering;

            // Set his visible to to the root
            pBlip->AddVisibleToReference ( pVisibleTo );
            pBlip->AttachTo ( pElement );

            // Tell everyone about it
			if ( pResource->HasStarted() )
				pBlip->Sync ( true );           

            return pBlip;
        }
    }
    return NULL;
}


bool CStaticFunctionDefinitions::GetBlipIcon ( CBlip* pBlip, unsigned char& ucIcon )
{
    assert ( pBlip );

    ucIcon = pBlip->m_ucIcon;
    return true;
}


bool CStaticFunctionDefinitions::GetBlipSize ( CBlip* pBlip, unsigned char& ucSize )
{
    assert ( pBlip );

    ucSize = pBlip->m_ucSize;
    return true;
}


bool CStaticFunctionDefinitions::GetBlipColor ( CBlip* pBlip, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha )
{
    assert ( pBlip );

    ucRed = pBlip->m_ucColorRed;
    ucGreen = pBlip->m_ucColorGreen;
    ucBlue = pBlip->m_ucColorBlue;
    ucAlpha = pBlip->m_ucColorAlpha;
    return true;
}


bool CStaticFunctionDefinitions::GetBlipOrdering ( CBlip* pBlip, short& sOrdering )
{
    assert ( pBlip );

    sOrdering = pBlip->m_sOrdering;
    return true;
}


bool CStaticFunctionDefinitions::SetBlipIcon ( CElement* pElement, unsigned char ucIcon )
{
    assert ( pElement );

    if ( CBlipManager::IsValidIcon ( ucIcon ) )
    {
        RUN_CHILDREN SetBlipIcon ( *iter, ucIcon );

        if ( IS_BLIP ( pElement ) )
        {
            // Grab the blip and set the new icon
            CBlip* pBlip = static_cast < CBlip* > ( pElement );
            if ( pBlip->m_ucIcon != ucIcon )
            {
                pBlip->m_ucIcon = ucIcon;

                CBitStream bitStream;
                bitStream.pBitStream->Write ( pBlip->GetID () );
                bitStream.pBitStream->Write ( ucIcon );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_BLIP_ICON, *bitStream.pBitStream ) );

                return true;
            }
        }        
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipSize ( CElement* pElement, unsigned char ucSize )
{
    if ( ucSize <= 25 )
    {
        RUN_CHILDREN SetBlipSize ( *iter, ucSize );

        if ( IS_BLIP ( pElement ) )
        {
            // Grab the blip and set the new size
            CBlip* pBlip = static_cast < CBlip* > ( pElement );
            if ( pBlip->m_ucSize != ucSize )
            {
                pBlip->m_ucSize = ucSize;

                CBitStream bitStream;
                bitStream.pBitStream->Write ( pBlip->GetID () );
                bitStream.pBitStream->Write ( ucSize );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_BLIP_SIZE, *bitStream.pBitStream ) );

                return true;
            }
        }

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipColor ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    RUN_CHILDREN SetBlipColor ( *iter, ucRed, ucGreen, ucBlue, ucAlpha );

    if ( IS_BLIP ( pElement ) )
    {
        // Grab the blip and set the new color
        CBlip* pBlip = static_cast < CBlip* > ( pElement );
        if ( pBlip->m_ucColorRed != ucRed ||
             pBlip->m_ucColorGreen != ucGreen ||
             pBlip->m_ucColorBlue != ucBlue ||
             pBlip->m_ucColorAlpha != ucAlpha )
        {
            pBlip->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );

            CBitStream bitStream;
            bitStream.pBitStream->Write ( pBlip->GetID () );
            bitStream.pBitStream->Write ( ucRed );
            bitStream.pBitStream->Write ( ucGreen );
            bitStream.pBitStream->Write ( ucBlue );
            bitStream.pBitStream->Write ( ucAlpha );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_BLIP_COLOR, *bitStream.pBitStream ) );

            return true;
        }
    }

    return true;
}


bool CStaticFunctionDefinitions::SetBlipOrdering ( CElement* pElement, short sOrdering )
{
    RUN_CHILDREN SetBlipOrdering ( *iter, sOrdering );

    if ( IS_BLIP ( pElement ) )
    {
        // Grab the blip and set the new color
        CBlip* pBlip = static_cast < CBlip* > ( pElement );
        if ( pBlip->m_sOrdering != sOrdering )
        {
            pBlip->m_sOrdering = sOrdering;

            CBitStream bitStream;
            bitStream.pBitStream->Write ( pBlip->GetID () );
            bitStream.pBitStream->Write ( sOrdering );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_BLIP_ORDERING, *bitStream.pBitStream ) );

            return true;
        }
    }

    return true;
}


CObject* CStaticFunctionDefinitions::CreateObject ( CResource* pResource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation )
{
    //CObject* pObject = m_pObjectManager->Create ( m_pMapManager->GetRootElement () );
	CObject* pObject = m_pObjectManager->Create ( pResource->GetDynamicElementRoot() );
    if ( pObject )
    {
        // Convert the rotation from degrees to radians managed internally
        CVector vecRadians = vecRotation;
        ConvertDegreesToRadians ( vecRadians );

        pObject->SetPosition ( vecPosition );
        pObject->SetRotation ( vecRadians );
        pObject->SetModel ( usModelID );

		if ( pResource->HasStarted() )
		{
			CEntityAddPacket Packet;
			Packet.Add ( pObject );
			m_pPlayerManager->BroadcastOnlyJoined ( Packet );
		}

        return pObject;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetObjectRotation ( CObject* pObject, CVector& vecRotation )
{
    vecRotation = pObject->GetRotation ();
    ConvertRadiansToDegrees ( vecRotation );
    return true;
}


bool CStaticFunctionDefinitions::SetObjectRotation ( CElement* pElement, const CVector& vecRotation )
{
    RUN_CHILDREN SetObjectRotation ( *iter, vecRotation );

    if ( IS_OBJECT ( pElement ) )
    {
        CObject* pObject = static_cast < CObject* > ( pElement );

        // Convert the rotation given to radians and set it
        CVector vecRadians = vecRotation;
        ConvertDegreesToRadians ( vecRadians );
        pObject->SetRotation ( vecRadians );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pObject->GetID () );
        BitStream.pBitStream->Write ( vecRadians.fX );
        BitStream.pBitStream->Write ( vecRadians.fY );
        BitStream.pBitStream->Write ( vecRadians.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_OBJECT_ROTATION, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::MoveObject ( CElement* pElement, unsigned long ulTime, const CVector& vecPosition, const CVector& vecRotation )
{
    RUN_CHILDREN MoveObject ( *iter, ulTime, vecPosition, vecRotation );

    if ( IS_OBJECT ( pElement ) )
    {
        CObject* pObject = static_cast < CObject* > ( pElement );

        // Grab the source position and rotation
        CVector vecSourcePosition = pObject->GetPosition ();
        CVector vecSourceRotation = pObject->GetRotation ();

        // Convert the target rotation given to radians (don't wrap around as these can be rotated more than 360)
        CVector vecRadians = vecRotation;
        ConvertDegreesToRadiansNoWrap ( vecRadians );

        // Tell the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pObject->GetID () );
        BitStream.pBitStream->Write ( ulTime );
        BitStream.pBitStream->Write ( vecSourcePosition.fX );
        BitStream.pBitStream->Write ( vecSourcePosition.fY );
        BitStream.pBitStream->Write ( vecSourcePosition.fZ );
        BitStream.pBitStream->Write ( vecSourceRotation.fX );
        BitStream.pBitStream->Write ( vecSourceRotation.fY );
        BitStream.pBitStream->Write ( vecSourceRotation.fZ );
        BitStream.pBitStream->Write ( vecPosition.fX );
        BitStream.pBitStream->Write ( vecPosition.fY );
        BitStream.pBitStream->Write ( vecPosition.fZ );
        BitStream.pBitStream->Write ( vecRadians.fX );
        BitStream.pBitStream->Write ( vecRadians.fY );
        BitStream.pBitStream->Write ( vecRadians.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( MOVE_OBJECT, *BitStream.pBitStream ) );

        // Start moving it here so we can keep track of the position/rotation
        pObject->Move ( vecPosition, vecSourceRotation + vecRadians, ulTime );
    }

    return true;
}


bool CStaticFunctionDefinitions::StopObject ( CElement* pElement )
{
    RUN_CHILDREN StopObject ( *iter );

    if ( IS_OBJECT ( pElement ) )
    {
        CObject* pObject = static_cast < CObject* > ( pElement );

		pObject->StopMoving ();

        // Grab the source position and rotation
        CVector vecSourcePosition = pObject->GetPosition ();
        CVector vecSourceRotation = pObject->GetRotation ();		

        // Tell the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pObject->GetID () );
        BitStream.pBitStream->Write ( vecSourcePosition.fX );
        BitStream.pBitStream->Write ( vecSourcePosition.fY );
        BitStream.pBitStream->Write ( vecSourcePosition.fZ );
        BitStream.pBitStream->Write ( vecSourceRotation.fX );
        BitStream.pBitStream->Write ( vecSourceRotation.fY );
        BitStream.pBitStream->Write ( vecSourceRotation.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( STOP_OBJECT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


CRadarArea* CStaticFunctionDefinitions::CreateRadarArea ( CResource* pResource, const CVector2D& vecPosition2D, const CVector2D& vecSize, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha, CElement* pVisibleTo )
{
    // Create it
    //CRadarArea* pRadarArea = m_pRadarAreaManager->Create ( m_pMapManager->GetRootElement (), NULL );
	CRadarArea* pRadarArea = m_pRadarAreaManager->Create ( pResource->GetDynamicElementRoot(), NULL );
    if ( pRadarArea )
    {
        // Set the properties
        CVector vecPosition = CVector ( vecPosition2D.fX, vecPosition2D.fY, 0.0f );
        pRadarArea->SetPosition ( vecPosition );
        pRadarArea->SetSize ( vecSize );
        pRadarArea->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );

        // Make him visible to the root
        if ( pVisibleTo )
        {
            pRadarArea->AddVisibleToReference ( pVisibleTo );
        }

        // Tell all the players
		if ( pResource->HasStarted() )
			pRadarArea->Sync ( true );
        return pRadarArea;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::GetRadarAreaSize ( CRadarArea* pRadarArea, CVector2D& vecSize )
{
    assert ( pRadarArea );
    vecSize = pRadarArea->GetSize ();
    return true;
}


bool CStaticFunctionDefinitions::GetRadarAreaColor ( CRadarArea* pRadarArea, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha )
{
    assert ( pRadarArea );

    ucRed = pRadarArea->GetColorRed ();
    ucGreen = pRadarArea->GetColorGreen ();
    ucBlue = pRadarArea->GetColorBlue ();
    ucAlpha = pRadarArea->GetColorAlpha ();
    return true;
}


bool CStaticFunctionDefinitions::IsRadarAreaFlashing ( CRadarArea* pRadarArea )
{
    assert ( pRadarArea );
    return pRadarArea->IsFlashing ();
}


bool CStaticFunctionDefinitions::IsInsideRadarArea ( CRadarArea* pRadarArea, const CVector2D& vecPosition, bool& bInside )
{
    assert ( pRadarArea );

    CVector vecTemp = pRadarArea->GetPosition ();
    CVector2D vecSize = pRadarArea->GetSize ();
    // Remove this line if the position of radar areas isnt in the center
    vecTemp -= ( CVector ( vecSize.fX, vecSize.fY, 0.0f ) * CVector ( 0.5f, 0.5f, 0.5f ) );
    bInside = false;
    // Do the calc from the bottom left
    if ( vecPosition.fX >= vecTemp.fX && vecPosition.fX <= ( vecTemp.fX + vecSize.fX ) )
    {
        if ( vecPosition.fY >= vecTemp.fY && vecPosition.fY <= ( vecTemp.fY + vecSize.fY ) )
        {
            bInside = true;
        }
    }
    return true;
}


bool CStaticFunctionDefinitions::SetRadarAreaSize ( CElement* pElement, const CVector2D& vecSize )
{
    assert ( pElement );
    RUN_CHILDREN SetRadarAreaSize ( *iter, vecSize );

    if ( IS_RADAR_AREA ( pElement ) )
    {
        CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );
        pRadarArea->SetSize ( vecSize );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetRadarAreaColor ( CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    assert ( pElement );
    RUN_CHILDREN SetRadarAreaColor ( *iter, ucRed, ucGreen, ucBlue, ucAlpha );

    if ( IS_RADAR_AREA ( pElement ) )
    {
        CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );
        pRadarArea->SetColor ( ucRed, ucGreen, ucBlue, ucAlpha );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetRadarAreaFlashing ( CElement* pElement, bool bFlashing )
{
    assert ( pElement );
    RUN_CHILDREN SetRadarAreaFlashing ( *iter, bFlashing );

    if ( IS_RADAR_AREA ( pElement ) )
    {
        CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );
        pRadarArea->SetFlashing ( bFlashing );
    }

    return true;
}


CPickup* CStaticFunctionDefinitions::CreatePickup ( CResource* pResource, const CVector& vecPosition, unsigned char ucType, double dFive, unsigned long ulRespawnInterval, double dSix )
{
    // Is the type armor or health?
    CPickup* pPickup = NULL;
    if ( ucType == CPickup::ARMOR || ucType == CPickup::HEALTH )
    {
        // Is the fifth argument (health) a number between 0 and 100?
        if ( dFive >= 0 && dFive <= 100 )
        {
            // Create the pickup
            //pPickup = m_pPickupManager->Create ( m_pMapManager->GetRootElement () );
			pPickup = m_pPickupManager->Create ( pResource->GetDynamicElementRoot() );
            if ( pPickup )
            {
                // Set the health/armor
                pPickup->SetAmount ( static_cast < float > ( dFive ) );
            }
        }
    }
    else if ( ucType == CPickup::WEAPON )
    {
        // Get the weapon id
        unsigned char ucWeaponID = static_cast < unsigned char > ( dFive );
        if ( CPickupManager::IsValidWeaponID ( ucWeaponID ) )
        {
            // Limit ammo to 9999
            unsigned short usAmmo = static_cast < unsigned short > ( dSix );
            if ( dSix > 9999 )
            {
                usAmmo = 9999;
            }

            // Create the pickup
            //pPickup = m_pPickupManager->Create ( m_pMapManager->GetRootElement () );
			pPickup = m_pPickupManager->Create ( pResource->GetDynamicElementRoot() );
            if ( pPickup )
            {
                // Set the weapon type and ammo
                pPickup->SetWeaponType ( ucWeaponID );
                pPickup->SetAmmo ( usAmmo );
            }
        }
    }
   else if ( ucType == CPickup::CUSTOM )
   {
        // Get the model id
        unsigned short usModel = static_cast < unsigned short > ( dFive );
        if ( CObjectManager::IsValidModel ( usModel ) )
        {
            // Create the pickup
            //pPickup = m_pPickupManager->Create ( m_pMapManager->GetRootElement () );
			pPickup = m_pPickupManager->Create ( pResource->GetDynamicElementRoot() );
            if ( pPickup )
            {
                // Set the model id
                pPickup->SetModel ( usModel );
            }
        }
    }

    // Got a pickup?
    if ( pPickup )
    {
        // Apply the position, type too and send it
        pPickup->SetPosition ( vecPosition );
        pPickup->SetPickupType ( ucType );
        pPickup->SetRespawnIntervals ( ulRespawnInterval );

		if ( pResource->HasStarted() )
		{
			// Tell the clients
			CEntityAddPacket Packet;
			Packet.Add ( pPickup );
			m_pPlayerManager->BroadcastOnlyJoined ( Packet );
		}
    }

    return pPickup;
}


bool CStaticFunctionDefinitions::GetPickupType ( CPickup* pPickup, unsigned char& ucType )
{
    assert ( pPickup );
    ucType = pPickup->GetPickupType ();
    return true;
}


bool CStaticFunctionDefinitions::GetPickupWeapon ( CPickup* pPickup, unsigned char& ucWeapon )
{
    assert ( pPickup );
    ucWeapon = pPickup->GetWeaponType ();
    return true;
}


bool CStaticFunctionDefinitions::GetPickupAmount ( CPickup* pPickup, float& fAmount )
{
    assert ( pPickup );
    fAmount = pPickup->GetAmount ();
    return true;
}


bool CStaticFunctionDefinitions::GetPickupAmmo ( CPickup* pPickup, unsigned short& usAmmo )
{
    assert ( pPickup );
    usAmmo = pPickup->GetAmmo ();
    return true;
}


bool CStaticFunctionDefinitions::GetPickupRespawnInterval ( CPickup* pPickup, unsigned long & ulInterval )
{
    assert ( pPickup );
    ulInterval = pPickup->GetRespawnIntervals ();
    return true;
}


bool CStaticFunctionDefinitions::IsPickupSpawned ( CPickup* pPickup, bool & bSpawned )
{
    assert ( pPickup );
    bSpawned = pPickup->IsSpawned ();
    return true;
}


bool CStaticFunctionDefinitions::SetPickupType ( CElement* pElement, unsigned char ucType, double dThree, double dFour )
{
    assert ( pElement );
    RUN_CHILDREN SetPickupType ( *iter, ucType, dThree, dFour );

    if ( IS_PICKUP ( pElement ) )
    {
        CPickup* pPickup = static_cast < CPickup* > ( pElement );

        // Is the type armor or health?
        if ( ucType == CPickup::ARMOR || ucType == CPickup::HEALTH )
        {
            // Is the third argument (health) a number between 0 and 100?
            if ( dThree >= 0 && dThree <= 100 )
            {
                pPickup->SetPickupType ( ucType );
                pPickup->SetAmount ( static_cast < float > ( dThree ) );

                // Tell all the players about the new type and weapontype
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPickup->GetID () );
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( static_cast < float > ( dThree ) );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PICKUP_TYPE, *BitStream.pBitStream ) );

                return true;
            }
        }
        else if ( ucType == CPickup::WEAPON )
        {
            // Get the weapon id
            unsigned char ucWeaponID = static_cast < unsigned char > ( dThree );
            if ( CPickupManager::IsValidWeaponID ( ucWeaponID ) )
            {
                // Limit ammo to 9999
                unsigned short usAmmo = static_cast < unsigned short > ( dFour );
                if ( dFour > 9999 )
                {
                    usAmmo = 9999;
                }

                // Set the type, weapon type and ammo
                pPickup->SetPickupType ( ucType );
                pPickup->SetWeaponType ( ucWeaponID );
                pPickup->SetAmmo ( usAmmo );

                // Tell all the players about the new type and weapontype
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPickup->GetID () );
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( ucWeaponID );
                BitStream.pBitStream->Write ( usAmmo );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PICKUP_TYPE, *BitStream.pBitStream ) );

                return true;
            }
        }
        else if ( ucType == CPickup::CUSTOM )
        {
            // Get the weapon id
            unsigned short usModel = static_cast < unsigned short > ( dThree );
            if ( CObjectManager::IsValidModel ( usModel ) )
            {
                // Set the type, weapon type and ammo
                pPickup->SetPickupType ( ucType );
                pPickup->SetModel ( usModel );

                // Tell all the players about the new type and weapontype
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pPickup->GetID () );
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( usModel );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PICKUP_TYPE, *BitStream.pBitStream ) );

                return true;
            }
        }

        return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetPickupRespawnInterval ( CElement * pElement, unsigned long ulInterval )
{
    assert ( pElement );
    RUN_CHILDREN SetPickupRespawnInterval ( *iter, ulInterval );

    if ( pElement->GetType () == CElement::PICKUP )
    {
        CPickup * pPickup = static_cast < CPickup * > ( pElement );
        pPickup->SetRespawnIntervals ( ulInterval );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::UsePickup ( CElement * pElement, CPlayer * pPlayer )
{
    assert ( pElement );
    assert ( pPlayer );
    RUN_CHILDREN UsePickup ( *iter, pPlayer );

    if ( pElement->GetType () == CElement::PICKUP )
    {
        CPickup * pPickup = static_cast < CPickup * > ( pElement );
        if ( pPickup->CanUse ( *pPlayer, false ) )
        {
            pPickup->Use ( *pPlayer );
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::CreateExplosion ( const CVector& vecPosition, unsigned char ucType, CElement* pElement )
{
	if ( pElement )
	{
		RUN_CHILDREN CreateExplosion ( vecPosition, ucType, *iter );
	}

	// Tell everyone
	CExplosionSyncPacket Packet ( vecPosition, ucType );
	if ( pElement && IS_PLAYER ( pElement ) )
	{
		CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
		Packet.SetSourceElement ( pPlayer );
	}
	m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	return true;
}


bool CStaticFunctionDefinitions::CreateFire ( const CVector& vecPosition, float fSize, CElement* pElement )
{
    if ( pElement )
	{
		RUN_CHILDREN CreateFire ( vecPosition, fSize, *iter );
	}

	// Tell everyone
	CFireSyncPacket Packet ( vecPosition, fSize );
	if ( pElement && IS_PLAYER ( pElement ) )
	{
		CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
		Packet.SetSourceElement ( pPlayer );
	}
	m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	return true;
}


bool CStaticFunctionDefinitions::PlaySoundFrontEnd ( CElement* pElement, unsigned long ulSound )
{
	assert ( pElement );
    RUN_CHILDREN PlaySoundFrontEnd ( *iter, ulSound );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

	    // Tell them to play a sound
	    CBitStream BitStream;
	    BitStream.pBitStream->Write ( (unsigned char) AUDIO_FRONTEND );
        BitStream.pBitStream->Write ( ulSound );
        pPlayer->Send ( CLuaPacket ( PLAY_SOUND, *BitStream.pBitStream ) );
    }

	return true;
}


bool CStaticFunctionDefinitions::PlayMissionAudio ( CElement* pElement, CVector* vecPosition, unsigned short usSlot )
{
    assert ( pElement );

    RUN_CHILDREN PlayMissionAudio ( *iter, vecPosition, usSlot );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

	    // Tell them to play a sound
	    CBitStream BitStream;
	    BitStream.pBitStream->Write ( (unsigned char) AUDIO_MISSION_PLAY );
        BitStream.pBitStream->Write ( usSlot );

	    pPlayer->Send ( CLuaPacket ( PLAY_SOUND, *BitStream.pBitStream ) );
    }

	return true;
}


bool CStaticFunctionDefinitions::PreloadMissionAudio ( CElement* pElement, unsigned short usSound, unsigned short usSlot )
{
    assert ( pElement );

    RUN_CHILDREN PreloadMissionAudio ( *iter, usSound, usSlot );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

	    // Tell them to play a sound
	    CBitStream BitStream;
	    BitStream.pBitStream->Write ( (unsigned char) AUDIO_MISSION_PRELOAD );
		BitStream.pBitStream->Write ( usSound );
        BitStream.pBitStream->Write ( usSlot );

	    pPlayer->Send ( CLuaPacket ( PLAY_SOUND, *BitStream.pBitStream ) );
    }

	return true;
}


bool CStaticFunctionDefinitions::BindKey ( CPlayer* pPlayer, const char* szKey, const char* szHitState, CLuaMain* pLuaMain, int iLuaFunction, CLuaArguments& Arguments )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( szHitState );
    assert ( pLuaMain );

    bool bSuccess = false;

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    bool bHitState = true;

    if ( stricmp ( szHitState, "down" ) == 0 || stricmp ( szHitState, "both" ) == 0 )
    {
        if ( ( pKey && pKeyBinds->AddKeyFunction ( pKey, bHitState, pLuaMain, iLuaFunction, Arguments ) ) ||
            ( pControl && pKeyBinds->AddControlFunction ( pControl, bHitState, pLuaMain, iLuaFunction, Arguments ) ) )
        {
            unsigned char ucKeyLength = static_cast < unsigned char > ( strlen ( szKey ) );

            CBitStream bitStream;
            bitStream.pBitStream->Write ( ucKeyLength );
            bitStream.pBitStream->Write ( const_cast < char* > ( szKey ), ucKeyLength );
            bitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bHitState ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( BIND_KEY, *bitStream.pBitStream ) );
            
            bSuccess = true;
        }
    }
    bHitState = false;
    if ( stricmp ( szHitState, "up" ) == 0 || stricmp ( szHitState, "both" ) == 0 )
    {
        if ( ( pKey && pKeyBinds->AddKeyFunction ( pKey, bHitState, pLuaMain, iLuaFunction, Arguments ) ) ||
            ( pControl && pKeyBinds->AddControlFunction ( pControl, bHitState, pLuaMain, iLuaFunction, Arguments ) ) )
        {
            unsigned char ucKeyLength = static_cast < unsigned char > ( strlen ( szKey ) );

            CBitStream bitStream;
            bitStream.pBitStream->Write ( ucKeyLength );
            bitStream.pBitStream->Write ( const_cast < char* > ( szKey ), ucKeyLength );
            bitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bHitState ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( BIND_KEY, *bitStream.pBitStream ) );
            
            bSuccess = true;
        }
    }

    return bSuccess;
}


bool CStaticFunctionDefinitions::UnbindKey ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, int iLuaFunction )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( pLuaMain );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    bool bCheckHitState = false, bHitState = true;
    if ( szHitState )
    {
        if ( !stricmp ( szHitState, "down" ) )
        {
            bCheckHitState = true, bHitState = true;
        }
        else if ( !stricmp ( szHitState, "up" ) )
        {
            bCheckHitState = true, bHitState = false;
        }
    }
    /* If we have a key or control, removed the bind and dont have ANY other binds to this key,
       remove it */
    bool bSuccess = false;
    if ( ( pKey &&
           ( bSuccess = pKeyBinds->RemoveKeyFunction ( szKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ) &&
           !pKeyBinds->KeyFunctionExists ( szKey, NULL, bCheckHitState, bHitState ) ) ||
         ( pControl &&
           ( bSuccess = pKeyBinds->RemoveControlFunction ( szKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ) &&
           !pKeyBinds->ControlFunctionExists ( szKey, NULL, bCheckHitState, bHitState ) ) )
    {    

        unsigned char ucKeyLength = static_cast < unsigned char > ( strlen ( szKey ) );

        CBitStream bitStream;
        bitStream.pBitStream->Write ( ucKeyLength );
        bitStream.pBitStream->Write ( const_cast < char* > ( szKey ), ucKeyLength );
        bitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bHitState ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( UNBIND_KEY, *bitStream.pBitStream ) );
    }

    return bSuccess;
}


bool CStaticFunctionDefinitions::IsKeyBound ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, int iLuaFunction, bool& bBound )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( pLuaMain );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    bool bCheckHitState = false, bHitState = true;
    if ( szHitState )
    {
        if ( stricmp ( szHitState, "down" ) == 0 )
        {
            bCheckHitState = true, bHitState = true;
        }
        else if ( stricmp ( szHitState, "up" ) == 0 )
        {
            bCheckHitState = true, bHitState = false;
        }
    }
    if ( pKey || pControl )
    {
        if ( ( pKey && pKeyBinds->KeyFunctionExists ( szKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ) ||
            ( pControl && pKeyBinds->ControlFunctionExists ( szKey, pLuaMain, bCheckHitState, bHitState, iLuaFunction ) ) )
        {
            bBound = true;
            
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetControlState ( CPlayer* pPlayer, char* szControl, bool& bState )
{
    assert ( pPlayer );
    assert ( szControl );

    return ( pPlayer->GetPad ()->GetControlState ( szControl, bState ) );
}


bool CStaticFunctionDefinitions::IsControlEnabled ( CPlayer* pPlayer, char* szControl, bool& bEnabled )
{
    assert ( pPlayer );
    assert ( szControl );

    return ( pPlayer->GetPad ()->IsControlEnabled ( szControl, bEnabled ) );
}    


bool CStaticFunctionDefinitions::SetControlState ( CPlayer* pPlayer, char* szControl, bool bState )
{
    assert ( pPlayer );
    assert ( szControl );

    unsigned int uiControlLength = strlen ( szControl );
    if ( uiControlLength < 256 )
    {
        if ( pPlayer->GetPad ()->SetControlState ( szControl, bState ) )
        {
            CBitStream BitStream;
            unsigned char ucControlLength = static_cast < unsigned char > ( uiControlLength );
            BitStream.pBitStream->Write ( ucControlLength );
            BitStream.pBitStream->Write ( szControl, ucControlLength );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bState ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( SET_CONTROL_STATE, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::ToggleControl ( CPlayer* pPlayer, char* szControl, bool bEnabled )
{
    assert ( pPlayer );
    assert ( szControl );

    unsigned int uiControlLength = strlen ( szControl );
    if ( uiControlLength < 256 )
    {
        // Jax: removed check between bEnabled and currently-enabled, as it can be changed client-side
        if ( pPlayer->GetPad ()->SetControlEnabled ( szControl, bEnabled ) )
        {
            CBitStream BitStream;
            unsigned char ucControlLength = static_cast < unsigned char > ( uiControlLength );
            BitStream.pBitStream->Write ( ucControlLength );
            BitStream.pBitStream->Write ( szControl, ucControlLength );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bEnabled ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( TOGGLE_CONTROL_ABILITY, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::ToggleAllControls ( CPlayer* pPlayer, bool bGTAControls, bool bMTAControls, bool bEnabled )
{
    assert ( pPlayer );

    if ( bGTAControls )
        pPlayer->GetPad ()->SetAllControlsEnabled ( bEnabled );

    // TODO: Add mta controls to CPad

    CBitStream BitStream;
    BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bGTAControls ) ? 1 : 0 ) );
    BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bMTAControls ) ? 1 : 0 ) );
    BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bEnabled ) ? 1 : 0 ) );
    pPlayer->Send ( CLuaPacket ( TOGGLE_ALL_CONTROL_ABILITY, *BitStream.pBitStream ) );

    return true;
}


CTeam* CStaticFunctionDefinitions::CreateTeam ( CResource* pResource, char* szTeamName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    assert ( szTeamName );

    // If it already exists, do nothing
    CTeam* pTeam = m_pTeamManager->GetTeam ( szTeamName );
    if ( pTeam )
        return NULL;

    // Create the new team
    //pTeam = new CTeam ( m_pTeamManager, m_pMapManager->GetRootElement (), NULL, szTeamName, ucRed, ucGreen, ucBlue );
	pTeam = new CTeam ( m_pTeamManager, pResource->GetDynamicElementRoot(), NULL, szTeamName, ucRed, ucGreen, ucBlue );

    // Tell everyone to add this team
	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pTeam );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pTeam;
}


CTeam* CStaticFunctionDefinitions::GetTeamFromName ( const char* szTeamName )
{
    assert ( szTeamName );

    CTeam* pTeam = m_pTeamManager->GetTeam ( szTeamName );
    return pTeam;
}


bool CStaticFunctionDefinitions::GetTeamName ( CTeam* pTeam, char* szBuffer, unsigned int uiBufferLength )
{
    assert ( pTeam );
    assert ( szBuffer );
    assert ( uiBufferLength );

    strncpy ( szBuffer, pTeam->GetTeamName (), uiBufferLength );
    szBuffer [ uiBufferLength - 1 ] = 0;
    return true;
}


bool CStaticFunctionDefinitions::GetTeamColor ( CTeam* pTeam, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue )
{
    assert ( pTeam );

    pTeam->GetColor ( ucRed, ucGreen, ucBlue );
    return true;
}


bool CStaticFunctionDefinitions::CountPlayersInTeam ( CTeam* pTeam, unsigned int& uiCount )
{
    assert ( pTeam );

    uiCount = pTeam->CountPlayers ();
    return true;
}


bool CStaticFunctionDefinitions::GetTeamFriendlyFire ( CTeam* pTeam, bool& bFriendlyFire )
{
    assert ( pTeam );

    bFriendlyFire = pTeam->GetFriendlyFire ();
    return true;
}


bool CStaticFunctionDefinitions::SetTeamName ( CTeam* pTeam, char* szTeamName )
{
    assert ( pTeam );
    assert ( szTeamName );

    // If its a valid name
    if ( szTeamName [ 0 ] != 0 )
    {
        // Change it
        pTeam->SetTeamName ( szTeamName );

        // Tell everyone the new team name
        CBitStream BitStream;
        unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szTeamName ) );
        BitStream.pBitStream->Write ( pTeam->GetID () );
        BitStream.pBitStream->Write ( usNameLength );
        BitStream.pBitStream->Write ( szTeamName, usNameLength );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_TEAM_NAME, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetTeamColor ( CTeam* pTeam, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    assert ( pTeam );

    unsigned char ucCurrentRed, ucCurrentGreen, ucCurrentBlue;
    pTeam->GetColor ( ucCurrentRed, ucCurrentGreen, ucCurrentBlue );

    if ( ucRed != ucCurrentRed || ucGreen != ucCurrentGreen || ucBlue != ucCurrentBlue )
    {
        pTeam->SetColor ( ucRed, ucGreen, ucBlue );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pTeam->GetID () );
        BitStream.pBitStream->Write ( ucRed );
        BitStream.pBitStream->Write ( ucGreen );
        BitStream.pBitStream->Write ( ucBlue );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_TEAM_COLOR, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPlayerTeam ( CPlayer* pPlayer, CTeam* pTeam )
{
    assert ( pPlayer );

    // If its a different team
    if ( pTeam != pPlayer->GetTeam () )
    {
        // Change his team
        pPlayer->SetTeam ( pTeam, true );

        ElementID TeamID = INVALID_ELEMENT_ID;
        if ( pTeam )
            TeamID = pTeam->GetID ();

        // Tell everyone his new team
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pPlayer->GetID () );
        BitStream.pBitStream->Write ( TeamID );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_PLAYER_TEAM, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetTeamFriendlyFire ( CTeam* pTeam, bool bFriendlyFire )
{
    assert ( pTeam );

    if ( pTeam->GetFriendlyFire () != bFriendlyFire )
    {
        pTeam->SetFriendlyFire ( bFriendlyFire );

        // Tell everyone the new setting
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pTeam->GetID () );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bFriendlyFire ) ? 1 : 0 ) );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_TEAM_FRIENDLY_FIRE, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


CColCircle* CStaticFunctionDefinitions::CreateColCircle ( CResource* pResource, const CVector& vecPosition, float fRadius )
{
    //CColCircle * pColShape = new CColCircle ( m_pColManager, m_pMapManager->GetRootElement (), vecPosition, fRadius );
	CColCircle * pColShape = new CColCircle ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, fRadius );
    
    // Run collision detection on some elements
    list < CPlayer * > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd () ; iterPlayers++ )
    {
        CPlayer * pPlayer = *iterPlayers;
        m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer, pColShape );
    }
    list < CVehicle * > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd () ; iterVehicles++ )
    {
        CVehicle * pVehicle = *iterVehicles;
        m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle, pColShape );
    }

	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pColShape );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pColShape;
}


CColCuboid* CStaticFunctionDefinitions::CreateColCuboid ( CResource* pResource, const CVector& vecPosition, const CVector& vecSize )
{
    //CColCuboid * pColShape = new CColCuboid ( m_pColManager, m_pMapManager->GetRootElement (), vecPosition, vecSize );
	CColCuboid * pColShape = new CColCuboid ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, vecSize );
    
    // Run collision detection on some elements
    list < CPlayer * > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd () ; iterPlayers++ )
    {
        CPlayer * pPlayer = *iterPlayers;
        m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer, pColShape );
    }
    list < CVehicle * > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd () ; iterVehicles++ )
    {
        CVehicle * pVehicle = *iterVehicles;
        m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle, pColShape );
    }

	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pColShape );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pColShape;
}


CColSphere* CStaticFunctionDefinitions::CreateColSphere ( CResource* pResource, const CVector& vecPosition, float fRadius )
{
    //CColSphere * pColShape = new CColSphere ( m_pColManager, m_pMapManager->GetRootElement (), vecPosition, fRadius );
	CColSphere * pColShape = new CColSphere ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, fRadius );
    
    // Run collision detection on some elements
    list < CPlayer * > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd () ; iterPlayers++ )
    {
        CPlayer * pPlayer = *iterPlayers;
        m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer, pColShape );
    }
    list < CVehicle * > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd () ; iterVehicles++ )
    {
        CVehicle * pVehicle = *iterVehicles;
        m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle, pColShape );
    }

	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pColShape );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pColShape;
}


CColRectangle* CStaticFunctionDefinitions::CreateColRectangle ( CResource* pResource, const CVector& vecPosition, const CVector2D& vecSize )
{
	//CColRectangle * pColShape = new CColRectangle ( m_pColManager, m_pMapManager->GetRootElement(), vecPosition, vecSize );
    CColRectangle * pColShape = new CColRectangle ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, vecSize );
    
    // Run collision detection on some elements
    list < CPlayer * > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd () ; iterPlayers++ )
    {
        CPlayer * pPlayer = *iterPlayers;
        m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer, pColShape );
    }
    list < CVehicle * > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd () ; iterVehicles++ )
    {
        CVehicle * pVehicle = *iterVehicles;
        m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle, pColShape );
    }

	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pColShape );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pColShape;
}


CColTube* CStaticFunctionDefinitions::CreateColTube ( CResource* pResource, const CVector& vecPosition, float fRadius, float fHeight )
{
    //CColTube * pColShape = new CColTube ( m_pColManager, m_pMapManager->GetRootElement (), vecPosition, fRadius, fHeight );
	CColTube * pColShape = new CColTube ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, fRadius, fHeight );
    
    // Run collision detection on some elements
    list < CPlayer * > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd () ; iterPlayers++ )
    {
        CPlayer * pPlayer = *iterPlayers;
        m_pColManager->DoHitDetection ( pPlayer->GetLastPosition (), pPlayer->GetPosition (), 0.0f, pPlayer, pColShape );
    }
    list < CVehicle * > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd () ; iterVehicles++ )
    {
        CVehicle * pVehicle = *iterVehicles;
        m_pColManager->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle, pColShape );
    }

	if ( pResource->HasStarted() )
	{
		CEntityAddPacket Packet;
		Packet.Add ( pColShape );
		m_pPlayerManager->BroadcastOnlyJoined ( Packet );
	}

    return pColShape;
}


bool CStaticFunctionDefinitions::GetWeaponNameFromID ( unsigned char ucID, char* szName )
{
    if ( ucID < NUMBER_OF_WEAPON_NAMES )
    {
        // Grab the name and check it's length
        const char* szNamePointer = CWeaponNames::GetWeaponName ( ucID );
        if ( strlen ( szNamePointer ) > 0 )
        {
            strncpy ( szName, szNamePointer, 256 );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetWeaponIDFromName ( const char* szName, unsigned char& ucID )
{
    // Grab the weapon id
    ucID = CWeaponNames::GetWeaponID ( szName );
    return ucID != 0xFF;
}


bool CStaticFunctionDefinitions::GetBodyPartName ( unsigned char ucID, char* szName )
{
    if ( ucID <= 59 )
    {
        // Grab the name and check it's length
        const char* szNamePointer = CPlayer::GetBodyPartName ( ucID );
        if ( strlen ( szNamePointer ) > 0 )
        {
            strncpy ( szName, szNamePointer, 256 );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetClothesByTypeIndex ( unsigned char ucType, unsigned char ucIndex, char* szTextureReturn, char* szModelReturn )
{
    SPlayerClothing* pPlayerClothing = CPlayerClothes::GetClothingGroup ( ucType );
    if ( pPlayerClothing )
    {
        if ( ucIndex < CPlayerClothes::GetClothingGroupMax ( ucType ) )
        {
            if ( szTextureReturn )
                strcpy ( szTextureReturn, pPlayerClothing [ ucIndex ].szTexture );
            if ( szModelReturn )
                strcpy ( szModelReturn, pPlayerClothing [ ucIndex ].szModel );
            
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetTypeIndexFromClothes ( char* szTexture, char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn )
{
    if ( szTexture == NULL && szModel == NULL )
        return false;

    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
    {
        SPlayerClothing* pPlayerClothing = CPlayerClothes::GetClothingGroup ( ucType );
        if ( pPlayerClothing )
        {
            for ( unsigned char ucIter = 0 ; pPlayerClothing [ ucIter ].szTexture != NULL ; ucIter++ )
            {
                if ( ( szTexture == NULL || strcmp ( szTexture, pPlayerClothing [ ucIter ].szTexture ) == 0 ) &&
                     ( szModel == NULL || strcmp ( szModel, pPlayerClothing [ ucIter ].szModel ) == 0 ) )
                {
                    ucTypeReturn = ucType;
                    ucIndexReturn = ucIter;

                    return true;
                }
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetClothesTypeName ( unsigned char ucType, char* szNameReturn )
{
    assert ( szNameReturn );

    char* szName = CPlayerClothes::GetClothingName ( ucType );
    if ( szName )
    {
        strcpy ( szNameReturn, szName );

        return true;
    }

    return false;
}


unsigned char CStaticFunctionDefinitions::GetMaxPlayers ( void )
{
    return static_cast < unsigned char > ( m_pMainConfig->GetMaxPlayers () );
}


bool CStaticFunctionDefinitions::OutputChatBox ( const char* szText, CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded )
{
    assert ( pElement );
    assert ( szText );
    RUN_CHILDREN OutputChatBox ( szText, *iter, ucRed, ucGreen, ucBlue, bColorCoded );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        pPlayer->Send ( CChatEchoPacket ( szText, ucRed, ucGreen, ucBlue, bColorCoded ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::OutputConsole ( const char* szText, CElement* pElement )
{
    assert ( pElement );
    assert ( szText );
    RUN_CHILDREN OutputConsole ( szText, *iter );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        pPlayer->Send ( CConsoleEchoPacket ( szText ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::GetTime ( unsigned char& ucHour, unsigned char& ucMinute )
{
    m_pClock->Get ( ucHour, ucMinute );
    return true;
}


bool CStaticFunctionDefinitions::GetWeather ( unsigned char& ucWeather, unsigned char& ucWeatherBlendingTo )
{
    // Grab the current weather
    CBlendedWeather* pWeather = m_pMapManager->GetWeather ();
    ucWeather = pWeather->GetWeather ();

    // If we're blending to some weather, also return which. Otherwize just return 0xFF which will return false
    if ( pWeather->GetBlendedStopHour () != 0xFF )
        ucWeatherBlendingTo = pWeather->GetWeatherBlendingTo ();
    else
        ucWeatherBlendingTo = 0xFF;

    return true;
}


bool CStaticFunctionDefinitions::GetZoneName ( CVector& vecPosition, char* szBuffer, unsigned int uiBufferLength, bool bCitiesOnly )
{
    assert ( szBuffer );
    assert ( uiBufferLength );

    const char* szZone = NULL;
    if ( bCitiesOnly )
        szZone = g_pGame->GetZoneNames ()->GetCityName ( vecPosition );
    else
        szZone = g_pGame->GetZoneNames ()->GetZoneName ( vecPosition );

    strncpy ( szBuffer, szZone, uiBufferLength );
    szBuffer [ uiBufferLength - 1 ] = 0;
    return true;
}


bool CStaticFunctionDefinitions::GetGravity ( float& fGravity )
{
    fGravity = g_pGame->GetGravity ();
    return true;
}


bool CStaticFunctionDefinitions::GetGameSpeed ( float& fSpeed )
{
    fSpeed = g_pGame->GetGameSpeed ();
    return true;
}


bool CStaticFunctionDefinitions::GetWaveHeight ( float& fHeight )
{
    fHeight = g_pGame->GetWaveHeight ();
    return true;
}


bool CStaticFunctionDefinitions::GetFPSLimit ( unsigned short& usLimit )
{
	usLimit = g_pGame->GetConfig()->GetFPSLimit();
    return true;
}


bool CStaticFunctionDefinitions::GetMinuteDuration ( unsigned long& ulDuration )
{
	CClock* pClock = m_pMapManager->GetServerClock ();

	ulDuration = pClock->GetMinuteDuration ();

    return true;
}

bool CStaticFunctionDefinitions::IsGarageOpen ( unsigned char ucGarageID, bool& bIsOpen )
{
    if ( ucGarageID >= 0 && ucGarageID < MAX_GARAGES )
    {
        bool* pbGarageStates = g_pGame->GetGarageStates();
        bIsOpen = pbGarageStates[ ucGarageID ];
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetTime ( unsigned char ucHour, unsigned char ucMinute )
{
    // Verify the range
    if ( ucHour < 24 && ucMinute < 60 )
    {
        // Set the time
        m_pClock->Set ( ucHour, ucMinute );

        // Tell all the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucHour );
        BitStream.pBitStream->Write ( ucMinute );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_TIME, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWeather ( unsigned char ucWeather )
{
    // Verify it's within the max valid weather id
    if ( ucWeather <= MAX_VALID_WEATHER )
    {
        // Set the weather
        m_pMapManager->GetWeather ()->SetWeather ( ucWeather );

        // Tell all the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucWeather );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEATHER, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWeatherBlended ( unsigned char ucWeather )
{
    // Verify it's within the max valid weather id
    if ( ucWeather <= MAX_VALID_WEATHER )
    {
        CBlendedWeather* pWeather = m_pMapManager->GetWeather ();
        CClock* pClock = m_pMapManager->GetServerClock ();

        // Grab the next hour
        unsigned char ucHour;
        unsigned char ucMinute;
        m_pClock->Get ( ucHour, ucMinute );
        ++ucHour;
        if ( ucHour > 23 ) ucHour = 0;

        // Set the weather blended
        pWeather->SetWeatherBlended ( ucWeather, ucHour );

        // Tell all the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucWeather );
        BitStream.pBitStream->Write ( ucHour );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEATHER_BLENDED, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetGravity ( float fGravity )
{
    if ( fGravity >= MIN_GRAVITY && fGravity <= MAX_GRAVITY )
    {
        g_pGame->SetGravity ( fGravity );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fGravity );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_GRAVITY, *BitStream.pBitStream ) );
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetGameSpeed ( float fSpeed )
{
    if ( fSpeed >= 0.0f && fSpeed <= 10.0f )
    {
        g_pGame->SetGameSpeed ( fSpeed );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fSpeed );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_GAME_SPEED, *BitStream.pBitStream ) );
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWaveHeight ( float fHeight )
{
    if ( fHeight >= -1.0f && fHeight <= 100.0f )
    {
        g_pGame->SetWaveHeight ( fHeight );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fHeight );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WAVE_HEIGHT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetFPSLimit ( unsigned short usLimit )
{
    if ( usLimit >= 25 && usLimit <= 100 )
    {
		g_pGame->GetConfig()->SetFPSLimit ( usLimit );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( (short)usLimit );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_FPS_LIMIT, *BitStream.pBitStream ) );
        
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMinuteDuration ( unsigned long ulDuration )
{
    if ( ulDuration > 0 && ulDuration <= ULONG_MAX )
    {
		CClock* pClock = m_pMapManager->GetServerClock ();

		pClock->SetMinuteDuration ( ulDuration );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ulDuration );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_MINUTE_DURATION, *BitStream.pBitStream ) );
        
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetGarageOpen ( unsigned char ucGarageID, bool bIsOpen )
{
    if ( ucGarageID >= 0 && ucGarageID < MAX_GARAGES )
    {
        bool* pbGarageStates = g_pGame->GetGarageStates();
        pbGarageStates [ ucGarageID ] = bIsOpen;

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucGarageID );
        BitStream.pBitStream->Write ( bIsOpen );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_GARAGE_OPEN, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetSkyGradient ( unsigned char ucTopRed, unsigned char ucTopGreen, unsigned char ucTopBlue, unsigned char ucBottomRed, unsigned char ucBottomGreen, unsigned char ucBottomBlue )
{
    g_pGame->SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );
    g_pGame->SetHasSkyGradient ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( ucTopRed );
    BitStream.pBitStream->Write ( ucTopGreen );
    BitStream.pBitStream->Write ( ucTopBlue );
    BitStream.pBitStream->Write ( ucBottomRed );
    BitStream.pBitStream->Write ( ucBottomGreen );
    BitStream.pBitStream->Write ( ucBottomBlue );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_SKY_GRADIENT, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::ResetSkyGradient ( void )
{
    g_pGame->SetHasSkyGradient ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_SKY_GRADIENT, *BitStream.pBitStream ) );
    return true;
}


CElement* CStaticFunctionDefinitions::GetRootElement ( void )
{
    return m_pMapManager->GetRootElement ();
}


 CElement* CStaticFunctionDefinitions::LoadMapData ( CLuaMain* pLuaMain, CElement* pParent, CXMLNode* pNode )
{
    assert ( pLuaMain );
    assert ( pParent );
    assert ( pNode );

    // Grab the VM's resource
    CResource* pResource = pLuaMain->GetResource ();
    if ( pResource )
    {
        // Load the map data
        return m_pMapManager->LoadMapData ( *pResource, *pParent, *pNode );
    }

    // Failed
    return NULL;
}

 CXMLNode* CStaticFunctionDefinitions::SaveMapData ( CElement* pElement, CXMLNode* pNode, bool bChildren )
{
    assert ( pElement );
    assert ( pNode );

    // Make sure it has no subnodes already
    pNode->DeleteAllSubNodes();

	if ( bChildren )
	{
		list < CElement* > ::const_iterator iter = pElement->IterBegin();
		for ( ; iter != pElement->IterEnd(); iter++ )
		{
			(*iter)->OutputToXML ( pNode );
		}
		return pNode;
	}
    // Write it out to XML
    return pElement->OutputToXML ( pNode );
}


bool CStaticFunctionDefinitions::SetGameType ( const char * szGameType )
{
    ASE * ase = ASE::GetInstance();

    // Check the validity of the CGame pointer.
    if ( ase )
    {
        // New text?
        if ( szGameType && szGameType [0] != '\0' )
        {
            char* szTemp = new char [ strlen ( szGameType ) + 10 ];
            sprintf ( szTemp, "MTA:SA: %s", szGameType );
            ase->SetGameType ( szTemp );
            delete [] szTemp;
            return true;
        }
        else
        {
            char szTemp [32];
            strcpy ( szTemp, "MTA:SA" );
            ase->SetGameType ( szTemp );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMapName ( const char * szMapName )
{
    // Grab ase
    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        if ( szMapName )
        {
            ase->SetMapName ( szMapName );
        }
        else
        {
            ase->SetMapName ( "" );
        }

        return true;
    }

    return false;
}


char* CStaticFunctionDefinitions::GetRuleValue ( char* szKey )
{
    assert ( szKey );

    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        return ase->GetRuleValue ( szKey );
    }
    return NULL;
}


bool CStaticFunctionDefinitions::SetRuleValue ( char* szKey, char* szValue )
{
    assert ( szKey );

    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        ase->SetRuleValue ( szKey, szValue );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveRuleValue ( char* szKey )
{
    assert ( szKey );

    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        return ase->RemoveRuleValue ( szKey );
    }
    return false;
}


void CStaticFunctionDefinitions::ExecuteSQLCreateTable ( std::string strTable, std::string strDefinition )
{
	m_pRegistry->CreateTable ( strTable, strDefinition );
}


bool CStaticFunctionDefinitions::ExecuteSQLQuery ( std::string strQuery, CLuaArguments *pArgs, CRegistryResult* pResult )
{
	return m_pRegistry->Query ( strQuery, pArgs, pResult );
}

const std::string& CStaticFunctionDefinitions::SQLGetLastError ( void ) {
	return m_pRegistry->GetLastError ();
}

void CStaticFunctionDefinitions::ExecuteSQLDropTable ( std::string strTable )
{
	m_pRegistry->DropTable ( strTable );
}


bool CStaticFunctionDefinitions::ExecuteSQLDelete ( std::string strTable, std::string strWhere )
{
	return m_pRegistry->Delete ( strTable, strWhere );
}


bool CStaticFunctionDefinitions::ExecuteSQLInsert ( std::string strTable, std::string strValues, std::string strColumns )
{
	return m_pRegistry->Insert ( strTable, strValues, strColumns );
}


bool CStaticFunctionDefinitions::ExecuteSQLSelect ( std::string strTable, std::string strColumns, std::string strWhere, unsigned int uiLimit, CRegistryResult* pResult )
{
	return m_pRegistry->Select ( strColumns, strTable, strWhere, uiLimit, pResult );
}


bool CStaticFunctionDefinitions::ExecuteSQLUpdate ( std::string strTable, std::string strSet, std::string strWhere )
{
	return m_pRegistry->Update ( strTable, strSet, strWhere );
}


CClient* CStaticFunctionDefinitions::GetAccountClient ( CAccount* pAccount )
{
    assert ( pAccount );

    return pAccount->GetClient ();
}


bool CStaticFunctionDefinitions::IsGuestAccount ( CAccount* pAccount, bool& bGuest )
{
    assert ( pAccount );

    bGuest = ( pAccount->IsRegistered () == false );    
    return true;
}


CLuaArgument* CStaticFunctionDefinitions::GetAccountData ( CAccount* pAccount, char* szKey )
{
    assert ( pAccount );
    assert ( szKey );

    return pAccount->GetData ( szKey );
}


CAccount* CStaticFunctionDefinitions::AddAccount ( const char* szName, const char* szPassword )
{
    assert ( szName );
    assert ( szPassword );

    CAccount* pCurrentAccount = m_pAccountManager->Get ( szName );
    if ( pCurrentAccount == NULL )
    {
        CAccount* pAccount = new CAccount ( m_pAccountManager, true, szName );
		pAccount->SetPassword ( szPassword );
        return pAccount;
    }
    return NULL;
}

CAccount* CStaticFunctionDefinitions::GetAccount ( const char* szName, const char * szPassword )
{
    assert ( szName );

    CAccount* pCurrentAccount = m_pAccountManager->Get ( szName );
	if ( pCurrentAccount && ( !szPassword || pCurrentAccount->IsPassword ( szPassword ) ) )
		return pCurrentAccount;
	else
		return NULL;
}

bool CStaticFunctionDefinitions::GetAccounts ( CLuaMain* pLuaMain )
{
    lua_State* pLua = pLuaMain->GetVM();
    list < CAccount* > ::const_iterator iter = m_pAccountManager->IterBegin();
    unsigned int uiIndex = 0;
    unsigned int uiGuest = HashString ( "guest" );
    unsigned int uiHTTPGuest = HashString ( "http_guest" );
    unsigned int uiConsole = HashString ( "Console" );
    for ( ; iter != m_pAccountManager->IterEnd(); iter++ )
    {
        if ( ( (*iter)->GetNameHash() != uiGuest ) &&
             ( (*iter)->GetNameHash() != uiHTTPGuest ) &&
             ( (*iter)->GetNameHash() != uiConsole ) )
        {
            lua_pushnumber ( pLua, ++uiIndex );
            lua_pushaccount ( pLua, *iter );
            lua_settable ( pLua, -3 );
        }
    }
    return true;
}

bool CStaticFunctionDefinitions::RemoveAccount ( CAccount* pAccount )
{
    assert ( pAccount );

    if ( pAccount->IsRegistered () )
    {
        CClient* pClient = pAccount->GetClient ();
        if ( pClient )
        {
            CAccount* pGuestAccount = new CAccount ( m_pAccountManager, false, "guest" );
            pClient->SetAccount ( pGuestAccount );
        }
        delete pAccount;
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetAccountPassword ( CAccount* pAccount, const char* szPassword )
{
    assert ( pAccount );
    assert ( szPassword );

    if ( pAccount->IsRegistered () )
    {
        if ( strcmp ( pAccount->GetPassword ().c_str (), szPassword ) )
        {
            pAccount->SetPassword ( szPassword );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetAccountData ( CAccount* pAccount, char* szKey, CLuaArgument * pArgument )
{
    assert ( pAccount );
    assert ( szKey );

    if ( pArgument->GetType () != LUA_TNONE )
    {
        if ( pArgument->GetType () == LUA_TBOOLEAN && !pArgument->GetBoolean () )
        {
            pAccount->RemoveData ( szKey );
        }
        else
        {
            pAccount->SetData ( szKey, pArgument );
        }
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::CopyAccountData ( CAccount* pAccount, CAccount* pFromAccount )
{
    assert ( pAccount );
    assert ( pFromAccount );

    pAccount->CopyData ( pFromAccount );
    return true;
}


bool CStaticFunctionDefinitions::LogIn ( CPlayer* pPlayer, CAccount* pAccount, const char* szPassword )
{
    // Not already logged in?
    if ( !pAccount->GetClient () &&
         !pPlayer->IsRegistered () )
    {
        // Compare the passwords
        if ( szPassword && pAccount->IsPassword ( szPassword ) )
        {
            // Log him in
            m_pAccountManager->LogIn ( pPlayer, NULL, pAccount );
            return true;
        }
    }

    // Failed
    return false;
}


bool CStaticFunctionDefinitions::LogOut ( CPlayer* pPlayer )
{
    // Is he logged in?
    if ( pPlayer->IsRegistered () )
    {
        CAccount* pCurrentAccount = pPlayer->GetAccount ();
        pCurrentAccount->SetClient ( NULL );
        // TODO: Fix memoryleak
        CAccount* pAccount = new CAccount ( g_pGame->GetAccountManager (), false, "guest" );
        pPlayer->SetAccount ( pAccount );

        // Grab the sender's nick
        const char* szNick = pPlayer->GetNick ();
        if ( szNick )
        {
            // Tell the console
            CLogger::LogPrintf ( "LOGOUT: %s logged out\n", szNick );
        }

        // Call our script event
        CLuaArguments Arguments;
        Arguments.PushAccount ( pCurrentAccount );
        Arguments.PushAccount ( pAccount );
        pPlayer->CallEvent ( "onClientLogout", Arguments );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::KickPlayer ( CPlayer* pPlayer, CPlayer* pResponsible, const char* szReason )
{
    assert ( pPlayer );

    const char* szResponsible = "Console";
    if ( pResponsible )
        szResponsible = pResponsible->GetNick ();


    const char* szReasonPointer = NULL;
    char szMessage [256];
    szMessage [0] = '\0';

    // Got any reason?
    if ( szReason )
    {
        size_t sizeReason = strlen ( szReason );
        if ( sizeReason >= MIN_KICK_REASON_LENGTH && sizeReason <= MAX_KICK_REASON_LENGTH )
        {
            _snprintf ( szMessage, 256, "You were kicked by %s (%s)", szResponsible, szReason );
            szReasonPointer = szReason;
        }
        else
        {
            _snprintf ( szMessage, 256, "You were kicked by %s", szResponsible );
        }
    }
    else
    {
        _snprintf ( szMessage, 256, "You were kicked by %s", szResponsible );
    }
    szMessage[255] = '\0';

    // Make a message to send to everyone else
    char szInfoMessage [256];
    if ( szReasonPointer )
    {
        _snprintf ( szInfoMessage, 256, "%s was kicked from the game by %s (%s)", pPlayer->GetNick (), szResponsible, szReasonPointer );
    }
    else
    {
        _snprintf ( szInfoMessage, 256, "%s was kicked from the game by %s", pPlayer->GetNick (), szResponsible );
    }
    szInfoMessage [255] = 0;

    // Tell the player that was kicked why. QuitPlayer will delete the player.
    pPlayer->Send ( CPlayerDisconnectedPacket ( szMessage ) );
    g_pGame->QuitPlayer ( *pPlayer, CClient::QUIT_KICK, false );

    // Tell everyone else that he was kicked from the game including console
    // m_pPlayerManager->BroadcastOnlyJoined ( CChatEchoPacket ( szInfoMessage, CHATCOLOR_INFO ) );
    CLogger::LogPrintf ( "KICK: %s\n", szInfoMessage );

    return true;
}


bool CStaticFunctionDefinitions::BanPlayer ( CPlayer* pPlayer, CPlayer* pResponsible, const char* szReason )
{
    assert ( pPlayer );

    const char* szResponsible = "Console";
    if ( pResponsible )
        szResponsible = pResponsible->GetNick ();

    const char* szReasonPointer = NULL;
    char szMessage [256];
    szMessage [255] = 0;

    // Got any reason?
    if ( szReason )
    {
        size_t sizeReason = strlen ( szReason );
        if ( sizeReason >= MIN_BAN_REASON_LENGTH && sizeReason <= MAX_BAN_REASON_LENGTH )
        {
            _snprintf ( szMessage, 255, "You were banned by %s (%s)", szResponsible, szReason );
            szReasonPointer = szReason;
        }
        else
        {
            _snprintf ( szMessage, 255, "You were banned by %s", szResponsible );
        }
    }
    else
    {
        _snprintf ( szMessage, 255, "You were banned by %s", szResponsible );
    }

    // Make a message to send to everyone else
    char szInfoMessage [256];
    szInfoMessage [255] = 0;
    if ( szReasonPointer )
    {
        _snprintf ( szInfoMessage, 255, "%s was banned from the game by %s (%s)", pPlayer->GetNick (), szResponsible, szReasonPointer );
    }
    else
    {
        _snprintf ( szInfoMessage, 255, "%s was banned from the game by %s", pPlayer->GetNick (), szResponsible );
    }

    // Ban the player
    m_pBanManager->AddBan ( pPlayer, pResponsible, ( szReason ) ? szReason : "Unknown" );

    // Tell the player that was banned why. QuitPlayer will delete the player.
    pPlayer->Send ( CPlayerDisconnectedPacket ( szMessage ) );
    g_pGame->QuitPlayer ( *pPlayer, CClient::QUIT_BAN, false );

    // Tell everyone else that he was banned from the game including console
    // m_pPlayerManager->BroadcastOnlyJoined ( CChatEchoPacket ( szInfoMessage, CHATCOLOR_INFO ) );
    CLogger::LogPrintf ( "BAN: %s\n", szInfoMessage );

    return true;
}


bool CStaticFunctionDefinitions::BanIP ( const char* szIP, CPlayer* pResponsible )
{
    // Got an IP?
    if ( szIP )
    {
		char szMessage [256];
		szMessage[0] = '\0';

        if ( pResponsible )
        {
			CLuaArguments Arguments;
			Arguments.PushString ( szIP );
			if ( !pResponsible->CallEvent ( "onBan", Arguments ) )
				return false;

			m_pBanManager->AddBan ( szIP, pResponsible );
			CLogger::LogPrintf ( "BAN: %s was banned by %s\n", szIP, pResponsible->GetNick () );
			_snprintf ( szMessage, 255, "You were banned by %s", pResponsible->GetNick () );
        }
        else
        {
			CLuaArguments Arguments;
			Arguments.PushString ( szIP );
			if ( !m_pMapManager->GetRootElement()->CallEvent ( "onBan", Arguments ) )
				return false;

			m_pBanManager->AddBan ( szIP );
            CLogger::LogPrintf ( "BAN: %s was banned by Console\n", szIP );
			_snprintf ( szMessage, 255, "You were banned by Console" );
        }

		// Kick players that have this IP
		list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
		for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
		{	
			char szPlayerIP [20];
			szPlayerIP[0] = '\0';
			( *iter )->GetSourceIP ( szPlayerIP );
			if ( strcmp ( szIP, szPlayerIP ) == 0 )
			{
				CPlayer* pPlayer = *iter;
				pPlayer->Send ( CPlayerDisconnectedPacket ( szMessage ) );

				// Tell our scripts the player has quit
				CLuaArguments Arguments;
				Arguments.PushString ( "Banned" );
				pPlayer->CallEvent ( "onPlayerQuit", Arguments );

				// Output
				CLogger::LogPrintf ( "QUIT: %s left the game [Banned]\n", pPlayer->GetNick () );

				// Tell the map manager
				m_pMapManager->OnPlayerQuit ( *pPlayer );

				// Tell all the players except the parting one
				CPlayerQuitPacket Packet;
				Packet.SetPlayer ( pPlayer->GetID() );
				Packet.SetQuitReason ( static_cast < unsigned char > ( CClient::QUIT_BAN ) );
				m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );

				#ifdef MTA_VOICE
					// Remove from voice system.
					if ( m_pVoiceServer )
					{
						m_pVoiceServer->DeleteClient ( pPlayer->GetID ( ) );
					}
				#endif

				// Delete it, don't unlink yet, we could be inside the player-manager's iteration
				m_pElementDeleter->Delete ( pPlayer, false );
			}
		}

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::UnbanIP ( const char* szIP, CPlayer* pResponsible )
{
    // Got an IP?
    if ( szIP )
    {
        // Unban the player
        CBan* pBan = m_pBanManager->GetBan ( szIP );
        if ( pBan )
        {
            if ( pResponsible )
            {
				CLuaArguments Arguments;
				Arguments.PushString ( szIP );
				if ( !pResponsible->CallEvent ( "onUnban", Arguments ) )
					return false;

                CLogger::LogPrintf ( "UNBAN: %s was unbanned by %s\n", szIP, pResponsible->GetNick () );
            }
            else
            {
				CLuaArguments Arguments;
				Arguments.PushString ( szIP );
				if ( !m_pMapManager->GetRootElement()->CallEvent ( "onUnban", Arguments ) )
					return false;

                CLogger::LogPrintf ( "UNBAN: %s was unbanned by Console\n", szIP );
            }

			m_pBanManager->RemoveBan ( pBan );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::BanSerial ( const char* szSerial, CPlayer* pResponsible )
{
    // Got a Serial?
    if ( szSerial )
    {
		char szMessage [256];
		szMessage[0] = '\0';

        if ( pResponsible )
        {
			CLuaArguments Arguments;
			Arguments.PushString ( szSerial );
			if ( !pResponsible->CallEvent ( "onBan", Arguments ) )
				return false;

			m_pBanManager->AddSerialBan ( szSerial, "Unknown", pResponsible );
			CLogger::LogPrintf ( "BAN: a serial was banned by %s\n", pResponsible->GetNick () );
			_snprintf ( szMessage, 255, "You were banned by %s", pResponsible->GetNick () );
        }
        else
        {
			CLuaArguments Arguments;
			Arguments.PushString ( szSerial );
			if ( !m_pMapManager->GetRootElement()->CallEvent ( "onBan", Arguments ) )
				return false;

			m_pBanManager->AddSerialBan ( szSerial, "Unknown" );
            CLogger::LogPrintf ( "BAN: a serial was banned by Console\n" );
			_snprintf ( szMessage, 255, "You were banned by Console" );
        }

		// Check if the serial is currently in use and kick that player
		list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
		for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
		{	
			if ( (*iter)->GetSerial() == szSerial )
			{
				CPlayer* pPlayer = *iter;
				pPlayer->Send ( CPlayerDisconnectedPacket ( szMessage ) );

				// Tell our scripts the player has quit
				CLuaArguments Arguments;
				Arguments.PushString ( "Banned" );
				pPlayer->CallEvent ( "onPlayerQuit", Arguments );

				// Output
				CLogger::LogPrintf ( "QUIT: %s left the game [Banned]\n", pPlayer->GetNick () );

				// Tell the map manager
				m_pMapManager->OnPlayerQuit ( *pPlayer );

				// Tell all the players except the parting one
				CPlayerQuitPacket Packet;
				Packet.SetPlayer ( pPlayer->GetID() );
				Packet.SetQuitReason ( static_cast < unsigned char > ( CClient::QUIT_BAN ) );
				m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );

				#ifdef MTA_VOICE
					// Remove from voice system.
					if ( m_pVoiceServer )
					{
						m_pVoiceServer->DeleteClient ( pPlayer->GetID ( ) );
					}
				#endif

				// Delete it, don't unlink yet, we could be inside the player-manager's iteration
				m_pElementDeleter->Delete ( pPlayer, false );
			}
		}

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::UnbanSerial( const char* szSerial, CPlayer* pResponsible )
{
    // Got a serial
    if ( szSerial )
    {
        // Unban the serial
        CBan* pBan = m_pBanManager->GetSerialBan ( szSerial );
        if ( pBan )
        {
            if ( pResponsible )
            {
				CLuaArguments Arguments;
				Arguments.PushString ( szSerial );
				if ( !pResponsible->CallEvent ( "onUnban", Arguments ) )
					return false;

                CLogger::LogPrintf ( "UNBAN: a serial was unbanned by %s\n", pResponsible->GetNick () );
            }
            else
            {
				CLuaArguments Arguments;
				Arguments.PushString ( szSerial );
				if ( !m_pMapManager->GetRootElement()->CallEvent ( "onUnban", Arguments ) )
					return false;

                CLogger::LogPrintf ( "UNBAN: a serial was unbanned by Console\n" );
            }

			m_pBanManager->RemoveBan ( pBan );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::IsCursorShowing ( CPlayer* pPlayer, bool& bShowing )
{
    assert ( pPlayer );

    bShowing = pPlayer->IsCursorShowing ();
    return true;
}


bool CStaticFunctionDefinitions::ShowCursor ( CElement* pElement, CLuaMain* pLuaMain, bool bShow )
{
    assert ( pElement );
    RUN_CHILDREN ShowCursor ( *iter, pLuaMain, bShow );

    if ( IS_PLAYER ( pElement ) )
    {
        // Grab its resource
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Update the cursor showing state
            // TODO: isCursorShowing won't cope with this very well.
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
            pPlayer->SetCursorShowing ( bShow );

            // Get him to show/hide the cursor
            CBitStream BitStream;
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShow ) ? 1 : 0 ) );
            BitStream.pBitStream->Write ( static_cast < unsigned short > ( pResource->GetID () ) );
            pPlayer->Send ( CLuaPacket ( SHOW_CURSOR, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::ShowChat ( CElement* pElement, bool bShow )
{
    assert ( pElement );
    RUN_CHILDREN ShowChat ( *iter, bShow );

    if ( IS_PLAYER ( pElement ) )
    {
		CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Get him to show/hide the cursor
        CBitStream BitStream;
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShow ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( SHOW_CHAT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::ResetMapInfo ( CElement* pElement )
{
    if ( pElement )
    {
        RUN_CHILDREN ResetMapInfo ( *iter );
        if ( IS_PLAYER ( pElement ) )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

            CBitStream BitStream;
            pPlayer->Send ( CLuaPacket ( RESET_MAP_INFO, *BitStream.pBitStream ) );

            return true;
        }
    }
    else
    {
        CBitStream BitStream;
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_MAP_INFO, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


CElement* CStaticFunctionDefinitions::GetResourceMapRootElement ( CResource* pResource, char* szMap )
{
    if ( pResource )
    {
        list < CResourceFile* > ::const_iterator iter = pResource->IterBegin ();
        for ( ; iter != pResource->IterEnd () ; iter++ )
        {
            if ( (*iter)->GetType() == (*iter)->RESOURCE_FILE_TYPE_MAP )
            {
                if ( strcmp ( (*iter)->GetName(), szMap ) == 0 )
                {
                    CResourceMapItem* pMapItem = static_cast < CResourceMapItem* > ( *iter );
                    return pMapItem->GetMapRootElement ();
                }
            }
        }
    }
    
    return NULL;
}


CXMLNode* CStaticFunctionDefinitions::AddResourceMap ( CResource* pResource, const char* szMapNameUnmodified, int iDimension, CLuaMain* pLUA )
{
    // See if it's loaded
    if ( pResource->IsLoaded () )
    {
        // See if it's in use
        if ( !pResource->IsActive () )
        {
            // Is this a zip resource? We can't modify those
            if ( !pResource->IsResourceZip () )
            {
                // Check that the filepath is valid
                if ( IsValidFilePath ( szMapNameUnmodified ) )
                {
                    // Replace the \ with / for linux compatibility
                    char szMapName [MAX_PATH];
                    strncpy ( szMapName, szMapNameUnmodified, MAX_PATH );
                    ReplaceCharactersInString ( szMapName, '\\', '/' );

                    // Does this file already exist in this resource?
                    if ( !pResource->IncludedFileExists ( szMapName, CResourceFile::RESOURCE_FILE_TYPE_NONE ) )
                    {
                        // Create an new XML in the resource directory path
                        char szFilepath [MAX_PATH + 1];
                        _snprintf ( szFilepath, MAX_PATH, "%s/%s", pResource->GetResourceDirectoryPath ().c_str(), szMapName );
                        CXMLFile* pXML = pLUA->CreateXML ( szFilepath );
                        if ( pXML )
                        {
                            // Create a root node
                            CXMLNode* pRootNode = pXML->CreateRootNode ( "map" );

                            // Write it so the file is created
                            if ( pRootNode && pXML->Write () )
                            {
                                // Add it to the resource's meta file
                                if ( pResource->AddMapFile ( szMapName, szFilepath, iDimension ) )
                                {
                                    // Return the created XML's root node
                                    return pRootNode;
                                }
                                else
                                {
                                    CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Unable to alter meta file", szMapName, pResource->GetName ().c_str () );
                                }
                            }
                            else
                            {
                                CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Unable to write XML", szMapName, pResource->GetName ().c_str () );
                            }

                            // Destroy the XML if we failed
                            pLUA->DestroyXML ( pXML );
                        }
                        else
                            CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Unable to create XML", szMapName, pResource->GetName ().c_str () );
                    }
                    else
                        CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; File already exists in resource", szMapName, pResource->GetName ().c_str () );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Bad map name", szMapNameUnmodified, pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is in a zip file", szMapNameUnmodified, pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is in use", szMapNameUnmodified, pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is not loaded", szMapNameUnmodified, pResource->GetName ().c_str () );

    // Failed
    return NULL;
}


CXMLNode* CStaticFunctionDefinitions::AddResourceConfig ( CResource* pResource, const char* szConfigNameUnmodified, int iType, CLuaMain* pLUA )
{
    // See if it's loaded
    if ( pResource->IsLoaded () )
    {
        // See if it's in use
        if ( !pResource->IsActive () )
        {
            // Is this a zip resource? We can't modify those
            if ( !pResource->IsResourceZip () )
            {
                // Check that the filepath is valid
                if ( IsValidFilePath ( szConfigNameUnmodified ) )
                {
                    // Replace the \ with / for linux compatibility
                    char szConfigName [MAX_PATH];
                    strncpy ( szConfigName, szConfigNameUnmodified, MAX_PATH );
                    ReplaceCharactersInString ( szConfigName, '\\', '/' );

                    // Does this file already exist in this resource?
                    if ( !pResource->IncludedFileExists ( szConfigName, CResourceFile::RESOURCE_FILE_TYPE_NONE ) )
                    {
                        // Create an new XML in the resource directory path
                        char szFilepath [MAX_PATH + 1];
                        _snprintf ( szFilepath, MAX_PATH, "%s/%s", pResource->GetResourceDirectoryPath ().c_str(), szConfigName );
                        CXMLFile* pXML = pLUA->CreateXML ( szFilepath );
                        if ( pXML )
                        {
                            // Create a root node
                            CXMLNode* pRootNode = pXML->CreateRootNode ( "root" );

                            // Write it so the file is created
                            if ( pRootNode && pXML->Write () )
                            {
                                // Add it to the resource's meta file
                                if ( pResource->AddConfigFile ( szConfigName, szFilepath, iType ) )
                                {
                                    // Return the created XML's root node
                                    return pRootNode;
                                }
                                else
                                {
                                    CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to alter meta file", szConfigName, pResource->GetName ().c_str () );
                                }
                            }
                            else
                            {
                                CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to write XML", szConfigName, pResource->GetName ().c_str () );
                            }

                            // Destroy the XML if we failed
                            pLUA->DestroyXML ( pXML );
                        }
                        else
                            CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to create XML", szConfigName, pResource->GetName ().c_str () );
                    }
                    else
                        CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; File already exists in resource", szConfigName, pResource->GetName ().c_str () );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Bad config name", szConfigNameUnmodified, pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is in a zip file", szConfigNameUnmodified, pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is in use", szConfigNameUnmodified, pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is not loaded", szConfigNameUnmodified, pResource->GetName ().c_str () );

    // Failed
    return NULL;
}


bool CStaticFunctionDefinitions::RemoveResourceFile ( CResource* pResource, const char* szFilenameUnmodified )
{
    // See if it's loaded
    if ( pResource->IsLoaded () )
    {
        // See if it's in use
        if ( !pResource->IsActive () )
        {
            // Is this a zip resource? We can't modify those
            if ( !pResource->IsResourceZip () )
            {
                // Check that the filepath is valid
                if ( IsValidFilePath ( szFilenameUnmodified ) )
                {
                    // Replace the \ with / for linux compatibility
                    char szFilename [MAX_PATH];
                    strncpy ( szFilename, szFilenameUnmodified, MAX_PATH );
                    ReplaceCharactersInString ( szFilename, '\\', '/' );

                    // Try to remove the file
                    if ( pResource->RemoveFile ( szFilename ) )
                    {
                        return true;
                    }
                    else
                        CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; File does not exist", szFilename, pResource->GetName ().c_str () );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Bad filename", szFilenameUnmodified, pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is in a zip file", szFilenameUnmodified, pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is in use", szFilenameUnmodified, pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is not loaded", szFilenameUnmodified, pResource->GetName ().c_str () );

    // Failed
    return false;
}