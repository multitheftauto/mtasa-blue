#pragma once

#include "CTextureAtlas.h"
#include "CDXTexture.h"
#include <xatlas.h>
#include "xatlas_repack.h"

struct RpClump;

class C3DModelOptimizer
{
public:
    C3DModelOptimizer(RpClump* pTheClump, RwTexDictionary* pTxdDictionary, bool bDontLoadTextures = false);
    ~C3DModelOptimizer();

    void             SetOptimizationInfo(SOptimizedDFF* pOptimizedDFF) { m_pOptimizedDFF = pOptimizedDFF; }
    RwTexDictionary* GetAtlasTexDictionary() { return m_pAtlasTexDictionary; }
    bool             Optimize();
    bool             CreateAtlas();
    bool             OutputClumpAsOBJ();
    void             RemoveAtomicsWithNoUVDataFromList();
    bool             VectorNormalize(RwV3d* pIn, RwV3d* pOut);
    void             GetTextures();
    void             GetVerticesToMaterial();
    void             GetVerticesUVs();
    void             GetVertexIndices();
    void             GetFaceMaterials();
    bool             GetMaterialIndex(RpGeometry* pGeometry, RpMaterial* pMaterial, size_t* materialIndex);
    uint32_t         GetBestAtlasMaxResolution();
    bool             IsAtlasResolutionTooBig(unsigned int bestAtlasResolution);

    RpGeometry* CreateAtlasRpGeometry(RpGeometry* pOriginalGeometry, int numVerts, int numTriangles, int format);

    void GetUsedTexturesCount();
    void GetMostUsedTextureToIgnore();
    void DestroyMostUsedTexturesToIgnoreClones();

    bool             AddMeshesToXatlas();
    bool             GetModelOptimizationInfo(SOptimizedDFF& optimizedDFF);
    void             GetDenormalizedUVs();
    void             SetupAtlasDataForOptimizationInfo();
    void             SetupAtlasData();
    void             ReplaceGeometriesInClump();
    void             GetAtlasTextures(CTextureAtlas& textureAtlas);
    RwTexDictionary* CreateTXDAtlas();

private:
    bool                       m_bDontLoadTextures;
    xatlas::Atlas*             m_Atlas;
    SOptimizedDFF*             m_pOptimizedDFF;

    RpClump*               pClump;
    CRenderWare*           pRenderWare;
    std::vector<RpAtomic*> outAtomicList;

    std::vector<CDXTexture> texturesCache;
    std::vector<uint32_t>   textures;
    std::vector<uint16_t>   vertexToMaterial;
    std::vector<Vector2>    denormalizedUVs;
    std::vector<Vector2>    uvs;
    std::vector<uint32_t>   vertexIndices;
    std::vector<uint32_t>   faceMaterials;
    std::vector<RwTexture*> atlasTextures;

    // Key: original texture of clump
    std::set<RwTexture*> setOfUsedTextures;
    // Key: original texture of clump | Value: clone of the same texture and its raster
    //                                |        except that it will be added to atlas TXD.
    std::map<RwTexture*, RwTexture*> m_mapOfMostUsedTexturesToIgnore;

    RwTexDictionary* m_pTexDictionary;
    RwTexDictionary* m_pAtlasTexDictionary;

    // We'll only optimize DFF models which have lots of used textures
    const unsigned int minimumModelUsedTexturesRequired = 2;
    const float        texelsPerUnit = 1.0f;
};
