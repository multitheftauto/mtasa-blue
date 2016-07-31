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
    static void         RemovePtrValueRef   ( CClientEntity** ppPtr );
    static void         AddPtrValueRef      ( CClientEntity** ppPtr );
    static void         OnEntityDelete      ( CClientEntity* pEntity );
};


//
// Keep track of pointer contents
//
template < class T >
class CClientEntityBasePtr
{
public:
    CClientEntityBasePtr()
    {
        pEntity = NULL;
    }

    CClientEntityBasePtr( T* pNewValue )
    {
        pEntity = NULL;
        ChangeValue( pNewValue );
    }

    ~CClientEntityBasePtr()
    {
        ChangeValue( NULL );
    }

    void operator =( T* pNewValue )
    {
        ChangeValue( pNewValue );
    }

    operator T*()
    {
        return pEntity;
    }

    operator const T*() const
    {
        return pEntity;
    }

    T* operator-> ()
    {
        return pEntity;
    }

    const T* operator-> () const
    {
        return pEntity;
    }

private:
    void ChangeValue( T* pNewValue )
    {
        if ( pEntity != pNewValue )
        {
            if ( pEntity )
                CClientEntityRefManager::RemovePtrValueRef( (CClientEntity**)&pEntity );
            pEntity = pNewValue;
            if ( pEntity )
                CClientEntityRefManager::AddPtrValueRef( (CClientEntity**)&pEntity );
        }
    }

    T* pEntity;
};


typedef CClientEntityBasePtr < class CClientEntity >        CClientEntityPtr;
typedef CClientEntityBasePtr < class CClientPed >           CClientPedPtr;
typedef CClientEntityBasePtr < class CClientVehicle >       CClientVehiclePtr;
typedef CClientEntityBasePtr < class CClientPickup >        CClientPickupPtr;
typedef CClientEntityBasePtr < class CClientMarker >        CClientMarkerPtr;
typedef CClientEntityBasePtr < class CClientPlayer >        CClientPlayerPtr;
typedef CClientEntityBasePtr < class CClientProjectile >    CClientProjectilePtr;
