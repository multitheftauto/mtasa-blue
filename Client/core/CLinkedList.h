/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/data/clinkedlist.h
 *  PURPOSE:     Linked list template class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *  The following code deals with implementing a templated
 *  linked list with a built-in public iterator class.  This
 *  allows for extremely easy traversal back and forth through
 *  this list and extremely fast access to the data within.
 *
 *****************************************************************************/

#pragma once

// ignore the return type warning for the -> operator when we are potentially using pointers themselves
#pragma warning(disable:4284)

#include <windows.h>

template <class T>
class CLinkedList
{
private:
    template <class TN>
    class CNode
    {
        friend class CLinkedList<TN>;
        friend class CIterator;

    private:
        TN         m_data;
        CNode<TN>* m_pNext;
        CNode<TN>* m_pPrev;

    public:
        CNode() { m_pNext = m_pPrev = NULL; };

        CNode(TN data, CNode<TN>* pNextNode = NULL, CNode<TN>* pPrevNode = NULL)
        {
            m_data = data;
            m_pNext = pNextNode;
            m_pPrev = pPrevNode;
        };
    };

public:
    class CIterator
    {
        friend class CLinkedList<T>;

    private:
        CNode<T>* m_pCurrent;

        CIterator(CNode<T>* pNode) { m_pCurrent = pNode; };

    public:
        CIterator() { m_pCurrent = NULL; };

        T& operator*() const { return m_pCurrent->m_data; };

        T* operator->() const { return &m_pCurrent->m_data; };

        CIterator& operator++()
        {
            m_pCurrent = m_pCurrent->m_pNext;
            return *this;
        };

        T& operator[](int j) const
        {
            CNode<T>* m_pTrans = m_pCurrent;
            for (int i = 0; i < j; i++)
            {
                if (m_pTrans)
                    m_pTrans = m_pTrans->m_pNext;
            }
            return m_pTrans->m_data;
        };

        CIterator operator++(int iDummy)
        {
            CIterator itRetVal(*this);
            m_pCurrent = m_pCurrent->m_pNext;
            return itRetVal;
        };

        CIterator& operator--()
        {
            m_pCurrent = m_pCurrent->m_pPrev;
            return *this;
        };

        CIterator operator--(int iDummy)
        {
            CIterator itRetVal(*this);
            m_pCurrent = m_pCurrent->m_pPrev;
            return itRetVal;
        };

        BOOL operator==(const CIterator& itNode) const { return (m_pCurrent == itNode.m_pCurrent); };

        BOOL operator!=(const CIterator& itNode) const { return !(m_pCurrent == itNode.m_pCurrent); };
    };

private:
    unsigned int m_iNumElements;
    CNode<T>*    m_pHead;
    CNode<T>*    m_pTail;

public:
    CLinkedList()
    {
        m_iNumElements = 0;
        m_pHead = m_pTail = NULL;
    };

    ~CLinkedList() { removeAllNodes(); };

    BOOL doesContain(const T& data)
    {
        if (find(data) != getEnd())
        {
            return TRUE;
        }

        return FALSE;
    };

    CIterator find(const T& data) { return findAfter(data, getBegin()); };

    CIterator findAfter(const T& data, CIterator itStart)
    {
        CIterator itRetVal = itStart;
        while (itRetVal != getEnd() && *itRetVal != data)
        {
            itRetVal++;
        }

        return itRetVal;
    };

    CIterator getElementAt(UINT n)
    {
        CIterator itRetVal = CIterator(m_pHead);
        if (n < m_iNumElements)
            for (UINT i = 0; i < n; i++)
                itRetVal++;
        return itRetVal;
    }

    CIterator getBegin() { return CIterator(m_pHead); };

    CIterator getEnd() { return CIterator(NULL); };

    unsigned int getNumElements() { return m_iNumElements; };

    // the only case this would be needed is if they wanted to do a reverse iterate
    CIterator getTail() { return CIterator(m_pTail); };

    void insertAfter(CIterator& itNode, T data)
    {
        // if (itNode == m_pHead)
        if (isEmpty())
        {
            insertAtFront(data);
        }
        else
        {
            CNode<T>* pNewNode = new CNode<T>(data, itNode.m_pCurrent->m_pNext, itNode.m_pCurrent);

            if (itNode.m_pCurrent->m_pNext)
            {
                itNode.m_pCurrent->m_pNext->m_pPrev = pNewNode;
            }

            itNode.m_pCurrent->m_pNext = pNewNode;

            if (!pNewNode->m_pNext || itNode == m_pTail)
            {
                m_pTail = pNewNode;
            }

            m_iNumElements++;
        }
    };

    void insertBefore(CIterator& itNode, T data)
    {
        if (itNode != m_pHead)
        {
            //          CIterator itInsert = --itNode;
            //          insertAfter (itInsert, data);
            insertAfter(--itNode, data);
        }
        else
        {
            CNode<T>* pNewNode = new CNode<T>(data, m_pHead);
            // pNewNode->m_pPrev = pNewNode; //stuck in a loop!

            if (m_pHead)
            {
                m_pHead->m_pPrev = pNewNode;
            }

            if (getNumElements() == 0)
            {
                m_pTail = pNewNode;
            }

            m_pHead = pNewNode;
            m_iNumElements++;
        }
    };

    void insertAtBack(T data)
    {
        CIterator itInsert = getTail();
        insertAfter(itInsert, data);
    };

    void insertAtFront(T data)
    {
        CIterator itInsert = getBegin();
        insertBefore(itInsert, data);
    };

    void insertSorted(T data, BOOL bAscending = TRUE)
    {
        BOOL bInserted = FALSE;

        for (CIterator itLL = getBegin(); itLL != getEnd() && !bInserted; itLL++)
        {
            if ((bAscending && data < *itLL) || (!bAscending && data > *itLL))
            {
                insertBefore(itLL, data);
                bInserted = TRUE;
            }
        }

        if (!bInserted)
        {
            insertAtBack(data);
        }
    };

    void insertPointerSorted(T data, BOOL bAscending = TRUE)
    {
        BOOL bInserted = FALSE;

        for (CIterator itLL = getBegin(); itLL != getEnd() && !bInserted; itLL++)
        {
            if ((bAscending && *data < **itLL) || (!bAscending && *data > **itLL))
            {
                insertBefore(itLL, data);
                bInserted = TRUE;
            }
        }

        if (!bInserted)
        {
            insertAtBack(data);
        }
    };

    BOOL isEmpty() { return (m_iNumElements == 0); };

    void remove(CIterator& itNode, BOOL bIterateForwards = TRUE)
    {
        CNode<T>* pTempNode = itNode.m_pCurrent;
        CNode<T>* pTempSingleNode = NULL;            // will be used if only a single node remains

        BOOL bIteratorSet = FALSE;

        if (itNode == m_pHead)
        {
            m_pHead = m_pHead->m_pNext;
            pTempSingleNode = m_pHead;

            itNode.m_pCurrent = m_pHead;
            bIteratorSet = TRUE;
        }

        if (itNode == m_pTail)
        {
            m_pTail = m_pTail->m_pPrev;
            pTempSingleNode = m_pTail;

            itNode.m_pCurrent = m_pTail;
            bIteratorSet = TRUE;
        }

        if (!bIteratorSet)
        {
            if (bIterateForwards)
            {
                itNode.m_pCurrent = itNode.m_pCurrent->m_pPrev;
            }
            else
            {
                itNode.m_pCurrent = itNode.m_pCurrent->m_pNext;
            }
        }

        if (pTempNode->m_pPrev)
        {
            pTempNode->m_pPrev->m_pNext = pTempNode->m_pNext;
        }

        if (pTempNode->m_pNext)
        {
            pTempNode->m_pNext->m_pPrev = pTempNode->m_pPrev;
        }

        delete pTempNode;

        m_iNumElements--;

        if (m_iNumElements == 0)
        {
            m_pHead = m_pTail = NULL;
        }
        if (m_iNumElements == 1 && pTempSingleNode)
        {
            pTempSingleNode->m_pPrev = NULL;
            pTempSingleNode->m_pNext = NULL;
            m_pHead = m_pTail = pTempSingleNode;
        }
    };

    void removeAllNodes()
    {
        CIterator itRemove;

        //          remove (getTail ());

        while (!isEmpty())
        {
            itRemove = getTail();
            remove(itRemove);
        }

        m_iNumElements = 0;
        m_pHead = m_pTail = NULL;
    };
};
