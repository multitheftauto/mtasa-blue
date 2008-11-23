/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlaneSA.h
*  PURPOSE:     Header file for plane vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PLANE
#define __CGAMESA_PLANE

#include <game/CPlane.h>
#include "CAutomobileSA.h"

class CPlaneSAInterface : public CAutomobileSAInterface
{
	// + 2508 = undercarrige possition (float - 1.0 = up, 0.0 = down)
	// fill this
};

class CPlaneSA : public virtual CPlane, public virtual CAutomobileSA
{
private:
//	CBoatSAInterface		* internalInterface;
public:
	CPlaneSA( CPlaneSAInterface * plane );
	CPlaneSA( eVehicleTypes dwModelID );
};

#endif