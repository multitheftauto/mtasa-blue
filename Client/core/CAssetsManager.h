/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetsManager.cpp
 *  PURPOSE:     Header file for the assets class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <core/CAssetsManagerInterface.h>

class CAssetsManager : public CAssetsManagerInterface
{
public:
    CAssetsManager();
    ~CAssetsManager();
    DWORD GetAmbientColor();
    bool                RenderGroupExists(unsigned int idx) const { return m_renderingGroups.find(idx) != m_renderingGroups.end(); }
    CAssetInstance* GetRenderGroup(unsigned int idx);

private:
    DWORD ambientColor;            // argb ambient color
    std::unordered_map<unsigned int, CAssetInstance*> m_renderingGroups;
};
