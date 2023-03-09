/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CMaterialLine3DBatcher.h
 *  PURPOSE:
 *
 *
 *****************************************************************************/

typedef uint LINEINDEX;

struct SMaterialLine3DItem
{
    float          fDistSq;
    CVector        vecAvgPosition;
    CVector        vecFrom;
    CVector        vecTo;
    CMaterialItem* pMaterial;
    float          fU1, fV1;
    float          fU2, fV2;
    bool           bFlipUV;
    float          fWidth;
    ulong          ulColor;
    bool           bUseFaceToward;
    CVector        vecFaceToward;
};

//
// Batches 3D line drawing
//
class CMaterialLine3DBatcher
{
public:
    ZERO_ON_NEW
    CMaterialLine3DBatcher(bool bPreGUI);
    ~CMaterialLine3DBatcher();

    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void Flush();
    bool HasItems() { return !m_LineList.empty(); }
    void AddLine3D(const CVector& vecFrom, const CVector& vecTo, float fWidth, ulong ulColor, CMaterialItem* pMaterial, float fU, float fV, float fSizeU,
                   float fSizeV, bool bRelativeUV, bool bFlipUV, bool bUseFaceToward, const CVector& vecFaceToward);

protected:
    void DrawBatch(const CVector& vecCameraPos, uint* pBatchIndices, uint uiNumBatchLines, CMaterialItem* pMaterial);

    bool                             m_bPreGUI;
    IDirect3DDevice9*                m_pDevice;
    std::vector<SMaterialLine3DItem> m_LineList;
    static SMaterialLine3DItem*      ms_pLines;            // Used in sort callback
    ETextureAddress                  m_CurrentTextureAddress;
};
