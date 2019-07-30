#include "StdInc.h"
#include "CIMGArchive.h"
#include "CTextureAtlas.h"
#include "CDXTexture.h"
#include <xatlas.h>
#include "xatlas_repack.h"
#include "C3DModelOptimizer.h"

C3DModelOptimizer::C3DModelOptimizer(RpClump* pTheClump, unsigned int defaultTXDSizeInBytes, bool bDontLoadTextures)
{
    m_defaultTXDSizeInBytes = defaultTXDSizeInBytes;
    pClump = pTheClump;
    m_bDontLoadTextures = bDontLoadTextures;
    m_Atlas = nullptr;
    m_pOptimizedDFF = nullptr;
    m_pAtlasTexDictionary = nullptr;
    pRenderWare = g_pCore->GetGame()->GetRenderWare();
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);
}

C3DModelOptimizer::~C3DModelOptimizer()
{
    if (m_Atlas)
    {
        xatlas::Destroy(m_Atlas);
        m_Atlas = nullptr;
        DestroyMostUsedTexturesToIgnoreClones();
    }
}

void C3DModelOptimizer::SetOptimizationInfo(SOptimizedDFF* pOptimizedDFF)
{
    m_pOptimizedDFF = pOptimizedDFF;
    vecOptimizedTextures.reserve(m_pOptimizedDFF->GetTotalOptimizedTextures());
    for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
    {
        SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
        vecOptimizedTextures.emplace_back(&optimizedTexture);
    }
}

bool C3DModelOptimizer::Optimize()
{
    // REMOVE THIS LINE LATER
    // OutputClumpAsOBJ();
    ///*
    RemoveAtomicsWithNoUVDataFromList();

    if (!outAtomicList.empty())
    {
        GetUsedTexturesCount();
        if (setOfUsedTextures.size() >= minimumModelUsedTexturesRequired)
        {
            return CreateAtlas();
        }
    }
    return false;
    // */
}

bool C3DModelOptimizer::CreateAtlas()
{
    SetupAtlasData();

    // Generate the atlas.
    xatlas::SetPrint(printf, true);
    m_Atlas = xatlas::Create();

    if (!AddMeshesToXatlas())
    {
        return false;
    }
    xatlas::PackOptions packOptions;
    packOptions.padding = 1;
    packOptions.texelsPerUnit = texelsPerUnit;
    xatlas::PackCharts(m_Atlas, packOptions);
    return true;
}

bool C3DModelOptimizer::OutputClumpAsOBJ()
{
    auto RpGeometryLock = (RpGeometry * (__cdecl*)(RpGeometry * geometry, RwInt32 lockMode))0x74C7D0;
    auto RpGeometryUnlock = (RpGeometry * (__cdecl*)(RpGeometry * geometry))0x74C800;

    const char* modelFilePath = "C:\\Users\\danish\\Desktop\\clump_output.obj";
    FILE*       file;
    if (fopen_s(&file, modelFilePath, "w") != 0)
    {
        printf("ReMapClumpUVs: Failed to open file\n");
        return false;
    }

    GetTextures();

    int    firstIndex = 0;
    int    meshID = 0;
    size_t FirstMaterialIndex = 0;

    uint32_t materialIndex = UINT32_MAX;
    uint32_t previousMaterialIndex = UINT32_MAX;

    fprintf(file, "mtllib clump_output.mtl\n");

    for (auto& pAtomic : outAtomicList)
    {
        RpGeometry*           pGeometry = pAtomic->geometry;
        RwV3d*                pVertices = pGeometry->morphTarget->verts;
        RwTextureCoordinates* pTextureCoordinateArray = pGeometry->texcoords[0];
        RwV3d                 normal;

        printf("geometry vertices: %d | triangles: %d\n", pGeometry->vertices_size, pGeometry->triangles_size);

        for (int i = 0; i < pGeometry->vertices_size; i++)
        {
            VectorNormalize(&pVertices[i], &normal);
            fprintf(file, "v %g %g %g\n", pVertices[i].x, pVertices[i].y, pVertices[i].z);
            fprintf(file, "vn %g %g %g\n", normal.x, normal.y, normal.z);
            // fprintf(file, "v %g %g %g\n", pVertices[i].x, pVertices[i].z, pVertices[i].y);
            // fprintf(file, "vn %g %g %g\n", normal.x, normal.z, normal.y);
            fprintf(file, "vt %g %g\n", pTextureCoordinateArray[i].u, 1.0f - pTextureCoordinateArray[i].v);
        }
        fprintf(file, "o mesh%03u\n", meshID);
        // fprintf(file, "usemtl repack_atlas\n");
        fprintf(file, "s off\n");

        RpTriangle* triangles = pGeometry->triangles;
        for (int i = 0; i < pGeometry->triangles_size; i++)
        {
            RpTriangle& triangle = triangles[i];

            materialIndex = textures[FirstMaterialIndex + triangle.matIndex];
            if (materialIndex == UINT32_MAX)
            {
                materialIndex = 0;
            }
            else
            {
                materialIndex += 1;
            }

            if (materialIndex != previousMaterialIndex)
            {
                previousMaterialIndex = materialIndex;
                fprintf(file, "usemtl %d\n", materialIndex);
            }

            fprintf(file, "f ");
            for (uint32_t j = 0; j < 3; j++)
            {
                const uint32_t index = firstIndex + triangle.vertIndex[j] + 1;            // 1-indexed
                fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
            }
        }

        meshID++;
        firstIndex += pAtomic->geometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;
    }

    if (file)
    {
        fclose(file);
    }

    // Write the model.
    const char* materialFilename = "C:\\Users\\danish\\Desktop\\clump_output.mtl";
    printf("Writing '%s'...\n", materialFilename);
    if (fopen_s(&file, materialFilename, "w") != 0)
    {
        printf("ReMapClumpUVs: Failed to open file\n");
        return nullptr;
    }
    if (file)
    {
        int i = 0;
        fprintf(file, "newmtl %d\n", i);
        fprintf(file, "Ka  0.6 0.6 0.6\n");
        fprintf(file, "Kd  0.6 0.6 0.6\n");
        fprintf(file, "Ks  0.0 0.0 0.0\n");
        fprintf(file, "d  1.0\n");
        fprintf(file, "Ns  0.0\n");
        fprintf(file, "illum 2\n");

        i = 1;
        for (auto& dxTexture : texturesCache)
        {
            const std::string textureName = dxTexture.GetOriginalRwTexture()->name;

            fprintf(file, "newmtl %d\n", i);
            fprintf(file, "Ka  0.6 0.6 0.6\n");
            fprintf(file, "Kd  0.6 0.6 0.6\n");
            fprintf(file, "Ks  0.0 0.0 0.0\n");
            fprintf(file, "d  1.0\n");
            fprintf(file, "Ns  0.0\n");
            fprintf(file, "illum 2\n");
            fprintf(file, "map_Kd %s\n", (textureName + ".png").c_str());

            i++;
        }
    }

    if (file)
    {
        fclose(file);
    }
    return true;
}
void C3DModelOptimizer::RemoveAtomicsWithNoUVDataFromList()
{
    // remove atomics with no texcoords for its geometry
    for (auto it = outAtomicList.begin(); it != outAtomicList.end();)
    {
        if (!(*it)->geometry->texcoords[0])
        {
            it = outAtomicList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// Normalize pIn vector into pOut
bool C3DModelOptimizer::VectorNormalize(RwV3d* pIn, RwV3d* pOut)
{
    float len = (float)(std::sqrt(pIn->x * pIn->x + pIn->y * pIn->y + pIn->z * pIn->z));
    if (len)
    {
        pOut->x = pIn->x / len;
        pOut->y = pIn->y / len;
        pOut->z = pIn->z / len;
        return true;
    }
    return false;
}

void C3DModelOptimizer::GetTextures()
{
    std::set<unsigned int> setOfTextureNameHashes;

    size_t totalPossibleTextures = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        totalPossibleTextures += pGeometry->materials.entries;
    }

    texturesCache.reserve(totalPossibleTextures);

    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        for (int i = 0; i < pGeometry->materials.entries; i++)
        {
            RpMaterial* pMaterial = pGeometry->materials.materials[i];

            if (!pMaterial->texture)
            {
                // printf("GetTextures: No texture\n");
                textures.emplace_back(UINT32_MAX);
                continue;
            }

            auto itTextureToIgnore = m_mapOfMostUsedTexturesToIgnore.find(pMaterial->texture);
            if (itTextureToIgnore != m_mapOfMostUsedTexturesToIgnore.end())
            {
                // printf("GetTextures: Ignoring a texture\n");
                textures.emplace_back(UINT32_MAX);
                continue;
            }

            const unsigned int textureNameHash = HashString(pMaterial->texture->name);
            auto               it = setOfTextureNameHashes.find(textureNameHash);
            if (it == setOfTextureNameHashes.end())
            {
                textures.emplace_back(texturesCache.size());
                if (m_bDontLoadTextures)
                {
                    RwRaster*   raster = pMaterial->texture->raster;
                    CDXTexture& dxTexture = texturesCache.emplace_back(raster->width, raster->height);
                }
                else
                {
                    CDXTexture& dxTexture = texturesCache.emplace_back(pMaterial->texture, RW_TEXTURE_DELETE_ON_UNLOAD);
                    dxTexture.LockTexture(D3DLOCK_READONLY);
                }

                setOfTextureNameHashes.insert(textureNameHash);

                // printf("GetTextures: added texture to cache: %s\n", pMaterial->texture->name);
            }
            else
            {
                unsigned int index = std::distance(setOfTextureNameHashes.begin(), it);
                textures.emplace_back(textures[index]);

                // printf("GetTextures: Added index to `textures` as tex exists: %s\n", pMaterial->texture->name);
            }
        }
    }
}

void C3DModelOptimizer::GetVerticesToMaterial()
{
    size_t totalVertices = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        totalVertices += static_cast<size_t>(pGeometry->vertices_size);
    }

    vertexToMaterial.resize(totalVertices);

    size_t FirstVertexIndex = 0;
    size_t FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        for (int i = 0; i < pGeometry->triangles_size; i++)
        {
            RpTriangle& triangle = pGeometry->triangles[i];
            for (int j = 0; j < 3; j++)
            {
                int vertexIndex = triangle.vertIndex[j];

                unsigned short materialIndex = triangle.matIndex + FirstMaterialIndex;
                vertexToMaterial[vertexIndex + FirstVertexIndex] = materialIndex;
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;
    }
}

void C3DModelOptimizer::GetVerticesUVs()
{
    size_t totalVertices = 0;
    size_t FirstVertexIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*             pAtomic = outAtomicList[geometryIndex];
        RwTextureCoordinates* pTextureCoordinateArray = pAtomic->geometry->texcoords[0];
        RwV3d*                pVertices = pAtomic->geometry->morphTarget->verts;
        for (int i = 0; i < pAtomic->geometry->vertices_size; i++)
        {
            RwTextureCoordinates& textureCoords = pTextureCoordinateArray[i];

            // Flip V for xatlas :)
            float theV = 1.0f - textureCoords.v;
            uvs[FirstVertexIndex + i] = Vector2(textureCoords.u, theV);
        }

        FirstVertexIndex += pAtomic->geometry->vertices_size;
    }
}

void C3DModelOptimizer::GetVertexIndices()
{
    size_t totalIndices = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;

        totalIndices += (pGeometry->triangles_size * 3);
    }

    vertexIndices.resize(totalIndices);

    size_t FirstIndex = 0;
    size_t FirstVertexIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        int         triangleIndex = 0;
        uint32_t    totalGeometryIndices = pGeometry->triangles_size * 3;
        for (uint32_t f = 0; f < totalGeometryIndices; f += 3, triangleIndex++)
        {
            RpTriangle& triangle = pGeometry->triangles[triangleIndex];
            for (int j = 0; j < 3; j++)
            {
                uint32_t vertexIndex = triangle.vertIndex[j] + FirstVertexIndex;
                vertexIndices[FirstIndex + f + j] = vertexIndex;
            }
        }

        FirstIndex += totalGeometryIndices;
        FirstVertexIndex += pGeometry->vertices_size;
    }
}

void C3DModelOptimizer::GetFaceMaterials()
{
    size_t totalTriangles = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        totalTriangles += pGeometry->triangles_size;
    }

    faceMaterials.resize(totalTriangles);

    size_t FirstIndex = 0;
    size_t FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        int         triangleIndex = 0;
        for (int triangleIndex = 0; triangleIndex < pGeometry->triangles_size; triangleIndex++)
        {
            RpTriangle& triangle = pGeometry->triangles[triangleIndex];

            unsigned int materialIndex = triangle.matIndex + FirstMaterialIndex;
            faceMaterials[FirstIndex + triangleIndex] = materialIndex;
        }

        FirstIndex += pGeometry->triangles_size;
        FirstMaterialIndex += pGeometry->materials.entries;
    }
}

bool C3DModelOptimizer::GetMaterialIndex(RpGeometry* pGeometry, RpMaterial* pMaterial, size_t* materialIndex)
{
    for (int i = 0; i < pGeometry->materials.entries; i++)
    {
        if (pGeometry->materials.materials[i] == pMaterial)
        {
            *materialIndex = i;
            return true;
        }
    }
    return false;
}

uint32_t C3DModelOptimizer::GetBestAtlasMaxResolution()
{
    assert(m_Atlas != nullptr);

    uint32_t maxResolution = std::max(m_Atlas->width, m_Atlas->height);
    while (maxResolution % 4 != 0)
    {
        maxResolution += 3;
    }
    return maxResolution;
}

unsigned int GetDXT1TextureSizeInBytes(unsigned int resolution)
{
    unsigned int blockSizeInBytes = 8;
    unsigned int thePitch = std::max((unsigned int)1, (unsigned int)((resolution + 3) / 4)) * blockSizeInBytes;
    float        atlasSizeInBytes = (resolution / 4) * thePitch;
    return atlasSizeInBytes;
}

bool C3DModelOptimizer::IsAtlasResolutionTooBig(unsigned int bestAtlasResolution)
{
    // Implying we are using DXT1 format for compressing textures.

    float       atlasSizeInBytes = GetDXT1TextureSizeInBytes(bestAtlasResolution);
    float       atlasSizeInMbs = ((atlasSizeInBytes / 1024) / 1024);
    const float sizePermittedInMBs = 5.0f;
    printf("\n[DEBUG] atlasSizeInMBs: %f | bestAtlasResolution: %u\n\n", atlasSizeInMbs, bestAtlasResolution);
    if (atlasSizeInMbs > sizePermittedInMBs)
    {
        return true;
    }
    return false;
}

RpGeometry* C3DModelOptimizer::CreateAtlasRpGeometry(RpGeometry* pOriginalGeometry, int numVerts, int numTriangles, int format)
{
    auto RpGeometryUnlock = (RpGeometry * (__cdecl*)(RpGeometry * geometry))0x74C800;
    auto RpGeometryCreate = (RpGeometry * (__cdecl*)(int numVerts, int numTriangles, int format))0x74CA90;
    // auto _rpMaterialListCopy = (RpMaterials * (__cdecl*)(RpMaterials * matListOut, const RpMaterials* matListIn))0x74E1F0;
    auto RpMaterialCreate = (RpMaterial * (__cdecl*)())0x74D990;
    auto _rpMaterialListAlloc = (RpMaterial * *(__cdecl*)(RwUInt32 count))0x74E1C0;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    RpGeometry* pGeometry = RpGeometryCreate(numVerts, numTriangles, format);

    if (!pGeometry)
    {
        printf(" failed to create geometry\n");
        return nullptr;
    }

    RpGeometryUnlock(pGeometry);

    if (pOriginalGeometry->triangles)
    {
        memcpy(pGeometry->triangles, pOriginalGeometry->triangles, sizeof(RpTriangle) * numTriangles);
    }

    pGeometry->materials.entries = pOriginalGeometry->materials.entries;
    pGeometry->materials.unknown = pOriginalGeometry->materials.entries;
    pGeometry->materials.materials = _rpMaterialListAlloc(pOriginalGeometry->materials.entries);

    for (int i = 0; i < pOriginalGeometry->materials.entries; i++)
    {
        RpMaterial* pMaterial = RpMaterialCreate();
        *pMaterial = *pOriginalGeometry->materials.materials[i];
        pMaterial->texture = NULL;
        pMaterial->refs = 2;
        pGeometry->materials.materials[i] = pMaterial;
    }

    // _rpMaterialListCopy(&pGeometry->materials, &pOriginalGeometry->materials);

    pRenderWare->CopyGeometryPlugins(pGeometry, pOriginalGeometry);

    return pGeometry;
}

void C3DModelOptimizer::GetUsedTexturesCount()
{
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        if (pGeometry->materials.entries <= 0)
        {
            continue;
        }
        for (int i = 0; i < pGeometry->triangles_size; i++)
        {
            unsigned short materialIndex = pGeometry->triangles[i].matIndex;
            if (materialIndex > pGeometry->materials.entries)
            {
                continue;
            }
            RpMaterial* pMaterial = pGeometry->materials.materials[materialIndex];
            if (pMaterial->texture)
            {
                if (!MapContains(setOfUsedTextures, pMaterial->texture))
                {
                    printf("[INSERT] setOfUsedTextures: %s\n", pMaterial->texture->name);
                    setOfUsedTextures.insert(pMaterial->texture);
                }
            }
        }
    }
    printf("\ntotal Textures used: %u\n\n", setOfUsedTextures.size());
}

SOptimizedTexture* C3DModelOptimizer::RemoveHighestSizeOptimizedTextureFromContainer()
{
    size_t             optimizedTextureWithHighestSizeIndex = 0;
    SOptimizedTexture* optimizedTextureWithHighestSize = vecOptimizedTextures[0];
    for (size_t textureIndex = 0; textureIndex < vecOptimizedTextures.size(); textureIndex++)
    {
        SOptimizedTexture* optimizedTexture = vecOptimizedTextures[textureIndex];
        unsigned int       textureSizeInBytes = optimizedTexture->m_textureSizeWithinAtlasInBytes;
        if (textureSizeInBytes > optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes)
        {
            optimizedTextureWithHighestSize = optimizedTexture;
            optimizedTextureWithHighestSizeIndex = textureIndex;
        }
    }
    vecOptimizedTextures.erase(vecOptimizedTextures.begin() + optimizedTextureWithHighestSizeIndex);
    return optimizedTextureWithHighestSize;
}

void C3DModelOptimizer::IgnoreTexture(unsigned int textureNameHash)
{
    bool bTextureFound = false;
    for (RwTexture* pTexture : setOfUsedTextures)
    {
        if (HashString(pTexture->name) == textureNameHash)
        {
            printf("\n[DEBUG] IgnoreTexture: %s\n\n", pTexture->name);
            pTexture->refs++;
            m_mapOfMostUsedTexturesToIgnore[pTexture] = pTexture;
            bTextureFound = true;
            break;
        }
    }
    assert(bTextureFound != false);
}

void C3DModelOptimizer::GetMostUsedTextureToIgnore()
{
    // /*
    unsigned int atlasSizeInBytes = 0;
    float        atlasSizeInMBs = 0.0f;
    float        defaultTXDSizeInMbs = 0.0f;
    float        atlasSizeInPercentange = 0.0f;
    unsigned int texturesToIgnore = 0; 
    unsigned int totalAtlasSizeReducedInBytes = 0;
    if (m_pOptimizedDFF)
    {
        defaultTXDSizeInMbs = (m_defaultTXDSizeInBytes / 1024.0f) / 1024.0f;
        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            const unsigned int maxSizeLimitInBytes = 1000 * 1024 * 1024;            // 1000 MB
            if (optimizedTexture.m_textureSizeWithinAtlasInBytes > maxSizeLimitInBytes)
            {
                optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
            }
            atlasSizeInBytes += optimizedTexture.m_textureSizeWithinAtlasInBytes;
        }

        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            if (optimizedTexture.m_textureSizeWithinAtlasInBytes > atlasSizeInBytes)
            {
                optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
            }
        }

        atlasSizeInMBs = (atlasSizeInBytes / 1024.0f) / 1024.0f;
        atlasSizeInPercentange = (atlasSizeInMBs * 100.0f) / defaultTXDSizeInMbs;

        if (defaultTXDSizeInMbs >= atlasSizeInMBs)
        {
            return;
        }

        const unsigned int maxTexturesToIgnore = 5;
        SOptimizedTexture* optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
        float              removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
        while (removedTextureSizeWithinAtlas >= 1.0f && vecOptimizedTextures.size() > 0)
        {
            if (setOfUsedTextures.size() <= 2)
            {
                printf("WARNING! setOfUsedTextures is %u and requires ignoring textures! atlasSizeInMBs: %f\n", setOfUsedTextures.size(), atlasSizeInMBs);
                break;
            }

            totalAtlasSizeReducedInBytes += optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes;
            IgnoreTexture(optimizedTextureWithHighestSize->m_textureNameHash);

            optimizedTextureWithHighestSize = RemoveHighestSizeOptimizedTextureFromContainer();
            removedTextureSizeWithinAtlas = ((optimizedTextureWithHighestSize->m_textureSizeWithinAtlasInBytes) / 1024.0f) / 1024.0f;
            texturesToIgnore++;
            if (texturesToIgnore >= maxTexturesToIgnore || texturesToIgnore == (setOfUsedTextures.size() - 1))
            {
                break;
            }
        }
    }
    //*/

    float atlasSizeReducedInMBs = ((totalAtlasSizeReducedInBytes) / 1024.0f) / 1024.0f;
    printf("\nAtlas Size Reduced: %f MBs | Textures Ignored: %u | texturesToIgnore variable: %u\n\n", 
        atlasSizeReducedInMBs, m_mapOfMostUsedTexturesToIgnore.size(), texturesToIgnore);
    if (m_pOptimizedDFF)
    {
        /*
        unsigned int atlasSizeInBytes = 0;
        float        atlasSizeInMBs = 0.0f;
        float        defaultTXDSizeInMbs = (m_defaultTXDSizeInBytes / 1024.0f) / 1024.0f;
        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            const unsigned int maxSizeLimitInBytes = 1000 * 1024 * 1024;            // 1000 MB
            if (optimizedTexture.m_textureSizeWithinAtlasInBytes > maxSizeLimitInBytes)
            {
                optimizedTexture.m_textureSizeWithinAtlasInBytes = 0;
            }
            atlasSizeInBytes += optimizedTexture.m_textureSizeWithinAtlasInBytes;
            vecOptimizedTextures.emplace_back(&optimizedTexture);
        }

        atlasSizeInMBs = (atlasSizeInBytes / 1024.0f) / 1024.0f;
        float atlasSizeInPercentange = (atlasSizeInMBs * 100.0f) / defaultTXDSizeInMbs;
        */
        printf("atlasSizeInMBs: %f | defaultTXDSizeInMbs: %f | atlasSizeInPercentange: %f\n", atlasSizeInMBs, defaultTXDSizeInMbs, atlasSizeInPercentange);

        // printf("\atlasSizeInBytes: %u | defaultTXDSizeInBytes: %u | atlasSizeInPercentange: %f\n",
        //     atlasSizeInBytes, m_defaultTXDSizeInBytes, atlasSizeInPercentange);

        for (unsigned int textureIndex = 0; textureIndex < m_pOptimizedDFF->GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = m_pOptimizedDFF->GetOptimizedTexture(textureIndex);
            for (auto& pTexture : setOfUsedTextures)
            {
                if (HashString(pTexture->name) == optimizedTexture.m_textureNameHash && optimizedTexture.m_textureSizeWithinAtlasInBytes > 0)
                {
                    float sizeInAtlas = (optimizedTexture.m_textureSizeWithinAtlasInBytes / 1024.0f) / 1024.0f;
                    printf("\n[DEBUG] texture: %s | SizeInAtlas: %f MBs, %u bytes\n\n", pTexture->name, sizeInAtlas,
                           optimizedTexture.m_textureSizeWithinAtlasInBytes);
                    // printf("\n[DEBUG] texture: %s | SizeInAtlas: %u bytes\n\n", pTexture->name, optimizedTexture.m_textureSizeWithinAtlasInBytes);
                    break;
                }
            }
        }
    }
}

void C3DModelOptimizer::DestroyMostUsedTexturesToIgnoreClones()
{
    for (auto& it : m_mapOfMostUsedTexturesToIgnore)
    {
        pRenderWare->DestroyTexture(it.second);
    }
    m_mapOfMostUsedTexturesToIgnore.clear();
}

bool C3DModelOptimizer::AddMeshesToXatlas()
{
    size_t FirstIndex = 0;
    size_t FirstVertexIndex = 0;
    size_t FirstFaceIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        uint32_t    totalGeometryIndices = pGeometry->triangles_size * 3;

        xatlas::UvMeshDecl meshDecl;
        meshDecl.vertexCount = (uint32_t)pGeometry->vertices_size;
        meshDecl.vertexUvData = &denormalizedUVs[FirstVertexIndex];
        meshDecl.vertexStride = sizeof(Vector2);
        meshDecl.indexCount = totalGeometryIndices;
        meshDecl.indexData = &vertexIndices[FirstIndex];
        meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
        meshDecl.indexOffset = -(int32_t)FirstVertexIndex;
        meshDecl.rotateCharts = false;
        meshDecl.faceMaterialData = &faceMaterials[FirstFaceIndex];
        xatlas::AddMeshError::Enum error = xatlas::AddUvMesh(m_Atlas, meshDecl);
        if (error != xatlas::AddMeshError::Success)
        {
            printf("Error adding mesh %d: %s\n", (int)geometryIndex, xatlas::StringForEnum(error));
            return false;
        }

        FirstIndex += totalGeometryIndices;
        FirstVertexIndex += pGeometry->vertices_size;
        FirstFaceIndex += pGeometry->triangles_size;
    }
    return true;
}

bool C3DModelOptimizer::GetModelOptimizationInfo(SOptimizedDFF& optimizedDFF)
{
    unsigned int resolution = std::max(m_Atlas->width, m_Atlas->height);
    while (resolution % 4 != 0)
    {
        resolution++;
    }
    unsigned int        atlasSizeInBytes = GetDXT1TextureSizeInBytes(resolution);
    const float         texelsPerUnit = 1.0f;
    xatlas::PackOptions packOptions;
    packOptions.padding = 1;
    packOptions.texelsPerUnit = texelsPerUnit;

    float theAtlasSize = (atlasSizeInBytes / 1024.0f) / 1024.0f;
    printf("atlas Size: %f mb | atlasSizeInBytes: %u\n", theAtlasSize, atlasSizeInBytes);

    for (auto& pTexture : setOfUsedTextures)
    {
        DestroyMostUsedTexturesToIgnoreClones();

        m_mapOfMostUsedTexturesToIgnore[pTexture] = nullptr;
        SetupAtlasDataForOptimizationInfo();

        xatlas::Destroy(m_Atlas);
        m_Atlas = xatlas::Create();
        if (!AddMeshesToXatlas())
        {
            return false;
        }

        xatlas::PackCharts(m_Atlas, packOptions);

        unsigned int theResolution = std::max(m_Atlas->width, m_Atlas->height);
        while (theResolution % 4 != 0)
        {
            theResolution++;
        }
        unsigned int atlasSizeWithoutOneTextureInBytes = GetDXT1TextureSizeInBytes(theResolution);
        unsigned int textureSizeWithinAtlasInBytes = atlasSizeInBytes - atlasSizeWithoutOneTextureInBytes;
        optimizedDFF.Addtexture(HashString(pTexture->name), textureSizeWithinAtlasInBytes);

        float theTextureSizeWithinAtlasInMBs = ((atlasSizeInBytes - atlasSizeWithoutOneTextureInBytes) / 1024.0f) / 1024.0f;
        std::printf("texture '%s' size in atlas: %f mb | textureSize In Atlas: %u\n", pTexture->name, theTextureSizeWithinAtlasInMBs,
                    textureSizeWithinAtlasInBytes);
    }
    printf("total Optimized Textures: %u\n", optimizedDFF.GetTotalOptimizedTextures());
    DestroyMostUsedTexturesToIgnoreClones();
    return true;
}

void C3DModelOptimizer::GetDenormalizedUVs()
{
    for (uint32_t i = 0; i < denormalizedUVs.size(); i++)
    {
        const uint16_t materialIndex = vertexToMaterial[i];
        CDXTexture*    dxTexture = nullptr;
        if (materialIndex != UINT16_MAX && textures[materialIndex] != UINT32_MAX)
        {
            dxTexture = &texturesCache[textures[materialIndex]];
        }

        if (dxTexture)
        {
            denormalizedUVs[i].x *= (float)dxTexture->GetWidth();
            denormalizedUVs[i].y *= (float)dxTexture->GetHeight();
        }
    }
}

void C3DModelOptimizer::SetupAtlasDataForOptimizationInfo()
{
    texturesCache.clear();
    textures.clear();
    denormalizedUVs.clear();
    std::vector<CDXTexture>().swap(texturesCache);
    std::vector<uint32_t>().swap(textures);
    std::vector<Vector2>().swap(denormalizedUVs);

    GetTextures();
    denormalizedUVs = uvs;
    GetDenormalizedUVs();
}

void C3DModelOptimizer::SetupAtlasData()
{
    GetMostUsedTextureToIgnore();
    GetTextures();

    // Map vertices to materials so rasterization knows which texture to sample.
    GetVerticesToMaterial();

    // Denormalize UVs by scaling them by texture dimensions.
    denormalizedUVs.resize(vertexToMaterial.size());
    uvs.resize(vertexToMaterial.size());
    GetVerticesUVs();

    denormalizedUVs = uvs;

    GetDenormalizedUVs();
    GetVertexIndices();
    GetFaceMaterials();
}

void C3DModelOptimizer::ReplaceGeometriesInClump()
{
    auto RpAtomicSetGeometry = (RpAtomic * (__cdecl*)(RpAtomic * atomic, RpGeometry * geometry, RwUInt32 flags))0x749D40;

    unsigned int FirstVertexIndex = 0;
    unsigned int FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*           pAtomic = outAtomicList[geometryIndex];
        RpGeometry*         pGeometry = pAtomic->geometry;
        const xatlas::Mesh& mesh = m_Atlas->meshes[geometryIndex];

        RpGeometry* pNewGeometry = CreateAtlasRpGeometry(pGeometry, mesh.vertexCount, pGeometry->triangles_size, pGeometry->flags);

        RwV3d*                destVertices = pNewGeometry->morphTarget->verts;
        RwV3d*                destNormals = pNewGeometry->morphTarget->normals;
        RwColor*              destColors = pNewGeometry->colors;
        RwTextureCoordinates* pDestTextureCoordinateArray = pNewGeometry->texcoords[0];

        RwV3d*                sourceVertices = pGeometry->morphTarget->verts;
        RwV3d*                sourceNormals = pGeometry->morphTarget->normals;
        RwColor*              sourceColors = pGeometry->colors;
        RwTextureCoordinates* pSourceTextureCoordinateArray = pGeometry->texcoords[0];

        for (uint32_t v = 0; v < mesh.vertexCount; v++)
        {
            const xatlas::Vertex& vertex = mesh.vertexArray[v];
            size_t                sourceVertexIndex = vertex.xref;

            destVertices[sourceVertexIndex] = sourceVertices[sourceVertexIndex];

            if (sourceNormals)
            {
                destNormals[sourceVertexIndex] = sourceNormals[sourceVertexIndex];
            }
            if (sourceColors)
            {
                destColors[sourceVertexIndex] = sourceColors[sourceVertexIndex];
            }

            unsigned int vertexXref = vertex.xref + FirstVertexIndex;
            uint16_t     materialIndex = vertexToMaterial[vertexXref] - FirstMaterialIndex;
            RpMaterial*  pMaterial = pGeometry->materials.materials[materialIndex];
            if (MapContains(m_mapOfMostUsedTexturesToIgnore, pMaterial->texture) || (!pMaterial->texture))
            {
                pDestTextureCoordinateArray[sourceVertexIndex] = pSourceTextureCoordinateArray[sourceVertexIndex];
                continue;
            }

            pDestTextureCoordinateArray[sourceVertexIndex] = {vertex.uv[0] / m_Atlas->width, vertex.uv[1] / m_Atlas->height};
        }

        size_t triangleIndex = 0;
        for (uint32_t f = 0; f < mesh.indexCount; f += 3, triangleIndex++)
        {
            uint16_t     materialIndex = UINT16_MAX;
            unsigned int vertexXref = UINT32_MAX;

            int32_t atlasIndex = -1;
            for (uint32_t j = 0; j < 3; j++)
            {
                uint32_t              vertexIndex = mesh.indexArray[f + j];
                const xatlas::Vertex& v = mesh.vertexArray[vertexIndex];
                vertexXref = v.xref + FirstVertexIndex;
                atlasIndex = v.atlasIndex;            // The same for every vertex in the triangle.
                materialIndex = vertexToMaterial[vertexXref] - FirstMaterialIndex;
                pNewGeometry->triangles[triangleIndex].vertIndex[j] = vertexIndex;
            }

            pNewGeometry->triangles[triangleIndex].matIndex = materialIndex;

            RpMaterial* pSourceMaterial = pGeometry->materials.materials[materialIndex];
            RpMaterial* pDestinationMaterial = pNewGeometry->materials.materials[materialIndex];

            auto it = m_mapOfMostUsedTexturesToIgnore.find(pSourceMaterial->texture);
            if (it != m_mapOfMostUsedTexturesToIgnore.end())
            {
                pDestinationMaterial->texture = it->second;
            }
            else if (pSourceMaterial->texture)
            {
                assert(atlasIndex >= 0);
                pDestinationMaterial->texture = atlasTextures[atlasIndex];
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;

        RpAtomicSetGeometry(pAtomic, pNewGeometry, 0);
    }
}

void C3DModelOptimizer::GetAtlasTextures(CTextureAtlas& textureAtlas)
{
    atlasTextures.resize(textureAtlas.atlasDXTextures.size());
    char buffer[100];
    for (size_t i = 0; i < textureAtlas.atlasDXTextures.size(); i++)
    {
        CDXTexture& atlasTexture = textureAtlas.atlasDXTextures[i];
        printf("compressing atlas texture to DXT1: index = %u\n", i);
        assert(atlasTexture.Compress(D3DFMT_DXT1) != false);
        sprintf(buffer, "myAtlas%d", i);
        RwTexture* pTexture = atlasTexture.GetRwTexture();
        memcpy(pTexture->name, buffer, strlen(buffer) + 1);
        atlasTextures[i] = pTexture;
    }
}

RwTexDictionary* C3DModelOptimizer::CreateTXDAtlas()
{
    xatlas::PackOptions packOptions;
    packOptions.padding = 1;
    packOptions.texelsPerUnit = texelsPerUnit;

    // DECIDE HOW MANY TEXTURES TO IGNORE HERE?

    uint32_t bestAtlasResolution = GetBestAtlasMaxResolution();

    if (IsAtlasResolutionTooBig(bestAtlasResolution))
    {
    }

    unsigned int atlasFindingAttempts = 0;
    do
    {
        atlasFindingAttempts++;
        xatlas::Destroy(m_Atlas);
        m_Atlas = xatlas::Create();
        if (!AddMeshesToXatlas())
        {
            return nullptr;
        }

        printf("Attempt #%u: finding atlas size where atlas can be within a single image\n", atlasFindingAttempts);
        packOptions.resolution = bestAtlasResolution;
        xatlas::PackCharts(m_Atlas, packOptions);
        bestAtlasResolution += 32;
    } while (m_Atlas->atlasCount > 1);

    printf("Copying texture data into atlas\n");

    CTextureAtlas& textureAtlas = CTextureAtlas(pClump, m_Atlas, packOptions, vertexToMaterial, texturesCache, textures, uvs);

    GetAtlasTextures(textureAtlas);

    RwTexDictionary* pAtlasTexDictionary = pRenderWare->CreateTextureDictionary(atlasTextures);

    for (auto& it : m_mapOfMostUsedTexturesToIgnore)
    {
        pRenderWare->AddTextureToDictionary(pAtlasTexDictionary, it.second);
    }

    ReplaceGeometriesInClump();

    m_pAtlasTexDictionary = pAtlasTexDictionary;
    return pAtlasTexDictionary;
}
