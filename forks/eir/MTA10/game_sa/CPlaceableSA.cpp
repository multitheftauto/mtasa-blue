/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlaceableSA.cpp
*  PURPOSE:     Transformation management interface
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CGameSA *pGame;
extern CBaseModelInfoSAInterface **ppModelInfo;

/*=========================================================
    CPlaceableSAInterface::constructor

    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F1E0
=========================================================*/
CPlaceableSAInterface::CPlaceableSAInterface( void )
{
    Placeable.m_transform.m_heading = 0;
    Placeable.matrix = NULL;
}

/*=========================================================
    CPlaceableSAInterface::destructor

    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F490
=========================================================*/
CPlaceableSAInterface::~CPlaceableSAInterface( void )
{
    FreeMatrix();

    // I do not know what this is. Entity count?
    (*(unsigned int*)0x00B74238)--;

    //m_matrix = &transformIdentity;
}

/*=========================================================
    CPlaceableSAInterface::AllocateMatrix

    Purpose:
        Allocates a static matrix to this entity. Static matrices
        are not automatically deallocated when there is a matrix
        shortage. If the entity already had a transform matrix,
        the engine ensures that it is a static matrix.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F4C0
=========================================================*/
void CPlaceableSAInterface::AllocateMatrix( void )
{
    if ( Placeable.matrix )
    {
        // We already have a matrix, make sure its in the active list
        LIST_REMOVE( *Placeable.matrix );
        LIST_APPEND( pTransform->m_activeList, *Placeable.matrix );
        return;
    }

    // Extend the matrix list
    if ( !pTransform->IsFreeMatrixAvailable() && !pTransform->FreeUnimportantMatrix() )
        pTransform->NewMatrix();

    // Allocate it
    Placeable.matrix = pTransform->AllocateStatic();
    Placeable.matrix->m_entity = this;
}

/*=========================================================
    CPlaceableSAInterface::AcquaintMatrix

    Purpose:
        Allocates a dynamic matrix to this entity. Dynamic
        matrices are deallocated from entities if there is a
        matrix shortage in the system. If the entity already
        has a matrix, this function does nothing.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F560
=========================================================*/
void CPlaceableSAInterface::AcquaintMatrix( void )
{
    if ( Placeable.matrix )
        return;

    // Extend the matrix list
    if ( !pTransform->IsFreeMatrixAvailable() && !pTransform->FreeUnimportantMatrix() )
        pTransform->NewMatrix();

    // Allocate it
    Placeable.matrix = pTransform->AllocateDynamic();
    Placeable.matrix->m_entity = this;
}

/*=========================================================
    CPlaceableSAInterface::FreeMatrix

    Purpose:
        Stores the matrix as z-rotation heading and position
        members while adding it to the free-list of the
        transformation matrix cache.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F3B0
=========================================================*/
void CPlaceableSAInterface::FreeMatrix( void )
{
    CTransformSAInterface *trans = Placeable.matrix;

    // Crashfix: do nothing if we have no transformation.
    if ( !trans )
        return;

    // Transform the entity
    Placeable.m_transform.m_translate = trans->vPos;
    Placeable.m_transform.m_heading = trans->ToHeading();

    // Free the matrix
    Placeable.matrix = NULL;
    trans->m_entity = NULL;

    LIST_REMOVE( *trans );
    LIST_APPEND( pTransform->m_freeList, *trans );
}

/*=========================================================
    CPlaceableSAInterface::Transform::GetOffsetByHeading

    Arguments:
        out - transformation output vector (offset by in vector)
        in - vector describing the offset based on the entity's position
    Purpose:
        Performs a rotation matrix transformation using
        the heading and translation members. This function
        is called if there is no matrix available.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054ECE0
=========================================================*/
void CPlaceableSAInterface::Transform::GetOffsetByHeading( CVector& out, const CVector& in ) const
{
    float ch = cos( m_transform.m_heading );
    float sh = sin( m_transform.m_heading );

    out[0] = ch * in[0] - sh * in[1] + m_transform.m_translate[0];
    out[1] = sh * in[0] + ch * in[1] + m_transform.m_translate[1];
    out[2] = in[2] + m_transform.m_translate[2];
}

/*=========================================================
    CPlaceableSAInterface::GetOffset

    Arguments:
        out - transformation output vector (offset by in vector)
        in - vector describing the offset based on the entity's position
    Purpose:
        Performs a rotation matrix transformation. If the matrix
        is available, it uses it. Otherwise if performs the transformation
        using the heading and translation members.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005334F0
=========================================================*/
void CPlaceableSAInterface::GetOffset( CVector& out, const CVector& in ) const
{
    if ( Placeable.matrix )
        Placeable.matrix->Transform( in, out );
    else
        Placeable.GetOffsetByHeading( out, in );
}

/*=========================================================
    CPlaceableSAInterface::Transform::GetMatrixFromHeading

    Arguments:
        mat - output matrix for this entity
    Purpose:
        Constructs a matrix using the heading and translation
        members. It describes a z-rotation only.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054EF40
=========================================================*/
void CPlaceableSAInterface::Transform::GetMatrixFromHeading( RwMatrix& mat ) const
{
    mat.FromHeading( m_transform.m_heading );
    mat.vPos = m_transform.m_translate;
}

void Placeable_Init( void )
{
    // We should handle the matrix allocation ourselves
    HookInstall( 0x0054F560, h_memFunc( &CPlaceableSAInterface::AcquaintMatrix ), 5 );
    HookInstall( 0x0054F4C0, h_memFunc( &CPlaceableSAInterface::AllocateMatrix ), 5 );
    HookInstall( 0x0054F3B0, h_memFunc( &CPlaceableSAInterface::FreeMatrix ), 5 );

    // Transformation hooks
    HookInstall( 0x0054F1B0, h_memFunc( &CPlaceableSAInterface::Transform::GetMatrixFromHeading ), 5 );
}

void Placeable_Shutdown( void )
{
}