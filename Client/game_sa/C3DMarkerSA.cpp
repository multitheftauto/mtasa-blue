/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkerSA.cpp
 *  PURPOSE:     3D Marker entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C3DMarkerSA.h"

void C3DMarkerSA::GetMatrix(CMatrix* pMatrix)
{
    CMatrix_Padded* mat = &GetInterface()->m_mat;
    MemCpyFast(&pMatrix->vPos, &mat->vPos, sizeof(CVector));
    MemCpyFast(&pMatrix->vFront, &mat->vFront, sizeof(CVector));
    MemCpyFast(&pMatrix->vRight, &mat->vRight, sizeof(CVector));
    MemCpyFast(&pMatrix->vUp, &mat->vUp, sizeof(CVector));
}

void C3DMarkerSA::SetMatrix(CMatrix* pMatrix)
{
    CMatrix_Padded* mat = &GetInterface()->m_mat;
    MemCpyFast(&mat->vPos, &pMatrix->vPos, sizeof(CVector));
    MemCpyFast(&mat->vFront, &pMatrix->vFront, sizeof(CVector));
    MemCpyFast(&mat->vRight, &pMatrix->vRight, sizeof(CVector));
    MemCpyFast(&mat->vUp, &pMatrix->vUp, sizeof(CVector));
}

void C3DMarkerSA::SetPosition(CVector* vecPosition)
{
    this->GetInterface()->m_mat.vPos = *vecPosition;
}

CVector* C3DMarkerSA::GetPosition()
{
    return &this->GetInterface()->m_mat.vPos;
}

DWORD C3DMarkerSA::GetType()
{
    return this->GetInterface()->m_nType;
}

void C3DMarkerSA::SetType(DWORD dwType)
{
    this->GetInterface()->m_nType = (unsigned short)(dwType);
}

bool C3DMarkerSA::IsActive()
{
    return this->GetInterface()->m_bIsUsed;
}

DWORD C3DMarkerSA::GetIdentifier()
{
    return this->GetInterface()->m_nIdentifier;
}

SharedUtil::SColor C3DMarkerSA::GetColor()
{
    // From ABGR
    unsigned long ulABGR = this->GetInterface()->rwColour;
    SharedUtil::SColor        color;
    color.A = (ulABGR >> 24) & 0xff;
    color.B = (ulABGR >> 16) & 0xff;
    color.G = (ulABGR >> 8) & 0xff;
    color.R = ulABGR & 0xff;
    return color;
}

void C3DMarkerSA::SetColor(const SharedUtil::SColor color)
{
    // To ABGR
    this->GetInterface()->rwColour = (color.A << 24) | (color.B << 16) | (color.G << 8) | color.R;
}

void C3DMarkerSA::SetPulsePeriod(WORD wPulsePeriod)
{
    this->GetInterface()->m_nPulsePeriod = wPulsePeriod;
}

void C3DMarkerSA::SetRotateRate(short RotateRate)
{
    this->GetInterface()->m_nRotateRate = RotateRate;
}

float C3DMarkerSA::GetSize()
{
    return this->GetInterface()->m_fSize;
}

void C3DMarkerSA::SetSize(float fSize)
{
    this->GetInterface()->m_fSize = fSize;
}

float C3DMarkerSA::GetBrightness()
{
    return this->GetInterface()->m_fBrightness;
}

void C3DMarkerSA::SetBrightness(float fBrightness)
{
    this->GetInterface()->m_fBrightness = fBrightness;
}

void C3DMarkerSA::SetCameraRange(float fCameraRange)
{
    this->GetInterface()->m_fCameraRange = fCameraRange;
}

void C3DMarkerSA::SetPulseFraction(float fPulseFraction)
{
    this->GetInterface()->m_fPulseFraction = fPulseFraction;
}

float C3DMarkerSA::GetPulseFraction()
{
    return this->GetInterface()->m_fPulseFraction;
}

void C3DMarkerSA::Disable()
{
    this->GetInterface()->m_nIdentifier = 0;
}

void C3DMarkerSA::Reset()
{
    this->internalInterface->m_lastPosition = this->internalInterface->m_mat.vPos;
}
