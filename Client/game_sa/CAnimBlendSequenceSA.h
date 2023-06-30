/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAnimBlendSequenceSA.cpp
 *  PURPOSE:     Header file for animation blend sequence class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAnimBlendSequence.h>

class CAnimBlendSequenceSAInterface
{
public:
    union
    {
        uint16_t m_boneId;            // m_boneId is set if ( sFlags & 0x10u ) is true
        uint32_t m_hash;              // otherwise m_hash is set
    };
    ushort sFlags;
    ushort sNumKeyFrames;
    BYTE*  pKeyFrames;
};

class CAnimBlendSequenceSA : public CAnimBlendSequence
{
public:
    CAnimBlendSequenceSA(CAnimBlendSequenceSAInterface* pInterface) { m_pInterface = pInterface; }
    void                           Initialize();
    void                           SetName(const char* szName);
    void                           SetBoneTag(int32_t i32BoneID);
    void                           SetKeyFrames(size_t cKeyFrames, bool bRoot, bool bCompressed, void* pKeyFrames);
    void*                          GetKeyFrame(size_t iFrame, uint32_t u32FrameSizeInBytes);
    uint32_t                       GetHash()                   { return m_pInterface->m_hash; }
    uint16_t                       GetBoneTag()                { return m_pInterface->m_boneId; }
    BYTE*                          GetKeyFrames()              { return m_pInterface->pKeyFrames; }
    unsigned short                 GetKeyFramesCount()         { return m_pInterface->sNumKeyFrames; }
    bool                           IsBigChunkForAllSequences() { return ((m_pInterface->sFlags >> 3) & 1); }
    void                           CopySequenceProperties(CAnimBlendSequenceSAInterface* pAnimSequenceInterface);
    CAnimBlendSequenceSAInterface* GetInterface() { return m_pInterface; }

protected:
    CAnimBlendSequenceSAInterface* m_pInterface;
};
