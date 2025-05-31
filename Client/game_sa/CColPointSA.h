/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColPointSA.h
 *  PURPOSE:     Header file for collision point class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CColPoint.h>
#include <CVector.h>

class CColPointSAInterface
{
public:
    CVector      Position;                  // 0
    float        fUnknown1;                 // 12
    CVector      Normal;                    // 16
    float        fUnknown2;                 // 28
    EColSurface  ucSurfaceTypeA;            // 32
    uint8        ucPieceTypeA;              // 33
    CColLighting lightingA;                 // 34
    EColSurface  ucSurfaceTypeB;            // 35
    uint8        ucPieceTypeB;              // 36
    CColLighting lightingB;                 // 37
    uint8        pad1;                      // 38
    uint8        pad2;                      // 39
    float        fDepth;                    // 40
};
static_assert(sizeof(CColPointSAInterface) == 0x2C, "Invalid size for CColPointSAInterface");

class CColPointSA : public CColPoint
{
private:
    CColPointSAInterface* m_pInternalInterface;

public:
    CColPointSA();
    CColPointSA(CColPointSAInterface* pInterface) { m_pInternalInterface = pInterface; };

    virtual CColPointSAInterface* GetInterface() { return m_pInternalInterface; }
    virtual const CVector&        GetPosition() { return GetInterface()->Position; }
    virtual void                  SetPosition(const CVector& vecPosition) { GetInterface()->Position = vecPosition; }

    virtual const CVector& GetNormal() { return GetInterface()->Normal; }
    virtual void           SetNormal(const CVector& vecNormal) { GetInterface()->Normal = vecNormal; }

    virtual EColSurface GetSurfaceTypeA() { return GetInterface()->ucSurfaceTypeA; }
    virtual EColSurface GetSurfaceTypeB() { return GetInterface()->ucSurfaceTypeB; }

    virtual void SetSurfaceTypeA(EColSurface surfaceType) { GetInterface()->ucSurfaceTypeA = surfaceType; }
    virtual void SetSurfaceTypeB(EColSurface surfaceType) { GetInterface()->ucSurfaceTypeB = surfaceType; }

    virtual BYTE GetPieceTypeA() { return GetInterface()->ucPieceTypeA; }
    virtual BYTE GetPieceTypeB() { return GetInterface()->ucPieceTypeB; }

    virtual void SetPieceTypeA(BYTE ucPieceType) { GetInterface()->ucPieceTypeA = ucPieceType; }
    virtual void SetPieceTypeB(BYTE ucPieceType) { GetInterface()->ucPieceTypeB = ucPieceType; }

    virtual CColLighting GetLightingA() { return GetInterface()->lightingA; }
    virtual CColLighting GetLightingB() { return GetInterface()->lightingB; }

    virtual void SetLightingA(CColLighting lighting) { GetInterface()->lightingA = lighting; }
    virtual void SetLightingB(CColLighting lighting) { GetInterface()->lightingB = lighting; }

    virtual float GetDepth() { return GetInterface()->fDepth; }
    virtual void  SetDepth(float fDepth) { GetInterface()->fDepth = fDepth; }

    virtual void Destroy()
    {
        if (m_pInternalInterface)
            delete m_pInternalInterface;
        delete this;
    }
    virtual float GetLightingForTimeOfDay();
};
