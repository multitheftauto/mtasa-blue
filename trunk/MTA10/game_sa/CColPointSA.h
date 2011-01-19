/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColPointSA.h
*  PURPOSE:     Header file for collision point class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLPOINT
#define __CGAMESA_COLPOINT

#include <game/CColPoint.h>
#include <CVector.h>

class CColPointSAInterface
{
public:
    CVector Position;
    FLOAT fUnknown1;
    CVector Normal;
    FLOAT fUnknown2;
    BYTE bSurfaceTypeA;
    BYTE bPieceTypeA;
    BYTE bLightingA;
    BYTE bSurfaceTypeB;
    BYTE bPieceTypeB;
    BYTE bLightingB;
    FLOAT fDepth;
/*
    +0 Position (12 Bytes  Vector)
    +12?
    +16 Normal (12 Bytes  Vector)
    +32 SurfaceTypeA
    +33 PieceTypeA
    +34 LightingA (assumed)
    +35 SurfaceTypeB 
    +36 PieceTypeB
    +37 LightingB 
    +40 Depth (float)*/
};

class CColPointSA : public CColPoint
{
private:
    CColPointSAInterface * internalInterface;
public:
    CColPointSAInterface * GetInterface() { return this->internalInterface; }
    CColPointSA();
    CColPointSA(CColPointSAInterface * pInterface ) { this->internalInterface = pInterface; };

    CVector * GetPosition()  { return &this->GetInterface()->Position; };
    VOID SetPosition(CVector * vecPosition)  { MemCpy8 (&this->GetInterface()->Position, vecPosition, sizeof(CVector)); };

    CVector * GetNormal() { return &this->GetInterface()->Normal; };
    VOID SetNormal(CVector * vecNormal) { MemCpy8 (&this->GetInterface()->Normal, vecNormal, sizeof(CVector)); };

    BYTE GetSurfaceTypeA() { return this->GetInterface()->bSurfaceTypeA; };
    BYTE GetSurfaceTypeB() { return this->GetInterface()->bSurfaceTypeB; };

    VOID SetSurfaceTypeA(BYTE bSurfaceType) { this->GetInterface()->bSurfaceTypeA = bSurfaceType; };
    VOID SetSurfaceTypeB(BYTE bSurfaceType) { this->GetInterface()->bSurfaceTypeB = bSurfaceType; };

    BYTE GetPieceTypeA() { return this->GetInterface()->bPieceTypeA; };
    BYTE GetPieceTypeB() { return this->GetInterface()->bPieceTypeB; };

    VOID SetPieceTypeA(BYTE bPieceType) { this->GetInterface()->bPieceTypeA = bPieceType; };
    VOID SetPieceTypeB(BYTE bPieceType) { this->GetInterface()->bPieceTypeB = bPieceType; };

    BYTE GetLightingA() { return this->GetInterface()->bLightingA; };
    BYTE GetLightingB() { return this->GetInterface()->bLightingB;};

    VOID SetLightingA(BYTE bLighting) { this->GetInterface()->bLightingA = bLighting; };
    VOID SetLightingB(BYTE bLighting) { this->GetInterface()->bLightingB = bLighting; };

    FLOAT GetDepth() { return this->GetInterface()->fDepth; };
    VOID SetDepth(FLOAT fDepth) { this->GetInterface()->fDepth = fDepth; };

    VOID Destroy() { if ( this->internalInterface ) delete this->internalInterface; delete this; }
};

#endif
