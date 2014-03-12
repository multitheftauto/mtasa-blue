/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEffectManager.cpp
*  PURPOSE:     Effect manager class
*
*****************************************************************************/

#include "StdInc.h"

CClientEffectManager::CClientEffectManager ( class CClientManager* pManager )
:
    m_pManager(pManager),
    m_bCanRemoveFromList(true)
{
}

CClientEffectManager::~CClientEffectManager ( )
{
    DeleteAll ();
}

void CClientEffectManager::DeleteAll( )
{
    m_bCanRemoveFromList = false;
    for ( std::list<CClientEffect*>::iterator i = m_Effects.begin(); i != m_Effects.end(); ++i)
    {
        delete *i;
    }
    m_Effects.clear();
    m_bCanRemoveFromList = true;
}

CClientEffect * CClientEffectManager::Create(const SString& strEffectName, const CVector &vecPosition, ElementID ID)
{
    CFxSystem * pFxSA = g_pGame->GetFxManager()->CreateFxSystem(strEffectName, vecPosition, NULL, 0);
    if ( pFxSA == NULL )
        return NULL; // GTA was unable to create the effect (e.g. wrong effect name)

    CClientEffect * pFx = new CClientEffect(m_pManager, pFxSA, ID);
    m_Effects.push_back( pFx );

    return pFx;
}


void CClientEffectManager::RemoveFromList ( CClientEffect* pEffect )
{
    if ( m_bCanRemoveFromList )
    {
        m_Effects.remove ( pEffect );
    }
}


CClientEffect* CClientEffectManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTEFFECT )
    {
        return static_cast < CClientEffect* > ( pEntity );
    }

    return NULL;
}

CClientEffect* CClientEffectManager::Get( void* pFxSAInterface )
{
    CClientEffect *pFx = NULL;
    for ( std::list<CClientEffect*>::iterator i = m_Effects.begin(); i != m_Effects.end(); ++i)
    {
        if( (*i)->GetFxSystem()->GetInterface() == pFxSAInterface )
        {
            return *i;
        }
    }
    
    return NULL;
}

void CClientEffectManager::SAEffectDestroyed ( void *pFxSAInterface )
{
    CClientEffect * pFx = Get(pFxSAInterface);
    if(pFx == NULL)
        return; // We didn't create that effect

    pFx->SetFxSystem(NULL);
    g_pClientGame->GetElementDeleter()->Delete(pFx);
}