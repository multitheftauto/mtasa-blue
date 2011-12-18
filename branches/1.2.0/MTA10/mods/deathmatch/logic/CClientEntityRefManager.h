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


#ifdef MTA_DEBUG
    #define ENTITY_REF_DEBUG( ptr, desc ) \
        SString ( "%08x %s", (int)ptr, desc )
#else
    #define ENTITY_REF_DEBUG( ptr, desc ) \
        NULL
#endif

//
// CClientEntityRefManager
//
class CClientEntityRefManager
{
public:
    static void         AddEntityRefs       ( const char* szDebugInfo, ... );
    static void         RemoveEntityRefs    ( const char* szDebugInfo, ... );
    static void         OnEntityDelete      ( CClientEntity* pEntity );
};
