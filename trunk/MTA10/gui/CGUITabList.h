/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUITabList.h
*  PURPOSE:     Tab-able Elements list class
*  DEVELOPERS:  Marcus Bauer <mabako@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <list>

typedef std::list<CGUITabListItem*>::const_iterator CGUITabIterator;

class CGUITabList
{
public:
                         CGUITabList  ( void )                    	{};
                        ~CGUITabList  ( void )                   	{};

    void                AddItem       ( CGUITabListItem* pItem )    { m_Items.push_back ( pItem ); };
    void                RemoveItem    ( CGUITabListItem* pItem )    { m_Items.remove ( pItem ); };

    void                SelectNext    ( CGUITabListItem* pBase );

private:
    std::list < CGUITabListItem* >     m_Items;
};
