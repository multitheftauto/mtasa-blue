/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*****************************************************************************/

#include "StdInc.h"
#include <set>


static std::set < CClientEntity** > ms_EntityRefList;
#ifdef MTA_DEBUG
    static std::map < CClientEntity**, SString > ms_EntityRefListDebugInfo;
#endif


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::AddEntityRefs
//
//
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::AddEntityRefs ( const char* szDebugInfo, ... )
{
    va_list vl;
    va_start(vl, szDebugInfo);
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CClientEntity** ppEntity = va_arg ( vl, CClientEntity** );
        if ( ppEntity == NULL )
            break;
#ifdef MTA_DEBUG
        assert ( ms_EntityRefList.find ( ppEntity ) == ms_EntityRefList.end () );
#endif
        ms_EntityRefList.insert ( ppEntity );        
#ifdef MTA_DEBUG
        assert ( ms_EntityRefList.find ( ppEntity ) != ms_EntityRefList.end () );
        MapSet ( ms_EntityRefListDebugInfo, ppEntity, SString ( "Index:%d Addr:0x%s", i, szDebugInfo ) );
#endif
    }
    va_end(vl);
}


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::RemoveEntityRefs
//
//
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::RemoveEntityRefs ( const char* szDebugInfo, ... )
{
    va_list vl;
    va_start(vl, szDebugInfo);
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CClientEntity** ppEntity = va_arg ( vl, CClientEntity** );
        if ( ppEntity == NULL )
            break;
#ifdef MTA_DEBUG
        assert ( ms_EntityRefList.find ( ppEntity ) != ms_EntityRefList.end () );
#endif
        ms_EntityRefList.erase ( ppEntity );
#ifdef MTA_DEBUG
        assert ( ms_EntityRefList.find ( ppEntity ) == ms_EntityRefList.end () );
        MapRemove ( ms_EntityRefListDebugInfo, ppEntity );
#endif
    }
    va_end(vl);
}


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::OnEntityDelete
//
//
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::OnEntityDelete ( CClientEntity* pEntity )
{
#ifdef MTA_DEBUG
    OutputDebugLine ( SString ( "ms_EntityRefList.size: %d", ms_EntityRefList.size () ) );
#endif
    std::set < CClientEntity** > ::iterator iter = ms_EntityRefList.begin ();
    for ( ; iter != ms_EntityRefList.end () ; ++iter )
    {
        CClientEntity*& pOther = **iter;
        if ( pOther == pEntity )
        {
            pOther = NULL;
#ifdef MTA_DEBUG
            SString* pstrDebugInfo = MapFind ( ms_EntityRefListDebugInfo, &pOther );
            assert ( pstrDebugInfo );
            OutputDebugLine ( SString ( "Did null %s (%08x @ %08x)", **pstrDebugInfo, pEntity, &pOther ) );     
#endif
        }
    }
}


