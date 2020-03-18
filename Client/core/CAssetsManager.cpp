/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetsManager.cpp
 *  PURPOSE:     Asset control class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CAssetsManager::CAssetsManager()
{
    ambientColor = D3DCOLOR_ARGB(255, 150, 150, 150);

}

CAssetsManager::~CAssetsManager()
{
}

CAssetInstance* CAssetsManager::GetRenderGroup(unsigned int idx)
{
    if (!RenderGroupExists(idx))
    {
        m_renderingGroups.insert({idx, new CAssetInstance()});
    }
    return m_renderingGroups.at(idx);
}

DWORD CAssetsManager::GetAmbientColor()
{
    return ambientColor;
}
