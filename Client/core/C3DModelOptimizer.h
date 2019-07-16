#pragma once

#include "CTextureAtlas.h"
#include "CDXTexture.h"
#include <xatlas.h>
#include "xatlas_repack.h"

struct RpClump;

class C3DModelOptimizer
{
public:
    C3DModelOptimizer(RpClump* pTheClump);
    ~C3DModelOptimizer() {}

    RwTexDictionary* GetAtlasTexDictionary() { return m_pAtlasTexDictionary; }
    bool             VectorNormalize(RwV3d* pIn, RwV3d* pOut);
    void             GetTextures();
    void             GetVerticesToMaterial();
    void             GetVerticesPositionsAndUVs();
    void             GetVertexIndices();
    void             GetFaceMaterials();
    void             CreateGeometryMaterials(RpGeometry* pOriginalGeometry, RpGeometry* pNewGeometry, unsigned int atlasCount);
    bool             GetMaterialIndex(RpGeometry* pGeometry, RpMaterial* pMaterial, size_t* materialIndex);
    uint32_t         GetBestAtlasMaxResolution(xatlas::Atlas* atlas, float texelsPerUnit);
    bool             IsAtlasResolutionTooBig(unsigned int bestAtlasResolution);

    RpGeometry*      CreateAtlasRpGeometry(RpGeometry* pOriginalGeometry, int numVerts, int numTriangles, int format);

    void             GetUsedTexturesCount();
    void             GetMostUsedTextureToIgnore();

    bool             AddMeshesToXatlas(xatlas::Atlas* atlas);
    RwTexDictionary* CreateTXDAtlas();

private:
    RpClump*               pClump;
    CRenderWare*           pRenderWare;
    std::vector<RpAtomic*> outAtomicList;

    std::vector<CDXTexture> texturesCache;
    std::vector<uint32_t>   textures;
    std::vector<uint16_t>   vertexToMaterial;
    std::vector<Vector2>    denormalizedUVs;
    std::vector<Vector2>    uvs;
    std::vector<Vector3>    vertices;
    std::vector<uint32_t>   vertexIndices;
    std::vector<uint32_t>   faceMaterials;
    std::vector<RwTexture*> atlasTextures;

    std::map<RwTexture*, unsigned int> mapOfUsedTextures;
    RwTexture*                         m_pMostUsedTextureToIgnore;

    RwTexDictionary*        m_pAtlasTexDictionary;
};
