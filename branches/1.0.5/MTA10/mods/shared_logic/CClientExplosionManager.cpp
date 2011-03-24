/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientExplosionManager.cpp
*  PURPOSE:     Explosion event manager class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

CClientExplosionManager * g_pExplosionManager = NULL;

CClientExplosionManager::CClientExplosionManager ( CClientManager * pManager )
{
    g_pExplosionManager = this;
    m_pManager = pManager;
    m_LastWeaponType = WEAPONTYPE_UNARMED;
    m_pLastCreator = NULL;
}


CClientExplosionManager::~CClientExplosionManager ( void )
{
    if ( g_pExplosionManager == this ) g_pExplosionManager = NULL;
}


bool CClientExplosionManager::Hook_StaticExplosionCreation ( CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition, eExplosionType explosionType )
{
    return g_pExplosionManager->Hook_ExplosionCreation ( pGameExplodingEntity, pGameCreator, vecPosition, explosionType );
}


bool CClientExplosionManager::Hook_ExplosionCreation ( CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition, eExplosionType explosionType )
{
    CClientPlayer * pLocalPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();

    // Grab the entity responsible
    CClientEntity * pResponsible = NULL;
    CEntity* pResponsibleGameEntity = ( pGameExplodingEntity ) ? pGameExplodingEntity : pGameCreator;
    if ( pResponsibleGameEntity )
        pResponsible = m_pManager->FindEntity ( pResponsibleGameEntity, false );

    unsigned short usModel;
    if ( pResponsible && ( pResponsible->IsLocalEntity () || ( CStaticFunctionDefinitions::GetElementModel ( *pResponsible, usModel ) && CClientObjectManager::IsBreakableModel ( usModel ) ) ) )
        return true;    // Handle this explosion client side only if entity is local or breakable (i.e. barrel)

    eWeaponType explosionWeaponType;
    switch ( explosionType )
    {
        case EXP_TYPE_GRENADE:
        {
            // Grenade type explosions from vehicles should only be freefall bombs
            // TODO: need a way to check if its a freefall bomb if creator is a ped
            if ( pGameCreator && pGameCreator->GetEntityType () == ENTITY_TYPE_VEHICLE )
                explosionWeaponType = WEAPONTYPE_FREEFALL_BOMB;
            else
                explosionWeaponType = WEAPONTYPE_GRENADE;
            break;
        }
        case EXP_TYPE_MOLOTOV:
            explosionWeaponType = WEAPONTYPE_MOLOTOV;
            break;
        case EXP_TYPE_ROCKET:
        case EXP_TYPE_ROCKET_WEAK:
            explosionWeaponType = WEAPONTYPE_ROCKET;
            break;
        case EXP_TYPE_TANK_GRENADE:
            explosionWeaponType = WEAPONTYPE_TANK_GRENADE;
            break;
        default:
            explosionWeaponType = WEAPONTYPE_EXPLOSION;
            break;
    }

    // Got a responsible entity?
    if ( pResponsible )
    {
        // Is the local player responsible for this?
        bool bLocal = ( ( pResponsible == pLocalPlayer ) ||
                      ( pResponsible == pLocalPlayer->GetOccupiedVehicle () ) ||
                      ( g_pClientGame->GetUnoccupiedVehicleSync ()->Exists ( static_cast < CDeathmatchVehicle * > ( pResponsible ) ) ) );

        if ( bLocal )
        {
            CClientEntity * pOriginSource = NULL;

            // Is this an exploding vehicle?
            if ( pGameExplodingEntity && pGameExplodingEntity->GetEntityType () == ENTITY_TYPE_VEHICLE )
            {
                // Set our origin-source to the vehicle
                pOriginSource = m_pManager->FindEntity ( pGameExplodingEntity, false );
            }                
            // If theres other players, sync it relative to the closest (lag compensation)
            else if ( m_pManager->GetPlayerManager ()->Count () > 1 )
            {
                switch ( explosionWeaponType )
                {
                    case WEAPONTYPE_ROCKET:
                    case WEAPONTYPE_ROCKET_HS:
                    {
                        CClientPlayer * pPlayer = g_pClientGame->GetClosestRemotePlayer ( vecPosition, 200.0f );
                        if ( pPlayer )
                        {
                            pOriginSource = pPlayer;
                        }
                        break;
                    }
                }                                   
            }
            // Request a new explosion
            g_pClientGame->SendExplosionSync ( vecPosition, explosionType, pOriginSource );
        }
    }

    // All explosions are handled server side
    return false;
}


CExplosion * CClientExplosionManager::Create ( eExplosionType explosionType, CVector & vecPosition, CClientEntity * pCreator, bool bMakeSound, float fCamShake, bool bNoDamage, eWeaponType responsibleWeapon )
{
    CEntity * pGameCreator = NULL;
    if ( pCreator ) pGameCreator = pCreator->GetGameEntity ();

    // Update our records first?
    m_pLastCreator = pCreator;
    if ( responsibleWeapon != WEAPONTYPE_UNARMED ) m_LastWeaponType = responsibleWeapon;
    else
    {
        switch ( explosionType )
        {
            case EXP_TYPE_GRENADE: m_LastWeaponType = WEAPONTYPE_GRENADE; break;
            case EXP_TYPE_MOLOTOV: m_LastWeaponType = WEAPONTYPE_MOLOTOV; break;
            case EXP_TYPE_ROCKET:
            case EXP_TYPE_ROCKET_WEAK: m_LastWeaponType = WEAPONTYPE_ROCKET; break;
            case EXP_TYPE_TANK_GRENADE: m_LastWeaponType = WEAPONTYPE_TANK_GRENADE; break;
            default: m_LastWeaponType = WEAPONTYPE_EXPLOSION; break;
        }
    }

    CExplosion * pExplosion = g_pGame->GetExplosionManager ()->AddExplosion ( NULL, pGameCreator, explosionType, vecPosition, 0, bMakeSound, fCamShake, bNoDamage );
    return pExplosion;
}