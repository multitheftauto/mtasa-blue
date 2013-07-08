/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackSA.cpp
*  PURPOSE:     Train Node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

inline float DistanceBetweenPoints3D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;
    float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

    return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
}

inline float DistanceBetweenPoints2D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;

    return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );
}

CTrainTrackSA::CTrainTrackSA( unsigned char ucTrackID, bool bLinkedLastNode, CTrainTrackManagerSA * pManager )
{
    // set our track ID
    m_ucTrackID = ucTrackID;
    // set our manager
    m_pManager = pManager;

    // Unitialised
    m_bInitialised = false;

    // Linked last nodes
    m_bLinkLastNodes = bLinkedLastNode;

    // Do not ever modify track positions in the constructor as nothing has loaded at this point.

}

SRailNodeSA * CTrainTrackSA::GetRailNode ( unsigned int uiNode )
{
    // Make sure we are within the realm of possibility
    if ( uiNode < m_dwNumberOfNodes )
    {
        // Grab our node and return it
        SRailNodeSA * pNode = m_pRailNodes + uiNode;
        return pNode;
    }
    return NULL;
}

void CTrainTrackSA::Initialise ( )
{
    // Set our rail node pointer
    m_pRailNodes = (SRailNodeSA *)m_pManager->GetRailNodePointer ( m_ucTrackID );

    // Recalculate our number of nodes
    m_dwNumberOfNodes = m_pManager->GetNumberOfRailNodes ( m_ucTrackID );
}

bool CTrainTrackSA::SetRailNodePosition ( unsigned int uiNode, CVector vecPosition )
{
    // we have to be initialised to call this
    if ( !m_bInitialised )
    {
        pGame->GetTrainTrackManager ( )->Initialise ( );
    }
    // Make sure we are in the realm of possibility
    if ( uiNode < m_dwNumberOfNodes )
    {
        // Rail nodes are limited to one within 0.2 units
        for ( unsigned int i = 0; i < m_dwNumberOfNodes - 1; i++ )
        {
            SRailNodeSA * pNode = GetRailNode ( i );
            CVector vecNodePosition = CVector ( pNode->sX / 8.0f, pNode->sY / 8.0f, pNode->sZ / 8.0f );
            if ( DistanceBetweenPoints3D( vecPosition, vecNodePosition ) < 0.2 )
            {
                return false;
            }
        }
        // Get our Rail Node
        SRailNodeSA * pNode = GetRailNode ( uiNode );
        // Positions are timesed by 8 and stored inside a short
        pNode->sX = (short)vecPosition.fX * 8;
        pNode->sY = (short)vecPosition.fY * 8;
        pNode->sZ = (short)vecPosition.fZ * 8;
        // Recalculate the entire track to make sure it is nice and smooth
        Recalculate ( );
        return true;
    }
    return false;
}

void CTrainTrackSA::Recalculate ( )
{
    // we have to be initialised to call this
    if ( !m_bInitialised )
    {
        Initialise ( );
    }
    // grab the current and next rail node
    SRailNodeSA * pNode = GetRailNode ( 0 );
    SRailNodeSA * pNextNode = GetRailNode ( 1 );
    SRailNodeSA * pStartNode = GetRailNode ( 0 );
    // default our distance to 0
    float fDistance = 0.0f;
    short sLastDistance = 0;
    float fLength = 0.0f;
    // two vectors for positions
    CVector vecNode, vecNextNode;
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
        // distance before is the rail distance
        unsigned short sDistBefore = pNode->sRailDistance;
        // call CTrainNode::SetLengthFromStart
        DWORD dwFunc = 0x6F5490;
        _asm
        {
            pushad
            mov ecx, pNode
            push fDistance
            call dwFunc
            popad
        }
        // Add to our distance
        fDistance += DistanceBetweenPoints2D(vecNextNode, vecNode);
        
        // Set our last distance
        sLastDistance = pNode->sRailDistance;
        // Our next node now becomes our current node
        pNode = pNextNode;
        // Advance our next node
        pNextNode++;
    }
    // Calculate distance from last node to the start again
    vecNode.fX = (float)(pStartNode->sX / 8);
    vecNode.fY = (float)(pStartNode->sY / 8);
    vecNode.fZ = (float)(pStartNode->sZ / 8);
    vecNextNode.fX = (float)(pNode->sX / 8);
    vecNextNode.fY = (float)(pNode->sY / 8);
    vecNextNode.fZ = (float)(pNode->sZ / 8);

    DWORD dwFunc = 0x6F5490;
    _asm
    {
        pushad
        mov ecx, pNode
        push fDistance
        call dwFunc
        popad
    }

    fDistance += DistanceBetweenPoints2D(vecNextNode, vecNode);
    SetLength ( fDistance );
}

bool CTrainTrackSA::SetLength ( float fLength )
{
    // we have to be initialised to call this
    if ( !m_bInitialised )
    {
        Initialise ( );
    }
    m_pManager->SetRailLength ( m_ucTrackID, fLength );
    return true;
}

float CTrainTrackSA::GetLength ( void )
{
    // we have to be initialised to call this
    if ( !m_bInitialised )
    {
        Initialise ( );
    }
    float fLength = m_pManager->GetRailLength ( m_ucTrackID );
    return fLength;
}

void CTrainTrackSA::DumpTrainData ( unsigned char ucTrackID )
{

    FILE* pFile = fopen ( SString("%s%u%s", "D:/trains", ucTrackID, ".txt"), "a+" );

    SRailNodeSA * pNode = GetRailNode ( 0 );
    for ( unsigned int i = 0; i < m_dwNumberOfNodes; i++ )
    {
        fprintf ( pFile, "    m_OriginalTrainTrackData [%u][%u] = SRailNodeSA ( %i, %i, %i, %i, %i );\n", ucTrackID, i, pNode->sX, pNode->sY, pNode->sZ, pNode->sRailDistance, pNode->padding );

        fprintf ( pFile, "\n" );
        pNode++;
    }

    pNode = GetRailNode ( m_dwNumberOfNodes - 1 );
    g_pCore->GetConsole()->Printf("%i", pNode->sRailDistance );

    fclose ( pFile );
}

void CTrainTrackSA::Reset ( )
{
    SRailNodeSA * pNode = GetRailNode ( 0 );
    for ( unsigned int i = 0; i < m_dwNumberOfNodes; i++ )
    {
        m_pManager->GetOriginalRailNode ( m_ucTrackID, i, pNode );
        pNode++;
    }
}

bool CTrainTrackSA::GetRailNodePosition ( unsigned int uiNode, CVector& vecPosition )
{
    SRailNodeSA * pNode = GetRailNode( uiNode );
    if ( pNode )
    {
        vecPosition.fX = (float)(pNode->sX / 8);
        vecPosition.fY = (float)(pNode->sY / 8);
        vecPosition.fZ = (float)(pNode->sZ / 8);
        return true;
    }
    return false;
}

bool CTrainTrackSA::SetNumberOfNodes ( unsigned int uiNodes )
{
    m_pManager->Reallocate ( this, uiNodes );
    return true;
}

bool CTrainTrackSA::SetTrackLength ( float fLength )
{ 
    return m_pManager->SetRailLength ( m_ucTrackID, fLength ); 
}

float CTrainTrackSA::GetTrackLength ( void )
{ 
    return m_pManager->GetRailLength ( m_ucTrackID ); 
}

void CTrainTrackSA::SetLastNodesLinked ( bool bLinked )
{
    // Store the updated status
    bool bRecalculate = bLinked != m_bLinkLastNodes;
    // Update
    m_bLinkLastNodes = bLinked;
    if ( bRecalculate )
    {
        // Recalculate.
        Recalculate();
    }
}