/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPathFindSA.h
*  PURPOSE:     Header file for game entity path finding class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PATHFIND
#define __CGAMESA_PATHFIND

#include <game/CPathFind.h>

#define FUNC_FindNthNodeClosestToCoors          0x44F8C0
#define FUNC_FindNodePointer                    0x420AC0
#define FUNC_SwitchRoadsOffInArea               0x452C80
#define FUNC_SwitchPedRoadsOffInArea            0x452F00

#define FUNC_CPathNode_GetCoors                 0x420A10

#define CLASS_CPathFind                         0x96F050

#define VAR_PedDensityMultiplier                0x8D2530
#define VAR_m_AllRandomPedsThisType             0x8D2534
#define VAR_MaxNumberOfPedsInUse                0x8D2538
#define VAR_NumberOfPedsInUseInterior           0x8D253C
#define VAR_CarDensityMultiplier                0x8A5B20
#define VAR_MaxNumberOfCarsInUse                0x8A5B24

class CPathFindSA : public CPathFind
{
public:
    CNodeAddress *  FindNthNodeClosestToCoors ( CVector * vecCoors, int iNodeNumber, int iType, CNodeAddress * pNodeAddress, float fDistance );
    CPathNode *     GetPathNode ( CNodeAddress * node );
    CVector *       GetNodePosition ( CPathNode * pNode, CVector * pPosition);
    CVector *       GetNodePosition ( CNodeAddress * pNode, CVector * pPosition );
    void            SwitchRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false );
    void            SwitchPedRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false );

    // Belong elsewhere - cpopulation and ccarctrl
    void            SetPedDensity ( float fPedDensity );
    void            SetVehicleDensity ( float fVehicleDensity );
    void            SetMaxPeds ( int iMaxPeds );
    void            SetMaxPedsInterior ( int iMaxPeds );
    void            SetMaxVehicles ( int iMaxVehicles );
    void            SetAllRandomPedsThisType ( int iType );
};

#endif
