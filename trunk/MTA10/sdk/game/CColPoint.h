/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CColPoint.h
*  PURPOSE:     Collision point interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CCOLPOINT
#define __CGAME_CCOLPOINT

#include <CVector.h>

// ChrML: Dunno where to put this, so I put it here.. but I guess this should be in some
//        kind of global section..

class CColPoint
{
public:
    virtual                 ~CColPoint ( void ) {};

    virtual class CColPointSAInterface * GetInterface() = 0;
    virtual CVector * GetPosition()=0;
    virtual VOID SetPosition(CVector * vecPosition)=0;

    virtual CVector * GetNormal()=0;
    virtual VOID SetNormal(CVector * vecNormal)=0;

    virtual BYTE GetSurfaceTypeA()=0;
    virtual BYTE GetSurfaceTypeB()=0;

    virtual VOID SetSurfaceTypeA(BYTE bSurfaceType)=0;
    virtual VOID SetSurfaceTypeB(BYTE bSurfaceType)=0;

    virtual BYTE GetPieceTypeA()=0;
    virtual BYTE GetPieceTypeB()=0;

    virtual VOID SetPieceTypeA(BYTE bPieceType)=0;
    virtual VOID SetPieceTypeB(BYTE bPieceType)=0;

    virtual BYTE GetLightingA()=0;
    virtual BYTE GetLightingB()=0;

    virtual VOID SetLightingA(BYTE bLighting)=0;
    virtual VOID SetLightingB(BYTE bLighting)=0;

    virtual FLOAT GetDepth()=0;
    virtual VOID SetDepth(FLOAT fDepth)=0;

    virtual VOID Destroy()=0;
    virtual float GetLightingForTimeOfDay ( void )=0;
};

#endif
