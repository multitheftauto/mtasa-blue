/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CGameEntityXRefManager
{
public:
    virtual                     ~CGameEntityXRefManager             ( void ) {}

    virtual void                AddEntityXRef                       ( CClientEntity* pEntity, CEntity* pEntitySA ) = 0;
    virtual void                RemoveEntityXRef                    ( CClientEntity* pEntity, CEntity* pEntitySA ) = 0;
    virtual CClientEntity*      FindClientEntity                    ( CEntitySAInterface* pEntitySAInterface ) = 0;
    virtual CClientEntity*      FindClientEntity                    ( CEntity* pEntitySA ) = 0;
    virtual CClientObject*      FindClientObject                    ( CEntity* pObjectSA ) = 0;
    virtual CClientPed*         FindClientPed                       ( CEntity* pPedSA ) = 0;
    virtual CClientVehicle*     FindClientVehicle                   ( CEntity* pVehicleSA ) = 0;
    virtual void                OnClientEntityCreate                ( CClientEntity* pEntity ) = 0;
    virtual void                OnClientEntityDelete                ( CClientEntity* pEntity ) = 0;
    virtual void                OnGameEntityDestruct                ( CEntitySAInterface* pEntitySAInterface ) = 0;
    virtual void                OnGameModelRemove                   ( ushort usModelId ) = 0;
};

CGameEntityXRefManager* NewGameEntityXRefManager ( void );
