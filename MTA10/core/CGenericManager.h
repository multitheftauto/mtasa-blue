/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGenericManager.h
*  PURPOSE:     Base manager template class
*  DEVELOPERS:  Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGENERICMANAGER_H
#define __CGENERICMANAGER_H

#include <assert.h>
#include <list>

template <class T>
class CGenericManager
{
protected:
    typedef CGenericManager < T > ManagerBase;
    typedef std::list < T > ManagerList;

public:
    CGenericManager ()
    {
        m_bCanDeleteItems = true;
    };

    ~CGenericManager ()
    {
        if ( m_bCanDeleteItems )
        {
            // Delete all of the items
            DeleteAll ();
        }
        else
        {
            // Remove all of the items
            RemoveAll ();
        }

        // Cleanup all of the deleted items
        Cleanup ();
    };

    bool Add ( T item )
    {
        item->LinkManager ( this );

        m_list.push_back ( item );

        return true;
    };

    void CanDeleteItems ( bool bCanDeleteItems )
    {
        m_bCanDeleteItems = bCanDeleteItems;
    };

    void Cleanup ()
    {
        if ( !Empty () )
        {
            for ( ManagerList::iterator iter = m_list.begin () ; iter != m_list.end () ; )
            {
                T currentItem = *iter;
                if ( currentItem )
                {
                    if ( currentItem->IsAwaitingRemovalFromManager () )
                    {
                        currentItem->UnlinkManager ();

                        if ( currentItem->IsAwaitingDeletionFromManager () )
                        {
                            delete currentItem;
                        }

                        iter = m_list.erase ( iter );
                    }
                    else
                    {
                        ++iter;
                    }
                }
                else
                {
                    iter = m_list.erase ( iter );
                }
            }

            if ( Empty () )
            {
                m_list.clear ();
            }
        }
    };

    size_t Count ()
    {
        return Size ();
    };

    bool Delete ( T item )
    {
        assert ( m_bCanDeleteItems );

        bool bDeleted = false;

        if ( !Empty () )
        {
            for ( ManagerList::iterator iter = m_list.begin () ; iter != m_list.end () ; ++iter )
            {
                T currentItem = *iter;
                if ( currentItem )
                {
                    if ( currentItem == item )
                    {
                        currentItem->SetAwaitingDeletionFromManager ();

                        bDeleted = true;
                    }
                }
            }
        }

        return bDeleted;
    };

    void DeleteAll ( bool bCleanup = false )
    {
        assert ( m_bCanDeleteItems );

        if ( !Empty () )
        {
            for ( ManagerList::iterator iter = m_list.begin () ; iter != m_list.end () ; ++iter )
            {
                T currentItem = *iter;
                if ( currentItem )
                {
                    currentItem->SetAwaitingDeletionFromManager ();
                }
            }
        }

        if ( bCleanup )
        {
            Cleanup ();
        }
    };

    bool Empty ()
    {
        return m_list.empty ();
    };

    bool Remove ( T item, bool removeImmediately = false )
    {
        bool bRemoved = false;

        if ( !Empty () )
        {
            for ( ManagerList::iterator iter = m_list.begin () ; iter != m_list.end () ; )
            {
                T currentItem = *iter;
                if ( currentItem )
                {
                    if ( currentItem == item )
                    {
                        if ( removeImmediately )
                        {
                            iter = m_list.erase ( iter );
                        }
                        else
                        {
                            currentItem->SetAwaitingRemovalFromManager ();

                            ++iter;
                        }

                        bRemoved = true;
                    }
                    else
                    {
                        ++iter;
                    }
                }
                else
                {
                    ++iter;
                }
            }
        }

        return bRemoved;
    };

    void RemoveAll ( bool bCleanup = false )
    {
        if ( !Empty () )
        {
            for ( ManagerList::iterator iter = m_list.begin () ; iter != m_list.end () ; ++iter )
            {
                T currentItem = *iter;
                if ( currentItem )
                {
                    currentItem->SetAwaitingRemovalFromManager ();
                }
            }
        }

        if ( bCleanup )
        {
            Cleanup ();
        }
    };

    size_t Size ()
    {
        if ( !Empty () )
        {
            return m_list.size ();
        }

        return 0;
    };

protected:
    ManagerList& List ()
    {
        return m_list;
    };

private:
    ManagerList m_list;

    bool m_bCanDeleteItems;
};

#endif

