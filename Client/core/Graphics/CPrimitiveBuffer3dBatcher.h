/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitiveBuffer3DBatcher.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClientPrimitiveBufferInterface;

class CPrimitiveBuffer3DBatcher
{
    friend CGraphics;

public:
    CPrimitiveBuffer3DBatcher(CGraphics* pGraphics, bool bPreGUI);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    void AddPrimitiveBuffer(CClientPrimitiveBufferInterface* pPrimitiveBuffer, PrimitiveBufferSettings& bufferSettings);
    bool HasItems() const { return !m_primitiveBufferMap.empty(); }
    void ClearQueue();

protected:
    bool m_bPreGUI;
    IDirect3DDevice9* m_pDevice;
    CGraphics*        m_pGraphics;

    std::unordered_map<CClientPrimitiveBufferInterface*, std::vector<PrimitiveBufferSettings>> m_primitiveBufferMap;
    ETextureAddress                                                                            m_CurrentTextureAddress;
};
