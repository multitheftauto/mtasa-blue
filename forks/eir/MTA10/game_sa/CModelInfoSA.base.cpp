/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.base.cpp
*  PURPOSE:     CBaseModelInfoSA routine definitions
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CBaseModelInfoSAInterface **ppModelInfo;

using namespace ModelInfo;


CBaseModelInfoSAInterface::CBaseModelInfoSAInterface( void )
{
}

CBaseModelInfoSAInterface::~CBaseModelInfoSAInterface( void )
{
}

/*=========================================================
    CBaseModelInfoSAInterface::GetAtomicModelInfo

    Purpose:
        Returns this interface if it inherits CAtomicModelInfoSA.
        Returns NULL by default.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4A80
=========================================================*/
CAtomicModelInfoSA* CBaseModelInfoSAInterface::GetAtomicModelInfo( void )
{
    return NULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::GetDamageAtomicModelInfo

    Purpose:
        Returns this interface if it inherits CDamageAtomicModelInfoSA.
        Returns NULL by default.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4A90
=========================================================*/
CDamageAtomicModelInfoSA* CBaseModelInfoSAInterface::GetDamageAtomicModelInfo( void )
{
    return NULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::GetLODAtomicModelInfo

    Purpose:
        Returns this interface if it inherits CLODAtomicModelInfoSA.
        Returns NULL by default.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4AA0
=========================================================*/
CLODAtomicModelInfoSA* CBaseModelInfoSAInterface::GetLODAtomicModelInfo( void )
{
    return NULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::Init

    Purpose:
        Initializes this model info with default values for
        first usage.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4B10
=========================================================*/
void CBaseModelInfoSAInterface::Init( void )
{
    usNumberOfRefs = 0;
    usTextureDictionary = -1;
    pColModel = NULL;
    usEffectID = -1;
    ucNumOf2DEffects = 0;
    usDynamicIndex = -1;
    fLodDistanceUnscaled = 2000;
    pRwObject = NULL;

    renderFlags = RENDER_COLMODEL | RENDER_BACKFACECULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::Shutdown

    Purpose:
        De-initializes a previously active model info. All
        resources attached to it are either destroyed or
        dereferenced.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4D50
=========================================================*/
void CBaseModelInfoSAInterface::Shutdown( void )
{
    DeleteRwObject();

    DeleteCollision();

    // We go with a dynamic colmodel by default.
    renderFlags |= RENDER_COLMODEL;

    usEffectID = -1;
    ucNumOf2DEffects = 0;

    usDynamicIndex = -1;
    usTextureDictionary = -1;
}

/*=========================================================
    CBaseModelInfoSAInterface::GetTimeInfo

    Purpose:
        Unknown yet. Needs investigation.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4AB0
=========================================================*/
timeInfo* CBaseModelInfoSAInterface::GetTimeInfo( void )
{
    return NULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::SetCollision

    Arguments:
        col - new collision interface for this model info
        dynamic - if true, the runtime is allowed to delete the collision
                  it is set for collision which is found in COL libraries
    Purpose:
        Applies another collision interface to this model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4BC0
=========================================================*/
void CBaseModelInfoSAInterface::SetCollision( CColModelSAInterface *col, bool dynamic )
{
    pColModel = col;

    if ( dynamic )
    {
        renderFlags |= RENDER_COLMODEL;

        timeInfo *timed = GetTimeInfo();

        if ( timed && timed->m_model != -1 )
            ppModelInfo[timed->m_model]->SetCollision( col, false );
    }
    else
        renderFlags &= ~RENDER_COLMODEL;
}

/*=========================================================
    CBaseModelInfoSAInterface::DeleteCollision

    Purpose:
        Unsets the current collision interface. It deletes the
        interface if it is set dynamic (COL library member).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4C40
=========================================================*/
void CBaseModelInfoSAInterface::DeleteCollision( void )
{
    if ( pColModel && IsDynamicCol() )
        delete pColModel;

    pColModel = NULL;
}

/*=========================================================
    CBaseModelInfoSAInterface::DereferenceTXD

    Purpose:
        Dereferences the acquired TXD block.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4B90
=========================================================*/
void CBaseModelInfoSAInterface::DereferenceTXD( void )
{
    if ( usTextureDictionary == -1 )
        return;

    TextureManager::GetTxdPool()->Get( usTextureDictionary )->Dereference();
}

/*=========================================================
    CBaseModelInfoSAInterface::Reference

    Purpose:
        References the resources of this model info. This does not
        mean that the model itself would stay loaded.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4BA0
=========================================================*/
void CBaseModelInfoSAInterface::Reference( void )
{
    usNumberOfRefs++;

    TextureManager::GetTxdPool()->Get( usTextureDictionary )->Reference();
}

/*=========================================================
    CBaseModelInfoSAInterface::Dereference

    Purpose:
        Removes a reference from this model info and it's resources.
        It does not unload the model itself.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4BB0
=========================================================*/
void CBaseModelInfoSAInterface::Dereference( void )
{
    usNumberOfRefs--;

    TextureManager::GetTxdPool()->Get( usTextureDictionary )->Dereference();
}

/*=========================================================
    CBaseModelInfoSAInterface::GetFlags

    Purpose:
       Returns the common flag values of this model info. These
       may be different for every deriviatation (clump, atomic, etc).
=========================================================*/
unsigned short CBaseModelInfoSAInterface::GetFlags( void )
{
    return flags;
}

/*=========================================================
    CBaseModelInfoSAInterface::SetColModel

    Arguments:
        col - new collision interface for this model info
        dynamic - if true, the runtime is allowed to delete the collision
                  it is set for collision which is found in COL libraries
    Purpose:
        Hook for SetCollision.
=========================================================*/
void CBaseModelInfoSAInterface::SetColModel( CColModelSAInterface *col, bool dynamic )
{
    SetCollision( col, dynamic );
}

/*=========================================================
    CBaseModelInfoSAInterface::UnsetColModel

    Purpose:
        Unsets the current collision interface. It deletes the
        interface if it is set dynamic (COL library member).
    Purpose:
        Hook for DeleteCollision.
=========================================================*/
void CBaseModelInfoSAInterface::UnsetColModel( void )
{
    DeleteCollision();
}

/*=========================================================
    CBaseModelInfoSAInterface::ValidateResource (MTA extension)

    Arguments:
        modelIndex - the model index associated the this base model info
        rwobj - resource that should be checked compatibility for
    Purpose:
        Returns a boolean that tells you whether the given resource
        is compatible with this model info structure.
=========================================================*/
bool CBaseModelInfoSAInterface::ValidateResource( modelId_t modelIndex, RwObject *rwobj )
{
    // Check compatibility based on model info type.
    eRwType modelInfoType = GetRwModelType();

    if ( modelInfoType == RW_ATOMIC )
    {
        // Make sure the object is an atomic as well.
        if ( rwobj->type != RW_ATOMIC )
            return false;
    }
    else if ( modelInfoType == RW_CLUMP )
    {
        // Make sure the object is a clump as well.
        if ( rwobj->type != RW_CLUMP )
            return false;

        RpClump *theClump = (RpClump*)rwobj;

        // Special checks for different model derivations.
        eModelType infoType = GetModelType();

        if ( infoType == MODEL_PED )
        {
            if ( modelIndex == 0 )
            {
                // Make sure the clump has an animation structure.
                RpAnimHierarchy *anim = theClump->GetAtomicAnimHierarchy();

                if ( !anim )
                    return false;
            }
        }
        else if ( infoType == MODEL_VEHICLE )
        {
            // todo.
        }
        else
        {
            // todo.
        }
    }
    else
        return false;

    return true;
}

void ModelInfoBase_Init( void )
{
    // Install some fixes/investigation
    HookInstall( 0x004C4BC0, h_memFunc( &CBaseModelInfoSAInterface::SetColModel ), 5 );
    HookInstall( 0x004C4C40, h_memFunc( &CBaseModelInfoSAInterface::UnsetColModel ), 5 );
}

void ModelInfoBase_Shutdown( void )
{
}