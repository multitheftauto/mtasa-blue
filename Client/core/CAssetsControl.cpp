/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetsControl.cpp
 *  PURPOSE:     Asset control class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CAssetsControl::CAssetsControl()
{
    ambientColor = D3DCOLOR_ARGB(255, 150, 150, 150);

}

CAssetsControl::~CAssetsControl()
{
}

CAssetsRenderGroup* CAssetsControl::GetRenderGroup(unsigned int idx)
{
    if (!RenderGroupExists(idx))
    {
        m_renderingGroups.insert({idx, new CAssetsRenderGroup()});
    }
    return m_renderingGroups.at(idx);
}

DWORD CAssetsControl::GetAmbientColor()
{
    return ambientColor;
}
