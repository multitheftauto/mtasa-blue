/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetsControl.cpp
 *  PURPOSE:     Header file for the assets class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <core/CAssetControlInterface.h>

class CAssetsControl : public CAssetControlInterface
{
public:
    CAssetsControl();
    ~CAssetsControl();
    DWORD GetAmbientColor();
    bool                RenderGroupExists(unsigned int idx) const { return m_renderingGroups.find(idx) != m_renderingGroups.end(); }
    CAssetsRenderGroup* GetRenderGroup(unsigned int idx);

private:
    DWORD ambientColor;            // argb ambient color
    std::unordered_map<unsigned int, CAssetsRenderGroup*> m_renderingGroups;
};
