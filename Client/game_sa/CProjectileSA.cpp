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

void CProjectileSA::Destroy( bool bBlow ) 
{
    if ( m_bDestroyed == false )
    {
        pGame->GetProjectileInfo ()->RemoveProjectile ( projectileInfo, this, bBlow );
        m_bDestroyed = true;
    }
}

// Corrects errors in the physics engine that cause projectiles to be far away from the objects they attached to
// issue #8122
bool CProjectileSA::CorrectPhysics ( void )
{
    // make sure we have an interface for our bomb/satchel
    CPhysicalSAInterface * pInterface = static_cast < CPhysicalSAInterface * > ( m_pInterface );
    // make sure we have an interface

    if ( pInterface != NULL )
    {
        // make sure we have an attached entity
        if ( pInterface->m_pAttachedEntity )
        {
            // get our position
            CVector vecStart = *GetPosition ( );
            // get the entity we collided with
            CEntitySAInterface * pCollidedWithInterface = pInterface->m_pAttachedEntity;
            // get our end position by projecting forward a few velocities more
            CVector vecEnd = vecStart - ( pInterface->m_vecCollisionImpactVelocity * 3 );

            // process forward another 1 unit
            if ( pGame->GetWorld ( )->CalculateImpactPosition ( vecStart, vecEnd )  )
            {
                // setup some variables
                CVector vecRotation;
                CVector vecTemp;
                CVector vecCollisionOffset;
                // get our current offset ( we want the rotation! )
                GetAttachedOffsets ( vecTemp, vecRotation );

                // create a matrix variable
                CMatrix attachedToMatrix;
                if ( pCollidedWithInterface->Placeable.matrix != NULL )
                {
                    // get our matrix
                    pCollidedWithInterface->Placeable.matrix->ConvertToMatrix ( attachedToMatrix );
                }
                else
                {
                    // get our matrix
                    attachedToMatrix = CMatrix ( pCollidedWithInterface->Placeable.m_transform.m_translate );
                }
                
                // transform our matrix into local (attached) space
                CVector vecPosition = attachedToMatrix.Inverse ().TransformVector ( vecEnd );
                
                // offset by enough that it isn't sticking inside anything
                vecPosition += attachedToMatrix.Inverse () * ( pInterface->m_vecCollisionImpactVelocity * CVector ( 0.2f, 0.2f, 0.3f ) );
                
                // set our attached offsets
                SetAttachedOffsets ( vecPosition, vecRotation );
            }
            return true;
        }
    }
    return false;
}