/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendSequenceSA.cpp
 *  PURPOSE:     Animation blend sequence
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CAnimBlendSequenceSA::Initialize()
{
    m_pInterface->m_boneId = -1;
    m_pInterface->sFlags = 0;
    m_pInterface->sNumKeyFrames = 0;
    m_pInterface->pKeyFrames = 0;
}

void CAnimBlendSequenceSA::SetName(const char* szName)
{
    // CAnimBlendSequence::SetName
    ((void(__thiscall*)(CAnimBlendSequenceSAInterface*))FUNC_CAnimBlendSequence_SetName)(m_pInterface);
}

void CAnimBlendSequenceSA::SetBoneTag(int32_t i32BoneID)
{
    // CAnimBlendSequence::SetBoneTag
    ((void(__thiscall*)(CAnimBlendSequenceSAInterface*, int))FUNC_CAnimBlendSequence_SetBoneTag)(m_pInterface, i32BoneID);
}

void CAnimBlendSequenceSA::SetKeyFrames(size_t cKeyFrames, bool bRoot, bool bCompressed, void* pKeyFrames)
{
    // CAnimBlendSequence::SetKeyFrames
    ((void(__thiscall*)(CAnimBlendSequenceSAInterface*, size_t, bool, bool, void*))FUNC_CAnimBlendSequence_SetKeyFrames)(m_pInterface, cKeyFrames, bRoot,
                                                                                                                         bCompressed, pKeyFrames);
}

void CAnimBlendSequenceSA::CopySequenceProperties(CAnimBlendSequenceSAInterface* pAnimSequenceInterface)
{
    *m_pInterface = *pAnimSequenceInterface;
}

void* CAnimBlendSequenceSA::GetKeyFrame(size_t iFrame, uint32_t u32FrameSizeInBytes)
{
    return (m_pInterface->pKeyFrames + u32FrameSizeInBytes * iFrame);
}
