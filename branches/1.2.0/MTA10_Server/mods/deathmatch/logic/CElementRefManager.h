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
    #define ELEMENT_REF_DEBUG( ptr, desc ) \
        SString ( "%08x %s", (int)ptr, desc )
#else
    #define ELEMENT_REF_DEBUG( ptr, desc ) \
        NULL
#endif

//
// CElementRefManager
//
class CElementRefManager
{
public:
    static void         AddElementRefs          ( const char* szDebugInfo, ... );
    static void         AddElementListRef       ( const char* szDebugInfo, std::list < CElement* >* pList );
    static void         RemoveElementRefs       ( const char* szDebugInfo, ... );
    static void         RemoveElementListRef    ( const char* szDebugInfo, std::list < CElement* >* pList );
    static void         OnElementDelete         ( CElement* pElement );
};
