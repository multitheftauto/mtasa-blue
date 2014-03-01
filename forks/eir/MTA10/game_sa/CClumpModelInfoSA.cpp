/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CClumpModelInfoSA.cpp
*  PURPOSE:     Clump model instance
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

/*=========================================================
    CClumpModelInfoSAInterface::Init

    Purpose:
        Initializes this interface for first usage.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4E40
=========================================================*/
void CClumpModelInfoSAInterface::Init( void )
{
    m_animBlock = -1;

    CBaseModelInfoSAInterface::Init();
}

/*=========================================================
    CClumpModelInfoSAInterface::DeleteRwObject

    Purpose:
        Deletes the associated clump RenderWare object from this
        model info. It derefences or destroys all it's resources,
        too.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4E70
=========================================================*/
void CClumpModelInfoSAInterface::DeleteRwObject( void )
{
    RpAtomic *atomic;
    RpClump *infoClump = GetRwObject();

    if ( !infoClump )
        return;

    atomic = infoClump->Find2dfx();

    if ( atomic )
    {
        // Update the 2dfx count
        Rw2dfx *eff = atomic->geometry->_2dfx;

        ucNumOf2DEffects = eff ? eff->count : 0;
    }

    RpClumpDestroy( infoClump );

    pRwObject = NULL;

    DereferenceTXD();

    if ( GetAnimFileIndex() != -1 )
        pGame->GetAnimManager()->RemoveAnimBlockRef( GetAnimFileIndex() );

    if ( flags & 0x0800 )
        DeleteCollision();
}

/*=========================================================
    CClumpModelInfoSAInterface::GetRwModelType

    Purpose:
        Returns the RenderWare object type associated with this
        model info (-> RW_CLUMP).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5730
=========================================================*/
eRwType CClumpModelInfoSAInterface::GetRwModelType( void )
{
    return RW_CLUMP;
}

/*=========================================================
    CClumpModelInfoSAInterface::CreateRwObjectEx

    Arguments:
        mat - modelling matrix to set to the object's parent frame
    Purpose:
        Creates a new model object. If the model info is not loaded,
        NULL is returned. The modelling matrix of the object's frame
        is set to mat. Then the new object is returned.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5110
=========================================================*/
RwObject* CClumpModelInfoSAInterface::CreateRwObjectEx( RwMatrix& mat )
{
    if ( !pRwObject )
        return NULL;

    RwObject *obj = CreateRwObject();
    
    obj->parent->modelling = mat;
    return obj;
}

/*=========================================================
    CClumpModelInfoSAInterface::CreateRwObject

    Purpose:
        Creates a new model object. If the model info is not loaded,
        NULL is returned. Before the object is returned, it
        is initialized.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5140
=========================================================*/
bool RpSetAtomicAnimHierarchy( RpAtomic *child, RpAnimHierarchy *anim )
{
    child->anim = anim;
    return false;
}

RwObject* CClumpModelInfoSAInterface::CreateRwObject( void )
{
    RpClump *infoClump = GetRwObject();

    if ( !infoClump )
        return NULL;

    Reference();

    RpClump *clump = RpClumpClone( infoClump );
    RpAtomic *atomic = clump->GetFirstAtomic();

    if ( atomic && !( renderFlags & RENDER_NOSKELETON ) )
    {
        if ( atomic->geometry->skeleton )
        {
            RpAnimHierarchy *anim = clump->GetAnimHierarchy();

            clump->ForAllAtomics( RpSetAtomicAnimHierarchy, anim );

#ifdef todo
            // Set up the animation
            RwAnimationInit( anim->anim, pGame->GetAnimManager()->CreateAnimation( anim ) );
#endif

            anim->flags = 0x3000;
        }
    }

    if ( renderFlags & RENDER_STATIC )
    {
        // Cache the animation and skeleton
        clump->InitStaticSkeleton();

#ifdef todo
        // Set idle animation
        if ( CAnimBlendHierarchySAInterface *anim = pGame->GetAnimManager()->GetAnimBlock( m_animBlock )->GetAnimation( GetHashKey() ) )
            pGame->GetAnimManager()->BlendAnimation( clump, anim, 2, 1.0 );
#endif
    }

    Dereference();
    return clump;
}

/*=========================================================
    CClumpModelInfoSAInterface::SetAnimFile

    Arguments:
        name - name of the anim file
    Purpose:
        Temporarily stores the name of the animation file.
        Yes, Rockstar did overwrite an integer field with
        a pointer.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5200
=========================================================*/
void CClumpModelInfoSAInterface::SetAnimFile( const char *name )
{
    char *anim;

    if ( strcmp(name, "null") == 0 )
        return;

    anim = (char*)malloc( strlen( name ) + 1 );

    strcpy( anim, name );

    // this is one nasty hack
    m_animBlock = (int)anim;
}

/*=========================================================
    CClumpModelInfoSAInterface::ConvertAnimFileIndex

    Purpose:
        Reads the temporary name hack storage and writes the
        actual animBlock index into it.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5250
=========================================================*/
void CClumpModelInfoSAInterface::ConvertAnimFileIndex( void )
{
    int animBlock;

    if ( m_animBlock == -1 )
        return;

    animBlock = pGame->GetAnimManager()->GetAnimBlockIndex( (const char*)m_animBlock );

    free( (void*)m_animBlock );

    // Yeah, weird
    m_animBlock = animBlock;
}

/*=========================================================
    CClumpModelInfoSAInterface::GetAnimFileIndex

    Purpose:
        Returns the index of the animation block this model
        info uses.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5740
=========================================================*/
int CClumpModelInfoSAInterface::GetAnimFileIndex( void )
{
    return m_animBlock;
}

/*=========================================================
    CClumpModelInfoSAInterface::GetCollision

    Purpose:
        Returns the collision interface for this model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5710
=========================================================*/
CColModelSAInterface* CClumpModelInfoSAInterface::GetCollision( void )
{
    return pColModel;
}

/*=========================================================
    CClumpModelInfoSAInterface::SetClump

    Arguments:
        clump - new clump to set to this model info
    Purpose:
        Sets a new clump RenderWare object to this model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4F70
=========================================================*/
bool RwAtomicSetupAnimHierarchy( RpAtomic *child, RpAnimHierarchy *anim )
{
    if ( anim )
    {
        child->anim = anim;
        return false;
    }

    child->anim = child->parent->GetAnimHierarchy();
    return true;
}

void CClumpModelInfoSAInterface::SetClump( RpClump *clump )
{
    RpClump *infoClump = GetRwObject();

    if ( infoClump )
    {
        RpAtomic *effAtomic = GetRwObject()->Find2dfx();

        // Decrease effect count
        if ( effAtomic )
        {
            Rw2dfx *effect = effAtomic->geometry->_2dfx;

            ucNumOf2DEffects -= effect ? effect->count : 0;
        }
    }

    pRwObject = clump;

    if ( clump )
    {
        RpAtomic *effAtomic = clump->Find2dfx();

        // Increase it with the new clump
        if ( effAtomic )
        {
            Rw2dfx *effect = effAtomic->geometry->_2dfx;

            ucNumOf2DEffects += effect ? effect->count : 0;
        }
    }

    // Set some callbacks
    RpClumpSetupFrameCallback( clump, (int)this );

    (*ppTxdPool)->Get( usTextureDictionary )->Reference();

    int anim = GetAnimFileIndex();

    if ( anim != -1 )
        pGame->GetAnimManager()->AddAnimBlockRef( anim );

    clump->SetupAtomicRender();

    RpAtomic *atomic = clump->GetFirstAtomic();

    if ( !atomic )
        return;

    RpSkeleton *skel = atomic->geometry->skeleton;

    if ( !skel )
        return;

    if ( renderFlags & RENDER_NOSKELETON )
    {
        clump->ForAllAtomics <RpAnimHierarchy*> ( RwAtomicSetupAnimHierarchy, NULL );
        return;
    }

    atomic->geometry->meshes[0].bounds.radius *= 1.2f;

    // Get the animation
    RpAnimHierarchy *hier = clump->GetAnimHierarchy();

    clump->ForAllAtomics( RwAtomicSetupAnimHierarchy, hier );

    for ( unsigned int n = 0; n < atomic->geometry->verticeSize; n++ )
    {
        RwV4d& info = skel->vertexInfo[n];
        float sum = info[0] + info[1] + info[2] + info[3];

        //sum /= 1.0;

        info[0] *= sum;
        info[1] *= sum;
        info[2] *= sum;
        info[3] *= sum;
    }

    // Set flag
    hier->flags = 0x3000;
}

/*=========================================================
    CClumpModelInfoSAInterface::AssignAtomics

    Arguments:
        comps - component construction information array
    Purpose:
        Assigns the component hierarchy information from a
        component registry.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5460
=========================================================*/
void CClumpModelInfoSAInterface::AssignAtomics( CComponentHierarchySAInterface *comps )
{
    RwFrame *frame = GetRwObject()->parent;

    for ( ; comps->m_name; comps++ )
    {
        if ( !( comps->m_flags & ATOMIC_HIER_ACTIVE ) )
            continue;

        RwFrame *component = frame->FindFreeChildByName( comps->m_name );

        if ( !component )
            continue;

        component->hierarchyId = comps->m_frameHierarchy;
    }
}