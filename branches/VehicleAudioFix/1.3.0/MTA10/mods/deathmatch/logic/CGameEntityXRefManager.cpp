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


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::FindClientObject
//
// Find the client object that is using the supplied game entity
//
///////////////////////////////////////////////////////////////
CClientObject* CGameEntityXRefManagerImpl::FindClientObject ( CEntity* pObjectSA )
{
    return DynamicCast < CClientObject > ( FindClientEntity ( pObjectSA ) );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::FindClientPed
//
// Find the client ped that is using the supplied game entity
//
///////////////////////////////////////////////////////////////
CClientPed* CGameEntityXRefManagerImpl::FindClientPed ( CEntity* pPedSA )
{
    return DynamicCast < CClientPed > ( FindClientEntity ( pPedSA ) );
}


///////////////////////////////////////////////////////////////
//
// CGameEntityXRefManagerImpl::FindClientVehicle
//
// Find the client vehicle that is using the supplied game entity
//
///////////////////////////////////////////////////////////////
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
    assert ( !MapContains ( m_InterfaceToClientMap, pEntitySAInterface ) );
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
