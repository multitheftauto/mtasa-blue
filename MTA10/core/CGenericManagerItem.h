/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGenericManagerItem.h
*  PURPOSE:     Base manager item class
*  DEVELOPERS:  Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGENERICMANAGERITEM_H
#define __CGENERICMANAGERITEM_H

#include "CGenericManager.h"

class CGenericManagerItem
{
public:
    enum ItemStatus
    {
        OK = 0,
        REMOVE_FROM_LIST,
        REMOVE_FROM_LIST_AND_DELETE
    };

public:
    CGenericManagerItem ()
    {
        m_itemStatus = ItemStatus::OK;

        m_pManager = NULL;
    };

    virtual ~CGenericManagerItem ()
    {
        UnlinkManager ();
    };

    bool IsAwaitingDeletionFromManager ()
    {
        return ( m_itemStatus == ItemStatus::REMOVE_FROM_LIST_AND_DELETE );
    };

    bool IsAwaitingRemovalFromManager ()
    {
        return ( m_itemStatus == ItemStatus::REMOVE_FROM_LIST || m_itemStatus == ItemStatus::REMOVE_FROM_LIST_AND_DELETE );
    };

    void LinkManager ( void* pManager )
    {
        m_pManager = pManager;
    };

    virtual void RemoveFromManager () = 0;

    void SetAwaitingDeletionFromManager ()
    {
        m_itemStatus = ItemStatus::REMOVE_FROM_LIST_AND_DELETE;
    };

    void SetAwaitingRemovalFromManager ()
    {
        m_itemStatus = ItemStatus::REMOVE_FROM_LIST;
    };

    void UnlinkManager ()
    {
        m_pManager = NULL;
    };

protected:
    void* m_pManager;

private:
    ItemStatus m_itemStatus;
};

#endif

