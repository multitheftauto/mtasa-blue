/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:     Help prevent invalid element pointers by removing refs when an element is deleted
*  DEVELOPERS:
*
*****************************************************************************/

#include "StdInc.h"
#include <set>


static std::set < CElement** > ms_ElementRefList;
static std::set < std::list < CElement* >* > ms_ElementRefListList;

#ifdef MTA_DEBUG
    static std::map < CElement**, SString > ms_ElementRefListDebugInfo;
    static std::map < std::list < CElement* >*, SString > ms_ElementRefListListDebugInfo;
#endif


///////////////////////////////////////////////////////////////
//
// CElementRefManager::AddElementRefs
//
//
//
///////////////////////////////////////////////////////////////
void CElementRefManager::AddElementRefs ( const char* szDebugInfo, ... )
{
    va_list vl;
    va_start(vl, szDebugInfo);

    // For each argument (up to 100)
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CElement** ppElement = va_arg ( vl, CElement** );
        if ( ppElement == NULL )
            break;

        dassert ( ms_ElementRefList.find ( ppElement ) == ms_ElementRefList.end () );

        ms_ElementRefList.insert ( ppElement );

        dassert ( ms_ElementRefList.find ( ppElement ) != ms_ElementRefList.end () );
#ifdef MTA_DEBUG
        MapSet ( ms_ElementRefListDebugInfo, ppElement, SString ( "Index:%d Addr:0x%s", i, szDebugInfo ) );
#endif
    }
    va_end(vl);
}

///////////////////////////////////////////////////////////////
//
// CElementRefManager::AddElementListRef
//
//
//
///////////////////////////////////////////////////////////////
void CElementRefManager::AddElementListRef ( const char* szDebugInfo, std::list < CElement* >* pList )
{
        dassert ( ms_ElementRefListList.find ( pList ) == ms_ElementRefListList.end () );

        ms_ElementRefListList.insert ( pList );

        dassert ( ms_ElementRefListList.find ( pList ) != ms_ElementRefListList.end () );
#ifdef MTA_DEBUG
        MapSet ( ms_ElementRefListListDebugInfo, pList, SString ( "list Addr:0x%s", szDebugInfo ) );
#endif
}


///////////////////////////////////////////////////////////////
//
// CElementRefManager::RemoveElementRefs
//
//
//
///////////////////////////////////////////////////////////////
void CElementRefManager::RemoveElementRefs ( const char* szDebugInfo, ... )
{
    va_list vl;
    va_start(vl, szDebugInfo);

    // For each argument (up to 100)
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CElement** ppElement = va_arg ( vl, CElement** );
        if ( ppElement == NULL )
            break;

        dassert ( ms_ElementRefList.find ( ppElement ) != ms_ElementRefList.end () );

        ms_ElementRefList.erase ( ppElement );

        dassert ( ms_ElementRefList.find ( ppElement ) == ms_ElementRefList.end () );
#ifdef MTA_DEBUG
        MapRemove ( ms_ElementRefListDebugInfo, ppElement );
#endif
    }
    va_end(vl);
}


///////////////////////////////////////////////////////////////
//
// CElementRefManager::RemoveElementListRef
//
//
//
///////////////////////////////////////////////////////////////
void CElementRefManager::RemoveElementListRef ( const char* szDebugInfo, std::list < CElement* >* pList )
{
        dassert ( ms_ElementRefListList.find ( pList ) != ms_ElementRefListList.end () );

        ms_ElementRefListList.erase ( pList );

        dassert ( ms_ElementRefListList.find ( pList ) == ms_ElementRefListList.end () );
#ifdef MTA_DEBUG
        MapRemove ( ms_ElementRefListListDebugInfo, pList );
#endif
}


///////////////////////////////////////////////////////////////
//
// CElementRefManager::OnElementDelete
//
// Remove all know refs to this element
//
///////////////////////////////////////////////////////////////
void CElementRefManager::OnElementDelete ( CElement* pElement )
{
    OutputDebugLine ( SString ( "ms_ElementRefList.size: %d  ms_ElementRefListList.size: %d", ms_ElementRefList.size (), ms_ElementRefListList.size () ) );
    {
        std::set < CElement** > ::iterator iter = ms_ElementRefList.begin ();
        for ( ; iter != ms_ElementRefList.end () ; ++iter )
        {
            CElement*& pOther = **iter;
            if ( pOther == pElement )
            {
                // NULL invalid pointer
                pOther = NULL;
    #ifdef MTA_DEBUG
                SString* pstrDebugInfo = MapFind ( ms_ElementRefListDebugInfo, &pOther );
                assert ( pstrDebugInfo );
                OutputDebugLine ( SString ( "Did null %s (%08x @ %08x)", **pstrDebugInfo, pElement, &pOther ) );
    #endif
            }
        }
    }

    {
        // For each list ref
        std::set < std::list < CElement* >* > ::iterator itRef = ms_ElementRefListList.begin ();
        for ( ; itRef != ms_ElementRefListList.end () ; ++itRef )
        {
            std::list < CElement* >& rList = **itRef;

            // For each element
            std::list < CElement* >::iterator itList = rList.begin ();
            while ( itList != rList.end () )
            {
                CElement* const pOther = *itList;
                if ( pOther == pElement )
                {
                    // Remove invalid pointer from list - TODO - Think man, think
                    itList = rList.erase ( itList );
        #ifdef MTA_DEBUG
                    SString* pstrDebugInfo = MapFind ( ms_ElementRefListListDebugInfo, &rList );
                    assert ( pstrDebugInfo );
                    OutputDebugLine ( SString ( "Did list item %s (%08x @ %08x)", **pstrDebugInfo, pElement, &rList ) );     
        #endif
                }
                else
                    ++itList;
            }
        }
    }
}
