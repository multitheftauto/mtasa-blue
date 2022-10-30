/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkerSA.h
 *  PURPOSE:     Header file for 3D Marker entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/C3DMarker.h>
#include <CMatrix_Pad.h>

class C3DMarkerSAInterface
{
public:
    CMatrix_Padded m_mat;                    // local space to world space transform // 0
    DWORD          dwPad, dwPad2;            // not sure why we need these, it appears to be this way though (eAi)  // 64/68
    RpClump*       m_pRwObject;              // 72
    DWORD*         m_pMaterial;              // 76

    WORD  m_nType;                  // 80
    bool  m_bIsUsed;                // has this marker been allocated this frame?    // 82
    DWORD m_nIdentifier;            // 84

    DWORD rwColour;                    // 88
    WORD  m_nPulsePeriod;              // 92
    short m_nRotateRate;               // deg per frame (in either direction) // 94
    DWORD m_nStartTime;                // 96
    float m_fPulseFraction;            // 100
    float m_fStdSize;                  // 104
    float m_fSize;                     // 108
    float m_fBrightness;               // 112
    float m_fCameraRange;              // 116

    CVector m_normal;            // Normal of the object point at             // 120
    // the following variables remember the last time we read the heigh of the
    // map. Using this we don't have to do this every frame and we can still have moving markers.
    WORD    m_LastMapReadX, m_LastMapReadY;            // 132 / 134
    float   m_LastMapReadResultZ;                      // 136
    float   m_roofHeight;                              // 140
    CVector m_lastPosition;                            // 144
    DWORD   m_OnScreenTestTime;                        // time last screen check was done // 156
};

class C3DMarkerSA : public C3DMarker
{
private:
    C3DMarkerSAInterface* internalInterface;

public:
    C3DMarkerSA(C3DMarkerSAInterface* markerInterface) { internalInterface = markerInterface; };

    C3DMarkerSAInterface* GetInterface() { return internalInterface; }

    void     GetMatrix(CMatrix* pMatrix);
    void     SetMatrix(CMatrix* pMatrix);
    void     SetPosition(CVector* vecPosition);
    CVector* GetPosition();
    DWORD    GetType();                        // need enum?
    void     SetType(DWORD dwType);            // doesn't work propperly (not virtualed)
    bool     IsActive();
    DWORD    GetIdentifier();
    SharedUtil::SColor   GetColor();
    void     SetColor(const SharedUtil::SColor color);            // actually BGRA
    void     SetPulsePeriod(WORD wPulsePeriod);
    void     SetRotateRate(short RotateRate);
    float    GetSize();
    void     SetSize(float fSize);
    float    GetBrightness();
    void     SetBrightness(float fBrightness);
    void     SetCameraRange(float fCameraRange);
    void     SetPulseFraction(float fPulseFraction);            // doesn't work propperly (not virtualed)
    float    GetPulseFraction();
    void     Disable();
    void     Reset();
    void     SetActive() { this->internalInterface->m_bIsUsed = true; };
    RpClump* GetRwObject() { return this->internalInterface->m_pRwObject; }
};
