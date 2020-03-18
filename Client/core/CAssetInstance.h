/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetInstance.cpp
 *  PURPOSE:     Header file for the asset instance class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CAssetInstance
{
public:
    CAssetInstance();
    ~CAssetInstance();

    float GetEffectiveDrawDistance() const { return m_fDrawDistance; } // combined all draw distance properties
    float GetDrawDistance() const { return m_fDrawDistance; }
    void  SetDrawDistance(float fDrawDistance) { m_fDrawDistance = fDrawDistance; }

private:
    CMatrix m_matrix;
    float m_fDrawDistance = 200;
};
