/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CPrimitive3DBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/
// Vertex type used by the primitives 3d batcher
struct sPrimitive3D
{
    D3DPRIMITIVETYPE                 eType;
    std::vector<PrimitiveVertice>* pVecVertices;
};

//
// Batches primitives drawing
//
class CPrimitive3DBatcher
{
public:
    ZERO_ON_NEW
    CPrimitive3DBatcher(bool bPreGUI);
    ~CPrimitive3DBatcher(void);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush(void);
    void AddPrimitive(D3DPRIMITIVETYPE eType, std::vector<PrimitiveVertice>* pVecVertices);
    bool HasItems(void) { return !m_primitiveList.empty(); }

protected:
    bool m_bPreGUI;
    IDirect3DDevice9*         m_pDevice;
    std::vector<sPrimitive3D> m_primitiveList;
};
