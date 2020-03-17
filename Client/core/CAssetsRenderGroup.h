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

class CAssetsRenderGroup
{
public:
    CAssetsRenderGroup();
    ~CAssetsRenderGroup();

    float GetDrawDistance() const { return m_fDrawDistance; }
    void  SetDrawDistance(float fDrawDistance) { m_fDrawDistance = fDrawDistance; }

private:
    float m_fDrawDistance = 200;
};
