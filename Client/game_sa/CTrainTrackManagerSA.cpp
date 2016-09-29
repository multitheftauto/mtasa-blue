/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackManagerSA.cpp
*  PURPOSE:     Train Node Manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include <TrackNodes.h>

CTrainTrackManagerSA::CTrainTrackManagerSA()
{
    m_pDefaultRailNodePointer = AllocateDefault();

    for (std::size_t i = 0; i < NumRailTracks; ++i)
    {
        // there are 4 train tracks so initialise them all
        m_pTrainTracks[i] = new CTrainTrackSA(i, true, this);
    }

    m_bInitialised = false;
}

CTrainTrackManagerSA::~CTrainTrackManagerSA ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so delete them all
        delete m_pTrainTracks[i];
        m_pTrainTracks[i] = NULL;
    }
}

CTrainTrackSA * CTrainTrackManagerSA::GetTrainTrack ( unsigned char ucTrack )
{
    if ( ucTrack <= m_dwNumberOfTracks )
    {
        return m_pTrainTracks[ucTrack];
    }
    return NULL;
}

void CTrainTrackManagerSA::Initialise ( )
{
    if ( !m_bInitialised )
    {
        for ( int i = 0; i < NUM_RAILTRACKS; i++ )
        {
            unsigned char ucIndex = 4 * i;
            m_pRailNodePointers[i] = (void*)*(DWORD*)(ARRAY_RailTrackNodePointers + ucIndex);
            m_fRailTrackLengths[i] = *(float*)(ARRAY_TrackLengths + ucIndex);
            m_dwNumberOfTrackNodes[i] = *(DWORD*)(ARRAY_NumRailTrackNodes + ucIndex);
            // there are 4 train tracks so initialise them all
            m_pTrainTracks[i]->Initialise ( );
            m_pTrainTracks[i]->Recalculate ( );
        }
        
        for ( int i = NUM_RAILTRACKS; i < MAX_TOTAL_TRACKS;i++ )
        {
            m_pTrainTracks[i] = NULL;
            m_pRailNodePointers[i] = NULL;
            DestroyTrainTrack ( i );
        }

        m_dwNumberOfTracks = NUM_RAILTRACKS;

        // pGlobalTrainNodes 0xC38014
        MemPut <DWORD> ( 0x6F58D2, (DWORD)&m_pRailNodePointers ); // 1
        MemPut <DWORD> ( 0x6F59FE, (DWORD)&m_pRailNodePointers ); // 2
        MemPut <DWORD> ( 0x6F6C06, (DWORD)&m_pRailNodePointers ); // 3
        MemPut <DWORD> ( 0x6F6D0A, (DWORD)&m_pRailNodePointers ); // 4
        MemPut <DWORD> ( 0x6F6EB1, (DWORD)&m_pRailNodePointers ); // 5
        MemPut <DWORD> ( 0x6F6F79, (DWORD)&m_pRailNodePointers ); // 6

        MemPut <DWORD> ( 0x6F7442, (DWORD)&m_pRailNodePointers ); // 7
        MemPut <DWORD> ( 0x6F7467, (DWORD)&m_pRailNodePointers ); // 8
        MemPut <DWORD> ( 0x6F74EC, (DWORD)&m_pRailNodePointers ); // 9
        MemPut <DWORD> ( 0x6F75B7, (DWORD)&m_pRailNodePointers ); // 10
        MemPut <DWORD> ( 0x6F7B73, (DWORD)&m_pRailNodePointers ); // 11
        MemPut <DWORD> ( 0x6F7C60, (DWORD)&m_pRailNodePointers ); // 12
        MemPut <DWORD> ( 0x6F7DC5, (DWORD)&m_pRailNodePointers ); // 13
        MemPut <DWORD> ( 0x6F7EE9, (DWORD)&m_pRailNodePointers ); // 14
        MemPut <DWORD> ( 0x6F8007, (DWORD)&m_pRailNodePointers ); // 15
        MemPut <DWORD> ( 0x6F809C, (DWORD)&m_pRailNodePointers ); // 16
        MemPut <DWORD> ( 0x6F871C, (DWORD)&m_pRailNodePointers ); // 17

        // fRailTrackOneLength 0xC37FEC
        MemPut <DWORD> ( 0x6F5BC4, (DWORD)&m_fRailTrackLengths ); // 1
        MemPut <DWORD> ( 0x6F5BD9, (DWORD)&m_fRailTrackLengths ); // 2
        MemPut <DWORD> ( 0x6F5C15, (DWORD)&m_fRailTrackLengths ); // 3
        MemPut <DWORD> ( 0x6F5C32, (DWORD)&m_fRailTrackLengths ); // 4
        MemPut <DWORD> ( 0x6F745D, (DWORD)&m_fRailTrackLengths ); // 5
        MemPut <DWORD> ( 0x6F8712, (DWORD)&m_fRailTrackLengths ); // 6
        MemPut <DWORD> ( 0x6F6FE8, (DWORD)&m_fRailTrackLengths ); // 7

        // iNumberOfTrackNodes 0xC38014
        MemPut <DWORD> ( 0x6F59EB, (DWORD)&m_dwNumberOfTrackNodes ); // 1
        MemPut <DWORD> ( 0x6F6BF3, (DWORD)&m_dwNumberOfTrackNodes ); // 2
        MemPut <DWORD> ( 0x6F6C96, (DWORD)&m_dwNumberOfTrackNodes ); // 3
        MemPut <DWORD> ( 0x6F6CD1, (DWORD)&m_dwNumberOfTrackNodes ); // 4
        MemPut <DWORD> ( 0x6F6D04, (DWORD)&m_dwNumberOfTrackNodes ); // 5
        MemPut <DWORD> ( 0x6F6F52, (DWORD)&m_dwNumberOfTrackNodes ); // 6

        MemPut <DWORD> ( 0x6F7DB6, (DWORD)&m_dwNumberOfTrackNodes ); // 7
        MemPut <DWORD> ( 0x6F7F05, (DWORD)&m_dwNumberOfTrackNodes ); // 8
        MemPut <DWORD> ( 0x6F7F12, (DWORD)&m_dwNumberOfTrackNodes ); // 9
        MemPut <DWORD> ( 0x6F8001, (DWORD)&m_dwNumberOfTrackNodes ); // 10
        MemPut <DWORD> ( 0x6F80B6, (DWORD)&m_dwNumberOfTrackNodes ); // 11
        MemPut <DWORD> ( 0x6F80C3, (DWORD)&m_dwNumberOfTrackNodes ); // 12
        MemPut <DWORD> ( 0x6F8723, (DWORD)&m_dwNumberOfTrackNodes ); // 13

        m_bInitialised = true;
    }
}

bool CTrainTrackManagerSA::GetOriginalRailNode ( unsigned char ucTrack, unsigned short usNode, SRailNode* pRailNode )
{
    if ( ucTrack < NUM_RAILTRACKS )
    {
        if ( usNode < OriginalTrackNodes[ucTrack].size() )
        {
            SRailNode* pOriginalRailNode = m_pTrainTracks[ucTrack]->GetRailNode ( usNode );
            *pRailNode = *pOriginalRailNode;
            return true;
        }
    }
    return false;
}

void CTrainTrackManagerSA::ResetTracks ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so reset them all
        m_pTrainTracks[i]->Reset ( );
    }

    m_dwNumberOfTracks = NUM_RAILTRACKS;
    MemSet ( (void*)COMP_NumberOfTracks, MAX_TOTAL_TRACKS, 1 );
}

void * CTrainTrackManagerSA::GetRailNodePointer ( unsigned char ucTrack )
{ 
    return m_pRailNodePointers[ucTrack]; 
}

void CTrainTrackManagerSA::SetRailNodePointer ( unsigned char ucTrack, void * pRailNodePointers ) 
{ 
    m_pRailNodePointers[ucTrack] = pRailNodePointers; 
}

float CTrainTrackManagerSA::GetRailLength ( unsigned char ucTrack )
{ 
    return m_fRailTrackLengths[ucTrack]; 
}

bool CTrainTrackManagerSA::SetRailLength ( unsigned char ucTrack, float fLength )
{ 
    m_fRailTrackLengths[ucTrack] = fLength; 
    return true; 
}

DWORD CTrainTrackManagerSA::GetNumberOfRailNodes ( unsigned char ucTrack )
{ 
    return m_dwNumberOfTrackNodes[ucTrack]; 
}

DWORD CTrainTrackManagerSA::SetNumberOfRailNodes ( unsigned char ucTrack, DWORD dwTrackNodes )
{ 
    return m_dwNumberOfTrackNodes[ucTrack] = dwTrackNodes; 
}

DWORD CTrainTrackManagerSA::GetRailNodePointers ( void )
{ 
    return (DWORD)&m_pRailNodePointers; 
}

CTrainTrackSA* CTrainTrackManagerSA::CreateTrainTrack ( unsigned int uiNodes, unsigned char ucTrackID, bool bLinkedLastNode )
{
    auto pTrainTrack = std::make_unique<CTrainTrackSA>(ucTrackID, false, this);
    m_pTrainTracks[ucTrackID] = pTrainTrack;

    // Reallocate and initialise
    Reallocate ( pTrainTrack, uiNodes );
    pTrainTrack->Initialise();
    m_fRailTrackLengths[ucTrackID]      = pTrainTrack->GetLength ( );
        
    return pTrainTrack;
    }
    return NULL;
}

bool CTrainTrackManagerSA::DestroyTrainTrack ( DWORD dwTrackID )
{
    if ( dwTrackID < MAX_TOTAL_TRACKS )
    {
        // If there was a previous allocation delete it
        void * pRailNodes = m_pRailNodePointers[dwTrackID];
        if ( pRailNodes != NULL && pRailNodes != m_pDefaultRailNodePointer )
        {
            delete pRailNodes;
        }

        // Set our data to default
        m_pRailNodePointers[dwTrackID]      = m_pDefaultRailNodePointer;
        m_fRailTrackLengths[dwTrackID]      = m_fDefaultRailTrackLengths;
        m_dwNumberOfTrackNodes[dwTrackID]   = m_dwDefaultNumberOfTrackNodes;

        // Delete our train track
        if ( m_pTrainTracks[dwTrackID] != NULL )
        {
            delete m_pTrainTracks[dwTrackID];
        }

        m_pTrainTracks[dwTrackID] = NULL;

        return true;
    }
    return false;
}


bool CTrainTrackManagerSA::Reallocate(CTrainTrackSA* pTrainTrack, std::size_t numNodes)
{
    // Allocate memory for our nodes
    auto nodes = new SRailNode[numNodes];
    
    // loop through nodes
    for (std::size_t i = 0; i < numNodes; ++i)
    {
        // grab our node at index i
        SRailNode* pNode = &nodes[i];

        // Call CTrainNode::CTrainNode()
        DWORD dwConstructor = FUNC_CTrainNode_CTrainNode;
        _asm
        {
            pushad
            mov ecx, pNode
            call dwConstructor
            popad
        }
        
        // Initialise our track with the main line positions
        GetOriginalRailNode ( 0, i, &nodes[i] );
    }

    // Fill gaps with zeroes
    for (auto i = numNodes; i < MAX_TOTAL_TRACKS; ++i)
    {
        m_pRailNodePointers[i] = nullptr;
    }

    // If there was a previous allocation delete it
    auto trackId = pTrainTrack->GetTrackID();

    void * pRailNodes = m_pRailNodePointers[trackId];
    if ( pRailNodes != NULL )
    {
        delete pRailNodes;
    }

    // Set our data
    m_pRailNodePointers[trackId]     = pMemory;
    m_dwNumberOfTrackNodes[trackId]   = uiNodes;

    // Initialise our track
    m_pTrainTracks[trackId]->Initialise ( );

    // Recalculate our track
    m_pTrainTracks[trackId]->Recalculate ( );
    return true;
}

inline float DistanceBetweenPoints2D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;

    return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );
}

inline float DistanceBetweenPoints3D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;
    float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

    return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
}

void * CTrainTrackManagerSA::AllocateDefault ( void )
{
    // Default track positions
    CVector vecDefaultPositions[10];
    vecDefaultPositions[0] = CVector( 2811.3762207031f, 1645.107421875f, 9.8203125f + 2.4973087310791f );
    vecDefaultPositions[1] = CVector( 2811.3762207031f, 1657.9461669922f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[2] = CVector( 2811.3664550781f, 1668.4057617188f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[3] = CVector( 2811.3447265625f, 1675.8885498047f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[4] = CVector( 2811.3317871094f, 1683.9735107422f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[5] = CVector( 2811.3317871094f, 1690.3039550781f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[6] = CVector( 2811.3317871094f, 1696.6267089844f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[7] = CVector( 2811.3317871094f, 1700.4559326172f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[8] = CVector( 2811.3317871094f, 1706.0006103516f, 9.8203125f + 2.4973087310791f  );
    vecDefaultPositions[9] = CVector( 2811.3317871094f, 1711.6954345703f, 9.8203125f + 2.4973087310791f  );

    // Allocate memory for our nodes
    void * pMemory = malloc ( sizeof ( SRailNode ) * 10 );
    // Distance starts at 0
    float fDistance = 0.0f;

    m_fDefaultRailTrackLengths = 10.0f;
    m_dwDefaultNumberOfTrackNodes = 9;


    // loop through nodes
    for ( unsigned int i = 0; i < 9; i++ )
    {
        // grab our node at index i
        SRailNode * pNode = (SRailNode *)pMemory + i;

        // Call CTrainNode::CTrainNode()
        DWORD dwConstructor = FUNC_CTrainNode_CTrainNode;
        _asm
        {
            pushad
            mov ecx, pNode
            call dwConstructor
            popad
        }
        // call CTrainNode::SetLengthFromStart
        DWORD dwFunc = FUNC_CTrainNode_SetLengthFromStart;
        _asm
        {
            pushad
            mov ecx, pNode
            push fDistance
            call dwFunc
            popad
        }
        pNode->sX = (short)vecDefaultPositions[i].fX * 8;
        pNode->sY = (short)vecDefaultPositions[i].fY * 8;
        pNode->sZ = (short)vecDefaultPositions[i].fZ * 8;
        if ( i < 9 )
        {
            // Add to our distance
            fDistance += DistanceBetweenPoints3D(vecDefaultPositions[i], vecDefaultPositions[i+1]);
        }
    }
    m_fDefaultRailTrackLengths = fDistance;
    return pMemory;
}


bool CTrainTrackManagerSA::IsValid ( unsigned char ucTrack )
{
    if ( ucTrack < NUM_RAILTRACKS )
        return true;
    else
        return m_pTrainTracks[ucTrack] != NULL;
}
