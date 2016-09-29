/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainTrackManager.cpp
*  PURPOSE:     Train Node Manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include <TrackNodes.h>

CTrainTrackManager::CTrainTrackManager ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so initialise them all
        m_pTrainTracks[i] = new CTrainTrack ( i, this, NULL, NULL );
    }
    m_fRailTrackLengths[0]      = 54278;
    m_dwNumberOfTrackNodes[0]   = 926;

    m_fRailTrackLengths[1]      = 15321;
    m_dwNumberOfTrackNodes[1]   = 67;

    m_fRailTrackLengths[2]      = 1945;
    m_dwNumberOfTrackNodes[2]   = 300;

    m_fRailTrackLengths[3]      = 26535;
    m_dwNumberOfTrackNodes[3]   = 656;

    m_bInitialised = false;
    Initialise ( );
}

CTrainTrackManager::~CTrainTrackManager ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so delete them all
        delete m_pTrainTracks[i];
        m_pTrainTracks[i] = NULL;
    }
}

CTrainTrack * CTrainTrackManager::GetTrainTrack ( unsigned char ucTrack )
{
    if ( ucTrack <= m_dwNumberOfTracks )
    {
        return m_pTrainTracks[ucTrack];
    }
    return nullptr;
}

void CTrainTrackManager::Initialise ( )
{
    if ( !m_bInitialised )
    {
        m_bInitialised = true;
        for ( int i = 0; i < MAX_TOTAL_TRACKS;i++ )
        {
            if ( i < NUM_RAILTRACKS )
            {
                m_pTrainTracks[i]->Initialise ( );
            }
            else
            {
                m_pTrainTracks[i] = NULL;
                UnreferenceTrainTrack ( i );
            }
        }

        m_dwNumberOfTracks = NUM_RAILTRACKS;

    }
}

bool CTrainTrackManager::GetOriginalRailNode ( unsigned char ucTrack, unsigned short usNode, SRailNode * pRailNode )
{
    if ( ucTrack < NUM_RAILTRACKS )
    {
        if ( m_OriginalTrainTrackData[ucTrack].find ( usNode ) != m_OriginalTrainTrackData[ucTrack].end ( ) )
        {
            SRailNode * pOriginalRailNode = &m_OriginalTrainTrackData[ucTrack][usNode];
            pRailNode->sX = pOriginalRailNode->sX;
            pRailNode->sY = pOriginalRailNode->sY;
            pRailNode->sZ = pOriginalRailNode->sZ;
            pRailNode->fRailDistance = pOriginalRailNode->fRailDistance;
            return true;
        }
    }
    return false;
}

void CTrainTrackManager::ResetTracks ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so reset them all
        m_pTrainTracks[i]->Reset ( );
    }

    m_dwNumberOfTracks = MAX_TOTAL_TRACKS;
}

CTrainTrack * CTrainTrackManager::CreateTrainTrack ( const std::vector<CVector>& vecNodeList, CElement * pParent, CXMLNode * pNode )
{
    if ( m_dwNumberOfTracks < MAX_TOTAL_TRACKS )
    {
        CTrainTrack * pTrainTrack = new CTrainTrack ( m_dwNumberOfTracks, this, pParent, pNode );
        m_pTrainTracks[m_dwNumberOfTracks] = pTrainTrack;

        // Reallocate and initialise
        Reallocate ( pTrainTrack, vecNodeList.size() );

        pTrainTrack->Initialise();
        
        // Set our number of tracks and update a comparison instruction
        m_dwNumberOfTracks++;

        auto it = vecNodeList.begin ();
        uint uiNode = 0;
        for ( it; it != vecNodeList.end (); it++ ) {
            pTrainTrack->SetRailNodePosition ( uiNode, *it );
            uiNode++;
        }

        return pTrainTrack;
    }
    return NULL;
}

bool CTrainTrackManager::UnreferenceTrainTrack ( DWORD dwTrackID )
{
    if ( dwTrackID < MAX_TOTAL_TRACKS )
    {
        // Set our data to default
        m_fRailTrackLengths[dwTrackID]      = 0;
        m_dwNumberOfTrackNodes[dwTrackID]   = 1;

        // Unreference our train track
        m_pTrainTracks[dwTrackID] = NULL;

        return true;
    }
    return false;
}


bool CTrainTrackManager::Reallocate ( CTrainTrack * pTrainTrack, unsigned int uiNodes )
{
    // Get our track ID
    DWORD dwTrackID = pTrainTrack->GetTrackID ( );

    // Set our data
    m_dwNumberOfTrackNodes[dwTrackID]   = uiNodes;

    CVector vecDefaultPositions[2];
    vecDefaultPositions[0] = CVector( 2811.3762207031f, 1645.107421875f, 9.8203125f + 2.4973087310791f );
    vecDefaultPositions[1] = CVector( 2811.3762207031f, 1657.9461669922f, 9.8203125f + 2.4973087310791f  );
    CVector vecDistance = (vecDefaultPositions[0] - vecDefaultPositions[1]) / (float)uiNodes;
    for ( unsigned int i = 0; i < uiNodes; i++ )
        m_pTrainTracks[dwTrackID]->SetRailNodePosition ( i, vecDefaultPositions[0] + (vecDistance * (float)i) );

    // Initialise our track
    m_pTrainTracks[dwTrackID]->Initialise ( );

    m_fRailTrackLengths[dwTrackID]      = m_pTrainTracks[dwTrackID]->GetTrackLength ( );
    return true;
}
