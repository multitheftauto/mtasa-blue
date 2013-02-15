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
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
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
static CWaterManager*                               m_pWaterManager;
static CCustomWeaponManager *                       m_pCustomWeaponManager;

// Used to run a function on all the children of the elements too
#define RUN_CHILDREN \
    if ( pElement->CountChildren () ) \
        for ( CChildListType::const_iterator iter = pElement->IterBegin () ; iter != pElement->IterEnd () ; iter++ )

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
    m_pWaterManager = pGame->GetWaterManager ();
    m_pCustomWeaponManager = pGame->GetCustomWeaponManager ();
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


bool CStaticFunctionDefinitions::AddEventHandler ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod )
{
    assert ( pLuaMain );
    assert ( szName );
    assert ( pElement );

    // We got an event with that name?
    if ( m_pEvents->Exists ( szName ) )
    {
        // Add the event handler
        if ( pElement->AddEvent ( pLuaMain, szName, iLuaFunction, bPropagated, eventPriority, fPriorityMod ) )
            return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::RemoveEventHandler ( CLuaMain* pLuaMain, const char* szName, CElement* pElement, const CLuaFunctionRef& iLuaFunction )
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


bool CStaticFunctionDefinitions::TriggerClientEvent ( const std::vector < CPlayer* >& sendList, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments )
{
    assert ( szName );
    assert ( pCallWithElement );

    // Make packet
    CLuaEventPacket Packet ( szName, pCallWithElement->GetID (), &Arguments );

    // Send packet to players
    CPlayerManager::Broadcast ( Packet, sendList );

    return true;
}


bool CStaticFunctionDefinitions::TriggerLatentClientEvent ( const std::vector < CPlayer* >& sendList, const char* szName, CElement* pCallWithElement, CLuaArguments& Arguments, int iBandwidth, CLuaMain* pLuaMain, ushort usResourceNetId )
{
    assert ( szName );
    assert ( pCallWithElement );

    // Make packet
    CLuaEventPacket Packet ( szName, pCallWithElement->GetID (), &Arguments );

    markerLatentEvent.Set ( "Make packet" );

    // Send packet to players
    g_pGame->EnableLatentSends ( true, iBandwidth, pLuaMain, usResourceNetId );
    CPlayerManager::Broadcast ( Packet, sendList );
    g_pGame->EnableLatentSends ( false );

    return true;
}


bool CStaticFunctionDefinitions::CancelEvent ( bool bCancel, const char* szReason )
{
    m_pEvents->CancelEvent ( bCancel, szReason );
    return true;
}


const char* CStaticFunctionDefinitions::GetCancelReason ( )
{
    return m_pEvents->GetLastError();
}


bool CStaticFunctionDefinitions::WasEventCancelled ( void )
{
    return m_pEvents->WasEventCancelled ();
}


CDummy* CStaticFunctionDefinitions::CreateElement ( CResource* pResource, const char* szTypeName, const char* szID )
{
    assert ( szTypeName );
    assert ( szID );

    EElementType elementType;
    bool bIsInternalType = StringToEnum ( szTypeName, elementType );

    // Allow use of some internal types for backwards compatibility
    if ( bIsInternalType )
    {
        bool bOldDissallowed = false;
        static const char* szOldDissallowedTypes[] = { "dummy", "player", "vehicle", "object", "marker", "blip", "pickup", "radararea", "console" };
        for ( uint i = 0 ; i < NUMELMS( szOldDissallowedTypes ) ; i++ )
            if ( strcmp ( szTypeName, szOldDissallowedTypes[i] ) == 0 )
                bOldDissallowed = true;

        if ( !bOldDissallowed )
        {
            // Maybe issue a warning about this one day 
            bIsInternalType = false;
        }
    }

    // Long enough typename and not an internal one?
    if ( strlen ( szTypeName ) > 0 && !bIsInternalType )
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
    CChildListType ::const_iterator iter = pElement->IterBegin ();
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
        std::list < CElement* > copyList;
        for ( CChildListType ::const_iterator iter = pElement->IterBegin (); iter != pElement->IterEnd (); iter++ )
        {
            copyList.push_back ( *iter );
        }

        // Loop through the children list doing this (cloning elements)
        for ( std::list < CElement* > ::iterator iter = copyList.begin (); iter != copyList.end (); iter++ )
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
            case CElement::PATH_NODE_UNUSED:
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

            CVehicle* pTemp = m_pVehicleManager->Create ( pVehicle->GetModel (), pVehicle->GetVariant(), pVehicle->GetVariant2(), pElement->GetParentEntity () );
            if ( pTemp )
            {
                CVector vecRotationDegrees;
                pVehicle->GetRotationDegrees ( vecRotationDegrees );
                pTemp->SetRotationDegrees ( vecRotationDegrees );

                pTemp->SetHealth ( pVehicle->GetHealth () );
                pTemp->SetColor ( pVehicle->GetColor () );
                pTemp->SetUpgrades ( pVehicle->GetUpgrades () );
                pTemp->m_ucDoorStates = pVehicle->m_ucDoorStates;
                pTemp->m_ucWheelStates = pVehicle->m_ucWheelStates;
                pTemp->m_ucPanelStates = pVehicle->m_ucPanelStates;
                pTemp->m_ucLightStates = pVehicle->m_ucLightStates;

                pNewElement = pTemp;
            }
            break;
        }

        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );

            CObject* pTemp = m_pObjectManager->Create ( pElement->GetParentEntity (), NULL, pObject->IsLowLod () );
            if ( pTemp )
            {
                CVector vecRotation;
                pObject->GetRotation ( vecRotation );
                pTemp->SetRotation ( vecRotation );
                pTemp->SetModel ( pObject->GetModel () );
                pTemp->SetParentObject ( pObject->GetParentEntity () );
                pTemp->SetLowLodObject ( pObject->GetLowLodObject () );

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
                pTemp->SetColor ( pBlip->GetColor () );

                if ( pResource->HasStarted() )
                    pTemp->Sync ( true );
                bAddEntity = false;

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
    CChildListType ::const_iterator iter = pElement->IterBegin ();
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


bool CStaticFunctionDefinitions::GetElementZoneName ( CElement* pElement, SString& strOutName, bool bCitiesOnly )
{
    assert ( pElement );

    CVector vecPosition = pElement->GetPosition ();

    if ( bCitiesOnly )
        strOutName = g_pGame->GetZoneNames ()->GetCityName ( vecPosition );
    else
        strOutName = g_pGame->GetZoneNames ()->GetZoneName ( vecPosition );

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
        default:
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


bool CStaticFunctionDefinitions::IsElementDoubleSided ( CElement* pElement, bool& bDoubleSided )
{
    assert ( pElement );

    bDoubleSided = pElement->IsDoubleSided ();
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
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            fHealth = pObject->GetHealth ();
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


bool CStaticFunctionDefinitions::GetElementAttachedOffsets ( CElement* pElement, CVector & vecPosition, CVector & vecRotation )
{
    pElement->GetAttachedOffsets ( vecPosition, vecRotation );
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
            return pPed->IsSyncable () ? static_cast < CElement* > ( pPed->GetSyncer() ) : NULL;
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            return pVehicle->IsUnoccupiedSyncable () ? static_cast < CElement* > ( pVehicle->GetSyncer() ) : NULL;
            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            return pObject->IsSyncable () ? static_cast < CElement* > ( pObject->GetSyncer () ) : NULL;
            break;
        }
        default:
            break;
    }

    return NULL;
}

bool CStaticFunctionDefinitions::GetElementCollisionsEnabled ( CElement* pElement )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            return pVehicle->GetCollisionEnabled ( );
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            return pObject->GetCollisionEnabled ( );
        }
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            return pPed->GetCollisionEnabled ( );
        }
        default: return false;
    }

    return false;
}


bool CStaticFunctionDefinitions::IsElementFrozen ( CElement* pElement, bool &bFrozen )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            bFrozen = pPed->IsFrozen ();
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            bFrozen = pVehicle->IsFrozen ();
            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            bFrozen = pObject->IsStatic ();
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetLowLodElement ( CElement* pElement, CElement* pLowLodElement )
{
    RUN_CHILDREN SetLowLodElement ( *iter, pLowLodElement );

    switch ( pElement->GetType () )
    {
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            CObject* pLowLodObject = NULL;
            if ( pLowLodElement && pLowLodElement->GetType () == CElement::OBJECT )
                pLowLodObject = static_cast < CObject* > ( pLowLodElement );
            if ( !pObject->SetLowLodObject ( pLowLodObject ) )
                return false;
            break;
        }
        default: return false;
    }

    ElementID elementID = pLowLodElement ? pLowLodElement->GetID () : INVALID_ELEMENT_ID;

    CBitStream BitStream;
    BitStream.pBitStream->Write ( elementID );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_LOW_LOD_ELEMENT, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::GetLowLodElement ( CElement* pElement, CElement*& pOutLowLodElement )
{
    pOutLowLodElement = NULL;

    switch ( pElement->GetType () )
    {
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            pOutLowLodElement = pObject->GetLowLodObject ();
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::IsElementLowLod ( CElement* pElement, bool& bOutIsLowLod )
{
    bOutIsLowLod = false;

    switch ( pElement->GetType () )
    {
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            bOutIsLowLod = pObject->IsLowLod ();
            break;
        }
        default: return false;
    }

    return true;
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
            if ( pPed->GetOccupiedVehicle() )
            {
                bInWater = pPed->GetOccupiedVehicle()->IsInWater();
                break;
            }
            else
            {
                bInWater = pPed->IsInWater ();
                break;
            }
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
    BitStream.pBitStream->Write ( usIDLength );
    BitStream.pBitStream->Write ( szID, usIDLength );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_NAME, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementData ( CElement* pElement, const char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain, bool bSynchronize )
{
    assert ( pElement );
    assert ( szName );
    assert ( strlen ( szName ) <= MAX_CUSTOMDATA_NAME_LENGTH );

    bool bIsSynced;
    CLuaArgument * pCurrentVariable = pElement->GetCustomData ( szName, false, &bIsSynced );
    if ( !pCurrentVariable || *pCurrentVariable != Variable || bIsSynced != bSynchronize )
    {
        // Set its custom data
        pElement->SetCustomData ( szName, Variable, pLuaMain, bSynchronize );

        if ( bSynchronize )
        {
            // Tell our clients to update their data
            unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
            CBitStream BitStream;
            BitStream.pBitStream->WriteCompressed ( usNameLength );
            BitStream.pBitStream->Write ( szName, usNameLength );
            Variable.WriteToBitStream ( *BitStream.pBitStream );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_DATA, *BitStream.pBitStream ) );
        }
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::RemoveElementData ( CElement* pElement, const char* szName )
{
    assert ( pElement );
    assert ( szName );
    assert ( strlen ( szName ) <= MAX_CUSTOMDATA_NAME_LENGTH );

    // Set its custom data
    if ( pElement->DeleteCustomData ( szName, false ) )
    {
        // Tell our clients to update their data
        unsigned short usNameLength = static_cast < unsigned short > ( strlen ( szName ) );
        CBitStream BitStream;
        BitStream.pBitStream->WriteCompressed ( usNameLength );
        BitStream.pBitStream->Write ( szName, usNameLength );
        BitStream.pBitStream->WriteBit ( false ); // Not recursive
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, REMOVE_ELEMENT_DATA, *BitStream.pBitStream ) );
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
        BitStream.pBitStream->Write ( pParent->GetID () );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_PARENT, *BitStream.pBitStream ) );
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


bool CStaticFunctionDefinitions::GetElementRotation ( CElement* pElement, CVector& vecRotation, const char* szRotationOrder )
{
    assert ( pElement );

    eEulerRotationOrder desiredRotOrder = EulerRotationOrderFromString(szRotationOrder);
    if (desiredRotOrder == EULER_INVALID)
    {
        return false;
    }

    int iType = pElement->GetType ();
    switch ( iType )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            vecRotation.fZ = ConvertRadiansToDegrees ( pPed->GetRotation () ); //No conversion since only Z is used

            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->GetRotationDegrees ( vecRotation );
            if (desiredRotOrder != EULER_DEFAULT && desiredRotOrder != EULER_ZYX)
            {
                vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZYX, desiredRotOrder);
            }

            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            pObject->GetRotation ( vecRotation );
            ConvertRadiansToDegrees ( vecRotation );
            if (desiredRotOrder != EULER_DEFAULT && desiredRotOrder != EULER_ZXY)
            {
                vecRotation = ConvertEulerRotationOrder(vecRotation, EULER_ZXY, desiredRotOrder);
            }

            break;
        }
        default: return false;
    }

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


bool CStaticFunctionDefinitions::SetElementPosition ( CElement* pElement, const CVector& vecPosition, bool bWarp )
{
    assert ( pElement );
    RUN_CHILDREN SetElementPosition ( *iter, vecPosition, bWarp );

    // Update our position for that entity.
    pElement->SetPosition ( vecPosition );

    if ( pElement->GetType () != CElement::COLSHAPE )
    {
        // Run colpoint checks
        m_pColManager->DoHitDetection ( pElement->GetPosition (), pElement );
    }

    // Construct the set position packet
    CBitStream BitStream;
    BitStream.pBitStream->Write ( vecPosition.fX );
    BitStream.pBitStream->Write ( vecPosition.fY );
    BitStream.pBitStream->Write ( vecPosition.fZ );
    BitStream.pBitStream->Write ( pElement->GenerateSyncTimeContext () );
    if ( IS_PLAYER ( pElement ) && !bWarp )
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( 0 ) );

    // Tell only the relevant clients about this elements new position
    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        m_pPlayerManager->Broadcast ( CElementRPCPacket ( pElement, SET_ELEMENT_POSITION, *BitStream.pBitStream ), static_cast < CPerPlayerEntity * > ( pElement )->GetPlayersList () );
    }
    // Tell all clients about its new position
    else
    {
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_POSITION, *BitStream.pBitStream ) );
    }

    return true;
}


bool CStaticFunctionDefinitions::SetElementRotation ( CElement* pElement, const CVector& vecRotation, const char* szRotationOrder, bool bNewWay )
{
    assert ( pElement );

    eEulerRotationOrder argumentRotOrder = EulerRotationOrderFromString(szRotationOrder);
    if (argumentRotOrder == EULER_INVALID)
    {
        return false;
    }

    int iType = pElement->GetType ();
    switch ( iType )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            SetPedRotation( pPed, vecRotation.fZ, bNewWay ); //No rotation order conversion required since only Z is used

            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            if (argumentRotOrder == EULER_DEFAULT || argumentRotOrder == EULER_ZYX)
            {
                SetVehicleRotation( pVehicle, vecRotation );
            }
            else
            {
                CVector vZYX = ConvertEulerRotationOrder( vecRotation, argumentRotOrder, EULER_ZYX ); 
                SetVehicleRotation( pVehicle, vZYX );
            }

            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            if (argumentRotOrder == EULER_DEFAULT || argumentRotOrder == EULER_ZXY)
            {
                SetObjectRotation( pObject, vecRotation );
            }
            else
            {
                CVector vZXY = ConvertEulerRotationOrder(vecRotation, argumentRotOrder, EULER_ZXY );
                SetObjectRotation( pObject, vZXY );
            }
        }
        default: return false;
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
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( vecVelocity.fX );
    BitStream.pBitStream->Write ( vecVelocity.fY );
    BitStream.pBitStream->Write ( vecVelocity.fZ );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_VELOCITY, *BitStream.pBitStream ) );

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
        BitStream.pBitStream->Write ( ucInterior );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bSetPosition ) ? 1 : 0 ) );
        if ( bSetPosition )
        {
            BitStream.pBitStream->Write ( vecPosition.fX );
            BitStream.pBitStream->Write ( vecPosition.fY );
            BitStream.pBitStream->Write ( vecPosition.fZ );
        }
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_INTERIOR, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetElementDimension ( CElement* pElement, unsigned short usDimension )
{
    assert ( pElement );
    RUN_CHILDREN SetElementDimension ( *iter, usDimension );

    if ( pElement->GetType () == CElement::TEAM )
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
    }

    if ( pElement->GetType () == CElement::VEHICLE )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        CPed* pOccupant;
        // Loop all seats of the vehicle
        for ( unsigned int i = 0 ; i < MAX_VEHICLE_SEATS ; i++ )
        {
            pOccupant = GetVehicleOccupant ( pVehicle, i );
            if ( pOccupant )
                // If the seat is occupied, set the occupants dimension
                SetElementDimension ( pOccupant, usDimension );
        }
    }

    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( !pPed->IsSpawned () )
            {
                return false;
            }
        }
        case CElement::TEAM:
        case CElement::COLSHAPE:
        case CElement::DUMMY:
        case CElement::VEHICLE:
        case CElement::OBJECT:
        case CElement::MARKER:
        case CElement::BLIP:
        case CElement::PICKUP:
        case CElement::RADAR_AREA:
        case CElement::WORLD_MESH_UNUSED:
        {
            pElement->SetDimension ( usDimension );
            CBitStream bitStream;
            bitStream.pBitStream->Write ( usDimension );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_DIMENSION, *bitStream.pBitStream ) );
            return true;
        }
        default:
            break;
    }

    return false;
}


bool CStaticFunctionDefinitions::AttachElements ( CElement* pElement, CElement* pAttachedToElement, CVector& vecPosition, CVector& vecRotation )
{
    assert ( pElement );
    assert ( pAttachedToElement );

    // Check the elements we are attaching are not already connected
    std::set < CElement* > history;
    for ( CElement* pCurrent = pAttachedToElement ; pCurrent ; pCurrent = pCurrent->GetAttachedToElement () )
    {
        if ( pCurrent == pElement )
            return false;
        if ( MapContains ( history, pCurrent ) )
            break;  // This should not be possible, but you never know
        MapInsert ( history, pCurrent );
    }

    if ( pElement->IsAttachToable () && pAttachedToElement->IsAttachable () && pElement->GetDimension() == pAttachedToElement->GetDimension() )
    {
        pElement->SetAttachedOffsets ( vecPosition, vecRotation );
        ConvertDegreesToRadians ( vecRotation );
        pElement->AttachTo ( pAttachedToElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pAttachedToElement->GetID () );
        BitStream.pBitStream->Write ( vecPosition.fX );
        BitStream.pBitStream->Write ( vecPosition.fY );
        BitStream.pBitStream->Write ( vecPosition.fZ );
        BitStream.pBitStream->Write ( vecRotation.fX );
        BitStream.pBitStream->Write ( vecRotation.fY );
        BitStream.pBitStream->Write ( vecRotation.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, ATTACH_ELEMENTS, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::DetachElements ( CElement* pElement, CElement* pAttachedToElement )
{
    assert ( pElement );

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
            BitStream.pBitStream->Write ( pElement->GetSyncTimeContext () );
            BitStream.pBitStream->Write ( vecPosition.fX );
            BitStream.pBitStream->Write ( vecPosition.fY );
            BitStream.pBitStream->Write ( vecPosition.fZ );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, DETACH_ELEMENTS, *BitStream.pBitStream ) );

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
            SColor color = pMarker->GetColor ();
            color.A = ucAlpha;
            pMarker->SetColor ( color );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( ucAlpha );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_ALPHA, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementDoubleSided ( CElement* pElement, bool bDoubleSided )
{
    assert ( pElement );
    RUN_CHILDREN SetElementDoubleSided ( *iter, bDoubleSided );

    pElement->SetDoubleSided ( bDoubleSided );

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bDoubleSided );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_DOUBLESIDED, *BitStream.pBitStream ) );

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
                pPed->SetHealthChangeTime ( GetTickCount32 () );
            }
            else
                return false;
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetHealth ( fHealth );
            pVehicle->SetHealthChangeTime ( GetTickCount32 () );
            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            pObject->SetHealth ( fHealth );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fHealth );
    BitStream.pBitStream->Write ( pElement->GenerateSyncTimeContext () );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_HEALTH, *BitStream.pBitStream ) );

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
            CLuaArguments Arguments;
            Arguments.PushNumber ( pPed->GetModel() ); // Get the old model
            pPed->SetModel ( usModel ); // Set the new model
            Arguments.PushNumber ( usModel ); // Get the new model
            pPed->CallEvent ( "onElementModelChange", Arguments, false );
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            if ( pVehicle->GetModel () == usModel ) return false;
            if ( !CVehicleManager::IsValidModel ( usModel ) ) return false;
            CLuaArguments Arguments;
            Arguments.PushNumber ( pVehicle->GetModel () ); // Get the old model
            pVehicle->SetModel ( usModel ); // Set the new model
            Arguments.PushNumber ( usModel ); // Get the new model
            pVehicle->CallEvent ( "onElementModelChange", Arguments, false );

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
			CLuaArguments Arguments;
            Arguments.PushNumber ( pObject->GetModel () ); // Get the old model
            pObject->SetModel ( usModel ); // Set the new model
            Arguments.PushNumber ( usModel ); // Get the new model
            pObject->CallEvent ( "onElementModelChange", Arguments, false );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->Write ( usModel );
    if ( pElement->GetType () == CElement::VEHICLE )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        BitStream.pBitStream->Write ( pVehicle->GetVariant () );
        BitStream.pBitStream->Write ( pVehicle->GetVariant2 () );
    }
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_MODEL, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetElementAttachedOffsets ( CElement* pElement, CVector & vecPosition, CVector & vecRotation )
{
    RUN_CHILDREN SetElementAttachedOffsets ( *iter, vecPosition, vecRotation );

    CVector vecCurrentPos, vecCurrentRot;
    pElement->GetAttachedOffsets ( vecCurrentPos, vecCurrentRot );
    if ( vecPosition != vecCurrentPos || vecRotation != vecCurrentRot )
    {
        pElement->SetAttachedOffsets ( vecPosition, vecRotation );
        ConvertDegreesToRadians ( vecRotation );
        
        SPositionSync position ( true );
        position.data.vecPosition = vecPosition;

        SRotationRadiansSync rotation ( true );
        rotation.data.vecRotation = vecRotation;

        CBitStream BitStream;
        position.Write ( *BitStream.pBitStream );
        rotation.Write ( *BitStream.pBitStream );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_ATTACHED_OFFSETS, *BitStream.pBitStream ) );
    }
    return true;
}


bool CStaticFunctionDefinitions::ClearElementVisibleTo ( CElement* pElement )
{
    RUN_CHILDREN ClearElementVisibleTo ( *iter );

    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        CPerPlayerEntity* pEntity = static_cast < CPerPlayerEntity* > ( pElement );
        pEntity->ClearVisibleToReferences ();
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetElementSyncer ( CElement* pElement, CPlayer* pPlayer, bool bEnable )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::PED:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            pPed->SetSyncable ( bEnable );
            g_pGame->GetPedSync()->OverrideSyncer ( pPed, pPlayer );
            return true;
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetUnoccupiedSyncable ( bEnable );
            g_pGame->GetUnoccupiedVehicleSync()->OverrideSyncer ( pVehicle, pPlayer );
            return true;
            break;
        }
#ifdef WITH_OBJECT_SYNC
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            pObject->SetSyncable ( bEnable );
            g_pGame->GetObjectSync ()->OverrideSyncer ( pObject, pPlayer );
            return true;
            break;
        }
#endif
        default: return false;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetElementCollisionsEnabled ( CElement* pElement, bool bEnable )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetCollisionEnabled ( bEnable );
            break;
        }
        case CElement::OBJECT:
        {
            CObject* pObject = static_cast < CObject* > ( pElement );
            pObject->SetCollisionEnabled ( bEnable );
            break;
        }
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            pPed->SetCollisionEnabled ( bEnable );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bEnable );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_COLLISIONS_ENABLED, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetElementFrozen ( CElement* pElement, bool bFrozen )
{
    assert ( pElement );
    RUN_CHILDREN SetElementFrozen ( *iter, bFrozen );

    switch ( pElement->GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        {
            CPed * pPed = static_cast < CPed* > ( pElement );
            pPed->SetFrozen ( bFrozen );
            break;
        }
        case CElement::VEHICLE:
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            pVehicle->SetFrozen ( bFrozen );
            break;
        }
        case CElement::OBJECT:
        {
            CObject * pObject = static_cast < CObject* > ( pElement );
            pObject->SetStatic ( bFrozen );
            break;
        }
        default: return false;
    }

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bFrozen );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pElement, SET_ELEMENT_FROZEN, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::GetPlayerName ( CElement* pElement, SString& strOutNick )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::PLAYER:
        {
            strOutNick = static_cast < CPlayer* > ( pElement )->GetNick ();
            break;
        }
        case CElement::CONSOLE:
        {
            strOutNick = "Console";
            break;
        }
        default: return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::GetPlayerIP ( CElement* pElement, SString& strOutIP )
{
    assert ( pElement );

    switch ( pElement->GetType () )
    {
        case CElement::PLAYER:
        {
            strOutIP = static_cast < CPlayer* > ( pElement )->GetSourceIP ();
            break;
        }
        case CElement::CONSOLE:
        {
            strOutIP = "127.0.0.1";
            break;
        }
        default: return false;
    }

    return true;
}


CAccount* CStaticFunctionDefinitions::GetPlayerAccount ( CElement* pElement )
{
    assert ( pElement );

    CClient* pClient = pElement->GetClient ();
    if ( pClient )
    {
        return pClient->GetAccount ();
    }
    return NULL;
}


const SString& CStaticFunctionDefinitions::GetPlayerVersion ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    return pPlayer->GetPlayerVersion ();
}


bool CStaticFunctionDefinitions::SetPlayerName ( CElement* pElement, const char* szName )
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
                            // Call the event
                            CLuaArguments Arguments;
                            Arguments.PushString ( szNick );
                            Arguments.PushString ( szName );
                            pPlayer->CallEvent ( "onPlayerChangeNick", Arguments );

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

bool CStaticFunctionDefinitions::DetonateSatchels ( CElement* pElement )
{
    RUN_CHILDREN DetonateSatchels ( *iter );

    // Tell everyone
    if ( IS_PLAYER( pElement ) )
    {
        CPlayer* pPlayer = static_cast<CPlayer *>( pElement );
        if ( pPlayer->IsJoined () )
        {
            CDetonateSatchelsPacket Packet;
            Packet.SetSourceElement ( pPlayer );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
            return true;
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
            // Convert the rotation to radians
            float fRotationRadians = ConvertDegreesToRadians ( fRotation );
            // Clamp it to -PI .. PI
            if ( fRotationRadians < -PI )
            {
                do
                {
                    fRotationRadians += PI * 2.0f;
                } while ( fRotationRadians < -PI );
            }
            else if ( fRotationRadians > PI )
            {
                do
                {
                    fRotationRadians -= PI * 2.0f;
                } while ( fRotationRadians > PI );
            }

            pPed->SetPosition ( vecPosition );
            pPed->SetIsDead ( false );
            pPed->SetSpawned ( true );
            pPed->SetHealth ( 100.0f );
            pPed->SetSyncable ( bSynced );

            pPed->SetRotation ( fRotationRadians );

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


bool CStaticFunctionDefinitions::SetPlayerAmmo ( CElement* pElement, unsigned char ucSlot, unsigned short usAmmo, unsigned short usAmmoInClip )
{
    assert ( pElement );
    CPlayer* pPlayer = NULL;
    if ( IS_PLAYER ( pElement ) )
        pPlayer = static_cast < CPlayer* > ( pElement );
    if ( !pPlayer )
        return false;

    CWeapon* pWeapon = pPlayer->GetWeapon ( ucSlot );
    if ( pWeapon ) {
        unsigned char ucWeaponID = pWeapon->ucType;
        if ( ucWeaponID ) {
            if ( pPlayer->IsSpawned () )
            {
                CBitStream BitStream;

                SWeaponTypeSync weaponType;
                weaponType.data.ucWeaponType = ucWeaponID;
                BitStream.pBitStream->Write ( &weaponType );

                SWeaponAmmoSync ammo ( ucWeaponID, true, true );
                ammo.data.usTotalAmmo = usAmmo;
                ammo.data.usAmmoInClip = usAmmoInClip;

                BitStream.pBitStream->Write ( &ammo );
                pPlayer->Send ( CElementRPCPacket ( pPlayer, SET_WEAPON_AMMO, *BitStream.pBitStream ) );
                return true;
            }
        }
    }

    return false;
}


CPlayer* CStaticFunctionDefinitions::GetPlayerFromName ( const char* szNick )
{
    assert ( szNick );

    return m_pPlayerManager->Get ( szNick );
}


CVehicle* CStaticFunctionDefinitions::GetPedOccupiedVehicle ( CPed* pPed )
{
    assert ( pPed );

    return pPed->GetOccupiedVehicle ();
}


bool CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat ( CPed* pPed, unsigned int& uiSeat )
{
    assert ( pPed );

    if ( pPed->GetOccupiedVehicle () )
    {
        uiSeat = pPed->GetOccupiedVehicleSeat ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float fData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;

    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_WEAPON_RANGE:
            {
                if ( fData >= 0 && fData <= 350 )
                    pWeaponInfo->SetWeaponRange ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_TARGET_RANGE:
            {
                if ( fData >= 0 && fData <= 300 )
                    pWeaponInfo->SetTargetRange ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ACCURACY:
            {
                if ( fData >= 0 && fData <= 10000 )
                    pWeaponInfo->SetAccuracy ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_MOVE_SPEED:
            {
                if ( fData >= -1 && fData <= 100 )
                    pWeaponInfo->SetMoveSpeed ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM_LOOP_START:
            {
                if ( fData >= 0.0f && fData <= 5.0f )
                    pWeaponInfo->SetWeaponAnimLoopStart ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM_LOOP_STOP:
            {
                if ( fData >= 0.0f && fData <= 5.0f )
                    pWeaponInfo->SetWeaponAnimLoopStop ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            {
                if ( fData >= 0 && fData <= 2.0f )
                    pWeaponInfo->SetWeaponAnimLoopFireTime ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM2_LOOP_START:
            {
                if ( fData >= 0.0f && fData <= 5.0f )
                    pWeaponInfo->SetWeaponAnim2LoopStart ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM2_LOOP_STOP:
            {
                if ( fData >= 0.0f && fData <= 5.0f )
                    pWeaponInfo->SetWeaponAnim2LoopStop ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            {
                if ( fData >= 0 && fData <= 2.0f )
                    pWeaponInfo->SetWeaponAnim2LoopFireTime ( fData );
                else
                    return false;
                break;
            }
        case WEAPON_ANIM_BREAKOUT_TIME:
            {
                if ( fData >= 0 && fData <= 5.0f )
                    pWeaponInfo->SetWeaponAnimBreakoutTime ( fData );
                else
                    return false;
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    pWeaponInfo->SetChanged( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eWeapon ) );
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eSkillLevel ) );
    BitStream.pBitStream->Write ( fData );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEAPON_PROPERTY, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, short sData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;

    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( eWeapon, eSkillLevel );
    CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( eWeapon, eSkillLevel );
        
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_DAMAGE:
            {
                if ( sData >= -10000 && sData <= 10000 )
                    pWeaponInfo->SetDamagePerHit ( sData );
                else
                    return false;
                break;
            }
        case WEAPON_MAX_CLIP_AMMO:
            {
                if ( sData >= 0 && sData <= 1000 )
                    pWeaponInfo->SetMaximumClipAmmo ( sData );
                else
                    return false;
                break;
            }
        case WEAPON_FLAGS:
            {
                if ( pWeaponInfo->IsFlagSet ( sData ) )
                {
                    if ( sData == 0x800 )
                    {
                        // if it can support this anim group
                        if ( ( eWeapon >= WEAPONTYPE_PISTOL && eWeapon <= WEAPONTYPE_SNIPERRIFLE ) || eWeapon == WEAPONTYPE_MINIGUN )
                        {
                            // Revert anim group to default
                            pWeaponInfo->SetAnimGroup ( pOriginalWeaponInfo->GetAnimGroup ( ) );
                        }
                    }
                    pWeaponInfo->ClearFlag ( sData );
                }
                else
                {
                    if ( sData == 0x800 )
                    {
                        // if it can support this anim group
                        if ( ( eWeapon >= WEAPONTYPE_PISTOL && eWeapon <= WEAPONTYPE_SNIPERRIFLE ) || eWeapon == WEAPONTYPE_MINIGUN )
                        {
                            // sawn off shotgun anim group
                            pWeaponInfo->SetAnimGroup ( 17 );
                        }
                    }
                    pWeaponInfo->SetFlag ( sData );
                }
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    pWeaponInfo->SetChanged( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eWeapon ) );
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
    BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eSkillLevel ) );
    BitStream.pBitStream->Write ( sData );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WEAPON_PROPERTY, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::GetWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float & fData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;


    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_WEAPON_RANGE:
            {
                fData = pWeaponInfo->GetWeaponRange ( );
                break;
            }
        case WEAPON_TARGET_RANGE:
            {
                fData = pWeaponInfo->GetTargetRange ( );
                break;
            }
        case WEAPON_ACCURACY:
            {
                fData = pWeaponInfo->GetAccuracy ( );
                break;
            }
        case WEAPON_DAMAGE:
            {
                fData = pWeaponInfo->GetDamagePerHit ( );
                break;
            }
        case WEAPON_LIFE_SPAN:
            {
                fData = pWeaponInfo->GetLifeSpan ( );
                break;
            }
        case WEAPON_FIRING_SPEED:
            {
                fData = pWeaponInfo->GetFiringSpeed ( );
                break;
            }
        case WEAPON_MOVE_SPEED:
            {
                fData = pWeaponInfo->GetMoveSpeed ( );
                break;
            }
        case WEAPON_SPREAD:
            {
                fData = pWeaponInfo->GetSpread ( );
                break;
            }
        case WEAPON_REQ_SKILL_LEVEL:
            {
                fData = pWeaponInfo->GetRequiredStatLevel ( );
                break;
            }
        case WEAPON_ANIM_LOOP_START:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopStart ( );
                break;
            }
        case WEAPON_ANIM_LOOP_STOP:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopStop ( );
                break;
            }
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopFireTime ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_START:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopStart ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_STOP:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopStop ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopFireTime ( );
                break;
            }
        case WEAPON_ANIM_BREAKOUT_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnimBreakoutTime ( );
                break;
            }
        case WEAPON_RADIUS:
            {
                fData = pWeaponInfo->GetWeaponRadius ( );
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, short & sData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;


    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_DAMAGE:
            {
                sData = pWeaponInfo->GetDamagePerHit ( );
                break;
            }
        case WEAPON_MAX_CLIP_AMMO:
            {
                sData = pWeaponInfo->GetMaximumClipAmmo ( );
                break;
            }
        case WEAPON_ANIM_GROUP:
            {
                sData = (short)pWeaponInfo->GetAnimGroup ( );
                break;
            }
        case WEAPON_FLAGS:
            {
                sData = pWeaponInfo->GetFlags ( );
                break;
            }
        case WEAPON_FIRETYPE:
            {
                sData = pWeaponInfo->GetFireType ( );
                break;
            }
        case WEAPON_MODEL:
            {
                sData = pWeaponInfo->GetModel ( );
                break;
            }
        case WEAPON_MODEL2:
            {
                sData = pWeaponInfo->GetModel2 ( );
                break;
            }
        case WEAPON_SLOT:
            {
                sData = pWeaponInfo->GetSlot ( );
                break;
            }
        case WEAPON_AIM_OFFSET:
            {
                sData = pWeaponInfo->GetAimOffsetIndex ( );
                break;
            }
        case WEAPON_SKILL_LEVEL:
            {
                sData = pWeaponInfo->GetSkill ( );
                break;
            }
        case WEAPON_DEFAULT_COMBO:
            {
                sData = pWeaponInfo->GetDefaultCombo ( );
                break;
            }
        case WEAPON_COMBOS_AVAILABLE:
            {
                sData = pWeaponInfo->GetCombosAvailable ( );
                break;
            }

        default:
            return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector & vecData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;

    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_FIRE_OFFSET:
            {
                vecData = *pWeaponInfo->GetFireOffset ( );
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    return true;
}


bool CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, float & fData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;


    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_WEAPON_RANGE:
            {
                fData = pWeaponInfo->GetWeaponRange ( );
                break;
            }
        case WEAPON_TARGET_RANGE:
            {
                fData = pWeaponInfo->GetTargetRange ( );
                break;
            }
        case WEAPON_ACCURACY:
            {
                fData = pWeaponInfo->GetAccuracy ( );
                break;
            }
        case WEAPON_DAMAGE:
            {
                fData = pWeaponInfo->GetDamagePerHit ( );
                break;
            }
        case WEAPON_LIFE_SPAN:
            {
                fData = pWeaponInfo->GetLifeSpan ( );
                break;
            }
        case WEAPON_FIRING_SPEED:
            {
                fData = pWeaponInfo->GetFiringSpeed ( );
                break;
            }
        case WEAPON_MOVE_SPEED:
            {
                fData = pWeaponInfo->GetMoveSpeed ( );
                break;
            }
        case WEAPON_SPREAD:
            {
                fData = pWeaponInfo->GetSpread ( );
                break;
            }
        case WEAPON_REQ_SKILL_LEVEL:
            {
                fData = pWeaponInfo->GetRequiredStatLevel ( );
                break;
            }
        case WEAPON_ANIM_LOOP_START:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopStart ( );
                break;
            }
        case WEAPON_ANIM_LOOP_STOP:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopStop ( );
                break;
            }
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnimLoopFireTime ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_START:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopStart ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_STOP:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopStop ( );
                break;
            }
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnim2LoopFireTime ( );
                break;
            }
        case WEAPON_ANIM_BREAKOUT_TIME:
            {
                fData = pWeaponInfo->GetWeaponAnimBreakoutTime ( );
                break;
            }
        case WEAPON_RADIUS:
            {
                fData = pWeaponInfo->GetWeaponRadius ( );
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, short & sData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;


    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_DAMAGE:
            {
                sData = pWeaponInfo->GetDamagePerHit ( );
                break;
            }
        case WEAPON_MAX_CLIP_AMMO:
            {
                sData = pWeaponInfo->GetMaximumClipAmmo ( );
                break;
            }
        case WEAPON_ANIM_GROUP:
            {
                sData = (short)pWeaponInfo->GetAnimGroup ( );
                break;
            }
        case WEAPON_FLAGS:
            {
                sData = pWeaponInfo->GetFlags ( );
                break;
            }
        case WEAPON_FIRETYPE:
            {
                sData = pWeaponInfo->GetFireType ( );
                break;
            }
        case WEAPON_MODEL:
            {
                sData = pWeaponInfo->GetModel ( );
                break;
            }
        case WEAPON_MODEL2:
            {
                sData = pWeaponInfo->GetModel2 ( );
                break;
            }
        case WEAPON_SLOT:
            {
                sData = pWeaponInfo->GetSlot ( );
                break;
            }
        case WEAPON_AIM_OFFSET:
            {
                sData = pWeaponInfo->GetAimOffsetIndex ( );
                break;
            }
        case WEAPON_SKILL_LEVEL:
            {
                sData = pWeaponInfo->GetSkill ( );
                break;
            }
        case WEAPON_DEFAULT_COMBO:
            {
                sData = pWeaponInfo->GetDefaultCombo ( );
                break;
            }
        case WEAPON_COMBOS_AVAILABLE:
            {
                sData = pWeaponInfo->GetCombosAvailable ( );
                break;
            }

        default:
            return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eWeaponProperty eProperty, eWeaponType eWeapon, eWeaponSkill eSkillLevel, CVector & vecData )
{
    if ( eProperty == WEAPON_INVALID_PROPERTY )
        return false;

    CWeaponStat* pWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( eWeapon, eSkillLevel );
    if ( pWeaponInfo )
    {
        switch ( eProperty )
        {
        case WEAPON_FIRE_OFFSET:
            {
                vecData = *pWeaponInfo->GetFireOffset ( );
                break;
            }
        default:
            return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetPlayerPing ( CPlayer* pPlayer, unsigned int& uiPing )
{
    assert ( pPlayer );

    uiPing = pPlayer->GetPing ();
    return true;
}


bool CStaticFunctionDefinitions::GetPlayerSourceIP ( CPlayer* pPlayer, SString& strOutIP )
{
    assert ( pPlayer );

    strOutIP = pPlayer->GetSourceIP ();
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
            // Check status
            if ( (*iter)->IsJoined () )
            {
                // This is the given index?
                if ( i++ == uiRandom )
                {
                    return *iter;
                }
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


bool CStaticFunctionDefinitions::CanPlayerUseFunction ( CPlayer* pPlayer, const char* szFunction, bool& bCanUse )
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


bool CStaticFunctionDefinitions::GetPlayerNametagText ( CPlayer* pPlayer, SString& strOutText )
{
    assert ( pPlayer );

    const char* szNametagText = pPlayer->GetNametagText ();
    if ( szNametagText == NULL )
        szNametagText = pPlayer->GetNick ();

    if ( szNametagText )
    {
        strOutText = szNametagText;
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
        return true;
    }

    return false;
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
        return true;
    }

    return false;
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
        return true;
    }

    return false;
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


bool CStaticFunctionDefinitions::ShowPlayerHudComponent ( CElement* pElement, eHudComponent component, bool bShow )
{
    assert ( pElement );

    RUN_CHILDREN ShowPlayerHudComponent ( *iter, component, bShow );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( component ) );
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShow ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( SHOW_PLAYER_HUD_COMPONENT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::TakePlayerScreenShot ( CElement* pElement, uint uiSizeX, uint uiSizeY, const SString& strTag, uint uiQuality, uint uiMaxBandwidth, uint uiMaxPacketSize, const SString& strResourceName )
{
    assert ( pElement );

    RUN_CHILDREN TakePlayerScreenShot ( *iter, uiSizeX, uiSizeY, strTag, uiQuality, uiMaxBandwidth, uiMaxPacketSize, strResourceName );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( static_cast < ushort > ( uiSizeX ) );
        BitStream.pBitStream->Write ( static_cast < ushort > ( uiSizeY ) );
        BitStream.pBitStream->WriteString ( strTag );
        BitStream.pBitStream->Write ( static_cast < uchar > ( uiQuality ) );
        BitStream.pBitStream->Write ( uiMaxBandwidth );
        BitStream.pBitStream->Write ( static_cast < ushort > ( uiMaxPacketSize ) );
        BitStream.pBitStream->WriteString ( strResourceName );
        BitStream.pBitStream->Write ( GetTickCount32 () );
        pPlayer->Send ( CLuaPacket ( TAKE_PLAYER_SCREEN_SHOT, *BitStream.pBitStream ) );

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
            if ( szText == NULL || IsNametagValid ( szText ) )
            {
                pPlayer->SetNametagText ( szText );

                unsigned short usTextLength = 0;
                if ( szText )
                    usTextLength = static_cast < unsigned short > ( strlen ( szText ) );

                CBitStream BitStream;
                BitStream.pBitStream->Write ( usTextLength );
                if ( usTextLength > 0 )
                    BitStream.pBitStream->Write ( szText, usTextLength );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPlayer, SET_PLAYER_NAMETAG_TEXT, *BitStream.pBitStream ) );

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
                pPlayer->RemoveNametagOverrideColor ();

                // Send a packet
                CBitStream BitStream;
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPlayer, REMOVE_PLAYER_NAMETAG_COLOR, *BitStream.pBitStream ) );

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
                BitStream.pBitStream->Write ( ucR );
                BitStream.pBitStream->Write ( ucG );
                BitStream.pBitStream->Write ( ucB );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPlayer, SET_PLAYER_NAMETAG_COLOR, *BitStream.pBitStream ) );

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
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bShowing ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPlayer, SET_PLAYER_NAMETAG_SHOWING, *BitStream.pBitStream ) );

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

        if ( bMuted != pPlayer->IsMuted ( ) )
        {
            bool bEventCancelled = false;

            CLuaArguments arguments;

            if ( bMuted )
                bEventCancelled = !pPlayer->CallEvent ( "onPlayerMute", arguments );
            else
                bEventCancelled = !pPlayer->CallEvent ( "onPlayerUnmute", arguments );

            if ( !bEventCancelled )
            {
                pPlayer->SetMuted ( bMuted );
                
                return true;
            }
        }
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

        if ( ucHostLength == 0 )
        {
            if ( pPlayer->GetBitStreamVersion () < 0x2E )
            {
                // Reconnect to same server didn't work before bitstream 0x2E, so disconnect with a message
                pPlayer->Send ( CPlayerDisconnectedPacket ( "Please reconnect" ) );
                g_pGame->QuitPlayer ( *pPlayer );
                return true;
            }
        }

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucHostLength );
        BitStream.pBitStream->Write ( szHost, ucHostLength );
        BitStream.pBitStream->Write ( usPort );
        if ( szPassword )
        {
            unsigned char ucPasswordLength = static_cast < unsigned char > ( strlen ( szPassword ) );
            BitStream.pBitStream->Write ( ucPasswordLength );
            BitStream.pBitStream->Write ( szPassword, ucPasswordLength );
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


bool CStaticFunctionDefinitions::GetPedClothes ( CPed* pPed, unsigned char ucType, SString& strOutTexture, SString& strOutModel )
{
    assert ( pPed );

    const SPlayerClothing* pClothing = pPed->GetClothes ()->GetClothing ( ucType );
    if ( pClothing )
    {
        strOutTexture = pClothing->szTexture;
        strOutModel = pClothing->szModel;
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

bool CStaticFunctionDefinitions::IsPedFrozen ( CPed * pPed, bool & bIsFrozen )
{
    assert ( pPed );

    bIsFrozen = pPed->IsFrozen ();
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
                pPed->SetArmorChangeTime ( GetTickCount32 () );

                unsigned char ucArmor = static_cast < unsigned char > ( fArmor * 1.25 );

                // Tell everyone
                CBitStream BitStream;
                BitStream.pBitStream->Write ( ucArmor );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_ARMOR, *BitStream.pBitStream ) );
                return true;
            }
        }

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
            if ( IS_PLAYER(pPed) ) {
                // Tell everyone to kill this player
                CPlayerWastedPacket WastedPacket ( pPed, pKiller, ucKillerWeapon, ucBodyPart, bStealth );
                m_pPlayerManager->BroadcastOnlyJoined ( WastedPacket );
                pPed->CallEvent ( "onPlayerWasted", Arguments );
            }
            else {
                // Tell everyone to kill this player
                CPedWastedPacket WastedPacket ( pPed, pKiller, ucKillerWeapon, ucBodyPart, bStealth );
                m_pPlayerManager->BroadcastOnlyJoined ( WastedPacket );
                pPed->CallEvent ( "onPedWasted", Arguments );
            }

            for (unsigned int slot = 0; slot < WEAPON_SLOTS; ++slot)
            {
                pPed->SetWeaponType(0, slot);
                pPed->SetWeaponAmmoInClip(0, slot);
                pPed->SetWeaponTotalAmmo(0, slot);
            }

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedRotation ( CElement* pElement, float fRotation, bool bNewWay )
{
    assert ( pElement );
    RUN_CHILDREN SetPedRotation ( *iter, fRotation, bNewWay );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        if ( pPed->IsSpawned () )
        {
            // Set his new rotation
            float fRadians = ConvertDegreesToRadians ( fRotation );

            // Clamp it to -PI .. PI
            if ( fRadians < -PI )
            {
                do
                {
                    fRadians += PI * 2.0f;
                } while ( fRadians < -PI );
            }
            else if ( fRadians > PI )
            {
                do
                {
                    fRadians -= PI * 2.0f;
                } while ( fRadians > PI );
            }
            pPed->SetRotation ( fRadians );

            // Tell the players
            CBitStream BitStream;

            SPedRotationSync rotation;
            rotation.data.fRotation = fRadians;
            BitStream.pBitStream->Write ( &rotation );
            BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
            uchar ucNewWay = bNewWay ? 1 : 0;
            BitStream.pBitStream->Write ( ucNewWay );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_ROTATION, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetPedStat ( CElement* pElement, unsigned short usStat, float fValue )
{
    assert ( pElement );

    // Check the stat
    if ( usStat < NUM_PLAYER_STATS && fValue >= 0.0f && fValue <= 1000.0f )
    {
        RUN_CHILDREN SetPedStat ( *iter, usStat, fValue );

        if ( IS_PLAYER ( pElement ) )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

            // Dont let them set visual stats if they dont have the CJ model
            if ( ( usStat != 21 /* FAT */ && usStat != 23 /* BODY_MUSCLE */ ) || pPlayer->GetModel () == 0 )
            {
                // Save the stat
                pPlayer->SetPlayerStat ( usStat, fValue );

                // Notify everyone
                CPlayerStatsPacket Packet;
                Packet.SetSourceElement ( pPlayer );
                Packet.Add ( usStat, fValue );
                m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                return true;
            }
        }
        else
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


bool CStaticFunctionDefinitions::AddPedClothes ( CElement* pElement, const char* szTexture, const char* szModel, unsigned char ucType )
{
    assert ( pElement );
    assert ( szTexture );
    assert ( szModel );

    if ( CPlayerClothes::IsValidClothing ( szTexture, szModel, ucType ) )
    {
        RUN_CHILDREN AddPedClothes ( *iter, szTexture, szModel, ucType );

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


bool CStaticFunctionDefinitions::RemovePedClothes ( CElement* pElement, unsigned char ucType, const char* szTexture, const char* szModel )
{
    assert ( pElement );

    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        RUN_CHILDREN RemovePedClothes ( *iter, ucType, szTexture, szModel );

        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );

            const SPlayerClothing* pClothing = pPed->GetClothes ()->GetClothing ( ucType );
            if ( pClothing )
            {
                if ( ( !szTexture || !stricmp ( pClothing->szTexture, szTexture ) ) &&
                     ( !szModel || !stricmp ( pClothing->szModel, szModel ) ) )
                {
                    pPed->GetClothes ()->RemoveClothes ( ucType );

                    CBitStream BitStream;
                    BitStream.pBitStream->Write ( ucType );
                    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, REMOVE_PED_CLOTHES, *BitStream.pBitStream ) );

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
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, GIVE_PED_JETPACK, *BitStream.pBitStream ) );

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
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, REMOVE_PED_JETPACK, *BitStream.pBitStream ) );

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
                BitStream.pBitStream->Write ( ucStyle );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_FIGHTING_STYLE, *BitStream.pBitStream ) );

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
        if ( IsValidMoveAnim( iMoveAnim ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            if ( pPed->GetMoveAnim () != iMoveAnim )
            {
                pPed->SetMoveAnim ( iMoveAnim );

                CBitStream BitStream;
                BitStream.pBitStream->WriteCompressed ( iMoveAnim );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_MOVE_ANIM, *BitStream.pBitStream ) );

                return true;
            }
        }
    }
    return false;
}


#define MAX_GRAVITY 1.0f
#define MIN_GRAVITY -1.0f

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
                BitStream.pBitStream->Write ( fGravity );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_GRAVITY, *BitStream.pBitStream ) );
                return true;
            }
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
                    bitStream.pBitStream->WriteBit ( bChoking );
                    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_CHOKING, *bitStream.pBitStream ) );

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

            SWeaponSlotSync slot;
            slot.data.uiSlot = ucWeaponSlot;
            BitStream.pBitStream->Write ( &slot );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_WEAPON_SLOT, *BitStream.pBitStream ) );

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
                BitStream.pBitStream->Write ( pVehicle->GetID () );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( uiSeat ) );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, WARP_PED_INTO_VEHICLE, *BitStream.pBitStream ) );

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
        unsigned char ucOccupiedSeat = pPed->GetOccupiedVehicleSeat ();
        if ( pVehicle )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
            if ( pPlayer && IS_PLAYER( pElement ) ) {
                CLuaArguments Arguments;
                Arguments.PushElement ( pVehicle );        // vehicle
                Arguments.PushNumber ( ucOccupiedSeat );    // seat
                Arguments.PushBoolean ( false );            // jacker
                pPlayer->CallEvent ( "onPlayerVehicleExit", Arguments );

                // Call the vehicle->player event
                CLuaArguments Arguments2;
                Arguments2.PushElement ( pPlayer );         // player
                Arguments2.PushNumber ( ucOccupiedSeat );    // seat
                Arguments2.PushBoolean ( false );            // jacker
                pVehicle->CallEvent ( "onVehicleExit", Arguments2 );
            }
             // Remove him from the vehicle
            pVehicle->SetOccupant ( NULL, ucOccupiedSeat );
            pPed->SetOccupiedVehicle ( NULL, 0 );
            pPed->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, REMOVE_PED_FROM_VEHICLE, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
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
                BitStream.pBitStream->WriteBit ( bGangDriveby );
                BitStream.pBitStream->Write ( pPed->GenerateSyncTimeContext () );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_DOING_GANG_DRIVEBY, *BitStream.pBitStream ) );

                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedAnimation ( CElement * pElement, const char * szBlockName, const char * szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame )
{
    assert ( pElement );
    RUN_CHILDREN SetPedAnimation ( *iter, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame );

    if ( IS_PED ( pElement ) )
    {
        CPed * pPed = static_cast < CPed * > ( pElement );
        if ( pPed->IsSpawned () )
        {
            // TODO: save their animation?

            // Tell the players
            CBitStream BitStream;
            if ( szBlockName && szAnimName )
            {
                unsigned char ucBlockSize = ( unsigned char ) strlen ( szBlockName );
                unsigned char ucAnimSize = ( unsigned char ) strlen ( szAnimName );

                BitStream.pBitStream->Write ( ucBlockSize );
                BitStream.pBitStream->Write ( szBlockName, ucBlockSize );
                BitStream.pBitStream->Write ( ucAnimSize );
                BitStream.pBitStream->Write ( szAnimName, ucAnimSize );
                BitStream.pBitStream->Write ( iTime );
                BitStream.pBitStream->WriteBit ( bLoop );
                BitStream.pBitStream->WriteBit ( bUpdatePosition );
                BitStream.pBitStream->WriteBit ( bInterruptable );
                BitStream.pBitStream->WriteBit ( bFreezeLastFrame );
            }
            else
            {
                // Inform them to kill the current animation instead
                BitStream.pBitStream->Write ( ( unsigned char ) 0 );
            }
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_ANIMATION, *BitStream.pBitStream ) );

            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedAnimationProgress ( CElement * pElement, const char * szAnimName, float fProgress )
{
    assert ( pElement );
    RUN_CHILDREN SetPedAnimationProgress ( *iter, szAnimName, fProgress );

    if ( IS_PED ( pElement ) )
    {
        CPed * pPed = static_cast < CPed * > ( pElement );
        if ( pPed->IsSpawned () )
        {
            CBitStream BitStream;
            if ( szAnimName )
            {
                unsigned char ucAnimSize = ( unsigned char ) strlen ( szAnimName );

                BitStream.pBitStream->Write ( ucAnimSize );
                BitStream.pBitStream->Write ( szAnimName, ucAnimSize );
                BitStream.pBitStream->Write ( fProgress );
            }
            else
            {
                // Inform them to kill the current animation instead
                BitStream.pBitStream->Write ( ( unsigned char ) 0 );
            }
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_ANIMATION_PROGRESS, *BitStream.pBitStream ) );

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

        BitStream.pBitStream->WriteBit ( bIsOnFire );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_ON_FIRE, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedHeadless ( CElement * pElement, bool bIsHeadless )
{
    assert ( pElement );
    RUN_CHILDREN SetPedHeadless ( *iter, bIsHeadless );

    if ( IS_PED ( pElement ) )
    {
        CBitStream BitStream;
        CPed * pPed = static_cast < CPed * > ( pElement );

        pPed->SetHeadless ( bIsHeadless );

        BitStream.pBitStream->WriteBit ( bIsHeadless );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_HEADLESS, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPedFrozen ( CElement * pElement, bool bIsFrozen )
{
    assert ( pElement );
    RUN_CHILDREN SetPedFrozen ( *iter, bIsFrozen );

    if ( IS_PED ( pElement ) )
    {
        CBitStream BitStream;
        CPed * pPed = static_cast < CPed * > ( pElement );

        pPed->SetFrozen ( bIsFrozen );

        BitStream.pBitStream->WriteBit ( bIsFrozen );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_PED_FROZEN, *BitStream.pBitStream ) );

        return true;
    }
    return false;
}
bool CStaticFunctionDefinitions::reloadPedWeapon ( CElement* pElement )
{
    assert ( pElement );
    RUN_CHILDREN reloadPedWeapon ( *iter );

    if ( IS_PED ( pElement ) )
    {
        CPed * pPed = static_cast < CPed * > ( pElement );
        CBitStream BitStream;
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, RELOAD_PED_WEAPON, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetCameraMatrix ( CPlayer* pPlayer, CVector& vecPosition, CVector& vecLookAt, float& fRoll, float& fFOV )
{
    assert ( pPlayer );

    CPlayerCamera * pCamera = pPlayer->GetCamera ();

    // Only allow this if we're in fixed mode?
    if ( pCamera->GetMode () == CAMERAMODE_FIXED )
    {
        pCamera->GetPosition ( vecPosition );
        pCamera->GetLookAt ( vecLookAt );
        fRoll = pCamera->GetRoll ();
        fFOV = pCamera->GetFOV ();
        return true;
    }
    return false;
}


CElement* CStaticFunctionDefinitions::GetCameraTarget ( CPlayer* pPlayer )
{
    assert ( pPlayer );
    CPlayerCamera * pCamera = pPlayer->GetCamera ();

    // Only allow this if we're targeting a player
    if ( pCamera->GetMode () == CAMERAMODE_PLAYER )
    {
        return pCamera->GetTarget ();
    }
    return NULL;
}


bool CStaticFunctionDefinitions::GetCameraInterior ( CPlayer * pPlayer, unsigned char & ucInterior )
{
    assert ( pPlayer );
    ucInterior = pPlayer->GetCamera ()->GetInterior ();
    return true;
}


bool CStaticFunctionDefinitions::SetCameraMatrix ( CElement* pElement, const CVector& vecPosition, CVector* pvecLookAt, float fRoll, float fFOV )
{
    assert ( pElement );
    RUN_CHILDREN SetCameraMatrix ( *iter, vecPosition, pvecLookAt, fRoll, fFOV );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        CPlayerCamera * pCamera = pPlayer->GetCamera ();

        pCamera->SetMode ( CAMERAMODE_FIXED );
        if ( pvecLookAt )
            pCamera->SetMatrix ( vecPosition, *pvecLookAt );
        else
            pCamera->SetPosition ( vecPosition );

        CVector vecLookAt;
        if ( pvecLookAt )
            vecLookAt = *pvecLookAt;
        else
            pCamera->GetLookAt ( vecLookAt );

        pCamera->SetRoll ( fRoll );
        pCamera->SetFOV ( fFOV );
        
        // Tell the player
        CBitStream BitStream;
        BitStream.pBitStream->Write ( vecPosition.fX );
        BitStream.pBitStream->Write ( vecPosition.fY );
        BitStream.pBitStream->Write ( vecPosition.fZ );
        BitStream.pBitStream->Write ( vecLookAt.fX );
        BitStream.pBitStream->Write ( vecLookAt.fY );
        BitStream.pBitStream->Write ( vecLookAt.fZ );
        if ( fRoll != 0.0f || fFOV != 70.0f )
        {
            BitStream.pBitStream->Write ( fRoll );
            BitStream.pBitStream->Write ( fFOV );
        }
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
        if ( !pTarget )
            pTarget = pPlayer;

        // Make sure our target is a player element
        if ( pTarget->GetType () == CElement::PLAYER )
        {
            pCamera->SetMode ( CAMERAMODE_PLAYER );
            pCamera->SetTarget ( pTarget );
            pCamera->SetRoll ( 0.0f );
            pCamera->SetFOV ( 70.0f );

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

        unsigned char ucFadeIn = bFadeIn ? 1:0;

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
                unsigned char ucCurrentWeapon = pPed->GetWeaponType ();
                if ( ucCurrentWeapon != ucWeaponID && bSetAsCurrent )
                {
                    // Call our weapon switch command
                    CLuaArguments Arguments;
                    Arguments.PushNumber ( ucCurrentWeapon );
                    Arguments.PushNumber ( ucWeaponID );
                    bool bEventRet;
                    if ( IS_PLAYER ( pElement ) )
                        bEventRet = pPed->CallEvent ( "onPlayerWeaponSwitch", Arguments );
                    else
                        bEventRet = pPed->CallEvent ( "onPedWeaponSwitch", Arguments );

                    if ( !bEventRet )
                        bSetAsCurrent = false;
                }

                unsigned char ucWeaponSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
                pPed->SetWeaponType ( ucWeaponID, ucWeaponSlot );
                if ( bSetAsCurrent )
                    pPed->SetWeaponSlot ( ucWeaponSlot );

                unsigned int uiTotalAmmo = pPed->GetWeaponTotalAmmo ( ucWeaponSlot );

                // Client ammo emulation mode - Try to ensure that the ammo we set on the server will be the same as the client)
                if ( ucWeaponSlot <= 1 || ucWeaponSlot >= 10 )
                    uiTotalAmmo = Min( 1U, uiTotalAmmo + usAmmo );  // If slot 0,1,10,11,12 - Ammo is max 1
                else
                if ( ( ucWeaponSlot >= 3 && ucWeaponSlot <= 5 ) || ucCurrentWeapon == ucWeaponID )
                    uiTotalAmmo += usAmmo;                          // If slot 3,4,5 or weapon the same, ammo is shared, so add
                else
                    uiTotalAmmo = usAmmo;                           // Otherwise ammo is not shared, so replace

                uiTotalAmmo = Min( 0xFFFFU, uiTotalAmmo );
                pPed->SetWeaponTotalAmmo ( uiTotalAmmo, ucWeaponSlot );

                CBitStream BitStream;

                SWeaponTypeSync weaponType;
                weaponType.data.ucWeaponType = ucWeaponID;
                BitStream.pBitStream->Write ( &weaponType );

                SWeaponAmmoSync ammo ( ucWeaponID, true, false );
                ammo.data.usTotalAmmo = usAmmo;
                BitStream.pBitStream->Write ( &ammo );

                BitStream.pBitStream->WriteBit ( bSetAsCurrent );

                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, GIVE_WEAPON, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::TakeWeapon ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo )
{
    assert ( pElement );
    RUN_CHILDREN TakeWeapon ( *iter, ucWeaponID, usAmmo );

    if ( CPickupManager::IsValidWeaponID ( ucWeaponID ) )
    {
        if ( IS_PED ( pElement ) )
        {
            CPed* pPed = static_cast < CPed* > ( pElement );
            unsigned char ucWeaponSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
            // Just because it's the same slot doesn't mean it's the same weapon -_- - Caz
            if ( pPed->IsSpawned () && pPed->GetWeapon ( ucWeaponSlot ) && pPed->GetWeaponType ( ucWeaponSlot ) == ucWeaponID  )
            {
                CBitStream BitStream;

                SWeaponTypeSync weaponType;
                weaponType.data.ucWeaponType = ucWeaponID;
                BitStream.pBitStream->Write ( &weaponType );
                if ( usAmmo < 9999 )
                {
                    SWeaponAmmoSync ammo ( ucWeaponID, true, false );
                    ammo.data.usTotalAmmo = usAmmo;
                    BitStream.pBitStream->Write ( &ammo );
                }

                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, TAKE_WEAPON, *BitStream.pBitStream ) );

                if ( usAmmo < 9999 )
                {
                    unsigned short usTotalAmmo = pPed->GetWeaponTotalAmmo ( ucWeaponSlot );
                    if ( usAmmo > usTotalAmmo )
                    {
                        usTotalAmmo = 0;
                        pPed->SetWeaponType ( 0, ucWeaponSlot );
                        pPed->SetWeaponAmmoInClip ( 0, ucWeaponSlot );
                    }
                    else
                        usTotalAmmo -= usAmmo;
                    pPed->SetWeaponTotalAmmo ( usTotalAmmo, ucWeaponSlot );
                }
                else
                {
                    pPed->SetWeaponType ( 0, ucWeaponSlot );
                    pPed->SetWeaponAmmoInClip ( 0, ucWeaponSlot );
                    pPed->SetWeaponTotalAmmo ( 0, ucWeaponSlot );
                }

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
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, TAKE_ALL_WEAPONS, *BitStream.pBitStream ) );

            for ( unsigned char ucWeaponSlot = 0; ucWeaponSlot < WEAPON_SLOTS; ++ ucWeaponSlot )
            {
                pPed->SetWeaponType ( 0, ucWeaponSlot );
                pPed->SetWeaponAmmoInClip ( 0, ucWeaponSlot );
                pPed->SetWeaponTotalAmmo ( 0, ucWeaponSlot );
            }

            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWeaponAmmo ( CElement* pElement, unsigned char ucWeaponID, unsigned short usAmmo, unsigned short usAmmoInClip )
{
    assert ( pElement );
    RUN_CHILDREN SetWeaponAmmo ( *iter, ucWeaponID, usAmmo, usAmmoInClip );

    if ( IS_PED ( pElement ) )
    {
        CPed* pPed = static_cast < CPed* > ( pElement );
        CWeapon* pWeapon = pPed->GetWeapon ( CWeaponNames::GetSlotFromWeapon ( ucWeaponID ) );
        if ( pPed->IsSpawned () && pWeapon ) //Check We have that weapon
        {
            unsigned char ucWeaponID = pWeapon->ucType;
            if ( ucWeaponID ) {
                CBitStream BitStream;

                SWeaponTypeSync weaponType;
                weaponType.data.ucWeaponType = ucWeaponID;
                BitStream.pBitStream->Write ( &weaponType );

                SWeaponAmmoSync ammo ( ucWeaponID, true, true );
                ammo.data.usTotalAmmo = usAmmo;
                ammo.data.usAmmoInClip = usAmmoInClip;
                BitStream.pBitStream->Write ( &ammo );

                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPed, SET_WEAPON_AMMO, *BitStream.pBitStream ) );

                unsigned char ucWeaponSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
                pPed->SetWeaponAmmoInClip ( usAmmoInClip, ucWeaponSlot );
                pPed->SetWeaponTotalAmmo ( usAmmo, ucWeaponSlot );

                return true;
            }
        }
    }

    return false;
}


CVehicle* CStaticFunctionDefinitions::CreateVehicle ( CResource* pResource, unsigned short usModel, const CVector& vecPosition, const CVector& vecRotation, const char* szRegPlate, unsigned char ucVariant, unsigned char ucVariant2 )
{
    unsigned char ucVariation = ucVariant;
    unsigned char ucVariation2 = ucVariant2;
    if ( ucVariant == 254 && ucVariant2 == 254 )
        CVehicleManager::GetRandomVariation ( usModel, ucVariation, ucVariation2 );

    if ( CVehicleManager::IsValidModel ( usModel ) && ( ucVariation <= 5 || ucVariation == 255 ) && ( ucVariation2 <= 5 || ucVariation2 == 255 ) )
    {
        //CVehicle* pVehicle = m_pVehicleManager->Create ( usModel, m_pMapManager->GetRootElement () );
        CVehicle* pVehicle = m_pVehicleManager->Create ( usModel, ucVariation, ucVariation2, pResource->GetDynamicElementRoot() );
        if ( pVehicle )
        {
            pVehicle->SetPosition ( vecPosition );
            pVehicle->SetRotationDegrees ( vecRotation );

            pVehicle->SetRespawnPosition ( vecPosition );
            pVehicle->SetRespawnRotationDegrees ( vecRotation );

            if ( szRegPlate && szRegPlate[0] )
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

bool CStaticFunctionDefinitions::SetVehicleVariant ( CVehicle* pVehicle, unsigned char ucVariant, unsigned char ucVariant2 )
{
    assert ( pVehicle );
    unsigned char ucVariation = ucVariant;
    unsigned char ucVariation2 = ucVariant2;
    if ( ucVariant == 254 && ucVariant2 == 254 )
        CVehicleManager::GetRandomVariation ( pVehicle->GetModel(), ucVariation, ucVariation2 );

    if ( ( ucVariation <= 5 || ucVariation == 255 ) && ( ucVariation2 <= 5 || ucVariation2 == 255 ) )
    {
        pVehicle->SetVariants ( ucVariation, ucVariation2 );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucVariation );
        BitStream.pBitStream->Write ( ucVariation2 );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_VARIANT, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GiveVehicleSirens( CVehicle* pVehicle, unsigned char ucSirenType, unsigned char ucSirenCount, SSirenInfo tSirenInfo )
{
    assert ( pVehicle );
    eVehicleType vehicleType = CVehicleManager::GetVehicleType ( pVehicle->GetModel ( ) );
    // Won't work with below.
    if ( vehicleType != VEHICLE_PLANE && vehicleType != VEHICLE_BOAT && vehicleType != VEHICLE_TRAILER && vehicleType != VEHICLE_HELI && vehicleType != VEHICLE_BIKE && vehicleType != VEHICLE_BMX )
    {
        if ( ucSirenType >= 1 && ucSirenType <= 6 )
        {
            if ( ucSirenCount <= SIREN_COUNT_MAX )
            {
                pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens = true;

                pVehicle->m_tSirenBeaconInfo.m_ucSirenCount = ucSirenCount;
                pVehicle->m_tSirenBeaconInfo.m_ucSirenType = ucSirenType;

                pVehicle->m_tSirenBeaconInfo.m_b360Flag = tSirenInfo.m_b360Flag;
                pVehicle->m_tSirenBeaconInfo.m_bDoLOSCheck = tSirenInfo.m_bDoLOSCheck;
                pVehicle->m_tSirenBeaconInfo.m_bUseRandomiser = tSirenInfo.m_bUseRandomiser;
                pVehicle->m_tSirenBeaconInfo.m_bSirenSilent = tSirenInfo.m_bSirenSilent;


                SVehicleSirenAddSync tSirenSync;
                tSirenSync.data.m_bOverrideSirens =  pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens;
                tSirenSync.data.m_b360Flag = pVehicle->m_tSirenBeaconInfo.m_b360Flag;
                tSirenSync.data.m_bDoLOSCheck = pVehicle->m_tSirenBeaconInfo.m_bDoLOSCheck;
                tSirenSync.data.m_bEnableSilent = pVehicle->m_tSirenBeaconInfo.m_bSirenSilent;
                tSirenSync.data.m_bUseRandomiser = pVehicle->m_tSirenBeaconInfo.m_bUseRandomiser;
                tSirenSync.data.m_ucSirenCount =  pVehicle->m_tSirenBeaconInfo.m_ucSirenCount;
                tSirenSync.data.m_ucSirenType =  pVehicle->m_tSirenBeaconInfo.m_ucSirenType;

                CBitStream BitStream;
                BitStream.pBitStream->Write ( &tSirenSync );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, GIVE_VEHICLE_SIRENS, *BitStream.pBitStream ) );
                return true;
            }
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetVehicleSirens ( CVehicle* pVehicle, unsigned char ucSirenID, SSirenInfo tSirenInfo )
{
    assert ( pVehicle );
    eVehicleType vehicleType = CVehicleManager::GetVehicleType ( pVehicle->GetModel ( ) );
    // Won't work with below.
    if ( vehicleType != VEHICLE_PLANE && vehicleType != VEHICLE_BOAT && vehicleType != VEHICLE_TRAILER && vehicleType != VEHICLE_HELI && vehicleType != VEHICLE_BIKE && vehicleType != VEHICLE_BMX )
    {
        if ( ucSirenID <= SIREN_ID_MAX )
        {
            pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[ ucSirenID ] = tSirenInfo.m_tSirenInfo[ ucSirenID ];

            SVehicleSirenSync tSirenSync;
            tSirenSync.data.m_bOverrideSirens =  pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens;
            tSirenSync.data.m_b360Flag = pVehicle->m_tSirenBeaconInfo.m_b360Flag;
            tSirenSync.data.m_bDoLOSCheck = pVehicle->m_tSirenBeaconInfo.m_bDoLOSCheck;
            tSirenSync.data.m_bEnableSilent = pVehicle->m_tSirenBeaconInfo.m_bSirenSilent;
            tSirenSync.data.m_bUseRandomiser = pVehicle->m_tSirenBeaconInfo.m_bUseRandomiser;
            tSirenSync.data.m_vecSirenPositions = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions;
            tSirenSync.data.m_colSirenColour = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour;
            tSirenSync.data.m_dwSirenMinAlpha = pVehicle->m_tSirenBeaconInfo.m_tSirenInfo[ ucSirenID ].m_dwMinSirenAlpha;
            tSirenSync.data.m_ucSirenID = ucSirenID;

            CBitStream BitStream;
            BitStream.pBitStream->Write ( &tSirenSync );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_SIRENS, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::RemoveVehicleSirens ( CVehicle* pVehicle )
{
    assert ( pVehicle );

    pVehicle->m_tSirenBeaconInfo.m_bOverrideSirens = false;
    pVehicle->RemoveVehicleSirens ( );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, REMOVE_VEHICLE_SIRENS, *BitStream.pBitStream ) );
    return true;
}

bool CStaticFunctionDefinitions::GetVehicleVariant ( CVehicle* pVehicle, unsigned char& ucVariant, unsigned char& ucVariant2 )
{
    assert ( pVehicle );
    ucVariant = pVehicle->GetVariant ();
    ucVariant2 = pVehicle->GetVariant2 ();
    return true;
}

bool CStaticFunctionDefinitions::GetVehicleColor ( CVehicle* pVehicle, CVehicleColor& color )
{
    assert ( pVehicle );
    color = pVehicle->GetColor ();
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


bool CStaticFunctionDefinitions::GetVehicleName ( CVehicle* pVehicle, SString& strOutName )
{
    assert ( pVehicle );

    strOutName = CVehicleNames::GetVehicleName ( pVehicle->GetModel () );
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleNameFromModel ( unsigned short usModel, SString& strOutName )
{
    strOutName = CVehicleNames::GetVehicleName ( usModel );
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


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned char ucSlot,SString& strOutName )
{
    strOutName = CVehicleUpgrades::GetSlotName ( ucSlot );
    return true;
}


bool CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( unsigned short usUpgrade, SString& strOutName )
{
    unsigned char ucSlot;
    if ( CVehicleUpgrades::GetSlotFromUpgrade ( usUpgrade, ucSlot ) )
    {
        strOutName = CVehicleUpgrades::GetSlotName ( ucSlot );
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


bool CStaticFunctionDefinitions::GetVehicleWheelStates ( CVehicle* pVehicle, unsigned char& ucFrontLeft, unsigned char& ucRearLeft, unsigned char& ucFrontRight, unsigned char& ucRearRight )
{
    assert ( pVehicle );

    ucFrontLeft = pVehicle->m_ucWheelStates [ FRONT_LEFT_WHEEL ];
    ucRearLeft = pVehicle->m_ucWheelStates [ REAR_LEFT_WHEEL ];
    ucFrontRight = pVehicle->m_ucWheelStates [ FRONT_RIGHT_WHEEL ];
    ucRearRight = pVehicle->m_ucWheelStates [ REAR_RIGHT_WHEEL ];
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


bool CStaticFunctionDefinitions::IsVehicleDamageProof ( CVehicle* pVehicle, bool& bDamageProof )
{
    assert ( pVehicle );

    bDamageProof = pVehicle->IsDamageProof ();

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

    bFrozen = pVehicle->IsFrozen ();

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


bool CStaticFunctionDefinitions::IsTrainDerailed ( CVehicle* pVehicle, bool& bDerailed )
{
    assert ( pVehicle );

    bDerailed = pVehicle->IsDerailed ();
    return true;
}

bool CStaticFunctionDefinitions::IsTrainDerailable ( CVehicle* pVehicle, bool& bDerailable )
{
    assert ( pVehicle );

    bDerailable = pVehicle->IsDerailable ();
    return true;
}


bool CStaticFunctionDefinitions::GetTrainDirection ( CVehicle* pVehicle, bool& bDirection )
{
    assert ( pVehicle );

    bDirection = pVehicle->GetTrainDirection ();
    return true;
}


bool CStaticFunctionDefinitions::GetTrainSpeed ( CVehicle* pVehicle, float& fSpeed )
{
    assert ( pVehicle );

    const CVector& vecVelocity = pVehicle->GetVelocity ();
    fSpeed = vecVelocity.Length ();
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
        pVehicle->ResetDoorsWheelsPanelsLights ();
        pVehicle->SetIsBlown ( false );

        // Tell everyone
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GetSyncTimeContext () );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, FIX_VEHICLE, *BitStream.pBitStream ) );

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
        if ( IsVehicleBlown( pVehicle ) == false ) {
            // Call the onVehicleExplode event
            CLuaArguments Arguments;
            pVehicle->CallEvent ( "onVehicleExplode", Arguments );
        }
        pVehicle->SetHealth ( 0.0f );
        if ( !bExplode )
            pVehicle->SetIsBlown ( true );

        //Update our engine State
        pVehicle->SetEngineOn( false );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( pVehicle->GenerateSyncTimeContext() );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, BLOW_VEHICLE, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}
bool CStaticFunctionDefinitions::IsVehicleBlown ( CVehicle* pVehicle )
{
    assert ( pVehicle );
    return pVehicle->GetIsBlown ();
}


bool CStaticFunctionDefinitions::GetVehicleHeadLightColor ( CVehicle * pVehicle, SColor& outColor )
{
    assert ( pVehicle );
    
    outColor = pVehicle->GetHeadLightColor ();
    return true;
}

bool CStaticFunctionDefinitions::GetVehicleDoorOpenRatio ( CVehicle* pVehicle, unsigned char ucDoor, float& fRatio )
{
    if ( ucDoor <= 5 && pVehicle != NULL )
    {
        fRatio = pVehicle->GetDoorOpenRatio ( ucDoor );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, CVector& vecValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( eProperty == HANDLING_CENTEROFMASS )
    {
        vecValue = pEntry->GetCenterOfMass ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, float &fValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, fValue ) )
        return true;

    return false;
}


bool CStaticFunctionDefinitions::GetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, std::string& strValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, strValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, unsigned int& uiValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, uiValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, unsigned char& ucValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( GetEntryHandling ( pEntry, eProperty, ucValue ) )
        return true;

    return false;
}

bool CStaticFunctionDefinitions::GetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, CVector& vecValue, bool bOriginal )
{
    const CHandlingEntry* pEntry = NULL;
    if ( bOriginal )
    {
        pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
    }
    else
    {
        pEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    }

    if ( pEntry )
    {
        if ( eProperty == HANDLING_CENTEROFMASS )
        {
            vecValue = pEntry->GetCenterOfMass ();
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, float &fValue, bool bOriginal )
{
    const CHandlingEntry* pEntry = NULL;
    if ( bOriginal )
    {
        pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
    }
    else
    {
        pEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    }

    if ( pEntry )
    {
        if ( GetEntryHandling ( pEntry, eProperty, fValue ) )
        {
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, unsigned int &uiValue, bool bOriginal )
{
    const CHandlingEntry* pEntry = NULL;
    if ( bOriginal )
    {
        pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
    }
    else
    {
        pEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    }

    if ( pEntry )
    {
        if ( GetEntryHandling ( pEntry, eProperty, uiValue ) )
        {
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, unsigned char &ucValue, bool bOriginal )
{
    const CHandlingEntry* pEntry = NULL;
    if ( bOriginal )
    {
        pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
    }
    else
    {
        pEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    }

    if ( pEntry )
    {
        if ( GetEntryHandling ( pEntry, eProperty, ucValue ) )
        {
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, std::string& strValue, bool bOriginal )
{
    const CHandlingEntry* pEntry = NULL;
    if ( bOriginal )
    {
        pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
    }
    else
    {
        pEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    }

    if ( pEntry )
    {
        if ( GetEntryHandling ( pEntry, eProperty, strValue ) )
        {
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, float fValue )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData ( eModel );
    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, fValue ) )
        {
            g_pGame->GetHandlingManager()->SetModelHandlingHasChanged ( eModel, true );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::SetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, CVector vecValue )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData ( eModel );

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, vecValue ) )
        {
            g_pGame->GetHandlingManager()->SetModelHandlingHasChanged ( eModel, true );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, std::string strValue )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData ( eModel );
    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, strValue ) )
        {
            g_pGame->GetHandlingManager()->SetModelHandlingHasChanged ( eModel, true );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::SetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, unsigned char ucValue )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData ( eModel );
    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, ucValue ) )
        {
            g_pGame->GetHandlingManager()->SetModelHandlingHasChanged ( eModel, true );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::SetModelHandling ( eVehicleTypes eModel, eHandlingProperty eProperty, unsigned int uiValue )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData ( eModel );
    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, uiValue ) )
        {
            g_pGame->GetHandlingManager()->SetModelHandlingHasChanged ( eModel, true );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::GetEntryHandling ( const CHandlingEntry* pEntry, eHandlingProperty eProperty, float &fValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_MASS:
                fValue = pEntry->GetMass ( );
                break;
            case HANDLING_TURNMASS:
                fValue = pEntry->GetTurnMass ( );
                break;
            case HANDLING_DRAGCOEFF:
                fValue = pEntry->GetDragCoeff ( );
                break;
            case HANDLING_TRACTIONMULTIPLIER:
                fValue = pEntry->GetTractionMultiplier ( );
                break;
            case HANDLING_ENGINEACCELERATION:
                fValue = pEntry->GetEngineAcceleration ( );
                break;
            case HANDLING_ENGINEINERTIA:
                fValue = pEntry->GetEngineInertia ( );
                break;
            case HANDLING_MAXVELOCITY:
                fValue = pEntry->GetMaxVelocity ( );
                break;
            case HANDLING_BRAKEDECELERATION:
                fValue = pEntry->GetBrakeDeceleration ( );
                break;
            case HANDLING_BRAKEBIAS:
                fValue = pEntry->GetBrakeBias ( );
                break;
            case HANDLING_STEERINGLOCK:
                fValue = pEntry->GetSteeringLock ( );
                break;
            case HANDLING_TRACTIONLOSS:
                fValue = pEntry->GetTractionLoss ( );
                break;
            case HANDLING_TRACTIONBIAS:
                fValue = pEntry->GetTractionBias ( );
                break;
            case HANDLING_SUSPENSION_FORCELEVEL:
                fValue = pEntry->GetSuspensionForceLevel ( );
                break;
            case HANDLING_SUSPENSION_DAMPING:
                fValue = pEntry->GetSuspensionDamping ( );
                break;
            case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                fValue = pEntry->GetSuspensionHighSpeedDamping ( );
                break;
            case HANDLING_SUSPENSION_UPPER_LIMIT:
                fValue = pEntry->GetSuspensionUpperLimit ( );
                break;
            case HANDLING_SUSPENSION_LOWER_LIMIT:
                fValue = pEntry->GetSuspensionLowerLimit ( );
                break;
            case HANDLING_SUSPENSION_FRONTREARBIAS:
                fValue = pEntry->GetSuspensionFrontRearBias ( );
                break;
            case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                fValue = pEntry->GetSuspensionAntiDiveMultiplier ( );
                break;
            case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                fValue = pEntry->GetCollisionDamageMultiplier ( );
                break;
            case HANDLING_SEATOFFSETDISTANCE:
                fValue = pEntry->GetSeatOffsetDistance ( );
                break;
            case HANDLING_ABS: // bool
                fValue = (float)(pEntry->GetABS () ? 1 : 0);
                break;
            default:
                return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( const CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int &uiValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_PERCENTSUBMERGED: // unsigned int
                uiValue = pEntry->GetPercentSubmerged ( );
                break;
            case HANDLING_MONETARY:
                uiValue = pEntry->GetMonetary ( );
                break;
            case HANDLING_HANDLINGFLAGS:
                uiValue = pEntry->GetHandlingFlags ( );
                break;
            case HANDLING_MODELFLAGS:
                uiValue = pEntry->GetModelFlags ( );
                break;
            default:
                return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( const CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char &ucValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_NUMOFGEARS:
                ucValue = pEntry->GetNumberOfGears ( );
                break;
            case HANDLING_ANIMGROUP:
                ucValue = pEntry->GetAnimGroup ( );
                break;
            case HANDLING_DRIVETYPE:
                ucValue = pEntry->GetCarDriveType ( );
                break;
            case HANDLING_ENGINETYPE:
                ucValue = pEntry->GetCarEngineType ( );
                break;
            default:
                return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::GetEntryHandling ( const CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string& strValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_DRIVETYPE:
            {
                CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ( );
                if ( eDriveType == CHandlingEntry::FWD )
                    strValue = "fwd";
                else if ( eDriveType == CHandlingEntry::RWD )
                    strValue = "rwd";
                else if (eDriveType == CHandlingEntry::FOURWHEEL )
                    strValue = "awd";
                else
                    return false;
                break;
            }
            case HANDLING_ENGINETYPE:
            {
                CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType ( );
                if ( eEngineType == CHandlingEntry::PETROL )
                    strValue = "petrol";
                else if ( eEngineType == CHandlingEntry::DIESEL )
                    strValue = "diesel";
                else if ( eEngineType == CHandlingEntry::ELECTRIC )
                    strValue = "electric";
                else
                    return false;
                break;
            }
            case HANDLING_HEADLIGHT:
            {
                CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight ( );
                if ( eHeadType == CHandlingEntry::SMALL )
                    strValue = "small";
                else if ( eHeadType == CHandlingEntry::LONG )
                    strValue = "long";
                else if ( eHeadType == CHandlingEntry::BIG )
                    strValue = "big";
                else if ( eHeadType == CHandlingEntry::TALL )
                    strValue = "tall";
                else
                    return false;
                break;
            }
            case HANDLING_TAILLIGHT:
            {
                CHandlingEntry::eLightType eTailType = pEntry->GetTailLight ( );
                if ( eTailType == CHandlingEntry::SMALL )
                    strValue = "small";
                else if ( eTailType == CHandlingEntry::LONG )
                    strValue = "long";
                else if ( eTailType == CHandlingEntry::BIG )
                    strValue = "big";
                else if ( eTailType == CHandlingEntry::TALL )
                    strValue = "tall";
                else
                    return false;
                break;
            }
            default:
                return false;
        }
    }
    else
        return false;

    return true;
}

bool CStaticFunctionDefinitions::SetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned int uiValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_PERCENTSUBMERGED:
            {
                if ( uiValue > 0 && uiValue <= 200 )
                {
                    pEntry->SetPercentSubmerged ( uiValue );
                    return true;
                }
                break;
            }
            /*case HANDLING_MONETARY:
                pEntry->SetMonetary ( uiValue );
                break;*/
            case HANDLING_HANDLINGFLAGS:
            {
                // Disable NOS and Hydraulic installed properties.
                if ( uiValue & 0x00080000 )
                   uiValue &= ~0x00080000;
                if ( uiValue & 0x00020000 )
                   uiValue &= ~0x00020000;

                pEntry->SetHandlingFlags ( uiValue );
                return true;
            }
            case HANDLING_MODELFLAGS:
            {
                pEntry->SetModelFlags ( uiValue );
                return true;
            }
            default:
            {
                    return false;
            }
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, unsigned char ucValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_NUMOFGEARS:
            {
                if ( ucValue > 0 && ucValue <= 5 )
                {
                    pEntry->SetNumberOfGears ( ucValue );
                    return true;
                }
                break;
            }
            case HANDLING_ANIMGROUP:
            {
                if ( ucValue >= 0 && ucValue <= 29 )
                {
                    if ( ucValue != 3 && ucValue != 8 
                        && ucValue != 17 && ucValue != 23 ) 
                        return true;    // Pretend it worked to avoid script warnings

                    pEntry->SetAnimGroup ( ucValue );
                    return true;
                }
                break;
            }
            default:
            {
                return false;
            }
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, float fValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_MASS:
            {
                if ( fValue > 0 && fValue <= 100000 )
                {
                    pEntry->SetMass ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_TURNMASS:
            {
                if ( fValue > 0 && fValue <= 10000000 )
                {  
                    pEntry->SetTurnMass ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_DRAGCOEFF:
            {
                if ( fValue >= -200 && fValue <= 200 )
                { 
                    pEntry->SetDragCoeff ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_TRACTIONMULTIPLIER:
            {
                if ( fValue >= -100000 && fValue <= 100000 )
                {
                    pEntry->SetTractionMultiplier ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_ENGINEACCELERATION:
            {
                if ( fValue >= 0 && fValue <= 100000 )
                {
                    pEntry->SetEngineAcceleration ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_ENGINEINERTIA:
            {
                if ( fValue >= -1000 && fValue <= 1000 && fValue != 0.0 )
                {
                    pEntry->SetEngineInertia ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_MAXVELOCITY:
            { 
                if ( fValue >= 0.0 && fValue <= 200000 )
                {
                    pEntry->SetMaxVelocity ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_BRAKEDECELERATION:
            {
                if ( fValue >= 0.0 && fValue <= 100000 )
                {
                    pEntry->SetBrakeDeceleration ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_BRAKEBIAS:
            {
                if ( fValue >= 0.0 && fValue <= 1.0 )
                {
                    pEntry->SetBrakeBias ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_STEERINGLOCK:
            {
                if ( fValue >= 0.0 && fValue <= 360 )
                {
                    pEntry->SetSteeringLock ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_TRACTIONLOSS:
            {            
                if ( fValue >= 0.0 && fValue <= 100 )
                {
                    pEntry->SetTractionLoss ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_TRACTIONBIAS:
            {
                if ( fValue >= 0.0 && fValue <= 1.0 )
                {
                    pEntry->SetTractionBias ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_FORCELEVEL:
            {
                if ( fValue > 0.0 && fValue <= 100 )
                {
                    pEntry->SetSuspensionForceLevel ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_DAMPING:
            {
                if ( fValue > 0.0 && fValue <= 100 )
                {
                    pEntry->SetSuspensionDamping ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
            {
                if ( fValue >= 0.0 && fValue <= 600 )
                {
                    pEntry->SetSuspensionHighSpeedDamping ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_UPPER_LIMIT:
            {
                if ( fValue >= -50.0 && fValue <= 50 )
                {
                    pEntry->SetSuspensionUpperLimit ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_LOWER_LIMIT:
            {
                if ( fValue >= -50.0 && fValue <= 50 )
                {
                    pEntry->SetSuspensionLowerLimit ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_FRONTREARBIAS:
            {
                if ( fValue >= 0.0 && fValue <= 3.0 )
                {
                    pEntry->SetSuspensionFrontRearBias ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
            {
                if ( fValue >= 0.0 && fValue <= 30 )
                {
                    pEntry->SetSuspensionAntiDiveMultiplier ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_COLLISIONDAMAGEMULTIPLIER:
            {
                if ( fValue >= 0.0 && fValue <= 100 )
                {
                    pEntry->SetCollisionDamageMultiplier ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_SEATOFFSETDISTANCE:
            {
                if ( fValue >= -20 && fValue <= 20 )
                {
                    pEntry->SetSeatOffsetDistance ( fValue );
                    return true;
                }
                break;
            }
            case HANDLING_ABS:
            {
                pEntry->SetABS ( ( fValue > 0.0f ) ? true : false );
                return true;
            }
            default:
            {
                return false;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::SetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, CVector vecValue )
{
    if ( pEntry )
    {
        if ( eProperty == HANDLING_CENTEROFMASS )
        {
            if ( vecValue.fX >= -10.0 && vecValue.fX <= 10.0 &&
                vecValue.fY >= -10.0 && vecValue.fY <= 10.0 &&
                vecValue.fZ >= -10.0 && vecValue.fZ <= 10.0 )
            {
                pEntry->SetCenterOfMass ( vecValue );
                return true;
            }
        }
    }


    return false;
}


bool CStaticFunctionDefinitions::SetEntryHandling ( CHandlingEntry* pEntry, eHandlingProperty eProperty, std::string strValue )
{
    if ( pEntry )
    {
        switch ( eProperty )
        {
            case HANDLING_DRIVETYPE:
            {
                if ( strValue == "fwd" )
                {
                    pEntry->SetCarDriveType ( CHandlingEntry::FWD );
                    return true;
                }
                else if ( strValue == "rwd" )
                {
                    pEntry->SetCarDriveType ( CHandlingEntry::RWD );
                    return true;
                }
                else if ( strValue == "awd" )
                {
                    pEntry->SetCarDriveType ( CHandlingEntry::FOURWHEEL );
                    return true;
                }
                else
                    return false;
                break;
            }
            case HANDLING_ENGINETYPE:
            {
                if ( strValue == "petrol" )
                {
                    pEntry->SetCarEngineType ( CHandlingEntry::PETROL );
                    return true;
                }
                else if ( strValue == "diesel" )
                {
                    pEntry->SetCarEngineType ( CHandlingEntry::DIESEL );
                    return true;
                }
                else if ( strValue == "electric" )
                {
                    pEntry->SetCarEngineType ( CHandlingEntry::ELECTRIC );
                    return true;
                }
                else
                    return false;
                break;
            }
            /*case HANDLING_HEADLIGHT:
            {
                if ( strValue == "small" )
                {
                    pEntry->SetHeadLight ( CHandlingEntry::SMALL );
                    ucChar = CHandlingEntry::SMALL;
                    return true;
                }
                else if ( strValue == "long" )
                {
                    pEntry->SetHeadLight ( CHandlingEntry::LONG );
                    ucChar = CHandlingEntry::LONG;
                    return true;
                }
                else if ( strValue == "big" )
                {
                    pEntry->SetHeadLight ( CHandlingEntry::BIG );
                    ucChar = CHandlingEntry::BIG;
                    return true;
                }
                else if ( strValue == "tall" )
                {
                    pEntry->SetHeadLight ( CHandlingEntry::TALL );
                    ucChar = CHandlingEntry::TALL;
                    return true;
                }
                else
                    return false;
                break;
            }
            case HANDLING_TAILLIGHT:
            {
                if ( strValue == "small" )
                {
                    pEntry->SetTailLight ( CHandlingEntry::SMALL );
                    ucChar = CHandlingEntry::SMALL;
                    return true;
                }
                else if ( strValue == "long" )
                {
                    pEntry->SetTailLight ( CHandlingEntry::LONG );
                    ucChar = CHandlingEntry::LONG;
                    return true;
                }
                else if ( strValue == "big" )
                {
                    pEntry->SetTailLight ( CHandlingEntry::BIG );
                    ucChar = CHandlingEntry::BIG;
                    return true;
                }
                else if ( strValue == "tall" )
                {
                    pEntry->SetTailLight ( CHandlingEntry::TALL );
                    ucChar = CHandlingEntry::TALL;
                    return true;
                }
                else
                    return false;
                break;
            }*/
            default:
                return false;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleColor ( CElement* pElement, const CVehicleColor& color )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleColor ( *iter, color );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
        pVehicle->SetColor ( color );

        CBitStream BitStream;

        CVehicleColor& vehColor = pVehicle->GetColor ();
        uchar ucNumColors = vehColor.GetNumColorsUsed () - 1;
        BitStream.pBitStream->WriteBits ( &ucNumColors, 2 );
        for ( uint i = 0 ; i <= ucNumColors ; i++ )
        {
            SColor RGBColor = vehColor.GetRGBColor ( i );
            BitStream.pBitStream->Write ( RGBColor.R );
            BitStream.pBitStream->Write ( RGBColor.G );
            BitStream.pBitStream->Write ( RGBColor.B );
        }
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_COLOR, *BitStream.pBitStream ) );
        return true;
    }

    return false;
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
            BitStream.pBitStream->Write ( ucLandingGearDown );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_LANDING_GEAR_DOWN, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleLocked ( CElement* pElement, bool bLocked )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleLocked ( *iter, bLocked );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        //if ( bLocked != pVehicle->IsLocked () )   // this lock check is redundant
        {
            // Set the new locked state
            pVehicle->SetLocked ( bLocked );

            // Tell all the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bLocked ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_LOCKED, *BitStream.pBitStream ) );

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
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bDoorsUndamageable ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DOORS_UNDAMAGEABLE, *BitStream.pBitStream ) );

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
        BitStream.pBitStream->Write ( vecRotation.fX );
        BitStream.pBitStream->Write ( vecRotation.fY );
        BitStream.pBitStream->Write ( vecRotation.fZ );
        BitStream.pBitStream->Write ( pVehicle->GenerateSyncTimeContext () );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_ROTATION, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleSirensOn ( CElement* pElement, bool bSirensOn )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleSirensOn ( *iter, bSirensOn );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        // Has Sirens and different state than before?
        if ( ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) || pVehicle->DoesVehicleHaveSirens ( ) ) &&
             bSirensOn != pVehicle->IsSirenActive () )
        {
            // Set the new state
            pVehicle->SetSirenActive ( bSirensOn );

            // Tell everyone
            unsigned char ucSirensOn = 0;
            if ( bSirensOn ) ucSirensOn = 1;

            CBitStream BitStream;
            BitStream.pBitStream->Write ( ucSirensOn );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_SIRENE_ON, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}
bool CStaticFunctionDefinitions::SetVehicleTaxiLightOn ( CElement* pElement, bool bTaxiLightState )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleTaxiLightOn ( *iter, bTaxiLightState );

    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( CVehicleManager::HasTaxiLight ( pVehicle->GetModel () ) && bTaxiLightState != pVehicle->IsTaxiLightOn() )
        {
            pVehicle->SetTaxiLightOn ( bTaxiLightState );
            CBitStream BitStream;
            unsigned char ucTaxiLightState = bTaxiLightState ? 1 : 0;
            BitStream.pBitStream->Write ( ucTaxiLightState );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_TAXI_LIGHT_ON, *BitStream.pBitStream ) );
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
        BitStream.pBitStream->Write ( vecTurnVelocity.fX );
        BitStream.pBitStream->Write ( vecTurnVelocity.fY );
        BitStream.pBitStream->Write ( vecTurnVelocity.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_TURNSPEED, *BitStream.pBitStream ) );
        return true;
    }

    return false;
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
                BitStream.pBitStream->Write ( usUpgrade );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, ADD_VEHICLE_UPGRADE, *BitStream.pBitStream ) );

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
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, ADD_ALL_VEHICLE_UPGRADES, *BitStream.pBitStream ) );

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
                    BitStream.pBitStream->Write ( ucUpgrade );
                    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, REMOVE_VEHICLE_UPGRADE, *BitStream.pBitStream ) );

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
                case VT_RCTIGER:
                case VT_BANDITO:
                case VT_KART:
                case VT_MOWER:
                case VT_RCCAM:
                case VT_RCGOBLIN:
                    return false;
                    break;
                default: break;
            }

            if ( ucState != pVehicle->m_ucDoorStates [ ucDoor ] )
            {
                pVehicle->m_ucDoorStates [ ucDoor ] = ucState;

                CBitStream BitStream;
                unsigned char ucObject = 0;
                BitStream.pBitStream->Write ( ucObject );
                BitStream.pBitStream->Write ( ucDoor );
                BitStream.pBitStream->Write ( ucState );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetVehicleWheelStates ( CElement* pElement, int iFrontLeft, int iRearLeft, int iFrontRight, int iRearRight )
{
    assert ( pElement );
    RUN_CHILDREN SetVehicleWheelStates ( *iter, iFrontLeft, iRearLeft, iFrontRight, iRearRight );

    unsigned char a = -1;
    if ( a == (unsigned char)-1 )
        a = 2;
    if ( IS_VEHICLE ( pElement ) )
    {
        CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );

        if ( ( iFrontLeft >= -1 && iFrontLeft <= DT_WHEEL_INTACT_COLLISIONLESS ) &&
             ( iRearLeft >= -1 && iRearLeft  <= DT_WHEEL_INTACT_COLLISIONLESS ) &&
             ( iFrontRight >= -1 && iFrontRight <= DT_WHEEL_INTACT_COLLISIONLESS ) &&
             ( iRearRight >= -1 && iRearRight <= DT_WHEEL_INTACT_COLLISIONLESS ) )
        {
            // If atleast 1 wheel state is different
            if ( ( iFrontLeft != -1 && iFrontLeft != pVehicle->m_ucWheelStates [ FRONT_LEFT_WHEEL ] ) ||
                 ( iRearLeft != -1 && iRearLeft != pVehicle->m_ucWheelStates [ REAR_LEFT_WHEEL ] ) ||
                 ( iFrontRight != -1 && iFrontRight != pVehicle->m_ucWheelStates [ FRONT_RIGHT_WHEEL ] ) ||
                 ( iRearRight != -1 && iRearRight != pVehicle->m_ucWheelStates [ REAR_RIGHT_WHEEL ] ) )
            {
                if ( iFrontLeft != -1 ) pVehicle->m_ucWheelStates [ FRONT_LEFT_WHEEL ] = iFrontLeft;
                if ( iRearLeft != -1 ) pVehicle->m_ucWheelStates [ REAR_LEFT_WHEEL ] = iRearLeft;
                if ( iFrontRight != -1 ) pVehicle->m_ucWheelStates [ FRONT_RIGHT_WHEEL ] = iFrontRight;
                if ( iRearRight != -1 )  pVehicle->m_ucWheelStates [ REAR_RIGHT_WHEEL ] = iRearRight;

                CBitStream BitStream;
                BitStream.pBitStream->Write ( ( const char * ) &pVehicle->m_ucWheelStates[0], MAX_WHEELS );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_WHEEL_STATES, *BitStream.pBitStream ) );

                return true;
            }
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
                unsigned char ucObject = 2;
                BitStream.pBitStream->Write ( ucObject );
                BitStream.pBitStream->Write ( ucLight );
                BitStream.pBitStream->Write ( ucState );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );

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
                unsigned char ucObject = 3;
                BitStream.pBitStream->Write ( ucObject );
                BitStream.pBitStream->Write ( ucPanel );
                BitStream.pBitStream->Write ( ucState );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DAMAGE_STATE, *BitStream.pBitStream ) );
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
        pVehicle->SetBlowRespawnInterval ( ulTime );

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
        pVehicle->SetIdleRespawnInterval ( ulTime );

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
        pVehicle->SetIsBlown ( false );

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
        pVehicle->StopIdleTimer ();

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
        pVehicle->SpawnAt ( vecPosition, vecRotation );

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
        pVehicle->Respawn ();

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
            BitStream.pBitStream->Write ( ucLights );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_OVERRIDE_LIGHTS, *BitStream.pBitStream ) );

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
            if ( !pVehicle->SetTowedVehicle ( pTrailer ) || !pTrailer->SetTowedByVehicle ( pVehicle ) )
            {
                pVehicle->SetTowedVehicle ( NULL );
                pTrailer->SetTowedByVehicle ( NULL );
                return false;
            }

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
                pVehicle->SetTowedVehicle ( NULL );
                pTrailer->SetTowedByVehicle ( NULL );

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
        pVehicle->SetEngineOn ( bState );

        CBitStream BitStream;
        BitStream.pBitStream->WriteBit ( bState );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_ENGINE_STATE, *BitStream.pBitStream ) );

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
        BitStream.pBitStream->Write ( fDirtLevel );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DIRT_LEVEL, *BitStream.pBitStream ) );

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
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bDamageProof ) ? 1 : 0 ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_DAMAGE_PROOF, *BitStream.pBitStream ) );

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
            BitStream.pBitStream->Write ( ucPaintjob );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_PAINTJOB, *BitStream.pBitStream ) );

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
            BitStream.pBitStream->WriteBit ( bExplodable );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_FUEL_TANK_EXPLODABLE, *BitStream.pBitStream ) );

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
    BitStream.pBitStream->WriteBit ( bFrozen );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_FROZEN, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetTrainDerailed ( CVehicle* pVehicle, bool bDerailed )
{
    assert ( pVehicle );

    pVehicle->SetDerailed ( bDerailed );

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bDerailed );

    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_TRAIN_DERAILED, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetTrainDerailable ( CVehicle* pVehicle, bool bDerailable )
{
    assert ( pVehicle );

    pVehicle->SetDerailable ( bDerailable );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( bDerailable );

    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_TRAIN_DERAILABLE, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetTrainDirection ( CVehicle* pVehicle, bool bDirection )
{
    assert ( pVehicle );

    pVehicle->SetTrainDirection ( bDirection );

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bDirection );

    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_TRAIN_DIRECTION, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetTrainSpeed ( CVehicle* pVehicle, float fSpeed )
{
    assert ( pVehicle );

    CVector vecVelocity = pVehicle->GetVelocity ();
    vecVelocity.Normalize ();
    vecVelocity *= fSpeed;
    pVehicle->SetVelocity ( vecVelocity );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fSpeed );

    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_TRAIN_SPEED, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleHeadLightColor ( CVehicle* pVehicle, const SColor color )
{
    assert ( pVehicle );

    if ( color != pVehicle->GetHeadLightColor () )
    {
        pVehicle->SetHeadLightColor ( color );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( color.R );
        BitStream.pBitStream->Write ( color.G );
        BitStream.pBitStream->Write ( color.B );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HEADLIGHT_COLOR, *BitStream.pBitStream ) );
    }

    return true;
}

/*bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, bool bValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry->GetABS () == bValue )
        return false;

    pEntry->SetABS ( bValue );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( pVehicle->GetID () );
    BitStream.pBitStream->Write ( HANDLING_ABS );
    BitStream.pBitStream->Write ( bValue );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
    return true;
}*/
bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, unsigned char ucValue )
{
    assert ( pVehicle );
    
    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, ucValue ) )
        {
            CBitStream BitStream;
            pVehicle->SetHasHandlingChanged ( true );

            BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
            BitStream.pBitStream->Write ( ucValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, unsigned int uiValue )
{
    assert ( pVehicle );
    
    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, uiValue ) )
        {
            CBitStream BitStream;
            pVehicle->SetHasHandlingChanged ( true );

            BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
            BitStream.pBitStream->Write ( uiValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, float fValue )
{
    assert ( pVehicle );
    
    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, fValue ) )
        {
            CBitStream BitStream;
            pVehicle->SetHasHandlingChanged ( true );

            BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, std::string strValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, strValue ) )
        {
            CBitStream BitStream;
            unsigned char ucValue = 0;
            pVehicle->SetHasHandlingChanged ( true );
            GetEntryHandling( pEntry, eProperty, ucValue );

            BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
            BitStream.pBitStream->Write ( ucValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetVehicleHandling ( CVehicle* pVehicle, eHandlingProperty eProperty, CVector vecValue )
{
    assert ( pVehicle );

    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

    if ( pEntry )
    {
        if ( SetEntryHandling ( pEntry, eProperty, vecValue ) )
        {
            CBitStream BitStream;
            pVehicle->SetHasHandlingChanged ( true );

            BitStream.pBitStream->Write ( static_cast < unsigned  char > ( eProperty ) );
            BitStream.pBitStream->Write ( vecValue.fX );
            BitStream.pBitStream->Write ( vecValue.fY );
            BitStream.pBitStream->Write ( vecValue.fZ );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::ResetVehicleHandling ( CVehicle* pVehicle, bool bUseOriginal )
{
    assert ( pVehicle );

    eVehicleTypes eModel = (eVehicleTypes) pVehicle->GetModel();
    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    const CHandlingEntry* pNewEntry;
    CBitStream BitStream;

    if ( bUseOriginal )
    {
        pNewEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, RESET_VEHICLE_HANDLING, *BitStream.pBitStream ) );
    }
    else
    {
        pNewEntry = g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );

        SVehicleHandlingSync handling;
        handling.data.fMass                         = pNewEntry->GetMass ();
        handling.data.fTurnMass                     = pNewEntry->GetTurnMass ();
        handling.data.fDragCoeff                    = pNewEntry->GetDragCoeff ();
        handling.data.vecCenterOfMass               = pNewEntry->GetCenterOfMass ();
        handling.data.ucPercentSubmerged            = pNewEntry->GetPercentSubmerged ();
        handling.data.fTractionMultiplier           = pNewEntry->GetTractionMultiplier ();
        handling.data.ucDriveType                   = pNewEntry->GetCarDriveType ();
        handling.data.ucEngineType                  = pNewEntry->GetCarEngineType ();
        handling.data.ucNumberOfGears               = pNewEntry->GetNumberOfGears ();
        handling.data.fEngineAcceleration           = pNewEntry->GetEngineAcceleration ();
        handling.data.fEngineInertia                = pNewEntry->GetEngineInertia ();
        handling.data.fMaxVelocity                  = pNewEntry->GetMaxVelocity ();
        handling.data.fBrakeDeceleration            = pNewEntry->GetBrakeDeceleration ();
        handling.data.fBrakeBias                    = pNewEntry->GetBrakeBias ();
        handling.data.bABS                          = pNewEntry->GetABS ();
        handling.data.fSteeringLock                 = pNewEntry->GetSteeringLock ();
        handling.data.fTractionLoss                 = pNewEntry->GetTractionLoss ();
        handling.data.fTractionBias                 = pNewEntry->GetTractionBias ();
        handling.data.fSuspensionForceLevel         = pNewEntry->GetSuspensionForceLevel ();
        handling.data.fSuspensionDamping            = pNewEntry->GetSuspensionDamping ();
        handling.data.fSuspensionHighSpdDamping     = pNewEntry->GetSuspensionHighSpeedDamping ();
        handling.data.fSuspensionUpperLimit         = pNewEntry->GetSuspensionUpperLimit ();
        handling.data.fSuspensionLowerLimit         = pNewEntry->GetSuspensionLowerLimit ();
        handling.data.fSuspensionFrontRearBias      = pNewEntry->GetSuspensionFrontRearBias ();
        handling.data.fSuspensionAntiDiveMultiplier = pNewEntry->GetSuspensionAntiDiveMultiplier ();
        handling.data.fCollisionDamageMultiplier    = pNewEntry->GetCollisionDamageMultiplier ();
        handling.data.uiModelFlags                  = pNewEntry->GetModelFlags ();
        handling.data.uiHandlingFlags               = pNewEntry->GetHandlingFlags ();
        handling.data.fSeatOffsetDistance           = pNewEntry->GetSeatOffsetDistance ();
        //handling.data.uiMonetary                  = pNewEntry->GetMonetary ();
        //handling.data.ucHeadLight                 = pNewEntry->GetHeadLight ();
        //handling.data.ucTailLight                 = pNewEntry->GetTailLight ();
        handling.data.ucAnimGroup                   = pNewEntry->GetAnimGroup ();

        // Lower and Upper limits cannot match or LSOD (unless boat)
        //if ( eModel != VEHICLE_BOAT )     // Commented until fully tested
        {
            float fSuspensionLimitSize = handling.data.fSuspensionUpperLimit - handling.data.fSuspensionLowerLimit;
            if ( fSuspensionLimitSize > -0.1f && fSuspensionLimitSize < 0.1f )
            {
                if ( fSuspensionLimitSize >= 0.f )
                    handling.data.fSuspensionUpperLimit = handling.data.fSuspensionLowerLimit + 0.1f;
                else
                    handling.data.fSuspensionUpperLimit = handling.data.fSuspensionLowerLimit - 0.1f;
            }
        }

        BitStream.pBitStream->Write ( &handling );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING, *BitStream.pBitStream ) );
    }

    pEntry->ApplyHandlingData( pNewEntry );
    return true;
}

bool CStaticFunctionDefinitions::ResetVehicleHandlingProperty ( CVehicle* pVehicle, eHandlingProperty eProperty, bool bUseOriginal )
{
    assert ( pVehicle );

    eVehicleTypes eModel = (eVehicleTypes) pVehicle->GetModel ();
    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
    if ( pEntry )
    {
        CBitStream BitStream;

        float fValue = 0.0f;
        CVector vecValue = CVector( 0.0f, 0.0f, 0.0f );
        SString strValue = "";
        unsigned int uiValue = 0;
        unsigned int ucValue = 0;
        if ( GetModelHandling ( eModel, eProperty, fValue, bUseOriginal ) )
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( eProperty ) );

            SetEntryHandling ( pEntry, eProperty, fValue );

            BitStream.pBitStream->Write ( fValue );
        }
        else if ( GetModelHandling ( eModel, eProperty, uiValue, bUseOriginal ) )
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( eProperty ) );

            SetEntryHandling ( pEntry, eProperty, uiValue );

            BitStream.pBitStream->Write ( uiValue );
        }
        else if ( GetModelHandling ( eModel, eProperty, ucValue, bUseOriginal ) )
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( eProperty ) );

            SetEntryHandling ( pEntry, eProperty, ucValue );

            BitStream.pBitStream->Write ( ucValue );
        }
        else if ( GetModelHandling ( eModel, eProperty, strValue, bUseOriginal ) )
        {
            unsigned char ucValue = 0;
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( eProperty ) );

            SetEntryHandling ( pEntry, eProperty, strValue );
            ucValue = GetEntryHandling( pEntry, eProperty, strValue );

            BitStream.pBitStream->Write ( ucValue );
        }
        else if ( GetModelHandling ( eModel, eProperty, vecValue, bUseOriginal ) )
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( eProperty ) );

            SetEntryHandling ( pEntry, eProperty, vecValue );

            BitStream.pBitStream->Write ( vecValue.fX );
            BitStream.pBitStream->Write ( vecValue.fY );
            BitStream.pBitStream->Write ( vecValue.fZ );
        }
        else
        {
            return false;
        }

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_HANDLING_PROPERTY, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::ResetModelHandling ( eVehicleTypes eModel )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );
    if ( pEntry )
    {
        const CHandlingEntry * pHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
        if ( pHandlingEntry )
        {
            pEntry->ApplyHandlingData( pHandlingEntry );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::ResetModelHandlingProperty ( eVehicleTypes eModel, eHandlingProperty eProperty )
{
    CHandlingEntry* pEntry = ( CHandlingEntry* ) g_pGame->GetHandlingManager()->GetModelHandlingData( eModel );

    float fValue = 0.0f;
    CVector vecValue = CVector ( 0.0f, 0.0f, 0.0f );
    SString strValue = "";
    uint uiValue = 0;
    uchar ucValue = 0;

    if ( GetModelHandling( eModel, eProperty, fValue, true) )
    {
        SetEntryHandling ( pEntry, eProperty, fValue );
    }
    else if ( GetModelHandling( eModel, eProperty, strValue, true) )
    {
        SetEntryHandling ( pEntry, eProperty, strValue );
    }
    else if ( GetModelHandling( eModel, eProperty, vecValue, true) )
    {
        SetEntryHandling ( pEntry, eProperty, vecValue );
    }
    else if ( GetModelHandling( eModel, eProperty, uiValue, true) )
    {
        SetEntryHandling ( pEntry, eProperty, uiValue );
    }
    else if ( GetModelHandling( eModel, eProperty, ucValue, true) )
    {
        SetEntryHandling ( pEntry, eProperty, ucValue );
    }
    else
    {
        return false;
    }

    return true;
}


bool CStaticFunctionDefinitions::SetVehicleTurretPosition ( CVehicle* pVehicle, float fHorizontal, float fVertical )
{
    assert ( pVehicle );

    pVehicle->SetTurretPosition ( fHorizontal, fVertical );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fHorizontal );
    BitStream.pBitStream->Write ( fVertical );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pVehicle, SET_VEHICLE_TURRET_POSITION, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetVehicleDoorOpenRatio ( CElement* pElement, unsigned char ucDoor, float fRatio, unsigned long ulTime )
{
    if ( ucDoor <= 5 )
    {
        RUN_CHILDREN SetVehicleDoorOpenRatio ( *iter, ucDoor, fRatio, ulTime );

        if ( IS_VEHICLE(pElement) )
        {
            CVehicle& Vehicle = static_cast < CVehicle& > ( *pElement );
            Vehicle.SetDoorOpenRatio ( ucDoor, fRatio );

            CBitStream BitStream;
            SIntegerSync < unsigned char, 3 > ucDoorSync ( ucDoor );
            SDoorOpenRatioSync angle;
            angle.data.fRatio = fRatio;
            BitStream.pBitStream->Write ( &ucDoorSync );
            BitStream.pBitStream->Write ( &angle );
            BitStream.pBitStream->WriteCompressed ( static_cast < unsigned int > ( ulTime ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( &Vehicle, SET_VEHICLE_DOOR_OPEN_RATIO, *BitStream.pBitStream ) );

            return true;
        }
    }

    return false;
}


CMarker* CStaticFunctionDefinitions::CreateMarker ( CResource* pResource, const CVector& vecPosition, const char* szType, float fSize, const SColor color, CElement* pVisibleTo )
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
            pMarker->SetColor ( color );
            pMarker->SetSize ( fSize );

            // Make him visible to the given element
            if ( pVisibleTo )
            {
                pMarker->RemoveVisibleToReference ( m_pMapManager->GetRootElement() );
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


bool CStaticFunctionDefinitions::GetMarkerColor ( CMarker* pMarker, SColor& outColor )
{
    assert ( pMarker );

    outColor = pMarker->GetColor ();
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
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetMarkerColor ( CElement* pElement, const SColor color )
{
    assert ( pElement );
    RUN_CHILDREN SetMarkerColor ( *iter, color );

    // Is this a marker?
    if ( IS_MARKER ( pElement ) )
    {
        // Set the new color
        CMarker* pMarker = static_cast < CMarker* > ( pElement );
        pMarker->SetColor ( color );
        return true;
    }

    return false;
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


CBlip* CStaticFunctionDefinitions::CreateBlip ( CResource* pResource, const CVector& vecPosition, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance, CElement* pVisibleTo )
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
            pBlip->SetColor ( color );
            pBlip->m_sOrdering = sOrdering;
            pBlip->m_usVisibleDistance = usVisibleDistance;

            // Make him visible to the given element
            if ( pVisibleTo )
            {
                pBlip->RemoveVisibleToReference ( m_pMapManager->GetRootElement() );
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


CBlip* CStaticFunctionDefinitions::CreateBlipAttachedTo ( CResource* pResource, CElement* pElement, unsigned char ucIcon, unsigned char ucSize, const SColor color, short sOrdering, unsigned short usVisibleDistance, CElement* pVisibleTo )
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
            pBlip->SetColor ( color );
            pBlip->m_sOrdering = sOrdering;
            pBlip->m_usVisibleDistance = usVisibleDistance;

            // Set his visible to element
            if ( pVisibleTo )
            {
                pBlip->RemoveVisibleToReference ( m_pMapManager->GetRootElement() );
                pBlip->AddVisibleToReference ( pVisibleTo );
            }
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


bool CStaticFunctionDefinitions::GetBlipColor ( CBlip* pBlip, SColor& outColor )
{
    assert ( pBlip );

    outColor = pBlip->m_Color;
    return true;
}


bool CStaticFunctionDefinitions::GetBlipVisibleDistance ( CBlip* pBlip, unsigned short& usVisibleDistance )
{
    assert ( pBlip );

    usVisibleDistance = pBlip->m_usVisibleDistance;
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

                SIntegerSync < unsigned char, 6 > icon ( ucIcon );
                bitStream.pBitStream->Write ( &icon );

                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pBlip, SET_BLIP_ICON, *bitStream.pBitStream ) );

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

                SIntegerSync < unsigned char, 5 > size ( ucSize );
                bitStream.pBitStream->Write ( &size );

                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pBlip, SET_BLIP_SIZE, *bitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetBlipColor ( CElement* pElement, const SColor color )
{
    RUN_CHILDREN SetBlipColor ( *iter, color );

    if ( IS_BLIP ( pElement ) )
    {
        // Grab the blip and set the new color
        CBlip* pBlip = static_cast < CBlip* > ( pElement );
        if ( pBlip->m_Color != color )
        {
            pBlip->SetColor ( color );

            CBitStream bitStream;

            SColorSync colorSync ( color );
            bitStream.pBitStream->Write ( &colorSync );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pBlip, SET_BLIP_COLOR, *bitStream.pBitStream ) );

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
            bitStream.pBitStream->WriteCompressed ( sOrdering );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pBlip, SET_BLIP_ORDERING, *bitStream.pBitStream ) );

            return true;
        }
    }

    return true;
}


bool CStaticFunctionDefinitions::SetBlipVisibleDistance ( CElement* pElement, unsigned short usVisibleDistance )
{
    RUN_CHILDREN SetBlipVisibleDistance ( *iter, usVisibleDistance );

    if ( IS_BLIP ( pElement ) )
    {
        // Grab the blip and set the new visible distance
        CBlip* pBlip = static_cast < CBlip* > ( pElement );
        if ( pBlip->m_usVisibleDistance != usVisibleDistance )
        {
            pBlip->m_usVisibleDistance = usVisibleDistance;

            CBitStream bitStream;

            SIntegerSync < unsigned short, 14 > visibleDistance ( usVisibleDistance );
            bitStream.pBitStream->Write ( &visibleDistance );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pBlip, SET_BLIP_VISIBLE_DISTANCE, *bitStream.pBitStream ) );

            return true;
        }
    }

    return true;
}


CObject* CStaticFunctionDefinitions::CreateObject ( CResource* pResource, unsigned short usModelID, const CVector& vecPosition, const CVector& vecRotation, bool bIsLowLod )
{
    //CObject* pObject = m_pObjectManager->Create ( m_pMapManager->GetRootElement () );
    CObject* pObject = m_pObjectManager->Create ( pResource->GetDynamicElementRoot(), NULL, bIsLowLod );
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
    pObject->GetRotation ( vecRotation );
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
        BitStream.pBitStream->Write ( vecRadians.fX );
        BitStream.pBitStream->Write ( vecRadians.fY );
        BitStream.pBitStream->Write ( vecRadians.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pObject, SET_OBJECT_ROTATION, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetObjectScale ( CElement* pElement, float fScale )
{
    RUN_CHILDREN SetObjectScale ( *iter, fScale );

    if ( IS_OBJECT ( pElement ) )
    {
        CObject* pObject = static_cast < CObject* > ( pElement );

        pObject->SetScale ( fScale );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fScale );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pObject, SET_OBJECT_SCALE, *BitStream.pBitStream ) );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::MoveObject ( CResource * pResource, CElement* pElement, unsigned long ulTime, const CVector& vecPosition, const CVector& vecRotation, CEasingCurve::eType a_easingType, double a_fEasingPeriod, double a_fEasingAmplitude, double a_fEasingOvershoot )
{
    RUN_CHILDREN MoveObject ( pResource, *iter, ulTime, vecPosition, vecRotation, a_easingType, a_fEasingPeriod, a_fEasingAmplitude, a_fEasingOvershoot );

    if ( IS_OBJECT ( pElement ) )
    {
        CObject* pObject = static_cast < CObject* > ( pElement );

        // Grab the source position and rotation
        CVector vecSourcePosition = pObject->GetPosition ();
        CVector vecSourceRotation;
        pObject->GetRotation ( vecSourceRotation );

        // Convert the target rotation given to radians (don't wrap around as these can be rotated more than 360)
        CVector vecDeltaRadians = vecRotation;
        ConvertDegreesToRadiansNoWrap ( vecDeltaRadians );

        if ( a_easingType == CEasingCurve::EASING_INVALID )
        {
            return false;
        }

        CPositionRotationAnimation moveAnimation;
        moveAnimation.SetSourceValue ( SPositionRotation ( vecSourcePosition, vecSourceRotation ) );
        moveAnimation.SetTargetValue ( SPositionRotation ( vecPosition, vecDeltaRadians ), true );
        moveAnimation.SetEasing ( a_easingType, a_fEasingPeriod, a_fEasingAmplitude, a_fEasingOvershoot );
        moveAnimation.SetDuration ( ulTime );

        // Start moving it here so we can keep track of the position/rotation
        pObject->Move ( moveAnimation );

        // Has this resource started yet?
        if ( pResource->HasStarted() )
        {
            // Tell the players
            CBitStream BitStream;

            moveAnimation.ToBitStream ( *BitStream.pBitStream, false );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pObject, MOVE_OBJECT, *BitStream.pBitStream ) );
        }        
        return true;
    }

    return false;
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
        CVector vecSourceRotation;
        pObject->GetRotation ( vecSourceRotation );

        // Tell the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( vecSourcePosition.fX );
        BitStream.pBitStream->Write ( vecSourcePosition.fY );
        BitStream.pBitStream->Write ( vecSourcePosition.fZ );
        BitStream.pBitStream->Write ( vecSourceRotation.fX );
        BitStream.pBitStream->Write ( vecSourceRotation.fY );
        BitStream.pBitStream->Write ( vecSourceRotation.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pObject, STOP_OBJECT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


CRadarArea* CStaticFunctionDefinitions::CreateRadarArea ( CResource* pResource, const CVector2D& vecPosition2D, const CVector2D& vecSize, const SColor color, CElement* pVisibleTo )
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
        pRadarArea->SetColor ( color );

        // Make him visible to the root
        if ( pVisibleTo )
        {
            pRadarArea->RemoveVisibleToReference ( m_pMapManager->GetRootElement() );
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


bool CStaticFunctionDefinitions::GetRadarAreaColor ( CRadarArea* pRadarArea, SColor& outColor )
{
    assert ( pRadarArea );
    outColor = pRadarArea->GetColor ();
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
    //vecTemp -= ( CVector ( vecSize.fX, vecSize.fY, 0.0f ) * CVector ( 0.5f, 0.5f, 0.5f ) );
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


bool CStaticFunctionDefinitions::SetRadarAreaColor ( CElement* pElement, const SColor color )
{
    assert ( pElement );
    RUN_CHILDREN SetRadarAreaColor ( *iter, color );

    if ( IS_RADAR_AREA ( pElement ) )
    {
        CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );
        pRadarArea->SetColor ( color );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetRadarAreaFlashing ( CElement* pElement, bool bFlashing )
{
    assert ( pElement );
    RUN_CHILDREN SetRadarAreaFlashing ( *iter, bFlashing );

    if ( IS_RADAR_AREA ( pElement ) )
    {
        CRadarArea* pRadarArea = static_cast < CRadarArea* > ( pElement );
        pRadarArea->SetFlashing ( bFlashing );
        return true;
    }

    return false;
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
        // Set the type and respawn intervals first!
        // Apply the position, type too and send it
        pPickup->SetPickupType ( ucType );
        pPickup->SetRespawnIntervals ( ulRespawnInterval );
        pPickup->SetPosition ( vecPosition );

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
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( static_cast < float > ( dThree ) );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPickup, SET_PICKUP_TYPE, *BitStream.pBitStream ) );

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
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( ucWeaponID );
                BitStream.pBitStream->Write ( usAmmo );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPickup, SET_PICKUP_TYPE, *BitStream.pBitStream ) );

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
                BitStream.pBitStream->Write ( ucType );
                BitStream.pBitStream->Write ( usModel );
                m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPickup, SET_PICKUP_TYPE, *BitStream.pBitStream ) );

                return true;
            }
        }
    }

    return false;
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

        // Tell everyone
        if ( IS_PLAYER ( pElement ) )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
            CExplosionSyncPacket Packet ( vecPosition, ucType );
            Packet.SetSourceElement ( pPlayer );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
            return true;
        }
    }
    else
    {
        CExplosionSyncPacket Packet ( vecPosition, ucType );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::CreateFire ( const CVector& vecPosition, float fSize, CElement* pElement )
{
    if ( pElement )
    {
        RUN_CHILDREN CreateFire ( vecPosition, fSize, *iter );

        // Tell everyone
        if ( IS_PLAYER ( pElement ) )
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
            CFireSyncPacket Packet ( vecPosition, fSize );
            Packet.SetSourceElement ( pPlayer );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
            return true;
        }
    }
    else
    {
        CFireSyncPacket Packet ( vecPosition, fSize );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::PlaySoundFrontEnd ( CElement* pElement, unsigned char ucSound )
{
    assert ( pElement );
    RUN_CHILDREN PlaySoundFrontEnd ( *iter, ucSound );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        // Tell them to play a sound
        CBitStream BitStream;
//      BitStream.pBitStream->Write ( (unsigned char) AUDIO_FRONTEND );

        SIntegerSync < unsigned char, 7 > sound ( ucSound );
        BitStream.pBitStream->Write ( &sound );

        pPlayer->Send ( CLuaPacket ( PLAY_SOUND, *BitStream.pBitStream ) );
        return true;
    }

    return false;
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
        return true;
    }

    return false;
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
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::BindKey ( CPlayer* pPlayer, const char* szKey, const char* szHitState, CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments& Arguments )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( szHitState );
    assert ( pLuaMain );

    bool bSuccess = false;

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    const SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
    bool bHitState = true;

    if ( stricmp ( szHitState, "down" ) == 0 || stricmp ( szHitState, "both" ) == 0 )
    {
        if ( ( pKey && pKeyBinds->AddKeyFunction ( pKey, bHitState, pLuaMain, iLuaFunction, Arguments ) ) ||
            ( pControl && pKeyBinds->AddControlFunction ( pControl, bHitState, pLuaMain, iLuaFunction, Arguments ) ) )
        {
            unsigned char ucKeyLength = static_cast < unsigned char > ( strlen ( szKey ) );

            CBitStream bitStream;
            bitStream.pBitStream->Write ( ucKeyLength );
            bitStream.pBitStream->Write ( szKey, ucKeyLength );
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
            bitStream.pBitStream->Write ( szKey, ucKeyLength );
            bitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bHitState ) ? 1 : 0 ) );
            pPlayer->Send ( CLuaPacket ( BIND_KEY, *bitStream.pBitStream ) );

            bSuccess = true;
        }
    }

    return bSuccess;
}

bool CStaticFunctionDefinitions::BindKey ( CPlayer* pPlayer, const char* szKey, const char* szHitState, const char* szCommandName, const char* szArguments, const char* szResource )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( szHitState );
    assert ( szCommandName );
    assert ( szResource );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    szArguments = szArguments ? szArguments : "";

    if ( pKey )
    {
        if ( stricmp ( szHitState, "down" ) == 0 || stricmp ( szHitState, "both" ) == 0 || stricmp ( szHitState, "up" ) == 0 )
        {
            unsigned char ucHitState = stricmp(szHitState, "down") == 0 ? 0 : (stricmp(szHitState, "up") == 0 ? 1 : (stricmp(szHitState, "both") == 0 ? 3 : -1 ) );
            if ( stricmp ( szCommandName, "" ) != 0 )
            {
                unsigned char ucLength = static_cast < unsigned char > ( strlen ( szKey ) );

                CBitStream bitStream;
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szKey, ucLength );
                bitStream.pBitStream->Write ( ucHitState );

                ucLength = static_cast < unsigned char > ( strlen ( szCommandName ) );
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szCommandName, ucLength );

                ucLength = static_cast < unsigned char > ( strlen ( szArguments ) );
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szArguments, ucLength );

                ucLength = static_cast < unsigned char > ( strlen ( szResource ) );
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szResource, ucLength );

                pPlayer->Send ( CLuaPacket ( BIND_COMMAND, *bitStream.pBitStream ) );
                return true;
            }
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::UnbindKey ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, const CLuaFunctionRef& iLuaFunction )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( pLuaMain );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    const SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
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
        bitStream.pBitStream->Write ( szKey, ucKeyLength );
        bitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bHitState ) ? 1 : 0 ) );
        pPlayer->Send ( CLuaPacket ( UNBIND_KEY, *bitStream.pBitStream ) );

        bSuccess = true;
    }

    return bSuccess;
}

bool CStaticFunctionDefinitions::UnbindKey ( CPlayer* pPlayer, const char* szKey, const char* szHitState, const char* szCommandName, const char* szResource )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( szHitState );
    assert ( szCommandName );
    assert ( szResource );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );

    if ( pKey )
    {
        if ( stricmp ( szHitState, "down" ) == 0 || stricmp ( szHitState, "both" ) == 0 || stricmp ( szHitState, "up" ) == 0 )
        {
            unsigned char ucHitState = stricmp(szHitState, "down") == 0 ? 0 : (stricmp(szHitState, "up") == 0 ? 1 : (stricmp(szHitState, "both") == 0 ? 3 : -1 ) );
            if ( stricmp ( szCommandName, "" ) != 0 )
            {
                unsigned char ucLength = static_cast < unsigned char > ( strlen ( szKey ) );

                CBitStream bitStream;
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szKey, ucLength );
                bitStream.pBitStream->Write ( ucHitState );

                ucLength = static_cast < unsigned char > ( strlen ( szCommandName ) );
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szCommandName, ucLength );

                ucLength = static_cast < unsigned char > ( strlen ( szResource ) );
                bitStream.pBitStream->Write ( ucLength );
                bitStream.pBitStream->Write ( szResource, ucLength );

                pPlayer->Send ( CLuaPacket ( UNBIND_COMMAND, *bitStream.pBitStream ) );
                return true;
            }
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::IsKeyBound ( CPlayer* pPlayer, const char* szKey, CLuaMain* pLuaMain, const char* szHitState, const CLuaFunctionRef& iLuaFunction, bool& bBound )
{
    assert ( pPlayer );
    assert ( szKey );
    assert ( pLuaMain );

    CKeyBinds* pKeyBinds = pPlayer->GetKeyBinds ();
    const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
    const SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
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


bool CStaticFunctionDefinitions::GetControlState ( CPlayer* pPlayer, const char* szControl, bool& bState )
{
    assert ( pPlayer );
    assert ( szControl );

    return ( pPlayer->GetPad ()->GetControlState ( szControl, bState ) );
}


bool CStaticFunctionDefinitions::IsControlEnabled ( CPlayer* pPlayer, const char* szControl, bool& bEnabled )
{
    assert ( pPlayer );
    assert ( szControl );

    return ( pPlayer->GetPad ()->IsControlEnabled ( szControl, bEnabled ) );
}


bool CStaticFunctionDefinitions::SetControlState ( CPlayer* pPlayer, const char* szControl, bool bState )
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


bool CStaticFunctionDefinitions::ToggleControl ( CPlayer* pPlayer, const char* szControl, bool bEnabled )
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


CTeam* CStaticFunctionDefinitions::CreateTeam ( CResource* pResource, const char* szTeamName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
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


bool CStaticFunctionDefinitions::GetTeamName ( CTeam* pTeam, SString& strOutName )
{
    assert ( pTeam );

    strOutName = pTeam->GetTeamName ();
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


bool CStaticFunctionDefinitions::SetTeamName ( CTeam* pTeam, const char* szTeamName )
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
        BitStream.pBitStream->Write ( usNameLength );
        BitStream.pBitStream->Write ( szTeamName, usNameLength );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pTeam, SET_TEAM_NAME, *BitStream.pBitStream ) );

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
        BitStream.pBitStream->Write ( ucRed );
        BitStream.pBitStream->Write ( ucGreen );
        BitStream.pBitStream->Write ( ucBlue );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pTeam, SET_TEAM_COLOR, *BitStream.pBitStream ) );

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

        // Tell everyone his new team
        CBitStream BitStream;
        BitStream.pBitStream->Write ( pTeam ? pTeam->GetID ( ) : INVALID_ELEMENT_ID );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pPlayer, SET_PLAYER_TEAM, *BitStream.pBitStream ) );

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
        BitStream.pBitStream->WriteBit ( bFriendlyFire );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pTeam, SET_TEAM_FRIENDLY_FIRE, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


CWater* CStaticFunctionDefinitions::CreateWater ( CResource* pResource, CVector* pV1, CVector* pV2, CVector* pV3, CVector* pV4 )
{
    if ( !pV1 || !pV2 || !pV3 )
        return NULL;

    CWater* pWater = m_pWaterManager->Create ( pV4 ? CWater::QUAD : CWater::TRIANGLE,
        pResource->GetDynamicElementRoot () );

    if ( pWater )
    {
        pWater->SetVertex ( 0, *pV1 );
        pWater->SetVertex ( 1, *pV2 );
        pWater->SetVertex ( 2, *pV3 );
        if ( pWater->GetWaterType () == CWater::QUAD )
            pWater->SetVertex ( 3, *pV4 );

        if ( !pWater->Valid () )
        {
            delete pWater;
            return NULL;
        }

        if ( pResource->HasStarted() )
        {
            CEntityAddPacket Packet;
            Packet.Add ( pWater );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );
        }

        return pWater;
    }

    return NULL;
}


bool CStaticFunctionDefinitions::SetElementWaterLevel ( CWater* pWater, float fLevel )
{
    assert ( pWater );
    g_pGame->GetWaterManager ()->SetElementWaterLevel ( pWater, fLevel );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fLevel );
    m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWater, SET_ELEMENT_WATER_LEVEL, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::SetAllElementWaterLevel ( float fLevel )
{
    g_pGame->GetWaterManager ()->SetAllElementWaterLevel ( fLevel );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fLevel );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_ALL_ELEMENT_WATER_LEVEL, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::SetWorldWaterLevel ( float fLevel, bool bIncludeWorldNonSeaLevel )
{
    g_pGame->GetWaterManager ()->SetWorldWaterLevel ( fLevel, bIncludeWorldNonSeaLevel );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fLevel );
    BitStream.pBitStream->WriteBit ( bIncludeWorldNonSeaLevel );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WORLD_WATER_LEVEL, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::ResetWorldWaterLevel ( void )
{
    CBitStream BitStream;
    g_pGame->GetWaterManager ()->ResetWorldWaterLevel ();
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_WORLD_WATER_LEVEL, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::GetWaterVertexPosition ( CWater* pWater, int iVertexIndex, CVector& vecPosition )
{
    iVertexIndex--;
    if ( !pWater || iVertexIndex < 0 || iVertexIndex >= pWater->GetNumVertices () )
        return false;

    return pWater->GetVertex ( iVertexIndex, vecPosition );
}


bool CStaticFunctionDefinitions::SetWaterVertexPosition ( CWater* pWater, int iVertexIndex, CVector& vecPosition )
{
    iVertexIndex--;
    if ( !pWater || iVertexIndex < 0 || iVertexIndex >= pWater->GetNumVertices () )
        return false;

    CVector vecOriginalPosition;
    pWater->GetVertex ( iVertexIndex, vecOriginalPosition );
    pWater->SetVertex ( iVertexIndex, vecPosition );
    if ( pWater->Valid () )
    {
        CBitStream BitStream;
        BitStream.pBitStream->Write ( (unsigned char) iVertexIndex );
        BitStream.pBitStream->Write ( (short) vecPosition.fX );
        BitStream.pBitStream->Write ( (short) vecPosition.fY );
        BitStream.pBitStream->Write ( vecPosition.fZ );
        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWater, SET_WATER_VERTEX_POSITION, *BitStream.pBitStream ) );
        return true;
    }
    else
    {
        pWater->SetVertex ( iVertexIndex, vecOriginalPosition );
        return false;
    }
}


bool CStaticFunctionDefinitions::GetWaterColor ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha )
{
    if ( g_pGame->HasWaterColor ( ) )
    {
        g_pGame->GetWaterColor ( ucRed, ucGreen, ucBlue, ucAlpha );
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetWaterColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    g_pGame->SetWaterColor ( ucRed, ucGreen, ucBlue, ucAlpha );
    g_pGame->SetHasWaterColor ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( ucRed );
    BitStream.pBitStream->Write ( ucGreen );
    BitStream.pBitStream->Write ( ucBlue );
    BitStream.pBitStream->Write ( ucAlpha );

    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WATER_COLOR, *BitStream.pBitStream ) );

    return true;
}


bool CStaticFunctionDefinitions::ResetWaterColor ( )
{
    g_pGame->SetHasWaterColor ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_WATER_COLOR, *BitStream.pBitStream ) );

    return true;
}


CColCircle* CStaticFunctionDefinitions::CreateColCircle ( CResource* pResource, const CVector& vecPosition, float fRadius )
{
    //CColCircle * pColShape = new CColCircle ( m_pColManager, m_pMapManager->GetRootElement (), vecPosition, fRadius );
    CColCircle * pColShape = new CColCircle ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition, fRadius );

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

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

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

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

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

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

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

    if ( pResource->HasStarted() )
    {
        CEntityAddPacket Packet;
        Packet.Add ( pColShape );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
    }

    return pColShape;
}


CColPolygon* CStaticFunctionDefinitions::CreateColPolygon ( CResource* pResource, const CVector& vecPosition )
{
    CColPolygon * pColShape = new CColPolygon ( m_pColManager, pResource->GetDynamicElementRoot(), vecPosition );

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

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

    // Run collision detection
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );

    if ( pResource->HasStarted() )
    {
        CEntityAddPacket Packet;
        Packet.Add ( pColShape );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
    }

    return pColShape;
}


// Make sure all colliders for a colshape are up to date
void CStaticFunctionDefinitions::RefreshColShapeColliders ( CColShape * pColShape )
{
    CElement* pRoot = m_pMapManager->GetRootElement ();
    m_pColManager->DoHitDetection ( pRoot->GetPosition (), pRoot, pColShape, true );
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

CCustomWeapon* CStaticFunctionDefinitions::CreateWeapon ( CResource* pResource, eWeaponType weaponType, CVector vecPosition )
{
    CCustomWeapon * pWeapon = new CCustomWeapon ( pResource->GetDynamicElementRoot(), NULL, m_pObjectManager, m_pCustomWeaponManager, weaponType );
    pWeapon->SetPosition ( vecPosition );
    
    if ( pResource->HasStarted() )
    {
        CEntityAddPacket Packet;
        Packet.Add ( pWeapon );
        m_pPlayerManager->BroadcastOnlyJoined ( Packet );
    }
    return pWeapon;
}

bool CStaticFunctionDefinitions::FireWeapon ( CCustomWeapon * pWeapon )
{
    if ( pWeapon )
    {
        // Tell our scripts the server has fired
        CLuaArguments Arguments;
        Arguments.PushElement ( NULL );

        if ( pWeapon->CallEvent ( "onWeaponFire", Arguments ) )
        {
            CBitStream BitStream;

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, FIRE_CUSTOM_WEAPON, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}


bool CStaticFunctionDefinitions::GetWeaponProperty ( CCustomWeapon * pWeapon, eWeaponProperty eProperty, short &sData )
{
    if ( pWeapon )
    {
        if ( eProperty == WEAPON_DAMAGE )
        {
            sData = pWeapon->GetWeaponStat ( )->GetDamagePerHit ( );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponProperty ( CCustomWeapon * pWeapon, eWeaponProperty eProperty, float &fData )
{
    if ( pWeapon )
    {
        if ( eProperty == WEAPON_ACCURACY )
        {
            fData = pWeapon->GetWeaponStat ( )->GetAccuracy ( );
            return true;
        }
        if ( eProperty == WEAPON_TARGET_RANGE )
        {
            fData = pWeapon->GetWeaponStat ( )->GetTargetRange ( );
            return true;
        }
        if ( eProperty == WEAPON_WEAPON_RANGE )
        {
            fData = pWeapon->GetWeaponStat ( )->GetWeaponRange ( );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponProperty ( CCustomWeapon * pWeapon, eWeaponProperty eProperty, short sData )
{
    if ( pWeapon )
    {
        if ( eProperty == WEAPON_DAMAGE )
        {
            pWeapon->GetWeaponStat ( )->SetDamagePerHit ( sData );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponProperty ( CCustomWeapon * pWeapon, eWeaponProperty eProperty, float fData )
{
    if ( pWeapon )
    {
        if ( eProperty == WEAPON_ACCURACY )
        {
            pWeapon->GetWeaponStat ( )->SetAccuracy ( fData );
            return true;
        }
        if ( eProperty == WEAPON_TARGET_RANGE )
        {
            pWeapon->GetWeaponStat ( )->SetTargetRange ( fData );
            return true;
        }
        if ( eProperty == WEAPON_WEAPON_RANGE )
        {
            pWeapon->GetWeaponStat ( )->SetWeaponRange ( fData );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponState ( CCustomWeapon * pWeapon, eWeaponState weaponState )
{
    if ( pWeapon )
    {
        pWeapon->SetWeaponState ( weaponState );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( (char) weaponState );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_STATE, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponTarget ( CCustomWeapon * pWeapon, CElement * pTarget, int targetBone )
{
    if ( pWeapon )
    {
        pWeapon->SetWeaponTarget ( pTarget, targetBone );
        CBitStream BitStream;

        BitStream.pBitStream->WriteBit ( false );
        BitStream.pBitStream->Write ( pTarget->GetID ( ) );
        BitStream.pBitStream->Write ( (char) targetBone );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_TARGET, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponTarget ( CCustomWeapon * pWeapon, CVector vecTarget )
{
    if ( pWeapon )
    {
        pWeapon->SetWeaponTarget ( vecTarget );
        CBitStream BitStream;

        BitStream.pBitStream->WriteBit ( true );
        BitStream.pBitStream->WriteVector ( vecTarget.fX, vecTarget.fY, vecTarget.fZ );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_TARGET, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::ClearWeaponTarget ( CCustomWeapon * pWeapon )
{
    if ( pWeapon )
    {
        pWeapon->ResetWeaponTarget ( );
        CBitStream BitStream;

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, RESET_CUSTOM_WEAPON_TARGET, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponFlags ( CCustomWeapon * pWeapon, eWeaponFlags flags, bool bData )
{
    if ( pWeapon )
    {
        if ( pWeapon->SetFlags ( flags, bData ) )
        {
            CBitStream BitStream;
            SWeaponConfiguration weaponConfig = pWeapon->GetFlags ( );

            BitStream.pBitStream->WriteBit ( weaponConfig.bDisableWeaponModel );
            BitStream.pBitStream->WriteBit ( weaponConfig.bInstantReload );
            BitStream.pBitStream->WriteBit ( weaponConfig.bShootIfTargetBlocked );
            BitStream.pBitStream->WriteBit ( weaponConfig.bShootIfTargetOutOfRange );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckBuildings );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckCarTires );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckDummies );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckObjects );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckPeds );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckVehicles );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bIgnoreSomeObjectsForCamera );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bSeeThroughStuff );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bShootThroughStuff );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_FLAGS, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponFlags ( CCustomWeapon * pWeapon, const SLineOfSightFlags& flags )
{
    if ( pWeapon )
    {
        if ( pWeapon->SetFlags ( flags ) )
        {
            CBitStream BitStream;
            SWeaponConfiguration weaponConfig = pWeapon->GetFlags ( );

            BitStream.pBitStream->WriteBit ( weaponConfig.bDisableWeaponModel );
            BitStream.pBitStream->WriteBit ( weaponConfig.bInstantReload );
            BitStream.pBitStream->WriteBit ( weaponConfig.bShootIfTargetBlocked );
            BitStream.pBitStream->WriteBit ( weaponConfig.bShootIfTargetOutOfRange );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckBuildings );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckCarTires );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckDummies );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckObjects );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckPeds );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bCheckVehicles );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bIgnoreSomeObjectsForCamera );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bSeeThroughStuff );
            BitStream.pBitStream->WriteBit ( weaponConfig.flags.bShootThroughStuff );

            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_FIRING_RATE, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponFlags ( CCustomWeapon * pWeapon, eWeaponFlags flags, bool &bData )
{
    if ( pWeapon )
    {
        return pWeapon->GetFlags ( flags, bData );
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponFlags ( CCustomWeapon * pWeapon, SLineOfSightFlags& flags )
{
    if ( pWeapon )
    {
        return pWeapon->GetFlags ( flags );
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponFiringRate ( CCustomWeapon * pWeapon, int iFiringRate )
{
    if ( pWeapon )
    {
        pWeapon->SetWeaponFireTime( iFiringRate );
        CBitStream BitStream;
        BitStream.pBitStream->Write ( iFiringRate );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_FIRING_RATE, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::ResetWeaponFiringRate ( CCustomWeapon * pWeapon )
{
    if ( pWeapon )
    {
        pWeapon->ResetWeaponFireTime( );
        CBitStream BitStream;

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, RESET_CUSTOM_WEAPON_FIRING_RATE, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponFiringRate ( CCustomWeapon * pWeapon, int &iFiringRate )
{
    if ( pWeapon )
    {
        iFiringRate = pWeapon->GetWeaponFireTime( );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponAmmo ( CCustomWeapon * pWeapon, int &iAmmo )
{
    if ( pWeapon )
    {
        iAmmo = pWeapon->GetAmmo( );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetWeaponClipAmmo ( CCustomWeapon * pWeapon, int &iAmmo )
{
    if ( pWeapon )
    {
        iAmmo = pWeapon->GetClipAmmo( );
        CBitStream BitStream;
        BitStream.pBitStream->Write ( iAmmo );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_CLIP_AMMO, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponAmmo ( CCustomWeapon * pWeapon, int iAmmo )
{
    if ( pWeapon )
    {
        pWeapon->SetAmmo( iAmmo );
        CBitStream BitStream;
        BitStream.pBitStream->Write ( iAmmo );

        m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_CUSTOM_WEAPON_AMMO, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponClipAmmo ( CCustomWeapon * pWeapon, int iAmmo )
{
    if ( pWeapon )
    {
        pWeapon->SetClipAmmo( iAmmo );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetWeaponOwner ( CCustomWeapon * pWeapon, CPlayer * pPlayer )
{
    if ( pWeapon )
    {
        
        pWeapon->SetOwner ( pPlayer );
        if ( pPlayer )
        {
            CBitStream BitStream;

            BitStream.pBitStream->Write ( pPlayer->GetID ( ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_WEAPON_OWNER, *BitStream.pBitStream ) );
            return true;
        }
        else
        {
            CBitStream BitStream;

            BitStream.pBitStream->Write ( INVALID_ELEMENT_ID );
            m_pPlayerManager->BroadcastOnlyJoined ( CElementRPCPacket ( pWeapon, SET_WEAPON_OWNER, *BitStream.pBitStream ) );
            return true;
        }
    }
    return false;
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
    const SPlayerClothing* pPlayerClothing = CPlayerClothes::GetClothingGroup ( ucType );
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


bool CStaticFunctionDefinitions::GetTypeIndexFromClothes ( const char* szTexture, const char* szModel, unsigned char& ucTypeReturn, unsigned char& ucIndexReturn )
{
    if ( szTexture == NULL && szModel == NULL )
        return false;

    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
    {
        const SPlayerClothing* pPlayerClothing = CPlayerClothes::GetClothingGroup ( ucType );
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

    const char* szName = CPlayerClothes::GetClothingName ( ucType );
    if ( szName )
    {
        strcpy ( szNameReturn, szName );

        return true;
    }

    return false;
}


unsigned int CStaticFunctionDefinitions::GetMaxPlayers ( void )
{
    return m_pMainConfig->GetMaxPlayers ();
}

bool CStaticFunctionDefinitions::SetMaxPlayers ( unsigned int uiMax )
{
    if ( uiMax > m_pMainConfig->GetHardMaxPlayers() )
        return false;
    m_pMainConfig->SetSoftMaxPlayers ( uiMax );
    g_pNetServer->SetMaximumIncomingConnections ( uiMax );
    return true;
}


bool CStaticFunctionDefinitions::OutputChatBox ( const char* szText, CElement* pElement, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, bool bColorCoded, CLuaMain* pLuaMain )
{
    assert ( pElement );
    assert ( szText );

    RUN_CHILDREN OutputChatBox ( szText, *iter, ucRed, ucGreen, ucBlue, bColorCoded, pLuaMain );

    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        pPlayer->Send ( CChatEchoPacket ( szText, ucRed, ucGreen, ucBlue, bColorCoded ) );
        return true;
    }
    
    if ( pElement == m_pMapManager->GetRootElement() )
    {
        CResource* pResource = pLuaMain->GetResource ();
        CLuaArguments Arguments;
        Arguments.PushString ( szText );
        if ( pResource )
            Arguments.PushResource ( pResource );
        m_pMapManager->GetRootElement()->CallEvent ( "onChatMessage", Arguments );
    }
    
    return false;
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
        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetServerPassword ( const SString& strPassword, bool bSave )
{
    if ( g_pGame->GetConfig()->SetPassword ( strPassword, bSave ) )
    {
        if ( !strPassword.empty () )
            CLogger::LogPrintf ( "Server password set to '%s'\n", *strPassword );
        else
            CLogger::LogPrintf ( "Server password cleared\n" );
        return true;
    }

    return false;
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


bool CStaticFunctionDefinitions::GetZoneName ( CVector& vecPosition, SString& strOutName,bool bCitiesOnly )
{
    if ( bCitiesOnly )
        strOutName = g_pGame->GetZoneNames ()->GetCityName ( vecPosition );
    else
        strOutName = g_pGame->GetZoneNames ()->GetZoneName ( vecPosition );

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
    fHeight = g_pGame->GetWaterManager ()->GetGlobalWaveHeight ();
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
        const SGarageStates& garageStates = g_pGame->GetGarageStates ();
        bIsOpen = garageStates[ ucGarageID ];
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetTrafficLightState ( unsigned char& ucState )
{
    ucState = g_pGame->GetTrafficLightState ();
    return true;
}

bool CStaticFunctionDefinitions::GetTrafficLightsLocked ( bool& bLocked )
{
    bLocked = g_pGame->GetTrafficLightsLocked ();
    return true;
}

bool CStaticFunctionDefinitions::GetJetpackMaxHeight ( float& fMaxHeight )
{
    fMaxHeight = g_pGame->GetJetpackMaxHeight ();
    return true;
}

bool CStaticFunctionDefinitions::GetInteriorSoundsEnabled ( bool& bEnabled )
{
    bEnabled = g_pGame->GetInteriorSoundsEnabled ( );

    return true;
}

bool CStaticFunctionDefinitions::GetRainLevel ( float& fRainLevel )
{
    if ( g_pGame->HasRainLevel ( ) )
    {
        fRainLevel = g_pGame->GetRainLevel ( );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetSunSize ( float& fSunSize )
{
    if ( g_pGame->HasSunSize ( ) )
    {
        fSunSize = g_pGame->GetSunSize ( );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetMoonSize ( int& iSize )
{
    if ( g_pGame->HasMoonSize ( ) )
    {
        iSize = g_pGame->GetMoonSize ( );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetSunColor ( unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG, unsigned char& ucCoronaB )
{
    if ( g_pGame->HasSunColor ( ) )
    {
        g_pGame->GetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetWindVelocity ( float& fVelX, float& fVelY, float& fVelZ )
{
    if ( g_pGame->HasWindVelocity ( ) )
    {
        g_pGame->GetWindVelocity ( fVelX, fVelY, fVelZ );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetFarClipDistance ( float& fFarClip )
{
    if ( g_pGame->HasFarClipDistance ( ) )
    {
        fFarClip = g_pGame->GetFarClipDistance ( );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetFogDistance ( float& fFogDist )
{
    if ( g_pGame->HasFogDistance ( ) )
    {
        fFogDist = g_pGame->GetFogDistance ( );
        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::GetAircraftMaxHeight ( float& fMaxHeight )
{
    fMaxHeight = g_pGame->GetAircraftMaxHeight ( );
    return true;
}

bool CStaticFunctionDefinitions::GetAircraftMaxVelocity ( float& fVelocity )
{
    fVelocity = g_pGame->GetAircraftMaxVelocity ( );
    return true;
}

bool CStaticFunctionDefinitions::GetOcclusionsEnabled ( bool& bEnabled )
{
    bEnabled = g_pGame->GetOcclusionsEnabled ( );
    return true;
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
bool CStaticFunctionDefinitions::SetTrafficLightState ( unsigned char ucState, bool bForced )
{
    if ( ucState >= 0 && ucState < 10 )
    {
        g_pGame->SetTrafficLightState ( ucState );
 
        CBitStream BitStream;
        BitStream.pBitStream->WriteBits ( &ucState, 4 );
        BitStream.pBitStream->WriteBit  ( bForced );

        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_TRAFFIC_LIGHT_STATE, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetTrafficLightsLocked ( bool bLocked )
{
    g_pGame->SetTrafficLightsLocked ( bLocked );
    return true;
}


bool CStaticFunctionDefinitions::SetJetpackMaxHeight ( float fMaxHeight )
{
    if ( fMaxHeight >= -20 )
    {
        g_pGame->SetJetpackMaxHeight ( fMaxHeight );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fMaxHeight );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_JETPACK_MAXHEIGHT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}

bool CStaticFunctionDefinitions::SetInteriorSoundsEnabled ( bool bEnable )
{
    g_pGame->SetInteriorSoundsEnabled ( bEnable );

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bEnable );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_INTERIOR_SOUNDS_ENABLED, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetRainLevel ( float fRainLevel )
{
    g_pGame->SetRainLevel ( fRainLevel );
    g_pGame->SetHasRainLevel ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fRainLevel );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_RAIN_LEVEL, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetSunSize ( float fSunSize )
{
    g_pGame->SetSunSize ( fSunSize );
    g_pGame->SetHasSunSize ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fSunSize );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_SUN_SIZE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetMoonSize ( int iMoonSize )
{    
    if ( iMoonSize >= 0 )
    {
        g_pGame->SetMoonSize ( iMoonSize );
        g_pGame->SetHasMoonSize ( true );
        
        CBitStream BitStream;
        BitStream.pBitStream->Write ( iMoonSize );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_MOON_SIZE, *BitStream.pBitStream ) );
        return true;
    }
    
    return false;
}

bool CStaticFunctionDefinitions::SetSunColor ( unsigned char ucCoreR, unsigned char ucCoreG, unsigned char ucCoreB, unsigned char ucCoronaR, unsigned char ucCoronaG, unsigned char ucCoronaB )
{
    g_pGame->SetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB );
    g_pGame->SetHasSunColor ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( ucCoreR );
    BitStream.pBitStream->Write ( ucCoreG );
    BitStream.pBitStream->Write ( ucCoreB );
    BitStream.pBitStream->Write ( ucCoronaR );
    BitStream.pBitStream->Write ( ucCoronaG );
    BitStream.pBitStream->Write ( ucCoronaB );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_SUN_COLOR, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetWindVelocity ( float fVelX, float fVelY, float fVelZ )
{
    g_pGame->SetWindVelocity ( fVelX, fVelY, fVelZ );
    g_pGame->SetHasWindVelocity ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fVelX );
    BitStream.pBitStream->Write ( fVelY );
    BitStream.pBitStream->Write ( fVelZ );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WIND_VELOCITY, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetFarClipDistance ( float fFarClip )
{
    g_pGame->SetFarClipDistance ( fFarClip );
    g_pGame->SetHasFarClipDistance ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fFarClip );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_FAR_CLIP_DISTANCE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetFogDistance ( float fFogDist )
{
    g_pGame->SetFogDistance ( fFogDist );
    g_pGame->SetHasFogDistance ( true );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fFogDist );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_FOG_DISTANCE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetAircraftMaxHeight ( float fMaxHeight )
{
    g_pGame->SetAircraftMaxHeight ( fMaxHeight );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( fMaxHeight );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_AIRCRAFT_MAXHEIGHT, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetAircraftMaxVelocity ( float fVelocity )
{
    CBitStream BitStream;
    g_pGame->SetAircraftMaxVelocity ( fVelocity );

    BitStream.pBitStream->Write ( fVelocity );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_AIRCRAFT_MAXVELOCITY, *BitStream.pBitStream ) );
    return true;
}

bool CStaticFunctionDefinitions::SetOcclusionsEnabled ( bool bEnabled )
{
    g_pGame->SetOcclusionsEnabled ( bEnabled );

    CBitStream BitStream;
    BitStream.pBitStream->WriteBit ( bEnabled );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_OCCLUSIONS_ENABLED, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetRainLevel ( )
{
    g_pGame->SetHasRainLevel ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_RAIN_LEVEL, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetSunSize ( )
{
    g_pGame->SetHasSunSize ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_SUN_SIZE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetSunColor ( )
{
    g_pGame->SetHasSunColor ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_SUN_COLOR, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetWindVelocity ( )
{
    g_pGame->SetHasWindVelocity ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_WIND_VELOCITY, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetFarClipDistance ( )
{
    g_pGame->SetHasFarClipDistance ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_FAR_CLIP_DISTANCE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetFogDistance ( )
{
    g_pGame->SetHasFogDistance ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_FOG_DISTANCE, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::RemoveWorldModel ( unsigned short usModel, float fRadius, float fX, float fY, float fZ, char cInterior )
{
    g_pGame->GetBuildingRemovalManager ( )->CreateBuildingRemoval( usModel, fRadius, CVector ( fX, fY, fZ ), cInterior );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( usModel );
    BitStream.pBitStream->Write ( fRadius );
    BitStream.pBitStream->Write ( fX );
    BitStream.pBitStream->Write ( fY );
    BitStream.pBitStream->Write ( fZ );
    if ( BitStream.pBitStream->Version() >= 0x039 )
    {
        BitStream.pBitStream->Write ( cInterior );
    }
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( REMOVE_WORLD_MODEL, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::RestoreWorldModel ( unsigned short usModel, float fRadius, float fX, float fY, float fZ, char cInterior )
{
    g_pGame->GetBuildingRemovalManager ( )->RestoreWorldModel( usModel, fRadius, CVector ( fX, fY, fZ ), cInterior );

    CBitStream BitStream;
    BitStream.pBitStream->Write ( usModel );
    BitStream.pBitStream->Write ( fRadius );
    BitStream.pBitStream->Write ( fX );
    BitStream.pBitStream->Write ( fY );
    BitStream.pBitStream->Write ( fZ );
    if ( BitStream.pBitStream->Version() >= 0x039 )
    {
        BitStream.pBitStream->Write ( cInterior );
    }

    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESTORE_WORLD_MODEL, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::RestoreAllWorldModels ( void )
{
    g_pGame->GetBuildingRemovalManager ( )->ClearBuildingRemovals ( );
    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESTORE_ALL_WORLD_MODELS, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::ResetMoonSize ( void )
{    
    CBitStream BitStream;
    g_pGame->SetHasMoonSize ( false );

    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_MOON_SIZE, *BitStream.pBitStream ) );
    return true;
}

bool CStaticFunctionDefinitions::SendSyncIntervals ( CPlayer* pPlayer )
{
    CBitStream BitStream;
    BitStream.pBitStream->Write ( g_TickRateSettings.iPureSync );
    BitStream.pBitStream->Write ( g_TickRateSettings.iLightSync );
    BitStream.pBitStream->Write ( g_TickRateSettings.iCamSync );
    BitStream.pBitStream->Write ( g_TickRateSettings.iPedSync );
    BitStream.pBitStream->Write ( g_TickRateSettings.iUnoccupiedVehicle );
    BitStream.pBitStream->Write ( g_TickRateSettings.iObjectSync );
    BitStream.pBitStream->Write ( g_TickRateSettings.iKeySyncRotation );
    BitStream.pBitStream->Write ( g_TickRateSettings.iKeySyncAnalogMove );
    if ( pPlayer )
        pPlayer->Send ( CLuaPacket ( SET_SYNC_INTERVALS, *BitStream.pBitStream ) );
    else
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_SYNC_INTERVALS, *BitStream.pBitStream ) );

    return true;
}

bool CStaticFunctionDefinitions::SetWeather ( unsigned char ucWeather )
{
    // Verify it's within the max valid weather id
#if MAX_VALID_WEATHER < 255
    if ( ucWeather <= MAX_VALID_WEATHER )
#endif
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
#if MAX_VALID_WEATHER < 255
    if ( ucWeather <= MAX_VALID_WEATHER )
#endif
    {
        CBlendedWeather* pWeather = m_pMapManager->GetWeather ();

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
        g_pGame->GetWaterManager ()->SetGlobalWaveHeight ( fHeight );

        CBitStream BitStream;
        BitStream.pBitStream->Write ( fHeight );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_WAVE_HEIGHT, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::SetFPSLimit ( unsigned short usLimit, bool bSave )
{
    if ( g_pGame->GetConfig()->SetFPSLimit ( usLimit, bSave ) )
    {
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
    if ( ucGarageID < MAX_GARAGES )
    {
        SGarageStates& garageStates = g_pGame->GetGarageStates();
        garageStates [ ucGarageID ] = bIsOpen;

        CBitStream BitStream;
        BitStream.pBitStream->Write ( ucGarageID );
        BitStream.pBitStream->Write ( bIsOpen );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_GARAGE_OPEN, *BitStream.pBitStream ) );

        return true;
    }

    return false;
}


bool CStaticFunctionDefinitions::GetSkyGradient ( unsigned char& ucTopRed, unsigned char& ucTopGreen, unsigned char& ucTopBlue, unsigned char& ucBottomRed, unsigned char& ucBottomGreen, unsigned char& ucBottomBlue )
{
    if ( g_pGame->HasSkyGradient ( ) )
    {
        g_pGame->GetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );
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


bool CStaticFunctionDefinitions::GetHeatHaze ( SHeatHazeSettings& settings )
{
    if ( g_pGame->HasHeatHaze () )
    {
        g_pGame->GetHeatHaze ( settings );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetHeatHaze ( const SHeatHazeSettings& settings )
{
    g_pGame->SetHeatHaze ( settings );
    g_pGame->SetHasHeatHaze ( true );

    CBitStream BitStream;
    SHeatHazeSync heatHaze ( settings );
    BitStream.pBitStream->Write ( &heatHaze );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_HEAT_HAZE, *BitStream.pBitStream ) );
    return true;
}


bool CStaticFunctionDefinitions::ResetHeatHaze ( void )
{
    g_pGame->SetHasHeatHaze ( false );

    CBitStream BitStream;
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_HEAT_HAZE, *BitStream.pBitStream ) );
    return true;
}

bool CStaticFunctionDefinitions::SetGlitchEnabled ( const std::string& strGlitchName, bool bEnabled )
{
    if ( g_pGame->IsGlitch ( strGlitchName ) )
    {
        if ( g_pGame->IsGlitchEnabled ( strGlitchName ) != bEnabled )
        {
            CBitStream BitStream;
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( g_pGame->GetGlitchIndex(strGlitchName) ) );
            BitStream.pBitStream->Write ( bEnabled );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_GLITCH_ENABLED, *BitStream.pBitStream ) );

            g_pGame->SetGlitchEnabled ( strGlitchName, bEnabled );
            return true;
        }
    }

    return false;
}

bool CStaticFunctionDefinitions::IsGlitchEnabled ( const std::string& strGlitchName, bool& bEnabled )
{
    if ( g_pGame->IsGlitch ( strGlitchName ) )
    {
        bEnabled = g_pGame->IsGlitchEnabled ( strGlitchName );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::SetJetpackWeaponEnabled ( eWeaponType weaponType, bool bEnabled )
{
    if ( g_pGame->GetJetpackWeaponEnabled( weaponType ) != bEnabled )
    {
        CBitStream BitStream;
        BitStream.pBitStream->Write ( static_cast < unsigned char > ( weaponType ) );
        BitStream.pBitStream->WriteBit ( bEnabled );
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_JETPACK_WEAPON_ENABLED, *BitStream.pBitStream ) );
        return true;
    }
    return false;
}

bool CStaticFunctionDefinitions::GetJetpackWeaponEnabled ( eWeaponType weaponType, bool& bEnabled )
{
    bEnabled = g_pGame->GetJetpackWeaponEnabled ( weaponType );
    return true;
}


bool CStaticFunctionDefinitions::SetCloudsEnabled ( bool bEnabled )
{
    CBitStream BitStream;
    BitStream.pBitStream->Write ( bEnabled );
    m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( SET_CLOUDS_ENABLED, *BitStream.pBitStream ) );

    g_pGame->SetCloudsEnabled ( bEnabled );
    return true;

}
bool CStaticFunctionDefinitions::GetCloudsEnabled ( void )
{
    return g_pGame->GetCloudsEnabled ();
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
        CChildListType ::const_iterator iter = pElement->IterBegin();
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
            ase->SetGameType ( szGameType );
            return true;
        }
        else
        {
            ase->SetGameType ( GAME_TYPE_STRING );
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


const char* CStaticFunctionDefinitions::GetRuleValue ( const char* szKey )
{
    assert ( szKey );

    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        return ase->GetRuleValue ( szKey );
    }
    return NULL;
}


bool CStaticFunctionDefinitions::SetRuleValue ( const char* szKey, const char* szValue )
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


bool CStaticFunctionDefinitions::RemoveRuleValue ( const char* szKey )
{
    assert ( szKey );

    ASE * ase = ASE::GetInstance();
    if ( ase )
    {
        return ase->RemoveRuleValue ( szKey );
    }
    return false;
}


bool CStaticFunctionDefinitions::GetPlayerAnnounceValue ( CElement* pElement, const std::string& strKey, std::string& strOutValue )
{
    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        strOutValue = pPlayer->GetAnnounceValue ( strKey );
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::SetPlayerAnnounceValue ( CElement* pElement, const std::string& strKey, const std::string& strValue )
{
    if ( IS_PLAYER ( pElement ) )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );

        pPlayer->SetAnnounceValue ( strKey, strValue );
        return true;
    }
    return false;
}


void CStaticFunctionDefinitions::ExecuteSQLCreateTable ( const std::string& strTable, const std::string& strDefinition )
{
    m_pRegistry->CreateTable ( strTable, strDefinition );
}


bool CStaticFunctionDefinitions::ExecuteSQLQuery ( const std::string& strQuery, CLuaArguments *pArgs, CRegistryResult* pResult )
{
    return m_pRegistry->Query ( strQuery, pArgs, pResult );
}

const std::string& CStaticFunctionDefinitions::SQLGetLastError ( void ) {
    return m_pRegistry->GetLastError ();
}

void CStaticFunctionDefinitions::ExecuteSQLDropTable ( const std::string& strTable )
{
    m_pRegistry->DropTable ( strTable );
}


bool CStaticFunctionDefinitions::ExecuteSQLDelete ( const std::string& strTable, const std::string& strWhere )
{
    return m_pRegistry->Delete ( strTable, strWhere );
}


bool CStaticFunctionDefinitions::ExecuteSQLInsert ( const std::string& strTable, const std::string& strValues, const std::string& strColumns )
{
    return m_pRegistry->Insert ( strTable, strValues, strColumns );
}


bool CStaticFunctionDefinitions::ExecuteSQLSelect ( const std::string& strTable, const std::string& strColumns, const std::string& strWhere, unsigned int uiLimit, CRegistryResult* pResult )
{
    return m_pRegistry->Select ( strColumns, strTable, strWhere, uiLimit, pResult );
}


bool CStaticFunctionDefinitions::ExecuteSQLUpdate ( const std::string& strTable, const std::string& strSet, const std::string& strWhere )
{
    return m_pRegistry->Update ( strTable, strSet, strWhere );
}


CClient* CStaticFunctionDefinitions::GetAccountPlayer ( CAccount* pAccount )
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


CLuaArgument* CStaticFunctionDefinitions::GetAccountData ( CAccount* pAccount, const char* szKey )
{
    assert ( pAccount );
    assert ( szKey );

    return m_pAccountManager->GetAccountData ( pAccount, szKey );
}


bool CStaticFunctionDefinitions::GetAllAccountData ( lua_State* pLua, CAccount* pAccount )
{
    assert ( pLua );
    assert ( pAccount );
    m_pAccountManager->GetAllAccountData ( pAccount, pLua );
    return true;
}


CAccount* CStaticFunctionDefinitions::AddAccount ( const char* szName, const char* szPassword )
{
    assert ( szName );
    assert ( szPassword );

    CAccount* pCurrentAccount = m_pAccountManager->Get ( szName );
    if ( pCurrentAccount == NULL && strlen ( szPassword ) > MIN_PASSWORD_LENGTH && strlen ( szPassword ) <= MAX_PASSWORD_LENGTH )
    {
        CAccount* pAccount = new CAccount ( m_pAccountManager, true, szName );
        pAccount->SetPassword ( szPassword );
        g_pGame->GetAccountManager ()->Register( pAccount );
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
    CMappedAccountList::const_iterator iter = m_pAccountManager->IterBegin();
    unsigned int uiIndex = 0;
    const char* szGuest =  GUEST_ACCOUNT_NAME;
    const char* szHTTPGuest = HTTP_GUEST_ACCOUNT_NAME;
    const char* szConsole = CONSOLE_ACCOUNT_NAME;
    unsigned int uiGuest = HashString ( szGuest );
    unsigned int uiHTTPGuest = HashString ( szHTTPGuest );
    unsigned int uiConsole = HashString ( szConsole );
    for ( ; iter != m_pAccountManager->IterEnd(); iter++ )
    {
        if ( ( (*iter)->GetNameHash() != uiGuest || (*iter)->GetName() != szGuest ) &&
             ( (*iter)->GetNameHash() != uiHTTPGuest || (*iter)->GetName() != szHTTPGuest ) &&
             ( (*iter)->GetNameHash() != uiConsole || (*iter)->GetName() != szConsole ) )
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
            if ( !g_pGame->GetAccountManager ()->LogOut ( pClient, NULL ) )
                return false;

            pClient->SendEcho ( "You were logged out of your account due to it being deleted" );
        }
        g_pGame->GetAccountManager ()->RemoveAccount ( pAccount );
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
        if ( strlen ( szPassword ) > MIN_PASSWORD_LENGTH && strlen ( szPassword ) <= MAX_PASSWORD_LENGTH )
        {
            pAccount->SetPassword ( szPassword );
            return true;
        }
    }

    return false;
}


bool CStaticFunctionDefinitions::SetAccountData ( CAccount* pAccount, const char* szKey, CLuaArgument * pArgument )
{
    assert ( pAccount );
    assert ( szKey );

    SString strArgumentAsString;
    pArgument->GetAsString ( strArgumentAsString );

    CLuaArguments Arguments;
    Arguments.PushAccount ( pAccount );
    Arguments.PushString ( szKey );
    Arguments.PushString ( strArgumentAsString );
    if ( m_pMapManager->GetRootElement()->CallEvent ( "onAccountDataChange", Arguments ) )
        return m_pAccountManager->SetAccountData ( pAccount, szKey, strArgumentAsString.substr ( 0, 128 ), pArgument->GetType() );
    return false;
}


bool CStaticFunctionDefinitions::CopyAccountData ( CAccount* pAccount, CAccount* pFromAccount )
{
    assert ( pAccount );
    assert ( pFromAccount );

    m_pAccountManager->CopyAccountData ( pFromAccount, pAccount );
    return true;
}


bool CStaticFunctionDefinitions::LogIn ( CPlayer* pPlayer, CAccount* pAccount, const char* szPassword )
{
    return m_pAccountManager->LogIn ( pPlayer, pPlayer, pAccount->GetName ().c_str (), szPassword );
}


bool CStaticFunctionDefinitions::LogOut ( CPlayer* pPlayer )
{
    return m_pAccountManager->LogOut ( pPlayer, pPlayer );
}


bool CStaticFunctionDefinitions::KickPlayer ( CPlayer* pPlayer, SString strResponsible, SString strReason )
{
    // Make sure we have a player
    assert ( pPlayer );

    // If our responsible string is too long, crop it to size and display ... in the end so it's obvious it's cropped
    if ( strResponsible.length( ) > MAX_KICK_RESPONSIBLE_LENGTH )
        strResponsible = strResponsible.substr ( 0, MAX_KICK_RESPONSIBLE_LENGTH - 3 ) + "...";

    // Declare the strings for use later on
    SString strMessage;
    SString strInfoMessage;
    
    // Get the size of the reason
    size_t sizeReason = strReason.length ( );

    // Got any reason?
    if ( sizeReason >= MIN_KICK_REASON_LENGTH )
    {
        // If our reaon string is too long, crop it to size and display ... in the end so it's obvious it's cropped
        if ( sizeReason > MAX_KICK_REASON_LENGTH )
            strReason = strReason.substr ( 0, MAX_KICK_REASON_LENGTH - 3 ) + "...";

        // Now create the messages which will be displayed to both the kicked player and the console
        strMessage.Format ( "You were kicked by %s (%s)", strResponsible.c_str( ), strReason.c_str( ) );
        strInfoMessage.Format ( "%s was kicked from the game by %s (%s)", pPlayer->GetNick( ), strResponsible.c_str( ), strReason.c_str( ) );
    }
    else
    {
        // Now create the messages which will be displayed to both the kicked player and the console
        strMessage.Format ( "You were kicked by %s", strResponsible.c_str( ) );
        strInfoMessage.Format ( "%s was kicked from the game by %s", pPlayer->GetNick( ), strResponsible.c_str( ) );
    }

    // Tell the player that was kicked why. QuitPlayer will delete the player.
    pPlayer->Send ( CPlayerDisconnectedPacket ( strMessage.c_str ( ) ) );
    g_pGame->QuitPlayer ( *pPlayer, CClient::QUIT_KICK, false, strReason.c_str( ), strResponsible.c_str( ) );

    // Tell everyone else that he was kicked from the game including console
    // m_pPlayerManager->BroadcastOnlyJoined ( CChatEchoPacket ( szInfoMessage, CHATCOLOR_INFO ) );
    CLogger::LogPrintf ( "KICK: %s\n", strInfoMessage.c_str ( ) );

    return true;
}


CBan* CStaticFunctionDefinitions::BanPlayer ( CPlayer* pPlayer, bool bIP, bool bUsername, bool bSerial, CPlayer* pResponsible, SString strResponsible, SString strReason, time_t tUnban )
{
    // Make sure we have a player
    assert ( pPlayer );

    // Initialize variables
    CBan* pBan = NULL;

    SString strMessage;
    SString strInfoMessage;

    // If the responsible string is too long, crop it
    if ( strResponsible.length ( ) > MAX_BAN_RESPONSIBLE_LENGTH )
        strResponsible = strResponsible.substr ( 0, MAX_BAN_RESPONSIBLE_LENGTH - 3 ) + "...";

    // Check if there's a reason
    size_t sizeReason = strReason.length( );
    if ( sizeReason >= MIN_BAN_REASON_LENGTH )
    {
        // If the reason is too long, crop it
        if ( sizeReason > MAX_BAN_REASON_LENGTH )
            strReason = strReason.substr ( 0, MAX_BAN_REASON_LENGTH - 3 ) + "...";

        // Format the messages for both the banned player and the console
        strMessage.Format     ( "You were banned by %s (%s)", strResponsible.c_str(), strReason.c_str() );
        strInfoMessage.Format ( "%s was banned from the game by %s (%s)", pPlayer->GetNick(), strResponsible.c_str(), strReason.c_str() );
    }
    else
    {
        // Format the messages for both the banned player and the console
        strMessage.Format     ( "You were banned by %s", strResponsible.c_str() );
        strInfoMessage.Format ( "%s was banned from the game by %s", pPlayer->GetNick(), strResponsible.c_str() );
    }

    // Ban the player
    if ( bIP )
        pBan = m_pBanManager->AddBan ( pPlayer, strResponsible, strReason, tUnban );
    else if (  bUsername || bSerial )
        pBan = m_pBanManager->AddBan ( strResponsible, strReason, tUnban );

    // If the ban was successful
    if ( pBan )
    {
        // Set the data if banned by either username or serial
        if ( bUsername ) pBan->SetAccount ( pPlayer->GetSerialUser () );
        if ( bSerial ) pBan->SetSerial ( pPlayer->GetSerial () );

        // Check if we passed a responsible player
        if ( pResponsible )
        {
            // Call the event with the responsible player as the source
            CLuaArguments Arguments;
            Arguments.PushBan ( pBan );
            pResponsible->CallEvent ( "onBan", Arguments );
        }
        else
        {
            // Call the event with the root element as the source
            CLuaArguments Arguments;
            Arguments.PushBan ( pBan );
            m_pMapManager->GetRootElement()->CallEvent ( "onBan", Arguments );
        }

        // Save the ban list
        m_pBanManager->SaveBanList ();

        // Call the event
        CLuaArguments Arguments;
        Arguments.PushBan ( pBan );
        if ( pResponsible )
            Arguments.PushElement ( pResponsible );
        pPlayer->CallEvent ( "onPlayerBan", Arguments );

        // Tell the player that was banned why. QuitPlayer will delete the player.
        pPlayer->Send ( CPlayerDisconnectedPacket ( strMessage.c_str() ) );
        g_pGame->QuitPlayer ( *pPlayer, CClient::QUIT_BAN, false, strReason.c_str(), strResponsible.c_str() );

        // Tell everyone else that he was banned from the game including console
        // m_pPlayerManager->BroadcastOnlyJoined ( CChatEchoPacket ( szInfoMessage, CHATCOLOR_INFO ) );
        CLogger::LogPrintf ( "BAN: %s\n", strInfoMessage.c_str() );

        return pBan;
    }
    return NULL;
}


CBan* CStaticFunctionDefinitions::AddBan ( SString strIP, SString strUsername, SString strSerial, CPlayer* pResponsible, SString strResponsible, SString strReason, time_t tUnban )
{
    CBan* pBan = NULL;

    // Check if the IP, username or serial are specified
    bool bIPSpecified       = strIP.length      ( ) > 0;
    bool bUsernameSpecified = strUsername.length( ) > 0;
    bool bSerialSpecified   = strSerial.length  ( ) == 32;

    // Crop the responsible string if too long
    if ( strResponsible.length ( ) > MAX_BAN_RESPONSIBLE_LENGTH )
        strResponsible = strResponsible.substr ( 0, MAX_BAN_RESPONSIBLE_LENGTH - 3 ) + "...";

    // Got an IP?
    if ( bIPSpecified && !m_pBanManager->IsSpecificallyBanned ( strIP ) )
    {
        pBan = m_pBanManager->AddBan ( strIP, strResponsible, strReason, tUnban );
    }
    // If not IP provided make sure a username or serial are there
    else if ( bSerialSpecified && !m_pBanManager->IsSerialBanned ( strSerial ) )
    {
        pBan = m_pBanManager->AddBan ( strResponsible, strReason, tUnban );
    }
    else if ( bUsernameSpecified && !m_pBanManager->IsAccountBanned ( strUsername ) )
    {
        pBan = m_pBanManager->AddBan ( strResponsible, strReason, tUnban );
    }

    // If the ban was added
    if ( pBan )
    {
        // Initialize the details value
        SString strDetails;

        // Check if there's a reason
        size_t sizeReason = strReason.length( );
        if ( sizeReason >= MIN_BAN_REASON_LENGTH )
        {
            // If it's too long, crop it
            if ( sizeReason > MAX_BAN_REASON_LENGTH )
                strReason = strReason.substr ( 0, MAX_BAN_REASON_LENGTH - 3 ) + "...";

            // Add reason to the details
            strDetails += " (" + strReason + ")";
        }

        // Check if there's a duration
        SString strDurationDesc = pBan->GetDurationDesc ();
        if ( strDurationDesc.length () )
        {
            // Add duration to the details
            strDetails += " (" + strDurationDesc + ")";
        }


        // Format the responsible element and the reason/duration into the message string
        SString strMessage ( "You were banned by %s%s", strResponsible.c_str(), strDetails.c_str () );

        // Limit overall length of message
        if ( strMessage.length () > 255 )
            strMessage = strMessage.substr ( 0, 255 );


        // Set the account or serial if either one is set to be banned
        if ( bUsernameSpecified ) pBan->SetAccount ( strUsername );
        if ( bSerialSpecified )   pBan->SetSerial  ( strSerial );

        // Check if we passed a responsible player
        if ( pResponsible )
        {
            // Call the event with the responsible player as the source
            CLuaArguments Arguments;
            Arguments.PushBan ( pBan );
            pResponsible->CallEvent ( "onBan", Arguments );
        }
        else
        {
            // Call the event with the root element as the source
            CLuaArguments Arguments;
            Arguments.PushBan ( pBan );
            m_pMapManager->GetRootElement()->CallEvent ( "onBan", Arguments );
        }

        // Log
        if ( bIPSpecified )
            CLogger::LogPrintf ( "BAN: %s was banned by %s%s\n", strIP.c_str(), strResponsible.c_str(), strDetails.c_str () );
        else if ( bUsernameSpecified )
            CLogger::LogPrintf ( "BAN: %s was banned by %s%s\n", strUsername.c_str(), strResponsible.c_str(), strDetails.c_str () );
        else
            CLogger::LogPrintf ( "BAN: Serial ban was added by %s%s\n", strResponsible.c_str(), strDetails.c_str () );

        // Initialize a variable to indicate whether the ban's nick has been set
        bool bNickSet = false;

        // Loop through players to see if we should kick anyone
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            // Default to not banning; if the IP, serial and username don't match, we don't want to kick the guy out
            bool bBan = false;

            // Check if the player's IP matches the specified one, if specified
            if ( bIPSpecified )
            {
                bBan = ( strIP == ( *iter )->GetSourceIP () );
            }

            // Check if the player's username matches the specified one, if specified, and he wasn't banned over IP yet
            if ( !bBan && bUsernameSpecified )
            {
                const std::string& strPlayerUsername = (*iter)->GetSerialUser ();
                bBan = stricmp ( strPlayerUsername.c_str (), strUsername.c_str () ) == 0;
            }

            // Check if the player's serial matches the specified one, if specified, and he wasn't banned over IP or username yet
            if ( !bBan && bSerialSpecified )
            {
                const std::string& strPlayerSerial = (*iter)->GetSerial ();
                bBan = stricmp ( strPlayerSerial.c_str (), strSerial.c_str () ) == 0;
            }

            // If either the IP, serial or username matched
            if ( bBan )
            {
                // Set the nick of the ban if this hasn't been done yet
                if ( !bNickSet )
                {
                    pBan->SetNick ( (*iter)->GetNick() );
                    bNickSet = true;
                }

                // Call the event
                CLuaArguments Arguments;
                Arguments.PushBan ( pBan );
                if ( pResponsible )
                    Arguments.PushElement ( pResponsible );
                (*iter)->CallEvent ( "onPlayerBan", Arguments );

                // Tell the player that was banned why. QuitPlayer will delete the player.
                (*iter)->Send ( CPlayerDisconnectedPacket ( strMessage.c_str() ) );
                g_pGame->QuitPlayer ( **iter, CClient::QUIT_BAN, false, strReason.c_str (), strResponsible.c_str () );
            }
        }

        // Save the ban list (at the end of the function so it saves after the nick has been set)
        m_pBanManager->SaveBanList ();

        // Return the ban
        return pBan;
    }
    return NULL;
}


bool CStaticFunctionDefinitions::RemoveBan ( CBan* pBan, CPlayer* pResponsible )
{
    CLuaArguments Arguments;
    Arguments.PushBan ( pBan );
    if ( pResponsible )
        Arguments.PushElement ( pResponsible );
    m_pMapManager->GetRootElement()->CallEvent ( "onUnban", Arguments );


    CLogger::LogPrintf ( "UNBAN: A ban was removed by %s\n", ( pResponsible ) ? pResponsible->GetNick () : "Console" );

    m_pBanManager->RemoveBan ( pBan );

    return true;
}

bool CStaticFunctionDefinitions::GetBans ( lua_State* pLua )
{
    list < CBan* > ::const_iterator iter = m_pBanManager->IterBegin();
    unsigned int uiIndex = 0;

    for ( ; iter != m_pBanManager->IterEnd(); iter++ )
    {
        lua_pushnumber ( pLua, ++uiIndex );
        lua_pushban ( pLua, *iter );
        lua_settable ( pLua, -3 );
    }
    return true;
}


bool CStaticFunctionDefinitions::ReloadBanList ( void )
{
    return m_pBanManager->ReloadBanList ();
}


bool CStaticFunctionDefinitions::GetBanIP ( CBan* pBan, SString& strOutIP )
{
    if ( !pBan->GetIP ().empty () )
    {
        strOutIP = pBan->GetIP ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBanSerial ( CBan* pBan, SString& strOutSerial )
{
    if ( !pBan->GetSerial ().empty () )
    {
        strOutSerial = pBan->GetSerial ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBanUsername ( CBan* pBan, SString& strOutUsername )
{
    if ( !pBan->GetAccount ().empty () )
    {
        strOutUsername = pBan->GetAccount ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBanNick ( CBan* pBan, SString& strOutNick )
{
    if ( !pBan->GetNick ().empty () )
    {
        strOutNick = pBan->GetNick ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBanTime ( CBan* pBan, time_t& time )
{
    time = pBan->GetTimeOfBan ();
    return true;
}


bool CStaticFunctionDefinitions::GetUnbanTime ( CBan* pBan, time_t& time )
{
    time = pBan->GetTimeOfUnban ();
    return true;
}


bool CStaticFunctionDefinitions::GetBanReason ( CBan* pBan, SString& strOutReason )
{
    if ( !pBan->GetReason ().empty () )
    {
        strOutReason = pBan->GetReason ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::GetBanAdmin ( CBan* pBan, SString& strOutAdmin )
{
    if ( !pBan->GetBanner ().empty () )
    {
        strOutAdmin = pBan->GetBanner ();
        return true;
    }
    return false;
}


bool CStaticFunctionDefinitions::IsCursorShowing ( CPlayer* pPlayer, bool& bShowing )
{
    assert ( pPlayer );

    bShowing = pPlayer->IsCursorShowing ();
    return true;
}


bool CStaticFunctionDefinitions::ShowCursor ( CElement* pElement, CLuaMain* pLuaMain, bool bShow, bool bToggleControls )
{
    assert ( pElement );
    RUN_CHILDREN ShowCursor ( *iter, pLuaMain, bShow, bToggleControls );

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
            BitStream.pBitStream->Write ( static_cast < unsigned short > ( pResource->GetNetID () ) );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( ( bToggleControls ) ? 1 : 0 ) );
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
            pPlayer->Reset();
            TakeAllWeapons( pPlayer );

            return true;
        }
    }
    else
    {
        CBitStream BitStream;
        m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( RESET_MAP_INFO, *BitStream.pBitStream ) );
        g_pGame->ResetMapInfo();
        m_pPlayerManager->ResetAll();
        TakeAllWeapons ( GetRootElement() );
        return true;
    }

    return false;
}


CElement* CStaticFunctionDefinitions::GetResourceMapRootElement ( CResource* pResource, const char* szMap )
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


CXMLNode* CStaticFunctionDefinitions::AddResourceMap ( CResource* pResource, const std::string& strFilePath, const std::string& strMapName, int iDimension, CLuaMain* pLUA )
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
                // Does this file already exist in this resource?
                if ( !pResource->IncludedFileExists ( strMapName.c_str(), CResourceFile::RESOURCE_FILE_TYPE_NONE ) )
                {
                    CXMLFile* pXML = pLUA->CreateXML ( strFilePath.c_str() );
                    if ( pXML )
                    {
                        // Create a root node
                        CXMLNode* pRootNode = pXML->CreateRootNode ( "map" );

                        // Write it so the file is created
                        if ( pRootNode && pXML->Write () )
                        {
                            // Add it to the resource's meta file
                            if ( pResource->AddMapFile ( strMapName.c_str(), strFilePath.c_str(), iDimension ) )
                            {
                                // Return the created XML's root node
                                return pRootNode;
                            }
                            else
                            {
                                CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Unable to alter meta file\n", strMapName.c_str(), pResource->GetName ().c_str () );
                            }
                        }
                        else
                        {
                            CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Unable to write XML\n", strMapName.c_str(), pResource->GetName ().c_str () );
                        }

                        // Destroy the XML if we failed
                        pLUA->DestroyXML ( pXML );
                    }
                }
                else
                    CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; File already exists in resource\n", strMapName.c_str(), pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is in a zip file\n", strMapName.c_str(), pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is in use\n", strMapName.c_str(), pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to add map %s to resource %s; Resource is not loaded\n", strMapName.c_str(), pResource->GetName ().c_str () );

    // Failed
    return NULL;
}


CXMLNode* CStaticFunctionDefinitions::AddResourceConfig ( CResource* pResource, const std::string& strFilePath, const std::string& strConfigName, int iType, CLuaMain* pLUA )
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
                // Does this file already exist in this resource?
                if ( !pResource->IncludedFileExists ( strConfigName.c_str(), CResourceFile::RESOURCE_FILE_TYPE_NONE ) )
                {
                    CXMLFile* pXML = pLUA->CreateXML ( strFilePath.c_str() );
                    if ( pXML )
                    {
                        // Create a root node
                        CXMLNode* pRootNode = pXML->CreateRootNode ( "root" );

                        // Write it so the file is created
                        if ( pRootNode && pXML->Write () )
                        {
                            // Add it to the resource's meta file
                            if ( pResource->AddConfigFile ( strConfigName.c_str(), strFilePath.c_str(), iType ) )
                            {
                                // Return the created XML's root node
                                return pRootNode;
                            }
                            else
                            {
                                CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to alter meta file\n", strConfigName.c_str(), pResource->GetName ().c_str () );
                            }
                        }
                        else
                        {
                            CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to write XML\n", strConfigName.c_str(), pResource->GetName ().c_str () );
                        }

                        // Destroy the XML if we failed
                        pLUA->DestroyXML ( pXML );
                    }
                    else
                        CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Unable to create XML\n", strConfigName.c_str(), pResource->GetName ().c_str () );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; File already exists in resource\n", strConfigName.c_str(), pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is in a zip file\n", strConfigName.c_str(), pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is in use\n", strConfigName.c_str(), pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to add config %s to resource %s; Resource is not loaded\n", strConfigName.c_str(), pResource->GetName ().c_str () );

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
                        CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; File does not exist\n", szFilename, pResource->GetName ().c_str () );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Bad filename\n", szFilenameUnmodified, pResource->GetName ().c_str () );
            }
            else
                CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is in a zip file\n", szFilenameUnmodified, pResource->GetName ().c_str () );
        }
        else
            CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is in use\n", szFilenameUnmodified, pResource->GetName ().c_str () );
    }
    else
        CLogger::ErrorPrintf ( "Unable to remove file %s from resource %s; Resource is not loaded\n", szFilenameUnmodified, pResource->GetName ().c_str () );

    // Failed
    return false;
}


/** Version functions **/
unsigned long CStaticFunctionDefinitions::GetVersion ()
{
    return MTA_DM_VERSION;
}

const char* CStaticFunctionDefinitions::GetVersionString ()
{
    return MTA_DM_VERSIONSTRING;
}

const char* CStaticFunctionDefinitions::GetVersionName ()
{
    return MTA_DM_FULL_STRING;
}

SString CStaticFunctionDefinitions::GetVersionBuildType ()
{
    SString strResult = MTA_DM_BUILDTYPE;
    strResult[0] = toupper ( strResult[0] );
    return strResult;
}

unsigned long CStaticFunctionDefinitions::GetNetcodeVersion ()
{
    return MTA_DM_NETCODE_VERSION;
}

const char* CStaticFunctionDefinitions::GetOperatingSystemName ()
{
    return MTA_OS_STRING;
}

const char* CStaticFunctionDefinitions::GetVersionBuildTag ()
{
    return MTA_DM_BUILDTAG_LONG;
}

SString CStaticFunctionDefinitions::GetVersionSortable ()
{
    return SString ( "%d.%d.%d-%d.%05d.%d"
                            ,MTASA_VERSION_MAJOR
                            ,MTASA_VERSION_MINOR
                            ,MTASA_VERSION_MAINTENANCE
                            ,MTASA_VERSION_TYPE
                            ,MTASA_VERSION_BUILD
                            ,0
                            );
}
