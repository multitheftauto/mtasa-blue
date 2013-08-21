/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileSA.cpp
*  PURPOSE:     Projectile entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CProjectileSA::CProjectileSA (CProjectileSAInterface* projectileInterface) : CObjectSA(projectileInterface)
{
    internalInterface = projectileInterface;
    projectileInfo = NULL;
    this->BeingDeleted = false;
    this->DoNotRemoveFromGame = false;  
    this->internalInterface->bStreamingDontDelete = true;
    this->internalInterface->bDontStream = true;
    this->internalInterface->bRemoveFromWorld = false;
    m_bDestroyed = false;
}

CProjectileSA::~CProjectileSA( )
{
    DEBUG_TRACE("CProjectileSA::~CProjectileSA( )");

    this->BeingDeleted = true;
    /*
    //OutputDebugString("Attempting to destroy Object\n");
    if(!this->BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD)this->GetInterface();
        
        CWorldSA * world = (CWorldSA *)pGame->GetWorld();
        world->Remove(this->GetInterface());
    
        DWORD dwThis = (DWORD)this->GetInterface();
        DWORD dwFunc = this->GetInterface()->vtbl->Remove;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }

        dwFunc = this->GetInterface()->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be type independent
        _asm    
        {
            mov     ecx, dwThis
            push    1           //delete too
            call    dwFunc
        }       

        this->BeingDeleted = true;
        //((CPoolsSA *)pGame->GetPools())->RemoveObject((CObject *)(CObjectSA *)this);
        //this->BeingDeleted = false;
        //delete this;
        //OutputDebugString("Destroying Object\n");
    }*/
    Destroy();
}

void CProjectileSA::Destroy() 
{
    if ( m_bDestroyed == false )
    {
        pGame->GetProjectileInfo ()->RemoveProjectile ( projectileInfo, this );
        m_bDestroyed = true;
    }
}
