/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTransformationSA.cpp
*  PURPOSE:     Transformation matrix management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTransformationSAInterface *const pTransform = (CTransformationSAInterface*)CLASS_CTransformation;

/*
    GTA:SA Transformation Management

    This interface allows (CPlaceableSAInterface mainly) access to cached matrices. They are managed
    in a free-list, a used-list and a active-list. As long as there are free matrices available,
    the engine pops them from the free-list. Otherwise it tries to deallocate already used matrices from
    in-game objects (used-list). The active-list contains matrices which must not be deallocated from
    the objects.

    Since the free list can consist of infinite matrices, MTA may hook the allocation routines and
    request new matrices if required. This would solve matrix corruption issues if too many entities
    are rendering.

    The more matrices we allocate in the beginning, the flatter the performance will be. Users with low
    amount of matrices allocated may experience slight slow down due to allocation of new matrices. The
    performance will always tune to the best performance and keep a maximum of matrices which has ever
    been used in the engine (we cannot waste memory so to speak).
*/

void Transformation_Init( void )
{
    // Block the original transformation init
    MemPut( (unsigned char*)FUNC_InitTransformation, 0xC3 );

    // Allocate enough matrices for everybody
    new (pTransform) CTransformationSAInterface( 20000 );
}

void Transformation_Shutdown( void )
{
    pTransform->~CTransformationSAInterface();
}

/*=========================================================
    CTransformSAInterface::constructor

    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F0C0
=========================================================*/
CTransformSAInterface::CTransformSAInterface( void )
{
    m_unk = NULL;
    m_unk2 = NULL;
    m_entity = NULL;
}

/*=========================================================
    CTransformSAInterface::destructor

    Binary offsets:
        (1.0 US and 1.0 EU): 0x0059ACD0
=========================================================*/
CTransformSAInterface::~CTransformSAInterface( void )
{
    LIST_REMOVE( *this );
}

/*=========================================================
    CTransformationSAInterface::constructor

    Arguments:
        max - number of matrices to initially allocate
    Purpose:
        Sets up the matrix cache system. The initial
        matrices are allocated on a big stack. We may allocate
        additional matrices during runtime.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054F0D0
=========================================================*/
CTransformationSAInterface::CTransformationSAInterface( unsigned int max )
{
    CAllocatedTransformSAInterface *trans = (CAllocatedTransformSAInterface*)malloc( max * sizeof(CTransformSAInterface) + sizeof(CAllocatedTransformSAInterface) );
    CTransformSAInterface *mat;

    // Store the count
    trans->m_count = max;

    m_stack = mat = trans->Get( 0 );

    // Init the lists
    LIST_CLEAR( m_usedList );
    LIST_CLEAR( m_freeList );
    LIST_CLEAR( m_activeList );

    LIST_APPEND( m_usedList, m_usedItem );
    LIST_APPEND( m_freeList, m_freeItem );
    LIST_APPEND( m_activeList, m_activeItem );

    while ( max-- )
    {
        new (mat) CTransformSAInterface();

        LIST_APPEND( m_freeList, *mat );
        mat++;
    }
}

/*=========================================================
    CTransformationSAInterface::destructor

    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054EB70
=========================================================*/
CTransformationSAInterface::~CTransformationSAInterface()
{
}

/*=========================================================
    CTransformationSAInterface::AllocateDynamic

    Purpose:
        Attempts to retrieve a matrix from the free-list and
        returns it. If there is no more free matrix available,
        it returns NULL.
        The resulting matrix is pushed onto m_usedList. These
        matrices are dynamically free'd from the entities if
        there is a matrix shortage in the system, hence dynamic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054E9D0
=========================================================*/
CTransformSAInterface* CTransformationSAInterface::AllocateDynamic( void )
{
    CTransformSAInterface *matrix = m_freeList.prev;

    if ( matrix == &m_freeItem )
        return NULL;

    LIST_REMOVE( *matrix );
    LIST_APPEND( m_usedList, *matrix );
    return matrix;
}

/*=========================================================
    CTransformationSAInterface::AllocateStatic

    Purpose:
        Attempts to retrieve a matrix from the free-list and
        returns it. If there is no more free matrix available,
        it returns NULL.
        The resulting matrix is pushed onto m_activeList. Those
        matrices are never deallocated from the entities unless
        the entity itself does so, hence static allocation.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0054EA10
=========================================================*/
CTransformSAInterface* CTransformationSAInterface::AllocateStatic( void )
{
    CTransformSAInterface *matrix = m_freeList.prev;

    if ( matrix == &m_freeItem )
        return NULL;

    LIST_REMOVE( *matrix );
    LIST_APPEND( m_activeList, *matrix );
    return matrix;
}

/*=========================================================
    CTransformationSAInterface::IsFreeMatrixAvailable

    Purpose:
        Returns whether a free matrix can be allocated.
=========================================================*/
bool CTransformationSAInterface::IsFreeMatrixAvailable( void )
{
    return m_freeList.prev != &m_freeItem;
}

/*=========================================================
    CTransformationSAInterface::FreeUnimportantMatrix

    Purpose:
        Attempts to free a matrix from the entity which allocated
        it dynamically. The entity which allocated it first in-time
        will lose it first. The more matrices are in the system,
        the less often entities lose their matrix.
    Note:
        This function has been inlined into the CPlaceableSAInterface
        matrix allocation functions.
=========================================================*/
bool CTransformationSAInterface::FreeUnimportantMatrix( void )
{
    // Crashfix
    if ( m_usedList.prev == &m_usedItem )
        return false;

    // Steal unimportant matrices :3
    m_usedList.next->m_entity->FreeMatrix();
    return true;
}

/*=========================================================
    CTransformationSAInterface::NewMatrix (MTA extension)

    Purpose:
        Allocates a new transformation matrix and puts it into
        the free-list. This function prevents a crash due to no
        more available matrices for the engine which may happen
        if there is heavy building activity.
=========================================================*/
void CTransformationSAInterface::NewMatrix( void )
{
    CTransformSAInterface *item = new CTransformSAInterface;

    LIST_APPEND( m_freeList, *item );
}