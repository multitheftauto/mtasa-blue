/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkerSA.cpp
 *  PURPOSE:     3D Marker entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C3DMarkerSA.h"
#include "CMatrix.h"

void C3DMarkerSA::GetMatrix(CMatrix* pMatrix)
{
    CMatrixSAInterface* mat = &GetInterface()->m_mat;
    MemCpyFast(pMatrix, &mat->GetMatrix(), sizeof(CMatrix));
}

void C3DMarkerSA::SetMatrix(CMatrix* pMatrix)
{
    CMatrixSAInterface* mat = &GetInterface()->m_mat;
    mat->SetMatrix(pMatrix->vRight, pMatrix->vFront, pMatrix->vUp, pMatrix->vPos);
}

void C3DMarkerSA::SetPosition(CVector* vecPosition)
{
    GetInterface()->m_mat.SetTranslateOnly(*vecPosition);
}

CVector* C3DMarkerSA::GetPosition()
{
    return &GetInterface()->m_mat.GetPosition();
}

e3DMarkerType C3DMarkerSA::GetType() const
{
    return static_cast<e3DMarkerType>(GetInterface()->m_nType);
}

void C3DMarkerSA::SetType(e3DMarkerType type)
{
    GetInterface()->m_nType = type;
}

bool C3DMarkerSA::IsActive()
{
    return GetInterface()->m_bIsUsed;
}

DWORD C3DMarkerSA::GetIdentifier()
{
    return GetInterface()->m_nIdentifier;
}

void C3DMarkerSA::SetColor(const SharedUtil::SColor color)
{
    GetInterface()->rwColour = RwColor{color.R, color.G, color.B, color.A};
}

void C3DMarkerSA::SetPulsePeriod(WORD wPulsePeriod)
{
    GetInterface()->m_nPulsePeriod = wPulsePeriod;
}

void C3DMarkerSA::SetRotateRate(short RotateRate)
{
    GetInterface()->m_nRotateRate = RotateRate;
}

float C3DMarkerSA::GetSize()
{
    return GetInterface()->m_fSize;
}

void C3DMarkerSA::SetSize(float fSize)
{
    GetInterface()->m_fSize = fSize;
}

float C3DMarkerSA::GetBrightness()
{
    return GetInterface()->m_fBrightness;
}

void C3DMarkerSA::SetBrightness(float fBrightness)
{
    GetInterface()->m_fBrightness = fBrightness;
}

void C3DMarkerSA::SetCameraRange(float fCameraRange)
{
    GetInterface()->m_fCameraRange = fCameraRange;
}

void C3DMarkerSA::SetPulseFraction(float fPulseFraction)
{
    GetInterface()->m_fPulseFraction = fPulseFraction;
}

float C3DMarkerSA::GetPulseFraction()
{
    return GetInterface()->m_fPulseFraction;
}

void C3DMarkerSA::Disable()
{
    GetInterface()->m_nIdentifier = 0;
}

void C3DMarkerSA::Reset()
{
    internalInterface->m_lastPosition = internalInterface->m_mat.GetPosition();
}
