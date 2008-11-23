/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObjectManager.cpp
*  PURPOSE:     Physical object entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

static unsigned int g_uiValidObjectModels[] = {
    2, 0, 0, 0, 0, 0, 0, 0, 0, -4096, -1053185, 4194303, 16383, 0, 0, 0, 0, 0, 0, -128,
    -515899393, -134217729, -1, -1, 33554431, -1, -1, -1, -14337, -1, -33, -129, -1, -1, -1, -1, -1, -8387585, -1, -1,
    -1, -16385, -1, -1, -1, -1, -1, -1, -1, -33, -1, -771751937, -1, -9, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -25, -1, -1, -1, -1, -1, -1, -65537, -2147483648, 34079999,
    2113536, -4825600, -5, -1, -3145729, -1, -16777217, -35, -1, -1, -1, -1, -201326593, -1, -1, -1, -1, -1, -257, -1,
    1073741823, -133122, -1, -1, -65, -1, -1, -1, -1, -1, -1, -2146435073, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 1073741823, -64, -1, -1, -1, -1, -2635777, 134086663, 0, -64, -1, -1, -1, -1, -1, -1, -1, -536870927, -131069,
    -1, -1, -1, -1, -1, -1, -1, -1, -16384, -1, -33554433, -1, -1, -1, -1, -1, -1610612737, 524285, -128, -1,
    2080309247, -1, -1, -1114113, -1, -1, -1, 66977343, -524288, -1, -1, -1, -1, -2031617, -1, 114687, -256, -1, -4097, -1,
    -4097, -1, -1, 1010827263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -1, -1,
    2147483647, -33554434, -1, -1, -49153, -1148191169, 2147483647, -100781080, -262145, -57, -134217729, -8388607, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1048577, -1, -449, -1017, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1835009, -2049, -1, -1, -1, -1, -1, -1, -8193, -1, -536870913, -1, -1, -1, -1, -1, -87041, -1, -1,
    -1, -1, -1, -1, -209860, -1023, -8388609, -2096385, -1, -1048577, -1, -1, -1, -1, -1, -1, -897, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1610612737, -1025, -28673, -1, -1, -1, -1537, -1, -1, -13, -1, -1,
    -1, -1, -1985, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1056964609, -1, -1, -1, -1,
    -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -236716037, -1, -1, -1,
    -1, -1, -1, -1, -536870913, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -2097153, -2109441, -1, 201326591, -4194304, -1, -1, -241, -1, -1, -1, -1, -1, -1, 7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, -32768, -1, -1, -1, -2, -671096835, -1, -8388609, -66061441, -13, -1793,
    -32257, -247809, -1, -1, -513, 16252911, 0, 0, 0, -131072, 33554383, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 8388607, 0, 0, 0, 0, 0, 0, -256, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -268435449, -1, -1, -2049, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 92274627, -65536, -2097153, -268435457, 591191935, 1, 0, -16777216, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
};



int CClientObjectManager::m_iEntryInfoNodeEntries = 0;
int CClientObjectManager::m_iPointerNodeSingleLinkEntries = 0;
int CClientObjectManager::m_iPointerNodeDoubleLinkEntries = 0;

CClientObjectManager::CClientObjectManager ( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}


CClientObjectManager::~CClientObjectManager ( void )
{
    // Destroy all objects
    DeleteAll ();
}


void CClientObjectManager::DoPulse ( void )
{
    UpdateLimitInfo ();

    CClientObject * pObject = NULL;
    // Loop through all our streamed-in objects
    list < CClientObject * > ::iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end () ; iter++ )
    {
        pObject = *iter;
        // We should have a game-object here
        assert ( pObject->GetGameObject () );
        pObject->StreamedInPulse ();
    }
}


void CClientObjectManager::DeleteAll ( void )
{
    // Delete all the objects
    m_bCanRemoveFromList = false;
    list < CClientObject* > ::const_iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Objects.clear ();
    m_bCanRemoveFromList = true;
}


CClientObject* CClientObjectManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTOBJECT )
    {
        return static_cast < CClientObject* > ( pEntity );
    }

    return NULL;
}


CClientObject* CClientObjectManager::Get ( CObject* pObject, bool bValidatePointer )
{
    if ( !pObject ) return NULL;

    if ( bValidatePointer )
    {
        list < CClientObject* > ::const_iterator iter = m_StreamedIn.begin ();
        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGameObject () == pObject )
            {
                return *iter;
            }
        }
    }
    else
    {
        return reinterpret_cast < CClientObject* > ( pObject->GetStoredPointer () );
    }
    return NULL;
}


CClientObject* CClientObjectManager::GetSafe ( CEntity * pEntity )
{
    if ( !pEntity ) return NULL;


    list < CClientObject* > ::const_iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end (); iter++ )
    {
        if ( dynamic_cast < CEntity * > ( (*iter)->GetGameObject () ) == pEntity )
        {
            return *iter;
        }
    }
    return NULL;
}


bool CClientObjectManager::IsValidModel ( unsigned long ulObjectModel )
{
    bool bIsValid = false;

    if ( ulObjectModel < 20001 )
    {
        unsigned int uiChunk = ulObjectModel / ( sizeof(unsigned int)*8 );
        unsigned int shift = ulObjectModel - ( uiChunk * sizeof(unsigned int)*8 );
        unsigned int bit = 1 << shift;
        bIsValid = !!( g_uiValidObjectModels[uiChunk] & bit );

        // Return whether we can grab the model information or not
        if ( bIsValid && ! g_pGame->GetModelInfo ( ulObjectModel ) )
        {
            bIsValid = false;
        }
    }

    return bIsValid;
}


/*
void CClientObjectManager::LoadObjectsAroundPoint ( const CVector& vecPosition, float fRadius )
{
    // Look for objects closer than that in our list
    CClientObject* pObject;
    CVector vecObjectPosition;
    CBoundingBox* pBoundingBox = NULL;
    list < CClientObject* > ::const_iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        // Grab the object
        pObject = *iter;

        if ( pObject->GetDimension () == m_pManager->GetObjectStreamer ()->GetDimension () )
        {
            // Grab the object radius
            float fObjectRadius = 0;
            pBoundingBox = g_pGame->GetModelInfo ( pObject->GetModel () )->GetBoundingBox ();
            if ( pBoundingBox )
            {
                fObjectRadius = pBoundingBox->fRadius;
            }

            // Is it close enough?
            pObject->GetPosition ( vecObjectPosition );
            if ( IsPointNearPoint3D ( vecObjectPosition, vecPosition, fRadius - fObjectRadius ) )
            {
                pObject->Create ();
            }
        }
    }
}
*/


bool CClientObjectManager::ObjectsAroundPointLoaded ( const CVector& vecPosition, float fRadius, unsigned short usDimension )
{
    // TODO: mix in with the streamer, cause this is way too slow
    return true;

    CVector vecObject;
    float fDistanceX, fDistanceY, fDistanceZ, fDistanceExp;

    // Radius exp 2
    float fRadius2 = fRadius * fRadius;

    // Loop through our objects
    CClientObject* pObject;
    list < CClientObject* > ::const_iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        pObject = *iter;

        // Is it not loaded?
        if ( !pObject->GetGameObject () )
        {
            if ( pObject->GetDimension () == usDimension )
            {
                // Grab its position
                pObject->GetPosition ( vecObject );

                // Grab the distance ^ 2
	            fDistanceX = vecObject.fX - vecPosition.fX;
	            fDistanceY = vecObject.fY - vecPosition.fY;
	            fDistanceZ = vecObject.fZ - vecPosition.fZ;
	            fDistanceExp = fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ;

                // Closer than the radius ^ 2?
                if ( fDistanceExp < fRadius2 )
                {
                    // We haven't loaded all the objects nearby that location
                    return false;
                }
            }
        }
    }

    // We have
    return true;
}


void CClientObjectManager::OnCreation ( CClientObject * pObject )
{
    m_StreamedIn.push_back ( pObject );
    UpdateLimitInfo ();
}


void CClientObjectManager::OnDestruction ( CClientObject * pObject )
{
    m_StreamedIn.remove ( pObject );
    UpdateLimitInfo ();
}


void CClientObjectManager::UpdateLimitInfo ( void )
{
    CPools* pPools = g_pGame->GetPools ();
    m_iEntryInfoNodeEntries = pPools->GetEntryInfoNodePool ()->GetNumberOfUsedSpaces ();
    m_iPointerNodeSingleLinkEntries = pPools->GetPointerNodeSingleLinkPool ()->GetNumberOfUsedSpaces ();
    m_iPointerNodeDoubleLinkEntries = pPools->GetPointerNodeDoubleLinkPool ()->GetNumberOfUsedSpaces ();
}


bool CClientObjectManager::IsObjectLimitReached ( void )
{
    // Make sure we haven't run out of entry node info's, single link nodes or double link nodes
    #define MAX_ENTRYINFONODES          2200    // Real limit is 2300
    #define MAX_POINTERNODESINGLELINK   65000   // Real limit is 70000
    #define MAX_POINTERNODESDOUBLELINK  3600    // Real limit is 4000

    // If we've run out of either of these limit, don't allow more objects
    if ( m_iEntryInfoNodeEntries >= MAX_ENTRYINFONODES ||
         m_iPointerNodeSingleLinkEntries >= MAX_POINTERNODESINGLELINK ||
         m_iPointerNodeDoubleLinkEntries >= MAX_POINTERNODESDOUBLELINK )
    {
        // Limit reached
        return true;
    }

    // Allow max 250 objects at once for now.
    // TODO: The real limit is up to 350 but we're limited by other limits.
    return g_pGame->GetPools ()->GetObjectCount () >= 250;
}


list < CClientObject* > ::const_iterator CClientObjectManager::IterGet ( CClientObject* pObject )
{
    // Find it in our list
    list < CClientObject* > ::const_iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        if ( *iter == pObject )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Objects.begin ();
}


list < CClientObject* > ::const_reverse_iterator CClientObjectManager::IterGetReverse ( CClientObject* pObject )
{
    // Find it in our list
    list < CClientObject* > ::reverse_iterator iter = m_Objects.rbegin ();
    for ( ; iter != m_Objects.rend (); iter++ )
    {
        if ( *iter == pObject )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Objects.rbegin ();
}


void CClientObjectManager::RemoveFromList ( CClientObject* pObject )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_Objects.empty() ) m_Objects.remove ( pObject );
    }
}


bool CClientObjectManager::Exists ( CClientObject* pObject )
{
    list < CClientObject* > ::const_iterator iter = m_Objects.begin ();
    for ( ; iter != m_Objects.end (); iter++ )
    {
        if ( *iter == pObject )
        {
            return true;
        }
    }

    return false;
}
