/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendSequenceSA.cpp
 *  PURPOSE:     Animation blend sequence
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendSequenceSA.h"

void CAnimBlendSequenceSA::Initialize()
{
    m_pInterface->m_boneId = -1;
    m_pInterface->sFlags = 0;
    m_pInterface->sNumKeyFrames = 0;
    m_pInterface->pKeyFrames = 0;
}

void CAnimBlendSequenceSA::SetName(const char* szName)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendSequence_SetName;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(dwThis), decltype(szName));
    reinterpret_cast<func_t>(dwFunc)(dwThis, szName);
    // clang-format on
}

void CAnimBlendSequenceSA::SetBoneTag(int32_t i32BoneID)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendSequence_SetBoneTag;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(dwThis), decltype(i32BoneID));
    reinterpret_cast<func_t>(dwFunc)(dwThis, i32BoneID);
    // clang-format on
}

void CAnimBlendSequenceSA::SetKeyFrames(size_t cKeyFrames, bool bRoot, bool bCompressed, void* pKeyFrames)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendSequence_SetKeyFrames;
    // clang-format off
    using func_t = void (__thiscall*)(decltype(dwThis), decltype(cKeyFrames), decltype(bRoot), decltype(bCompressed), decltype(pKeyFrames));
    reinterpret_cast<func_t>(dwFunc)(dwThis, cKeyFrames, bRoot, bCompressed, pKeyFrames);
    // clang-format on
}

void CAnimBlendSequenceSA::CopySequenceProperties(CAnimBlendSequenceSAInterface* pAnimSequenceInterface)
{
    *m_pInterface = *pAnimSequenceInterface;
}

void* CAnimBlendSequenceSA::GetKeyFrame(size_t iFrame, uint32_t u32FrameSizeInBytes)
{
    return (m_pInterface->pKeyFrames + u32FrameSizeInBytes * iFrame);
}
