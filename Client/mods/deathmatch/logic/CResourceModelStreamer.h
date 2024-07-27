/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceModelStreamer.h
 *  PURPOSE:     Resource model manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <unordered_map>

class CResourceModelStreamer
{
public:
    CResourceModelStreamer() = default;
    ~CResourceModelStreamer() = default;

    bool RequestModel(uint16_t modelId, bool addRef = false, bool blocking = false);
    bool ReleaseModel(uint16_t modelId, bool removeRef = false);

    void ReleaseAll();
    void FullyReleaseModel(uint16_t modelId);

private:
    std::unordered_map<uint16_t, uint16_t> m_requestedModels;
};
