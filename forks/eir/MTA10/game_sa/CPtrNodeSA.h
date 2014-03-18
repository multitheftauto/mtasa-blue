/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPtrNodeSA.h
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CPtrNodeSA_
#define _CPtrNodeSA_

template <typename pointerType>
class CPtrNodeSingleSA
{
public:
    void* CPtrNodeSingleSA::operator new( size_t )
    {
        return (*ppPtrNodeSinglePool)->Allocate();
    }

    void CPtrNodeSingleSA::operator delete( void *ptr )
    {
        (*ppPtrNodeSinglePool)->Free( (CPtrNodeSingleSA <void>*)ptr );
    }

    CPtrNodeSingleSA( pointerType *val ) : data( val )
    { }

    pointerType*            data;
    CPtrNodeSingleSA*       m_next;
};

typedef CPtrNodeSingleSA <void> CPtrNodeSingleSAInterface;

// Container used to store single linked lists in.
template <typename pointerType>
struct ptrNodeSingleContainer
{
    typedef CPtrNodeSingleSA <pointerType> ptrNode_t;

    ptrNodeSingleContainer( void )
    {
        m_list = NULL;
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00552400
    ~ptrNodeSingleContainer( void )
    {
        ptrNode_t *iter = m_list;

        while ( iter )
        {
            ptrNode_t *next = iter->m_next;

            RemoveItem( iter );

            delete iter;

            iter = next;
        }
    }

    ptrNode_t*  AllocateEntry( pointerType *dataPtr )
    {
        ptrNode_t *node = new ptrNode_t( dataPtr );
        node->m_next = m_list;

        m_list = node;
        return node;
    }

    void __thiscall     RemoveItem( ptrNode_t *node )
    {
        if ( node == m_list )
        {
            m_list = node->m_next;
            return;
        }

        ptrNode_t *prevNode = NULL;
        
        for ( ptrNode_t *iter = m_list; iter != NULL; iter = iter->m_next )
        {
            if ( iter == node )
            {
                if ( prevNode )
                    prevNode->m_next = node->m_next;
                else
                    m_list = node->m_next;

                break;
            }
        }
    }

    /*=========================================================
        ptrNodeSingleContainer::RemoveItem

        Arguments:
            dataPtr - pointer to data held by this list
        Purpose:
            Loops through all allocated pointer nodes and deallocates
            the ones which hold dataPtr.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00533610
    =========================================================*/
    void __thiscall     RemoveItem( pointerType *dataPtr, bool stopOnFound )
    {
        ptrNode_t *prevPtr = NULL;

        for ( ptrNode_t *node = m_list; node != NULL; node = node->m_next )
        {
            if ( node->data == dataPtr )
            {
                if ( node == m_list )
                {
                    m_list = node->m_next;
                }
                else if ( prevPtr )
                {
                    prevPtr->m_next = node->m_next;
                }

                delete node;

                if ( stopOnFound )
                    return;
            }
            else
                prevPtr = node;
        }
    }

    void __thiscall     RemoveItem( pointerType *dataPtr )
    {
        RemoveItem( dataPtr, true );
    }

    inline ptrNode_t*   GetList( void )
    {
        return m_list;
    }

    inline const ptrNode_t* GetList( void ) const
    {
        return m_list;
    }

    ptrNode_t*  m_list;
};

template <typename pointerType>
class CPtrNodeDoubleSA
{
public:
    CPtrNodeDoubleSA( pointerType *dataPtr ) : data( dataPtr ), m_next( NULL ), m_prev( NULL )
    { }

    void* CPtrNodeDoubleSA::operator new( size_t )
    {
        return (*ppPtrNodeDoublePool)->Allocate();
    }

    void CPtrNodeDoubleSA::operator delete( void *ptr )
    {
        (*ppPtrNodeDoublePool)->Free( (CPtrNodeDoubleSA <void>*)ptr );
    }

    inline void Insert( CPtrNodeDoubleSA *list )
    {
        m_next = list;
        
        if ( list )
            list->m_prev = this;
    }

    inline void Remove( void )
    {
        if ( CPtrNodeDoubleSA *prev = m_prev )
            prev->m_next = m_next;

        if ( CPtrNodeDoubleSA *next = m_next )
            next->m_prev = m_prev;
    }

    pointerType*            data;
    CPtrNodeDoubleSA*       m_next;
    CPtrNodeDoubleSA*       m_prev;
};

typedef CPtrNodeDoubleSA <void> CPtrNodeDoubleSAInterface;

#endif //_CPtrNodeSA_