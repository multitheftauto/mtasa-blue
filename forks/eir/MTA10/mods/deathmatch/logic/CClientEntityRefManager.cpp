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


std::multimap < CClientEntity*, CClientEntity** > ms_ActiveEntityRefMap;
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
#ifdef MTA_DEBUG
    va_list vl;
    va_start(vl, szDebugInfo);
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CClientEntity** ppEntity = va_arg ( vl, CClientEntity** );
        if ( ppEntity == NULL )
            break;
        MapSet ( ms_EntityRefListDebugInfo, ppEntity, SString ( "Index:%d Addr:0x%s", i, szDebugInfo ) );
    }
    va_end(vl);
#endif
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
#ifdef MTA_DEBUG
    va_list vl;
    va_start(vl, szDebugInfo);
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        CClientEntity** ppEntity = va_arg ( vl, CClientEntity** );
        if ( ppEntity == NULL )
            break;
        MapRemove ( ms_EntityRefListDebugInfo, ppEntity );
    }
    va_end(vl);
#endif
}


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::AddPtrValueRef
//
// Add ref to active map
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::AddPtrValueRef( CClientEntity** ppPtr )
{
    MapInsert( ms_ActiveEntityRefMap, *ppPtr, ppPtr );
}


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::RemovePtrValueRef
//
// Remove ref from active map
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::RemovePtrValueRef( CClientEntity** ppPtr )
{
    MapRemovePair( ms_ActiveEntityRefMap, *ppPtr, ppPtr );
}


///////////////////////////////////////////////////////////////
//
// CClientEntityRefManager::OnEntityDelete
//
// Find any danglies
//
///////////////////////////////////////////////////////////////
void CClientEntityRefManager::OnEntityDelete ( CClientEntity* pEntity )
{
    typedef std::multimap < CClientEntity*, CClientEntity** > ::const_iterator const_iter_t;
    std::pair < const_iter_t, const_iter_t > itp = ms_ActiveEntityRefMap.equal_range( pEntity );
    for ( const_iter_t iter = itp.first ; iter != itp.second ; )
    {
        assert( iter->first == pEntity );
        CClientEntity** ppPtr = iter->second;
        assert( *ppPtr == pEntity );

#ifdef MTA_DEBUG
        SString* pstrDebugInfo = MapFind( ms_EntityRefListDebugInfo, ppPtr );
        const char* szDebugInfo = pstrDebugInfo ? **pstrDebugInfo : pEntity->GetClassName();
        OutputDebugLine( SString( "[EntityRef] Did null %s (%08x @ %08x)", szDebugInfo, pEntity, ppPtr ) );
#endif
        // Check CClientEntityPtr size as we are going to manually poke it
        dassert( sizeof( CClientEntityPtr ) == sizeof( CClientEntity* ) );
        //  Zero the pointer here and remove from the active list
        *ppPtr = NULL;
        ms_ActiveEntityRefMap.erase( iter++ );
    }
}
