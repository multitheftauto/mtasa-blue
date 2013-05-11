/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkerSA.h
*  PURPOSE:     Header file for 3D Marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_3DMARKER
#define __CGAMESA_3DMARKER

#include <game/C3DMarker.h>
#include <CMatrix_Pad.h>
#include "Common.h"

#define FUNC_DeleteMarkerObject         0x722390

class C3DMarkerSAInterface
{
public:
    CMatrix_Padded  m_mat;  // local space to world space transform // 0
    DWORD           dwPad,dwPad2;   // not sure why we need these, it appears to be this way though (eAi)  // 64/68
    RpClump         *m_pRwObject; // 72
    DWORD           *m_pMaterial; // 76
    
    WORD            m_nType; // 80
    bool            m_bIsUsed;  // has this marker been allocated this frame?    // 82
    DWORD           m_nIdentifier; // 84

    DWORD           rwColour; // 88 
    WORD            m_nPulsePeriod; // 92
    short           m_nRotateRate;  // deg per frame (in either direction) // 94
    DWORD           m_nStartTime; // 96
    FLOAT           m_fPulseFraction;  // 100
    FLOAT           m_fStdSize; // 104
    FLOAT           m_fSize; // 108
    FLOAT           m_fBrightness; // 112
    FLOAT           m_fCameraRange; // 116

    CVector     m_normal;           // Normal of the object point at             // 120
    // the following variables remember the last time we read the heigh of the
    // map. Using this we don't have to do this every frame and we can still have moving markers.
    WORD            m_LastMapReadX, m_LastMapReadY; // 132 / 134
    FLOAT           m_LastMapReadResultZ; // 136
    FLOAT           m_roofHeight; // 140
    CVector         m_lastPosition; // 144
    DWORD           m_OnScreenTestTime;     // time last screen check was done // 156
};

class C3DMarkerSA : public C3DMarker
{
private:
    C3DMarkerSAInterface        * internalInterface;
public:
    // constructor
    C3DMarkerSA(C3DMarkerSAInterface * markerInterface) { internalInterface = markerInterface; };

    C3DMarkerSAInterface    * GetInterface() { return internalInterface; }

    void            GetMatrix       ( CMatrix * pMatrix );
    void            SetMatrix       ( CMatrix * pMatrix );
    void            SetPosition(CVector * vecPosition);
    CVector         * GetPosition();
    DWORD           GetType(); // need enum?
    void            SetType(DWORD dwType); // doesn't work propperly (not virtualed)
    BOOL            IsActive();
    DWORD           GetIdentifier();
    SColor          GetColor();
    void            SetColor(const SColor color); // actually BGRA
    void            SetPulsePeriod(WORD wPulsePeriod);
    void            SetRotateRate(short RotateRate);
    FLOAT           GetSize();
    void            SetSize(FLOAT fSize);
    FLOAT           GetBrightness();
    void            SetBrightness(FLOAT fBrightness);
    void            SetCameraRange(FLOAT fCameraRange);
    void            SetPulseFraction(FLOAT fPulseFraction); // doesn't work propperly (not virtualed)
    FLOAT           GetPulseFraction();
    void            Disable();
    void            DeleteMarkerObject();
    void            Reset();
    void            SetActive() { this->internalInterface->m_bIsUsed = true;};
    RpClump *       GetRwObject() { return this->internalInterface->m_pRwObject; }
};

#endif
