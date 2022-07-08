/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPathFindSA.cpp
 *  PURPOSE:     Game entity path finding logic
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CNodeAddress* CPathFindSA::FindNthNodeClosestToCoors(CVector* vecCoors, int iNodeNumber, int iType, CNodeAddress* pNodeAddress, float fDistance)
{
    return nullptr;
}

CPathNode* CPathFindSA::GetPathNode(CNodeAddress* node)
{
    if (node->sRegion >= 0 && node->sIndex >= 0)
        return ((CPathNode * (__thiscall*)(int, CNodeAddress*)) FUNC_FindNodePointer)(CLASS_CPathFind, node);

    return nullptr;
}

CVector* CPathFindSA::GetNodePosition(CPathNode* pNode, CVector* pPosition)
{
    return ((CVector * (__thiscall*)(CPathNode*, CVector*)) FUNC_CPathNode_GetCoors)(pNode, pPosition);
}

CVector* CPathFindSA::GetNodePosition(CNodeAddress* pNode, CVector* pPosition)
{
    CPathNode* pPathNode = GetPathNode(pNode);
    return GetNodePosition(pPathNode, pPosition);
}

void CPathFindSA::SwitchRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable)
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    ((void(__thiscall*)(int, float, float, float, float, float, float, bool, bool, bool))FUNC_SwitchRoadsOffInArea)(CLASS_CPathFind, fX1, fX2, fY1, fY2, fZ1,
                                                                                                                    fZ2, !bEnable, true, false);
}

void CPathFindSA::SwitchPedRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable)
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    ((void(__thiscall*)(int, float, float, float, float, float, float, bool, bool))FUNC_SwitchPedRoadsOffInArea)(CLASS_CPathFind, fX1, fX2, fY1, fY2, fZ1, fZ2,
                                                                                                                 !bEnable, false);
}

void CPathFindSA::SetPedDensity(float fPedDensity)
{
    MemPutFast<float>(VAR_PedDensityMultiplier, fPedDensity);
}

void CPathFindSA::SetVehicleDensity(float fVehicleDensity)
{
    MemPutFast<float>(VAR_CarDensityMultiplier, fVehicleDensity);
}

void CPathFindSA::SetMaxPeds(int iMaxPeds)
{
    MemPutFast<int>(VAR_MaxNumberOfPedsInUse, iMaxPeds);
}

void CPathFindSA::SetMaxPedsInterior(int iMaxPeds)
{
    MemPutFast<int>(VAR_NumberOfPedsInUseInterior, iMaxPeds);
}

void CPathFindSA::SetMaxVehicles(int iMaxVehicles)
{
    MemPutFast<int>(VAR_MaxNumberOfCarsInUse, iMaxVehicles);
}

void CPathFindSA::SetAllRandomPedsThisType(int iType)
{
    MemPutFast<int>(VAR_m_AllRandomPedsThisType, iType);
}
