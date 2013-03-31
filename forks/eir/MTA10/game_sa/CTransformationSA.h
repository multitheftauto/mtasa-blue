/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTransformationSA.h
*  PURPOSE:     Transformation matrix management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTransformationSA_
#define _CTransformationSA_

#define CLASS_CTransformation       0x00B74288
#define FUNC_InitTransformation     0x0054F3A0

class CMatrixEx : public RwMatrix
{
public:
    void operator =( const RwMatrix& mat )
    {
        assign( mat );
    }

    void*                                   m_unk;          // 64
    RwMatrix*                               m_unk2;         // 68, confirmed to be a dynamically allocated RwMatrix*
};

class CTransformSAInterface : public CMatrixEx
{
public:
                            CTransformSAInterface( void );
                            ~CTransformSAInterface( void );

    void operator =( const RwMatrix& mat )
    {
        assign( mat );
    }

    class CPlaceableSAInterface*            m_entity;       // 72

    CTransformSAInterface*                  next;           // 76
    CTransformSAInterface*                  prev;           // 80
};

class CAllocatedTransformSAInterface
{
public:
    inline CTransformSAInterface*           Get( unsigned int index )
    {
        if ( index >= m_count )
            return NULL;

        return (CTransformSAInterface*)(this + 1) + index;
    }

    unsigned int                            m_count;
};

class CTransformationSAInterface
{
public:
                                            CTransformationSAInterface      ( unsigned int max );
                                            ~CTransformationSAInterface     ( void );

    CTransformSAInterface*                  AllocateDynamic                 ( void );
    CTransformSAInterface*                  AllocateStatic                  ( void );
    bool                                    IsFreeMatrixAvailable           ( void );
    bool                                    FreeUnimportantMatrix           ( void );
    void                                    NewMatrix                       ( void );

    CTransformSAInterface                   m_usedList;         // 0, list of dynamic matrices (may be free'd from the entity)
    CTransformSAInterface                   m_usedItem;         // 84
    CTransformSAInterface                   m_activeList;       // 168, list of static matrices (must not be free'd from the entity)
    CTransformSAInterface                   m_activeItem;       // 252
    CTransformSAInterface                   m_freeList;         // 336, list of allocatable matrices
    CTransformSAInterface                   m_freeItem;         // 420
    CTransformSAInterface*                  m_stack;            // 504, initial matrix allocation stack
};

void Transformation_Init( void );
void Transformation_Shutdown( void );

extern CTransformationSAInterface *const pTransform;

#endif //_CTransformationSA_