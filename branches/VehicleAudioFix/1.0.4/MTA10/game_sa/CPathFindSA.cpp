/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPathFindSA.cpp
*  PURPOSE:     Game entity path finding logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CNodeAddress * CPathFindSA::FindNthNodeClosestToCoors ( CVector * vecCoors, int iNodeNumber, int iType, CNodeAddress * pNodeAddress, float fDistance )
{
    DWORD dwFunc = FUNC_FindNthNodeClosestToCoors;
    
    CNodeAddress node;
    CNodeAddress * nodeptr; 

    _asm
    {
        lea     eax, node
        push    eax
        push    0
        push    iType  // Type : 0 - cars, 1 - boats
        push    iNodeNumber // Nth
        push    0
        push    0
        push    fDistance
        push    0
        mov     eax, vecCoors
        push    [eax+8]
        push    [eax+4]
        push    [eax]
        lea     eax, nodeptr
        push    eax
        mov     ecx, CLASS_CPathFind
        call    dwFunc
    }

    *pNodeAddress = node;
    return pNodeAddress;
}

CPathNode * CPathFindSA::GetPathNode ( CNodeAddress * node )
{
    DWORD dwFunc = FUNC_FindNodePointer;
    if ( node->sRegion >= 0 && node->sIndex >= 0 )
    {
        DWORD dwPathNode = 0;
        _asm
        {
            push    node
            mov     ecx, CLASS_CPathFind
            call    dwFunc
            mov     dwPathNode, eax
        }
        return (CPathNode *)dwPathNode;
    }
    return NULL;
}

CVector * CPathFindSA::GetNodePosition ( CPathNode * pNode, CVector * pPosition)
{
    DWORD dwFunc = FUNC_CPathNode_GetCoors;
    _asm
    {
        push    pPosition
        mov     ecx, pNode
        call    dwFunc
    }
    return pPosition;
}

CVector * CPathFindSA::GetNodePosition ( CNodeAddress * pNode, CVector * pPosition )
{
    CPathNode * pPathNode = GetPathNode ( pNode );
    return GetNodePosition ( pPathNode, pPosition );
}

void CPathFindSA::SwitchRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable )
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    1
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SwitchPedRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable )
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchPedRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SetPedDensity ( float fPedDensity )
{
    MemPut < float > ( VAR_PedDensityMultiplier, fPedDensity );  //     *(float *)VAR_PedDensityMultiplier = fPedDensity;
}

void CPathFindSA::SetVehicleDensity ( float fVehicleDensity )
{
    MemPut < float > ( VAR_CarDensityMultiplier, fVehicleDensity );  //     *(float *)VAR_CarDensityMultiplier = fVehicleDensity;
}

void CPathFindSA::SetMaxPeds ( int iMaxPeds )
{
    MemPut < int > ( VAR_MaxNumberOfPedsInUse, iMaxPeds );  //     *(int *)VAR_MaxNumberOfPedsInUse = iMaxPeds;
}

void CPathFindSA::SetMaxPedsInterior ( int iMaxPeds )
{
     MemPut < int > ( VAR_NumberOfPedsInUseInterior, iMaxPeds );  //      *(int *)VAR_NumberOfPedsInUseInterior = iMaxPeds;
}

void CPathFindSA::SetMaxVehicles ( int iMaxVehicles )
{
    MemPut < int > ( VAR_MaxNumberOfCarsInUse, iMaxVehicles );  //     *(int *)VAR_MaxNumberOfCarsInUse = iMaxVehicles;
}

void CPathFindSA::SetAllRandomPedsThisType ( int iType )
{
    MemPut < int > ( VAR_m_AllRandomPedsThisType, iType );  //     *(int *)VAR_m_AllRandomPedsThisType = iType;
}
