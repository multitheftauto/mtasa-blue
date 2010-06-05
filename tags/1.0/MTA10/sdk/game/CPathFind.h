/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPathFind.h
*  PURPOSE:		Path finder interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PATHFIND
#define __CGAME_PATHFIND

#include <CVector.h>

// This class can be used to retrieve a CPathNode if required, using GetPathNode(..)
class CNodeAddress
{
public:
	short sRegion;
	short sIndex;

	CNodeAddress::CNodeAddress()
	{
		sRegion = -1;
	}
};

class CPathNode;

class CPathFind
{
public:
	virtual CNodeAddress *  FindNthNodeClosestToCoors ( CVector * vecCoors, int iNodeNumber, int iType, CNodeAddress * pNodeAddress, float fDistance )=0;
	virtual CPathNode *     GetPathNode ( CNodeAddress * node )=0;
	virtual CVector *       GetNodePosition ( CPathNode * pNode, CVector * pPosition)=0;
	virtual CVector *       GetNodePosition ( CNodeAddress * pNode, CVector * pPosition )=0;
    virtual void            SwitchRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false )=0;
    virtual void            SwitchPedRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false )=0;
    // belong in CCarCtrl or CPopulation
    virtual void            SetPedDensity ( float fPedDensity )=0;
    virtual void            SetVehicleDensity ( float fVehicleDensity )=0;
    virtual void            SetMaxPeds ( int iMaxPeds )=0;
    virtual void            SetMaxPedsInterior ( int iMaxPeds )=0;
    virtual void            SetMaxVehicles ( int iMaxVehicles )=0;
    virtual void            SetAllRandomPedsThisType ( int iType )=0;
};

#endif