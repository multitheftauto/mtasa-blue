/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/deathmatch/logic/CIdArray.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define SHARED_ARRAY_BASE_ID    0x02000000

CStack < SArrayId, 1 >          CIdArray::m_IDStack;
std::vector < SIdEntry >        CIdArray::m_Elements;
uint                            CIdArray::m_uiCapacity = 0;
bool                            CIdArray::m_bInitialized = false;
uint                            CIdArray::m_uiPopIdCounter = 0;
uint                            CIdArray::m_uiMinTicksBetweenIdReuse = 0;
CTickCount                      CIdArray::m_LastPeriodStart;


//
// Initialize
//
void CIdArray::Initialize ( void )
{
    if ( !m_bInitialized )
    {
        m_uiCapacity = 0;
        m_bInitialized = true;
        m_uiPopIdCounter = 0;
        m_uiMinTicksBetweenIdReuse = 1000 * 60 * 60;     // 1 hour
        m_LastPeriodStart = CTickCount::Now ();
        ExpandBy ( 2000 );
    }
}


//
// Add object to the list and get a new id for it
//
SArrayId CIdArray::PopUniqueId ( void* pObject, EIdClassType idClass )
{
    if ( !m_bInitialized )
        Initialize ();

    // Measure amount of time it took to use 50% of free ids
    m_uiPopIdCounter++;
    if ( m_uiPopIdCounter > m_IDStack.GetUnusedAmount () / 2 )
    {
        CTickCount now = CTickCount::Now ();
        uint uiTimeToUse50PercentOfFreeIds = (uint)( now - m_LastPeriodStart ).ToLongLong ();

        // If it's less than 50% of the allowed time, add new ids
        if ( uiTimeToUse50PercentOfFreeIds < m_uiMinTicksBetweenIdReuse / 2 )
        {
            ExpandBy ( 2000 );
        }
        else
        {
            m_LastPeriodStart = now;
            m_uiPopIdCounter = 0;
        }
    }

    // Ensure there is enough free ID's in all situations
    if ( m_IDStack.GetUnusedAmount () < 1 )
    {
        ExpandBy ( 2000 );
    }

    // Grab the next unused ID
    SArrayId ulPhysicalIndex;
    bool bSuccess = m_IDStack.Pop ( ulPhysicalIndex );
    assert ( bSuccess );

    // Checks
    assert ( ( ulPhysicalIndex != INVALID_ARRAY_ID ) &&
            ( ulPhysicalIndex <= m_uiCapacity ) &&
            ( m_Elements.size () > ulPhysicalIndex ) &&
            ( m_Elements [ ulPhysicalIndex ].pObject == NULL ) &&
            ( m_Elements [ ulPhysicalIndex ].idClass == EIdClass::NONE ) );

    m_Elements [ ulPhysicalIndex ].pObject = pObject;
    m_Elements [ ulPhysicalIndex ].idClass = idClass;

    // Map to ID
    SArrayId ulLogicalID = ( ulPhysicalIndex + SHARED_ARRAY_BASE_ID ) ;
    return ulLogicalID;
}


void CIdArray::PushUniqueId ( void* pObject, EIdClassType idClass, SArrayId id )
{
    dassert ( m_bInitialized );

    // Map to index
    SArrayId ulPhysicalIndex = id - SHARED_ARRAY_BASE_ID;

    // Checks
    assert ( ( id != INVALID_ARRAY_ID ) &&
            ( ulPhysicalIndex <= m_uiCapacity ) &&
            ( m_Elements [ ulPhysicalIndex ].pObject == pObject ) &&
            ( m_Elements [ ulPhysicalIndex ].idClass == idClass ) );

    m_IDStack.Push ( ulPhysicalIndex );
    m_Elements [ ulPhysicalIndex ].pObject = NULL;
    m_Elements [ ulPhysicalIndex ].idClass = EIdClass::NONE;
}


void* CIdArray::FindEntry ( SArrayId id, EIdClassType idClass )
{
    // Return the element with the given ID

    // Map to index
    SArrayId ulPhysicalIndex = id - SHARED_ARRAY_BASE_ID;

    if ( id != INVALID_ARRAY_ID && ulPhysicalIndex <= m_uiCapacity )
    {
        if ( m_Elements [ ulPhysicalIndex ].idClass == idClass )
            return m_Elements [ ulPhysicalIndex ].pObject;
    }
    return NULL;
}


void CIdArray::ExpandBy ( uint uiAmount )
{
    m_IDStack.ExpandBy ( uiAmount );
    SIdEntry blankEntry = { NULL, EIdClass::NONE };
    m_Elements.resize ( m_uiCapacity + uiAmount + 1, blankEntry );
    m_uiCapacity += uiAmount;
    assert ( m_IDStack.GetCapacity () == m_uiCapacity );
}


uint CIdArray::GetCapacity ( void )
{
    return m_uiCapacity;
}


uint CIdArray::GetUnusedAmount ( void )
{
    return m_IDStack.GetUnusedAmount ();
}
