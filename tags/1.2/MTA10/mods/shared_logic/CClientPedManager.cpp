/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedManager.cpp
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

CClientPedManager::CClientPedManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bRemoveFromList = true;
}


CClientPedManager::~CClientPedManager ( void )
{
    DeleteAll ();
}


void CClientPedManager::DeleteAll ( void )
{
    m_bRemoveFromList = false;
    vector < CClientPed* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_List.clear ();
    m_bRemoveFromList = true;
}


void CClientPedManager::DoPulse ( void )
{   
    CClientPed * pPed = NULL;
    // Loop through our streamed-in peds
    vector < CClientPed * > List = m_StreamedIn;
    vector < CClientPed* > ::iterator iter = List.begin ();
    for ( ; iter != List.end (); ++iter )
    {
        pPed = *iter;
        // We should have a game ped here
        assert ( pPed->GetGamePlayer () );
        pPed->StreamedInPulse ();
    }
}


CClientPed* CClientPedManager::Get ( ElementID ID, bool bCheckPlayers )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && ( pEntity->GetType () == CCLIENTPED || ( bCheckPlayers && pEntity->GetType () == CCLIENTPLAYER ) ) )
    {
        return static_cast < CClientPed* > ( pEntity );
    }

    return NULL;
}


CClientPed* CClientPedManager::Get ( CPlayerPed* pPlayer, bool bValidatePointer, bool bCheckPlayers )
{
    if ( !pPlayer ) return NULL;

    if ( bValidatePointer )
    {
        vector < CClientPed* > ::const_iterator iter = m_StreamedIn.begin ();
        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGamePlayer () == pPlayer )
            {
                return *iter;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast < CClientPed* > ( pPlayer->GetStoredPointer () );
        if ( pPed->GetType () == CCLIENTPED || bCheckPlayers )
        {
            return pPed;
        }
    }
    return NULL;
}


CClientPed* CClientPedManager::Get ( RpClump * pClump, bool bCheckPlayers )
{
    if ( !pClump ) return NULL;

    CClientPed * pPed = NULL;
    vector < CClientPed* > ::const_iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end (); iter++ )
    {
        pPed = *iter;
        if ( pPed->GetClump () == pClump && ( pPed->GetType () == CCLIENTPED || bCheckPlayers ) )
        {
            return pPed;
        }
    }
    return NULL;
}


CClientPed* CClientPedManager::GetSafe ( CEntity * pEntity, bool bCheckPlayers )
{
    if ( !pEntity ) return NULL;

    vector < CClientPed* > ::const_iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end (); iter++ )
    {
        if ( dynamic_cast < CEntity * > ( (*iter)->GetGamePlayer () ) == pEntity )
        {
            return *iter;
        }
    }
    return NULL;
}


bool CClientPedManager::Exists ( CClientPed* pPed )
{
    // Is it in our list?
    vector < CClientPed* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pPed )
            return true;
    }

    // Nope
    return false;
}


void CClientPedManager::RemoveFromList ( CClientPed* pPed )
{
    if ( m_bRemoveFromList )
    {
        ListRemove ( m_List, pPed );
    }
}


void CClientPedManager::OnCreation ( CClientPed * pPed )
{
    // Check not already in the list to avoid multiple calls to pPed->StreamedInPulse() later
    if ( !ListContains ( m_StreamedIn, pPed ) )
        m_StreamedIn.push_back ( pPed );
}


void CClientPedManager::OnDestruction ( CClientPed * pPed )
{
    ListRemove ( m_StreamedIn, pPed );
}

void CClientPedManager::RestreamPeds ( unsigned short usModel )
{
    // Store the affected vehicles
    CClientPed* pPed;
    std::vector < CClientPed* > ::const_iterator iter = IterBegin ();
    for ( ; iter != IterEnd (); iter++ )
    {
        pPed = *iter;

        // Streamed in and same vehicle ID?
        if ( pPed->IsStreamedIn () && pPed->GetModel () == usModel )
        {
            // Stream it out for a while until streamed decides to stream it
            // back in eventually
            pPed->StreamOutForABit ();
            // Hack fix for Players not unloading.
            if ( IS_PLAYER ( pPed ) )
            {
                // Awesome hack skills + 1, change him to another model while we unload for the lulz
                // Translation: My hack level has increased to ninety eight and we need to wait a frame before reloading the model ID in question so that the custom model unloads properly.
                // To do this we set him to CJ (Impossible to mod to my knowledge) and then set him back in CPed::StreamedInPulse
                pPed->SetModel ( 0, true );
            }
        }
    }
}
void CClientPedManager::RestreamWeapon ( unsigned short usModel )
{    
    eWeaponSlot eSlot = (eWeaponSlot) GetWeaponSlotFromModel ( usModel );
    // Store the affected vehicles
    CClientPed* pPed;
    std::vector < CClientPed* > ::const_iterator iter = IterBegin ();
    for ( ; iter != IterEnd (); iter++ )
    {
        pPed = *iter;

        // Streamed in and same vehicle ID?
        if ( pPed->IsStreamedIn () && 
            pPed->GetWeapon ( eSlot ) && 
            pPed->GetWeapon ( eSlot )->GetInfo ( WEAPONSKILL_STD )->GetModel () == usModel )
        {
            // Awesome hack skills + 1, change him to another model while we unload for the lulz
            // Translation: My hack level has increased to ninety nine and we need to wait a frame before reloading the model ID in question so that the custom model unloads properly.
            // To do this we take away the weapon and give it back in CPed::StreamedInPulse ergo reloading the model info
            pPed->StreamOutWeaponForABit ( eSlot );
        }
    }
}

unsigned short CClientPedManager::GetWeaponSlotFromModel ( DWORD dwModel )
{
    switch ( dwModel )
    {
        case 0:
        case 331:
        {
            return 0;
        }
        case 333:
        case 334:
        case 335:
        case 336:
        case 337:
        case 338:
        case 339:
        case 341:
        {
            return 1;
        }
        case 346:
        case 347:
        case 348:
        {
            return 2;
        }
        case 349:
        case 350:
        case 351:
        {
            return 3;
        }
        case 352:
        case 353:
        case 372:
        {
            return 4;
        }
        case 355:
        case 356:
        {
            return 5;
        }
        case 357:
        case 358:
        {
            return 6;
        }
        case 359:
        case 360:
        case 361:
        case 362:
        {
            return 7;
        }
        case 342:
        case 343:
        case 344:
        case 363:
        {
            return 8;
        }
        case 365:
        case 366:
        case 367:
        {
            return 9;
        }
        case 321:
        case 322:
        case 323:
        case 325:
        case 326:
        {
            return 10;
        }
        case 368:
        case 369:
        case 371:
        {
            return 11;
        }
        case 364:
        {
            return 12;
        }
    }

    return 0;
}

bool CClientPedManager::IsValidWeaponModel ( DWORD dwModel )
{
    switch ( dwModel )
    {
        case 0:
        case 331:
        case 333:
        case 334:
        case 335:
        case 336:
        case 337:
        case 338:
        case 339:
        case 341:
        case 346:
        case 347:
        case 348:
        case 349:
        case 350:
        case 351:
        case 352:
        case 353:
        case 372:
        case 355:
        case 356:
        case 357:
        case 358:
        case 359:
        case 360:
        case 361:
        case 362:
        case 342:
        case 343:
        case 344:
        case 363:
        case 365:
        case 366:
        case 367:
        case 321:
        case 322:
        case 323:
        case 325:
        case 326:
        case 368:
        case 369:
        case 371:
        case 364:
        {
            return true;
        }
    }

    return false;
}
