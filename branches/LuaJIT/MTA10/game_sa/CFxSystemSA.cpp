/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxSystemSA.cpp
*  PURPOSE:     Game effects handling
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CFxSystemSA::PlayAndKill ( )
{
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_FxSystem_c__PlayAndKill;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CFxSystemSA::GetMatrix(CMatrix& matrix)
{
    MemCpyFast (&matrix.vFront,     &m_pInterface->matPosition.at,   sizeof(CVector));
    MemCpyFast (&matrix.vPos,       &m_pInterface->matPosition.pos,     sizeof(CVector));
    MemCpyFast (&matrix.vUp,        &m_pInterface->matPosition.up,      sizeof(CVector));
    MemCpyFast (&matrix.vRight,     &m_pInterface->matPosition.right,   sizeof(CVector));
}

void CFxSystemSA::SetMatrix(const CMatrix &matrix)
{
    MemCpyFast (&m_pInterface->matPosition.at,    &matrix.vFront,   sizeof(CVector));    
    MemCpyFast (&m_pInterface->matPosition.pos,   &matrix.vPos,     sizeof(CVector));    
    MemCpyFast (&m_pInterface->matPosition.up,    &matrix.vUp,      sizeof(CVector));    
    MemCpyFast (&m_pInterface->matPosition.right, &matrix.vRight,   sizeof(CVector));    
}

void CFxSystemSA::GetPosition(CVector &vecPos)
{
    vecPos.fX = m_pInterface->matPosition.pos.x;
    vecPos.fY = m_pInterface->matPosition.pos.y;
    vecPos.fZ = m_pInterface->matPosition.pos.z;
}

void CFxSystemSA::SetPosition(const CVector &vecPos)
{
    m_pInterface->matPosition.pos.x = vecPos.fX;
    m_pInterface->matPosition.pos.y = vecPos.fY;
    m_pInterface->matPosition.pos.z = vecPos.fZ;

    // Set the update flag(s)
    // this is what RwMatrixUpdate (@0x7F18E0) does
    m_pInterface->matPosition.flags &= 0xFFFDFFFC;
}

float CFxSystemSA::GetEffectDensity()
{
    return m_pInterface->sRateMult / 1000.0f;
}

void CFxSystemSA::SetEffectDensity(float fDensity)
{
    m_pInterface->sRateMult = (short)(fDensity * 1000.0f);
}

float CFxSystemSA::GetEffectSpeed()
{
    return m_pInterface->sTimeMult / 1000.0f;
}

void CFxSystemSA::SetEffectSpeed(float fSpeed)
{
    m_pInterface->sTimeMult = (short)(fSpeed * 1000.0f);
}

