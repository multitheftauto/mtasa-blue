/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUITabList.cpp
*  PURPOSE:     Tab-able elements list class
*  DEVELOPERS:  Marcus Bauer <mabako@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CGUITabList::SelectNext ( CGUITabListItem* pBase )
{
    // Loop through all elements which can be activated
    bool bFound = false;
    for ( CGUITabIterator iter = m_Items.begin (); iter != m_Items.end () ; ++ iter )
    {
        if ( (*iter) == pBase )
        {
            bFound = true;
        }
        else if ( bFound && (*iter)->ActivateOnTab () )
        {
            // we found an element that wants to get selected
            return;
        }
    }

    // Contine to search an element from the beginning
    for ( CGUITabIterator iter = m_Items.begin (); iter != m_Items.end (); ++ iter )
    {
        if ( (*iter) == pBase )
        {
            // just where we started, so we don't have to do anything
            return;
        }
        else if ( (*iter)->ActivateOnTab () )
        {
                // finally found something different than the current element     
                return;
        }
    }
}
