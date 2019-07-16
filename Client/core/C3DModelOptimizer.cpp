#include "StdInc.h"
#include "CIMGArchive.h"
#include "CIDELoader.h"
#include "CTextureAtlas.h"
#include "CDXTexture.h"
#include <xatlas.h>
#include "xatlas_repack.h"
#include "C3DModelOptimizer.h"

C3DModelOptimizer::C3DModelOptimizer(RpClump* pTheClump, RwTexDictionary* pTxdDictionary)
{
    m_pTexDictionary = pTxdDictionary;
    m_pAtlasTexDictionary = nullptr;
    pClump = pTheClump;
    pRenderWare = g_pCore->GetGame()->GetRenderWare();
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);
    CreateTXDAtlas();
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

    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        for (int i = 0; i < pGeometry->materials.entries; i++)
        {
            RpMaterial* pMaterial = pGeometry->materials.materials[i];

            if (!pMaterial->texture)
            {
                printf("GetTextures: No texture\n");
                textures.emplace_back(UINT32_MAX);
                continue;
            }

            auto itTextureToIgnore = m_mapOfMostUsedTexturesToIgnore.find(pMaterial->texture);
            if (itTextureToIgnore != m_mapOfMostUsedTexturesToIgnore.end())
            {
                printf("GetTextures: Ignoring a texture\n");
                textures.emplace_back(UINT32_MAX);
                continue;
            }

            const unsigned int textureNameHash = HashString(pMaterial->texture->name);
            auto               it = setOfTextureNameHashes.find(textureNameHash);
            if (it == setOfTextureNameHashes.end())
            {
                textures.emplace_back(texturesCache.size());
                CDXTexture& dxTexture = texturesCache.emplace_back(pMaterial);
                dxTexture.LockTexture(D3DLOCK_READONLY);

                setOfTextureNameHashes.insert(textureNameHash);

                printf("GetTextures: added texture to cache: %s\n", pMaterial->texture->name);
            }
            else
            {
                unsigned int index = std::distance(setOfTextureNameHashes.begin(), it);
                textures.emplace_back(textures[index]);

                printf("GetTextures: Added index to `textures` as tex exists: %s\n", pMaterial->texture->name);
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

void C3DModelOptimizer::GetVerticesPositionsAndUVs()
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
            vertices[FirstVertexIndex + i] = Vector3(pVertices[i].x, pVertices[i].y, pVertices[i].z);

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
        for (uint32_t triangleIndex = 0; triangleIndex < pGeometry->triangles_size; triangleIndex++)
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

uint32_t C3DModelOptimizer::GetBestAtlasMaxResolution(xatlas::Atlas* atlas, float texelsPerUnit)
{
    xatlas::PackOptions packOptions;
    packOptions.padding = 1;
    packOptions.texelsPerUnit = texelsPerUnit;
    xatlas::PackCharts(atlas, packOptions);

    uint32_t maxResolution = std::max(atlas->width, atlas->height);
    while (maxResolution % 4 != 0)
    {
        maxResolution++;
    }
    return maxResolution;
}

bool C3DModelOptimizer::IsAtlasResolutionTooBig(unsigned int bestAtlasResolution)
{
    // Implying we are using DXT1 format for compressing textures.

    unsigned int blockSizeInBytes = 8;
    unsigned int thePitch = std::max((unsigned int)1, (unsigned int)((bestAtlasResolution + 3) / 4)) * blockSizeInBytes;
    float        atlasSizeInBytes = (bestAtlasResolution / 4) * thePitch;
    float        atlasSizeInMbs = ((atlasSizeInBytes / 1024) / 1024);
    const float  sizePermittedInMBs = 5.0f;
    printf("\n[DEBUG] atlasSizeInMBs: %f\n\n", atlasSizeInMbs);
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
        for (int i = 0; i < pGeometry->triangles_size; i++)
        {
            unsigned short materialIndex = pGeometry->triangles[i].matIndex;
            RpMaterial*    pMaterial = pGeometry->materials.materials[materialIndex];
            if (pMaterial->texture)
            {
                auto it = mapOfUsedTextures.find(pMaterial->texture);
                if (it != mapOfUsedTextures.end())
                {
                    it->second++;
                }
                else
                {
                    mapOfUsedTextures[pMaterial->texture] = 1;
                }
            }
        }
    }

    for (auto& it : mapOfUsedTextures)
    {
        printf("\ntex: %s | usage count: %u\n\n", it.first->name, it.second);
    }
}

void C3DModelOptimizer::GetMostUsedTextureToIgnore()
{
    const unsigned int      textureToIgnoreNameHash = HashString("ceiling_256");
    std::vector<RwTexture*> outTextureList;
    pRenderWare->GetTxdTextures(outTextureList, m_pTexDictionary);

    for (auto& pTexture : outTextureList)
    {
        if (HashString(pTexture->name) == textureToIgnoreNameHash)
        {
            m_mapOfMostUsedTexturesToIgnore[pTexture] = pRenderWare->CloneRwTexture(pTexture);
            printf("\n[DEBUG] m_pMostUsedTextureToIgnore: %s\n\n", pTexture->name);
            break;
        }
    }
}

bool C3DModelOptimizer::AddMeshesToXatlas(xatlas::Atlas* atlas)
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
        xatlas::AddMeshError::Enum error = xatlas::AddUvMesh(atlas, meshDecl);
        if (error != xatlas::AddMeshError::Success)
        {
            xatlas::Destroy(atlas);
            printf("Error adding mesh %d: %s\n", (int)geometryIndex, xatlas::StringForEnum(error));
            return false;
        }

        FirstIndex += totalGeometryIndices;
        FirstVertexIndex += pGeometry->vertices_size;
        FirstFaceIndex += pGeometry->triangles_size;
    }
    return true;
}

RwTexDictionary* C3DModelOptimizer::CreateTXDAtlas()
{
    auto RpMaterialCreate = (RpMaterial * (__cdecl*)())0x74D990;
    auto RpGeometryDestroy = (void(__cdecl*)(RpGeometry * geometry))0x74CCC0;
    auto RpAtomicSetGeometry = (RpAtomic * (__cdecl*)(RpAtomic * atomic, RpGeometry * geometry, RwUInt32 flags))0x749D40;
    auto _rpMaterialListAlloc = (RpMaterial * *(__cdecl*)(RwUInt32 count))0x74E1C0;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    GetUsedTexturesCount();
    GetMostUsedTextureToIgnore();
    GetTextures();

    // Map vertices to materials so rasterization knows which texture to sample.
    GetVerticesToMaterial();

    // Denormalize UVs by scaling them by texture dimensions.
    denormalizedUVs.resize(vertexToMaterial.size());
    uvs.resize(vertexToMaterial.size());
    vertices.resize(vertexToMaterial.size());
    GetVerticesPositionsAndUVs();

    denormalizedUVs = uvs;

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

    GetVertexIndices();
    GetFaceMaterials();

    // Generate the atlas.
    xatlas::SetPrint(printf, true);
    xatlas::Atlas* atlas = xatlas::Create();

    if (!AddMeshesToXatlas(atlas))
    {
        return false;
    }

    float    texelsPerUnit = 1.0f;
    uint32_t bestAtlasResolution = GetBestAtlasMaxResolution(atlas, texelsPerUnit);

    if (IsAtlasResolutionTooBig(bestAtlasResolution))
    {
    }
    unsigned int atlasFindingAttempts = 1;

    xatlas::PackOptions packOptions;
    do
    {
        xatlas::Destroy(atlas);
        atlas = xatlas::Create();
        if (!AddMeshesToXatlas(atlas))
        {
            return false;
        }

        printf("Attempt #%u: finding atlas size where atlas can be within a single image\n", atlasFindingAttempts);
        packOptions.padding = 1;
        packOptions.texelsPerUnit = texelsPerUnit;
        packOptions.resolution = bestAtlasResolution;
        xatlas::PackCharts(atlas, packOptions);
        bestAtlasResolution += 32;
        atlasFindingAttempts++;
    } while (atlas->atlasCount > 1);

    printf("Copying texture data into atlas\n");

    CTextureAtlas& textureAtlas = CTextureAtlas(pClump, atlas, packOptions, vertexToMaterial, texturesCache, textures, uvs);

    std::string textureAtlasFileExtension = ".png";

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

    RwTexDictionary* pAtlasTexDictionary = pRenderWare->CreateTextureDictionary(atlasTextures);

    for (auto& it : m_mapOfMostUsedTexturesToIgnore)
    {
        pRenderWare->AddTextureToDictionary(pAtlasTexDictionary, it.second);
    }

    unsigned int FirstVertexIndex = 0;
    unsigned int FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*           pAtomic = outAtomicList[geometryIndex];
        RpGeometry*         pGeometry = pAtomic->geometry;
        const xatlas::Mesh& mesh = atlas->meshes[geometryIndex];

        RpGeometry* pNewGeometry = CreateAtlasRpGeometry(pGeometry, mesh.vertexCount, pGeometry->triangles_size, pGeometry->flags);

        RwV3d*                destVertices = pNewGeometry->morphTarget->verts;
        RwV3d*                destNormals = pNewGeometry->morphTarget->normals;
        RwColor*              destColors = pNewGeometry->colors;
        RwTextureCoordinates* pDestTextureCoordinateArray = pNewGeometry->texcoords[0];

        RwV3d*   sourceVertices = pGeometry->morphTarget->verts;
        RwV3d*   sourceNormals = pGeometry->morphTarget->normals;
        RwColor* sourceColors = pGeometry->colors;
        RwTextureCoordinates* pSourceTextureCoordinateArray = pGeometry->texcoords[0];

        for (uint32_t v = 0; v < mesh.vertexCount; v++)
        {
            const xatlas::Vertex& vertex = mesh.vertexArray[v];
            size_t                sourceVertexIndex = vertex.xref;            // + FirstVertexIndex;

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

            pDestTextureCoordinateArray[sourceVertexIndex] = {vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height};
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
                if (atlasIndex < 0)
                {
                    printf("invalid atlasIndex for triangle: %u\n", triangleIndex);
                    return nullptr;
                }

                pDestinationMaterial->texture = atlasTextures[atlasIndex];
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;

        RpAtomicSetGeometry(pAtomic, pNewGeometry, 0);
    }

    m_pAtlasTexDictionary = pAtlasTexDictionary;
    return pAtlasTexDictionary;
}
