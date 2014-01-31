/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainTrack.cpp
*  PURPOSE:     Train Node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTrainTrack::CTrainTrack( unsigned char ucTrackID, CTrainTrackManager * pManager, CElement * pParent, CXMLNode * pNode ) : CElement ( pParent, pNode )
{
    // set our track ID
    m_ucTrackID = ucTrackID;
    // set our manager
    m_pManager = pManager;

    // Unitialised
    m_bInitialised = false;

    // Linked
    m_bLinkLastNodes = false;

    // Do not ever modify track positions in the constructor as nothing has loaded at this point.

    m_iType = CElement::TRAINTRACK;
    SetTypeName ( "train-track" );
}

CTrainTrack::~CTrainTrack ( void )
{
    m_pManager->UnreferenceTrainTrack ( m_ucTrackID );
}

SRailNode * CTrainTrack::GetRailNode ( unsigned int uiNode )
{
    // Make sure we are within the realm of possibility
    if ( m_RailNodes.find ( uiNode ) != m_RailNodes.end() )
    {
        // Grab our node and return it
        SRailNode * pRailNodeSA = m_RailNodes.find ( uiNode )->second;
        return pRailNodeSA;
    }
    return NULL;
}

void CTrainTrack::Initialise ( )
{
    // Recalculate our number of nodes
    m_dwNumberOfNodes = m_pManager->GetNumberOfRailNodes ( m_ucTrackID );
    m_bInitialised = true;
    for ( unsigned int i = 0; i <= m_dwNumberOfNodes; i++ )
    {
        SRailNode * pNode = new SRailNode ( );
        pNode->sX = 1;
        pNode->sY = 1;
        pNode->sZ = 1;
        MapSet ( m_RailNodes, i, pNode );
    }
    // Recalculate the entire track to make sure it is nice and smooth
    Recalculate ( );
}

bool CTrainTrack::SetRailNodePosition ( unsigned int uiNode, CVector vecPosition )
{
    // Make sure we are in the realm of possibility
    if ( uiNode < m_dwNumberOfNodes )
    {
        // Get our Rail Node
        SRailNode * pNode = GetRailNode ( uiNode );
        if ( pNode != NULL )
        {
            // Rail nodes are limited to one within 0.2 units
            for ( unsigned int i = 0; i < m_dwNumberOfNodes - 1; i++ )
            {
                SRailNode * pNode = GetRailNode ( i );
                CVector vecNodePosition = CVector ( (float)pNode->sX / 8, (float)pNode->sY / 8, (float)pNode->sZ / 8 );
                if ( DistanceBetweenPoints3D( vecPosition, vecNodePosition ) < 0.2 )
                {
                    return false;
                }
            }
            // Positions are timesed by 8 and stored inside a short
            pNode->sX = (short)vecPosition.fX * 8;
            pNode->sY = (short)vecPosition.fY * 8;
            pNode->sZ = (short)vecPosition.fZ * 8;
            // Recalculate the entire track to make sure it is nice and smooth
            Recalculate ( );
            return true;
        }
    }
    return false;
}

void CTrainTrack::Recalculate ( )
{
    // grab the current and next rail node
    SRailNode * pNode = GetRailNode ( 0 );
    SRailNode * pNextNode = GetRailNode ( 1 );
    SRailNode * pStartNode = GetRailNode ( 0 );
    // default our distance to 0
    float fDistance = 0.0f;
    float fLength = 0.0f;
    // two vectors for positions
    CVector vecNode, vecNextNode;
    unsigned int uiCount = m_dwNumberOfNodes - 1;
    if ( m_bLinkLastNodes == true )
    {
        uiCount++;
    }
    // loop through all our nodes
    for ( unsigned int i = 0; i < m_dwNumberOfNodes - 1; i++ )
    {
        // fill in our vectors
        vecNode.fX = (float)(pNode->sX / 8);
        vecNode.fY = (float)(pNode->sY / 8);
        vecNode.fZ = (float)(pNode->sZ / 8);
        vecNextNode.fX = (float)(pNextNode->sX / 8);
        vecNextNode.fY = (float)(pNextNode->sY / 8);
        vecNextNode.fZ = (float)(pNextNode->sZ / 8);

        // Add to our distance
        fDistance += DistanceBetweenPoints2D(vecNextNode, vecNode);

        // Our next node now becomes our current node
        pNode = pNextNode;
        // Advance our next node
        pNextNode = GetRailNode ( i + 1 );
    }
    // Calculate distance from last node to the start again
    vecNode.fX = (float)(pStartNode->sX / 8);
    vecNode.fY = (float)(pStartNode->sY / 8);
    vecNode.fZ = (float)(pStartNode->sZ / 8);
    vecNextNode.fX = (float)(pNode->sX / 8);
    vecNextNode.fY = (float)(pNode->sY / 8);
    vecNextNode.fZ = (float)(pNode->sZ / 8);

    fDistance += DistanceBetweenPoints2D(vecNextNode, vecNode);

    SetLength ( fDistance );
}

bool CTrainTrack::SetLength ( float fLength )
{
    m_pManager->SetRailLength ( m_ucTrackID, fLength );
    return true;
}

float CTrainTrack::GetLength ( void )
{
    float fLength = m_pManager->GetRailLength ( m_ucTrackID );
    return fLength;
}

void CTrainTrack::Reset ( )
{
    SRailNode * pNode = GetRailNode ( 0 );
    for ( unsigned int i = 0; i < m_dwNumberOfNodes; i++ )
    {
        m_pManager->GetOriginalRailNode ( m_ucTrackID, i, pNode );
        pNode++;
    }
}

bool CTrainTrack::GetRailNodePosition ( unsigned int uiNode, CVector &vecPosition )
{
    SRailNode * pNode = GetRailNode( uiNode );

    if ( pNode )
    {
        vecPosition.fX = (float)(pNode->sX / 8);
        vecPosition.fY = (float)(pNode->sY / 8);
        vecPosition.fZ = (float)(pNode->sZ / 8);
        return true;
    }
    return false;
}

bool CTrainTrack::SetNumberOfNodes ( unsigned int uiNodes )
{
    SRailNode * pNode = NULL;
    std::map < int, SRailNode * >::iterator iter;
    // loop through all our nodes
    for ( unsigned int i = uiNodes + 1; i < m_dwNumberOfNodes; i++ )
    {
        // Make sure we are within the realm of possibility
        if ( m_RailNodes.find ( i ) != m_RailNodes.end() )
        {
            // Grab our node and return it
            iter = m_RailNodes.find ( i );
            pNode = iter->second;
            delete pNode;
        }
    }
    m_dwNumberOfNodes = uiNodes;
    Recalculate ( );
    return true;
}

bool CTrainTrack::SetTrackLength ( float fLength )
{ 
    return m_pManager->SetRailLength ( m_ucTrackID, fLength ); 
}

float CTrainTrack::GetTrackLength ( void )
{ 
    return m_pManager->GetRailLength ( m_ucTrackID ); 
}