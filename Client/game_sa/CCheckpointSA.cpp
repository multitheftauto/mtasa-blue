/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCheckpointSA.cpp
 *  PURPOSE:     Checkpoint entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C3DMarkerSA.h"
#include "C3DMarkersSA.h"
#include "CCheckpointSA.h"

void CCheckpointSA::SetPosition(CVector* vecPosition)
{
    MemCpyFast(&GetInterface()->m_pos, vecPosition, sizeof(CVector));
}

CVector* CCheckpointSA::GetPosition()
{
    return &GetInterface()->m_pos;
}

void CCheckpointSA::SetPointDirection(CVector* vecPointDirection)
{
    MemCpyFast(&GetInterface()->m_pointDir, vecPointDirection, sizeof(CVector));
}

CVector* CCheckpointSA::GetPointDirection()
{
    return &GetInterface()->m_pointDir;
}

DWORD CCheckpointSA::GetType()
{
    return GetInterface()->m_nType;
}

void CCheckpointSA::SetType(WORD wType)
{
    GetInterface()->m_nType = wType;
}

bool CCheckpointSA::IsActive()
{
    return GetInterface()->m_bIsUsed;
}

void CCheckpointSA::Activate()
{
    GetInterface()->m_bIsUsed = true;
}

DWORD CCheckpointSA::GetIdentifier()
{
    return GetInterface()->m_nIdentifier;
}

void CCheckpointSA::SetIdentifier(DWORD dwIdentifier)
{
    GetInterface()->m_nIdentifier = dwIdentifier;
}

SharedUtil::SColor CCheckpointSA::GetColor()
{
    // From ABGR
    unsigned long ulABGR = GetInterface()->rwColour;
    SharedUtil::SColor        color;
    color.A = (ulABGR >> 24) & 0xff;
    color.B = (ulABGR >> 16) & 0xff;
    color.G = (ulABGR >> 8) & 0xff;
    color.R = ulABGR & 0xff;
    return color;
}

void CCheckpointSA::SetColor(const SharedUtil::SColor color)
{
    // To ABGR
    GetInterface()->rwColour = (color.A << 24) | (color.B << 16) | (color.G << 8) | color.R;
}

void CCheckpointSA::SetPulsePeriod(WORD wPulsePeriod)
{
    GetInterface()->m_nPulsePeriod = wPulsePeriod;
}

void CCheckpointSA::SetRotateRate(short RotateRate)
{
    GetInterface()->m_nRotateRate = RotateRate;

    if (RotateRate == 0)
    {
        GetInterface()->m_rotFlag = true;
    }
    else
    {
        GetInterface()->m_rotFlag = false;
    }
}

float CCheckpointSA::GetSize()
{
    return GetInterface()->m_fSize;
}

void CCheckpointSA::SetSize(float fSize)
{
    GetInterface()->m_fSize = fSize;
}

void CCheckpointSA::SetCameraRange(float fCameraRange)
{
    GetInterface()->m_fCameraRange = fCameraRange;
}

void CCheckpointSA::SetPulseFraction(float fPulseFraction)
{
    GetInterface()->m_fPulseFraction = fPulseFraction;
}

float CCheckpointSA::GetPulseFraction()
{
    return GetInterface()->m_fPulseFraction;
}

void CCheckpointSA::Remove()
{
    GetInterface()->m_bIsUsed = false;
    GetInterface()->m_nType = 257;
    GetInterface()->rwColour = 0;
}
