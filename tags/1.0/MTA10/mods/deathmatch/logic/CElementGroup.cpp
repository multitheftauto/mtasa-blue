/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementGroup.cpp
*  PURPOSE:     Grouping elements
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class stores a number of elements in a flat list. This is
// purely to allow actions to be done on the list as a whole,
// mainly the deletion of every element, but possibly also the 
// hiding and showing of them on demand. Each element can belong in
// only one CElementGroup at a time (though this will probably not 
// be enforced at this level).

#include "StdInc.h"

using std::list;

extern CClientGame * g_pClientGame;

CElementGroup::~CElementGroup()
{
    CElementDeleter * deleter = g_pClientGame->GetElementDeleter();

    list < CClientEntity* > ::iterator iter = m_elements.begin ();
    for ( ; iter != m_elements.end (); iter++ )
    {
        ( *iter )->SetElementGroup ( NULL );
        ( *iter )->DeleteAllEvents ();
        deleter->Delete ( *iter );
    }
}

void CElementGroup::Add ( class CClientEntity * element )
{
    m_elements.push_back ( element );
    element->SetElementGroup ( this );
}

void CElementGroup::Remove ( class CClientEntity * element )
{
    if ( !m_elements.empty() ) m_elements.remove ( element );
}

unsigned int CElementGroup::GetCount ( void )
{
    return m_elements.size();
}