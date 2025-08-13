/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabList.cpp
 *  PURPOSE:     Tab-able elements list class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CGUITabList::SelectNext(CGUITabListItem* pBase)
{
    // Loop through all elements which can be activated
    bool bFound = false;
    for (CGUITabIterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
    {
        CGUITabListItem* pItem = *iter;

        if (pItem == pBase)
        {
            bFound = true;
        }
        else if (bFound && pItem->IsEnabled())
        {
            // we found an element that wants to get selected
            pItem->ActivateOnTab();
            return;
        }
    }

    // Contine to search an element from the beginning
    for (CGUITabIterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
    {
        CGUITabListItem* pItem = *iter;

        if (pItem == pBase)
        {
            // just where we started, so we don't have to do anything
            return;
        }
        else if (pItem->IsEnabled())
        {
            // finally found something different than the current element
            pItem->ActivateOnTab();
            return;
        }
    }
}
