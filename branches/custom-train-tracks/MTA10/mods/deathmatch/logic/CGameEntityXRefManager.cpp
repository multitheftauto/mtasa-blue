/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


///////////////////////////////////////////////////////////////
//
//
// CGameEntityXRefManager
//
//
///////////////////////////////////////////////////////////////
class CGameEntityXRefManagerImpl : public CGameEntityXRefManager
{
public:
    ZERO_ON_NEW

    // CGameEntityXRefManager interface
    virtual void                AddEntityXRef                       ( CClientEntity* pEntity, CEntity* pEntitySA );
    virtual void                RemoveEntityXRef                    ( CClientEntity* pEntity, CEntity* pEntitySA );
    virtual CClientEntity*      FindClientEntity                    ( CEntitySAInterface* pEntitySAInterface );
    virtual CClientObject*      FindClientObject                    ( CEntitySAInterface* pObjectSAInterface );
    virtual CClientPed*         FindClientPed                       ( CEntitySAInterface* pPedSAInterface );
    virtual CClientVehicle*     FindClientVehicle                   ( CEntitySAInterface* pVehicleSAInterface );
    virtual CClientEntity*      FindClientEntity                    ( CEntity* pEntitySA );
    virtual CClientObject*      FindClientObject                    ( CEntity* pObjectSA );
    virtual CClientPed*         FindClientPed                       ( CEntity* pPedSA );
    virtual CClientVehicle*     FindClientVehicle                   ( CEntity* pVehicleSA );
    virtual void                OnClientEntityCreate                ( CClientEntity* pEntity );
    virtual void                OnClientEntityDelete                ( CClientEntity* pEntity );
    virtual void                OnGameEntityDestruct                ( CEntitySAInterface* pEntitySAInterface );
    virtual void                OnGameModelRemove                   ( ushort usModelId );

    // CGameEntityXRefManagerImpl methods
                                CGameEntityXRefManagerImpl          ( void );
                                ~CGameEntityXRefManagerImpl         ( void );
protected:
    std::map < CClientEntity*, CEntity* >               m_ClientToGameMap;
    std::map < CEntity*, CClientEntity* >               m_GameToClientMap;
    std::map < CEntitySAInterface*, CClientEntity* >    m_InterfaceToClientMap;
    std::set < CClientEntity* >                         m_ValidClientEntityMap;
};


///////////////////////////////////////////////////////////////
//
// Global new
//
//
///////////////////////////////////////////////////////////////
CGameEntityXRefManager* NewGameEntityXRefManager ()
{
    return new CGameEntityXRefManagerImpl ();
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::CGameEntityXRefManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CGameEntityXRefManagerImpl::CGameEntityXRefManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::~CGameEntityXRefManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CGameEntityXRefManagerImpl::~CGameEntityXRefManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::AddEntityXRef
//
// Add a new reference between a game entity and a client entity
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::AddEntityXRef ( CClientEntity* pEntity, CEntity* pEntitySA )
{
    CEntitySAInterface* pEntitySAInterface = pEntitySA->GetInterface ();

    assert ( !MapContains ( m_ClientToGameMap, pEntity ) );
    assert ( !MapContains ( m_GameToClientMap, pEntitySA ) );
    assert ( !MapContains ( m_InterfaceToClientMap, pEntitySAInterface ) );

    MapSet ( m_ClientToGameMap, pEntity, pEntitySA );
    MapSet ( m_GameToClientMap, pEntitySA, pEntity );
    MapSet ( m_InterfaceToClientMap, pEntitySAInterface, pEntity );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::RemoveEntityXRef
//
// Remove an existing reference between a game entity and a client entity
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::RemoveEntityXRef ( CClientEntity* pEntity, CEntity* pEntitySA )
{
    CEntitySAInterface* pEntitySAInterface = pEntitySA->GetInterface ();

    assert ( MapFindRef ( m_ClientToGameMap, pEntity ) == pEntitySA );
    assert ( MapFindRef ( m_GameToClientMap, pEntitySA ) == pEntity );
    assert ( MapFindRef ( m_InterfaceToClientMap, pEntitySAInterface ) == pEntity );

    MapRemove ( m_ClientToGameMap, pEntity );
    MapRemove ( m_GameToClientMap, pEntitySA );
    MapRemove ( m_InterfaceToClientMap, pEntitySAInterface );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::FindClientEntity
//
// Find the client entity that is using the supplied game entity interface
//
///////////////////////////////////////////////////////////////
CClientEntity* CGameEntityXRefManagerImpl::FindClientEntity ( CEntitySAInterface* pEntitySAInterface )
{
    return MapFindRef ( m_InterfaceToClientMap, pEntitySAInterface );
}

CClientObject* CGameEntityXRefManagerImpl::FindClientObject ( CEntitySAInterface* pObjectSAInterface )
{
    return DynamicCast < CClientObject > ( FindClientEntity ( pObjectSAInterface ) );
}

CClientPed* CGameEntityXRefManagerImpl::FindClientPed ( CEntitySAInterface* pPedSAInterface )
{
    return DynamicCast < CClientPed > ( FindClientEntity ( pPedSAInterface ) );
}

CClientVehicle* CGameEntityXRefManagerImpl::FindClientVehicle ( CEntitySAInterface* pVehicleSAInterface )
{
    return DynamicCast < CClientVehicle > ( FindClientEntity ( pVehicleSAInterface ) );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::FindClientEntity
//
// Find the client entity that is using the supplied game entity
//
///////////////////////////////////////////////////////////////
CClientEntity* CGameEntityXRefManagerImpl::FindClientEntity ( CEntity* pEntitySA )
{
    return MapFindRef ( m_GameToClientMap, pEntitySA );
}

CClientObject* CGameEntityXRefManagerImpl::FindClientObject ( CEntity* pObjectSA )
{
    return DynamicCast < CClientObject > ( FindClientEntity ( pObjectSA ) );
}

CClientPed* CGameEntityXRefManagerImpl::FindClientPed ( CEntity* pPedSA )
{
    return DynamicCast < CClientPed > ( FindClientEntity ( pPedSA ) );
}

CClientVehicle* CGameEntityXRefManagerImpl::FindClientVehicle ( CEntity* pVehicleSA )
{
    return DynamicCast < CClientVehicle > ( FindClientEntity ( pVehicleSA ) );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::OnClientEntityCreate
//
// Track valid ClientEntity instances
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::OnClientEntityCreate ( CClientEntity* pEntity )
{
    assert ( !MapContains ( m_ValidClientEntityMap, pEntity ) );
    MapInsert ( m_ValidClientEntityMap, pEntity );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::OnClientEntityDelete
//
// Track valid ClientEntity instances
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::OnClientEntityDelete ( CClientEntity* pEntity )
{
    assert ( MapContains ( m_ValidClientEntityMap, pEntity ) );
    MapRemove ( m_ValidClientEntityMap, pEntity );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::OnGameEntityDestruct
//
// Called when GTA is destroying a game entity
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::OnGameEntityDestruct ( CEntitySAInterface* pEntitySAInterface )
{
    if ( MapContains ( m_InterfaceToClientMap, pEntitySAInterface ) )
    {
        BYTE* pInterface = (BYTE*)pEntitySAInterface;
        DWORD InterfaceVtbl = *(DWORD*)pInterface;
        ushort InterfaceModelId = *(ushort*)(pInterface + 34);
        CClientEntity* pClientEntity = MapFindRef ( m_InterfaceToClientMap, pEntitySAInterface );
        bool bClientEntityValid = MapContains ( m_ValidClientEntityMap, pClientEntity );

        SString strClientEntityInfo;
        if ( bClientEntityValid )
        {
            CEntity* pGameEntity = NULL;

            if ( CClientPed* pPed = DynamicCast < CClientPed > ( pClientEntity ) )
                pGameEntity = pPed->GetGameEntity ();

            if ( CClientVehicle* pVehicle = DynamicCast < CClientVehicle > ( pClientEntity ) )
                pGameEntity = pVehicle->GetGameEntity ();

            if ( CClientObject* pObject = DynamicCast < CClientObject > ( pClientEntity ) )
                pGameEntity = pObject->GetGameEntity ();

            if ( CClientProjectile* pProjectile = DynamicCast < CClientProjectile > ( pClientEntity ) )
                pGameEntity = pProjectile->GetGameEntity ();

            if ( CClientPickup* pPickup = DynamicCast < CClientPickup > ( pClientEntity ) )
                pGameEntity = pPickup->GetGameObject ();

            CEntity* pMappedGameEntity = MapFindRef ( m_ClientToGameMap, pClientEntity );
            CClientEntity* pMappedClientEntity = MapFindRef ( m_GameToClientMap, pGameEntity );


            strClientEntityInfo = SString ( "%s Id:%x GameEntity:%08x MappedGameEntity:%08x MappedClientEntity:%08x"
                                            , pClientEntity->GetClassName ()
                                            , pClientEntity->GetID ()
                                            , (int)pGameEntity
                                            , (int)pMappedGameEntity
                                            , (int)pMappedClientEntity
                                        );
        }

        SString strMessage ( "EntitySAInterface:%08x  Vtbl:%08x  ModelId:%d   ClientEntity:%08x  [%s]"
                                , (int)pEntitySAInterface
                                , InterfaceVtbl
                                , InterfaceModelId
                                , (int)pClientEntity
                                , *strClientEntityInfo
                                );

        g_pCore->LogEvent ( 8542, "XRefManager", "GameEntity Mismatch", strMessage );
        AddReportLog ( 8542, strMessage );
        dassert ( 0 );
    }
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::OnGameModelRemove
//
// Called when GTA is removing model data
//
///////////////////////////////////////////////////////////////
void CGameEntityXRefManagerImpl::OnGameModelRemove ( ushort usModelId )
{
    // TODO - Something clever
}
