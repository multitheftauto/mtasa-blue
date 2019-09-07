/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

FBXBuffer::FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, unsigned long long ullMaterialId)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid;

    pFBXVertexBuffer = new FBXVertexBuffer(vecVertexList, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

    pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &i_buffer, NULL);
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    i_buffer->Unlock();

    indicesCount = vecIndexList.size();
    this->ullMaterialId = ullMaterialId;
}

template <typename T>
FBXVertexBuffer::FBXVertexBuffer(std::vector<T> vector, int FVF)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid;
    pDevice->CreateVertexBuffer(vector.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &v_buffer, NULL);
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vector.data(), vector.size() * sizeof(T));
    v_buffer->Unlock();

    bufferSize = vector.size();
    this->FVF = FVF;
    iTypeSize = sizeof(T);
}

void FBXVertexBuffer::Select(UINT StreamNumber)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    pDevice->SetStreamSource(StreamNumber, v_buffer, 0, iTypeSize);
}

FBXObjectBuffer::FBXObjectBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, std::vector<int> vecMaterialList,
                                 const ofbx::Mesh* const* pMesh)
{
    if (vecMaterialList.size() > 0)
    {
        std::map<int, std::vector<FBXVertex>> fbxBufferMap;
        std::map<int, std::vector<int>>       fbxIndexMap;
        std::map<int, unsigned long long>     fbxMaterialMap;

        int i = 0;
        for (int const& iMaterial : vecMaterialList)
        {
            if (fbxBufferMap.count(iMaterial) == 0)
            {
                fbxBufferMap[iMaterial] = std::vector<FBXVertex>();
            }
            if (fbxIndexMap.count(iMaterial) == 0)
            {
                fbxIndexMap[iMaterial] = std::vector<int>();
            }
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3]);
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3 + 1]);
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3 + 2]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3 + 1]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3 + 2]);
            i++;
        }

        const ofbx::Material* pMaterial;
        int                   iCount = 0;
        for (auto const& pair : fbxBufferMap)
        {
            pMaterial = (*pMesh)->getMaterial(pair.first);
            fbxMaterialMap[pair.first] = pMaterial->id;
            iCount++;
        }

        if (iCount > 0)
        {
            for (int i = 0; i < iCount; i++)
            {
                bufferList.push_back(new FBXBuffer(fbxBufferMap[i], fbxIndexMap[i], fbxMaterialMap[i]));
            }
        }
    }

    else            // model has 0 or 1 material
    {
        unsigned long long id = 0;
        int                count = (*pMesh)->getMaterialCount();
        if (count != 0)
        {
            id = (*pMesh)->getMaterial(0)->id;
        }
        bufferList.push_back(new FBXBuffer(vecVertexList, vecIndexList, id));
    }
}

void CFBXTemplate::Render(IDirect3DDevice9* pDevice, CFBXScene* pScene, D3DMATRIX* pOffsetMatrix)
{
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

    // select which vertex format we are using
    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
    FBXObjectBuffer* pObjectBuffer;
    D3DMATRIX*       pObjectMatrix = new D3DMATRIX();
    CTextureItem*    pTextureItem;
    float            fDrawDistance;
    CVector          vecTemplatePosition;
    CVector          vecTemplateObjectPosition;
    CVector          vecPosition(pOffsetMatrix->m[3][0], pOffsetMatrix->m[3][1], pOffsetMatrix->m[3][2]);
    pViewMatrix->GetBuffer((float*)pObjectMatrix);
    vecTemplatePosition = pViewMatrix->GetPosition();
    // pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    // pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    // pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);


    pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(102, 102, 102));

    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
    // pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    // pDevice->SetRenderState(D3DRS_ALPHAREF, 0xf);
    //pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    //pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);
    //pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    //pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    pDevice->SetRenderState(D3DRS_LASTPIXEL, FALSE);
    pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
    pDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
    pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

    // pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DRS_DIFFUSEMATERIALSOURCE);
    CMatrix* pCameraMatrix = new CMatrix();
    g_pCore->GetGame()->GetCamera()->GetMatrix(pCameraMatrix);
    CVector vecCameraPosition = pCameraMatrix->GetPosition();
    pDevice->SetVertexDeclaration(pScene->GetVertexDeclaration(0));

    pDevice->SetLight(7, g_pCore->GetFBX()->GetGlobalLight());
    pDevice->LightEnable(7, TRUE);

    for (auto const& object : m_objectMap)
    {
        object.second->GetDrawDistance(fDrawDistance);
        object.second->GetPosition(vecTemplateObjectPosition);

        pDevice->MultiplyTransform(D3DTS_WORLDMATRIX(0), pObjectMatrix);
        pDevice->SetTransform(D3DTS_WORLDMATRIX(0), pOffsetMatrix);

        pDevice->SetTransform(D3DTS_TEXTURE0, pScene->GetMatrixUVFlip());
        pObjectBuffer = pScene->GetFBXBuffer(object.second->ullObjectId);
        if (pObjectBuffer != nullptr)
        {
            object.second->pViewMatrix->GetBuffer((float*)pObjectMatrix);
            pDevice->MultiplyTransform(D3DTS_WORLDMATRIX(0), pObjectMatrix);

            if (((vecTemplatePosition + vecTemplateObjectPosition + vecPosition) - vecCameraPosition).Length() < fDrawDistance)
            {
                pDevice->SetRenderState(D3DRS_CULLMODE, object.second->cullMode);

                for (auto const& pBuffer : pObjectBuffer->bufferList)
                {
                    if (pBuffer->ullMaterialId == 0)
                    {
                        pDevice->SetTexture(0, nullptr);
                    }
                    else
                    {
                        pTextureItem = pScene->GetTexture(pBuffer->ullMaterialId);
                        if (pTextureItem != nullptr)
                        {
                            // Set texture addressing mode
                            pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pTextureItem->m_TextureAddress);
                            pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pTextureItem->m_TextureAddress);

                            if (pTextureItem->m_TextureAddress == TADDRESS_BORDER)
                                pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pTextureItem->m_uiBorderColor);
                            pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
                            pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
                        }
                    }

                    pDevice->SetMaterial(&object.second->material);

                    pBuffer->pFBXVertexBuffer->Select(0);
                    pObjectBuffer->diffuseBuffer->Select(1);
                    pDevice->SetIndices(pBuffer->i_buffer);

                    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pBuffer->indicesCount / 3);
                }
            }
        }
    }

    pDevice->LightEnable(7, FALSE);
    if (pSavedStateBlock)
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE(pSavedStateBlock);
    }
}

CFBXTemplate::CFBXTemplate()
{
    pViewMatrix = new CMatrix();
}

void CFBXTemplate::SetPosition(CVector& pos)
{
    pViewMatrix->SetPosition(pos);
}

void CFBXTemplate::SetRotation(CVector& rot)
{
    pViewMatrix->SetRotation(rot);
}

void CFBXTemplate::SetScale(CVector& scale)
{
    pViewMatrix->SetScale(scale);
}

void CFBXTemplate::SetDrawDistance(float fDrawDistance)
{
    this->fDrawDistance = fDrawDistance;
}

void CFBXTemplate::GetPosition(CVector& position)
{
    position = pViewMatrix->GetPosition();
}

void CFBXTemplate::GetRotation(CVector& rotation)
{
    rotation = pViewMatrix->GetRotation();
}

void CFBXTemplate::GetScale(CVector& scale)
{
    scale = pViewMatrix->GetScale();
}

void CFBXTemplate::GetDrawDistance(float& fDrawDistance)
{
    fDrawDistance = this->fDrawDistance;
}

unsigned int CFBXTemplate::AddTemplateObject(CFBXTemplateObject* pObject)
{
    m_objectMap[uiNextFreeObjectId] = pObject;
    uiNextFreeObjectId++;
    return uiNextFreeObjectId - 1;
}

CFBXTemplateObject::CFBXTemplateObject(unsigned long long ullObjectId) : ullObjectId(ullObjectId)
{
    pViewMatrix = new CMatrix();

    ZeroMemory(&material, sizeof(D3DMATERIAL9));
    material.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
    material.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
    material.Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.8f); // depends
    material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
    material.Power = 500.0f;
}

void CFBXTemplateObject::SetPosition(CVector& pos)
{
    pViewMatrix->SetPosition(pos);
}

void CFBXTemplateObject::SetRotation(CVector& rot)
{
    pViewMatrix->SetRotation(rot);
}

void CFBXTemplateObject::SetScale(CVector& scale)
{
    pViewMatrix->SetScale(scale);
}

void CFBXTemplateObject::SetDrawDistance(float fDrawDistance)
{
    this->fDrawDistance = fDrawDistance;
}

void CFBXTemplateObject::GetPosition(CVector& position)
{
    position = pViewMatrix->GetPosition();
}

void CFBXTemplateObject::GetRotation(CVector& rotation)
{
    rotation = pViewMatrix->GetRotation();
}

void CFBXTemplateObject::GetScale(CVector& scale)
{
    scale = pViewMatrix->GetScale();
}

void CFBXTemplateObject::GetDrawDistance(float& fDrawDistance)
{
    fDrawDistance = this->fDrawDistance;
}

void CFBXTemplateObject::GetCullMode(eCullMode& cullMode)
{
    cullMode = this->cullMode;
}

void CFBXTemplateObject::SetCullMode(eCullMode cullMode)
{
    this->cullMode = cullMode;
}

unsigned int CFBXScene::AddTemplete(CFBXTemplate* pTemplate)
{
    m_templateMap[uiNextFreeTemplateId] = pTemplate;
    return uiNextFreeTemplateId++;
}

FBXObjectBuffer* CFBXScene::GetFBXBuffer(unsigned long long ullId)
{
    if (m_mapMeshBuffer.find(ullId) != m_mapMeshBuffer.end())
    {
        return m_mapMeshBuffer[ullId];
    }
    return nullptr;
}

bool CFBXScene::CreateFBXBuffer(const ofbx::Object* const* pObject)
{
    if ((*pObject)->getType() != ofbx::Object::Type::MESH)
        return false;

    const ofbx::Mesh* const* pMesh = (const ofbx::Mesh* const*)pObject;
    const ofbx::Geometry*    pGeometry = (*pMesh)->getGeometry();
    std::vector<FBXVertex>   vecVertices;
    std::vector<int>         vecIndices(pGeometry->getIndicesCount());
    std::vector<int>         vecMaterials(pGeometry->getMaterialCount());
    std::vector<DWORD>       vecColors;

    const ofbx::Vec3* vertices = pGeometry->getVertices();
    const ofbx::Vec3* normals = pGeometry->getNormals();
    const ofbx::Vec4* colors = pGeometry->getColors();
    const ofbx::Vec2* UVs = pGeometry->getUVs(0);
    const ofbx::Vec3* vertex;
    const ofbx::Vec3* normal;
    const ofbx::Vec2* uv;
    DWORD             dColor;
    if (pGeometry->getColorCount() == 0)
    {
        if (m_mapMeshMaterials[*pMesh].size() > 0)
        {
            const ofbx::Material* pMaterialFirst = m_mapMeshMaterials[*pMesh].at(0);
            ofbx::Color           diffuseColor = (*pMaterialFirst).getDiffuseColor();
            dColor = D3DCOLOR_XRGB((DWORD)(diffuseColor.r * 255), (DWORD)(diffuseColor.g * 255), (DWORD)(diffuseColor.b * 255));
        }
        else
        {
            dColor = D3DCOLOR_XRGB(255, 255, 255);
        }
        for (int i = 0; i < pGeometry->getVertexCount(); i++)
        {
            vertex = vertices + i;
            normal = normals + i;
            uv = UVs + i;
            vecVertices.emplace_back(vertex->x, vertex->y, vertex->z, normal->x, normal->y, normal->z, uv->x, uv->y);
            vecColors.emplace_back(dColor);
        }
    }
    else
    {
        const ofbx::Vec4* color;
        for (int i = 0; i < pGeometry->getVertexCount(); i++)
        {
            vertex = vertices + i;
            normal = normals + i;
            color = colors + i;
            uv = UVs + i;
            dColor = D3DCOLOR_XRGB((DWORD)((*color).x * 255), (DWORD)((*color).y * 255), (DWORD)((*color).z * 255), (DWORD)((*color).w * 255));
            vecVertices.emplace_back(vertex->x, vertex->y, vertex->z, normal->x, normal->y, normal->z, uv->x, uv->y);
            vecColors.push_back(dColor);
        }
    }

    memcpy(vecIndices.data(), pGeometry->getFaceIndices(), sizeof(int) * vecIndices.size());
    memcpy(vecMaterials.data(), pGeometry->getMaterials(), sizeof(int) * vecMaterials.size());

    FBXObjectBuffer* pBuffer = new FBXObjectBuffer(vecVertices, vecIndices, vecMaterials, pMesh);
    pBuffer->diffuseBuffer = new FBXVertexBuffer(vecColors, D3DFVF_DIFFUSE);
    AddBuffer((*pObject)->id, pBuffer);
    return true;
}

bool CFBXScene::AddBuffer(unsigned long long ullObjectId, FBXObjectBuffer* pBuffer)
{
    if (m_mapMeshBuffer.find(ullObjectId) != m_mapMeshBuffer.end())
        return false;

    m_mapMeshBuffer[ullObjectId] = pBuffer;
    return true;
}

bool CFBXScene::IsTemplateModelValid(unsigned int uiTemplate, unsigned int uiModelId)
{
    if (m_templateMap.count(uiTemplate) == 0)
        return false;

    CFBXTemplate* pTemplate = m_templateMap[uiTemplate];
    if (!pTemplate->IsModelValid(uiModelId))
        return false;

    return true;
}

unsigned int CFBXScene::AddMeshToTemplate(unsigned int uiTemplate, unsigned long long uiModelId)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplate];
    CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject(uiModelId);
    unsigned int        uiNewObjectId = pTemplate->AddTemplateObject(pTemplateObject);
    return uiNewObjectId;
}

bool CFBXScene::GetBoundingBox(unsigned long long ullObjectId, CVector& min, CVector& max, float& fRadius)
{
    if (m_geometryBoundingBox.count(ullObjectId) == 0)
        return false;

    CFBXBoundingBox boundingBox = m_geometryBoundingBox[ullObjectId];

    min = boundingBox.min;
    max = boundingBox.max;
    fRadius = boundingBox.radius;
    return true;
}

CTextureItem* CFBXScene::GetTexture(unsigned long long ullMaterialId)
{
    const ofbx::Material* const* pMaterial = m_materialList[ullMaterialId];
    if (pMaterial == nullptr)
        return nullptr;

    const ofbx::Texture* pTexture = (*pMaterial)->getTexture(ofbx::Texture::TextureType::DIFFUSE);
    if (pTexture == nullptr)
        return nullptr;

    if (pClientFBXInterface == nullptr)
        return nullptr;

    if (!pClientFBXInterface->IsTextureCreated(pTexture->id))
    {
        char fileName[120];
        pTexture->getFileName().toString(fileName);
        SString  name = fileName;
        CPixels* pTexturePixels = m_textureContentList[name];

        pClientFBXInterface->CreateTexture(pTexture->id, pTexturePixels);
    }
    CMaterialItem* pMaterialItem = pClientFBXInterface->GetTextureById(pTexture->id);
    if (CShaderItem* pShaderItem = DynamicCast<CShaderItem>(pMaterialItem))
    {
        // If material is a shader, use its current instance
        pMaterialItem = pShaderItem->m_pShaderInstance;
    }
    if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(pMaterialItem))
    {
        return pTextureItem;
    }
    return nullptr;
}

CFBXScene::CFBXScene(ofbx::IScene* scene, CClientFBXInterface* pClientFBXInterface) : m_pScene(scene)
{
    m_pRoot = m_pScene->getRoot();
    this->pClientFBXInterface = pClientFBXInterface;
    FixIndices();
    CacheObjects();
    CacheMeshes();
    CacheTextures();
    CacheMaterials();
    CacheMeshMaterials();
    CacheBoundingBoxes();

    CMatrix* matrixFixInvertedUVs = new CMatrix();
    matrixFixInvertedUVs->SetPosition(CVector(0, 0, 0));
    matrixFixInvertedUVs->SetScale(CVector(1, 1, 1));
    matrixFixInvertedUVs->SetRotation(CVector(PI * 1.5, 0, 0));
    m_pMatrixUVFlip = (D3DXMATRIX*)matrixFixInvertedUVs;

    // test code, remove later
    unsigned int  uiTemplateId = CreateTemplate();
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->pViewMatrix->SetPosition(CVector(0, 0, 0));
    pTemplate->pViewMatrix->SetRotation(CVector(0, 0, 0));
    pTemplate->pViewMatrix->SetScale(CVector(1, 1, 1));
    int i = 0;
    for (const auto& pair : m_objectList)
    {
        if (CreateFBXBuffer(pair.second))
        {
            CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject((*pair.second)->id);
            pTemplateObject->pViewMatrix->SetPosition(CVector(i * 20, 0, 0));
            pTemplateObject->pViewMatrix->SetRotation(CVector(0, 0, 0));
            pTemplateObject->pViewMatrix->SetScale(CVector(1, 1, 1));
            pTemplate->AddTemplateObject(pTemplateObject);
            i++;
        }
    }
    /*
        CVector   pos;
    CVector   normal;
    CVector2D uv;
    */

    D3DVERTEXELEMENT9 dwDeclPosNormalTexColor[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        D3DDECL_END()
    };

    IDirect3DDevice9*            pDevice = g_pCore->GetGraphics()->GetDevice();
    pDevice->CreateVertexDeclaration(dwDeclPosNormalTexColor, &m_pVertexDeclaration[0]);
    D3DVERTEXELEMENT9 dwDeclPosNormalTex[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };

    pDevice->CreateVertexDeclaration(dwDeclPosNormalTex, &m_pVertexDeclaration[1]);
}

unsigned int CFBXScene::CreateTemplate()
{
    CFBXTemplate* pTemplate = new CFBXTemplate();
    unsigned int  uiTemplateId = AddTemplete(pTemplate);
    return uiTemplateId;
}

void CFBXScene::RemoveTemplate(unsigned int uiTemplateId)
{
}

void CFBXScene::AddToRenderQueue(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale)
{
    m_vecTemporaryRenderLoop.push_back(new CFBXRenderItem(uiTemplateId, vecPosition, vecRotation, vecScale));
}

void CFBXScene::CacheObjects()
{
    const ofbx::Object* const* pObject;
    for (int i = 0; i < m_pScene->getAllObjectCount(); i++)
    {
        pObject = m_pScene->getAllObjects() + i;
        m_objectList[(*pObject)->id] = pObject;
        m_objectIdsList.push_back((*pObject)->id);
    }
}

CFBXBoundingBox CFBXScene::CalculateBoundingBox(const ofbx::Geometry* pGeometry)
{
    unsigned long long id = pGeometry->id;

    CFBXBoundingBox   boundingBox;
    const ofbx::Vec3* vertices = pGeometry->getVertices();
    const ofbx::Vec3* vertex;
    CVector           center(0, 0, 0);
    for (int i = 0; i < pGeometry->getVertexCount(); i++)
    {
        vertex = vertices + i;
        boundingBox.max.fX = std::max((float)vertex->x, boundingBox.max.fX);
        boundingBox.max.fY = std::max((float)vertex->y, boundingBox.max.fY);
        boundingBox.max.fZ = std::max((float)vertex->z, boundingBox.max.fZ);
        boundingBox.min.fX = std::min((float)vertex->x, boundingBox.min.fX);
        boundingBox.min.fY = std::min((float)vertex->y, boundingBox.min.fY);
        boundingBox.min.fZ = std::min((float)vertex->z, boundingBox.min.fZ);
        boundingBox.radius = std::max(boundingBox.radius, (float)sqrt(vertex->x * vertex->x + vertex->y * vertex->y + vertex->z * vertex->z));
    }
    return boundingBox;
}

void CFBXScene::CacheMeshes()
{
    const ofbx::Mesh* pMesh;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        m_meshName[pMesh->id] = pMesh->name;
        m_meshList[pMesh->id] = pMesh;
    }
}

void CFBXScene::CacheBoundingBoxes()
{
    CFBXBoundingBox       boundingBox;
    const ofbx::Geometry* pGeometry;
    for (const auto& pair : m_meshList)
    {
        pGeometry = pair.second->getGeometry();
        boundingBox = CalculateBoundingBox(pGeometry);
        m_geometryBoundingBox[pGeometry->id] = boundingBox;
    }
}

void CFBXScene::CacheTextures()
{
    SString                     name;
    const ofbx::DataView const* pFilePath;
    const ofbx::DataView const* pContent;
    const ofbx::Texture*        pTexture;
    int                         iTextureSize;
    for (int i = 0; i < m_pScene->getTexturesCount(); i++)
    {
        pContent = *(m_pScene->getTextureContent() + i);
        pTexture = *(m_pScene->getTextures() + i);
        pFilePath = *(m_pScene->getTextureFilePath() + i);

        char fileName[120];
        pFilePath->toString(fileName);
        SString strFilePath = fileName;

        iTextureSize = pContent->end - pContent->begin - 4;
        CPixels* pPixels = new CPixels();

        pPixels->SetSize(iTextureSize);
        memcpy(pPixels->GetData(), pContent->begin + 4, pPixels->GetSize());

        m_textureContentList[strFilePath] = pPixels;
        m_texturePath[pTexture->id] = strFilePath;
    }
}

void CFBXScene::CacheMeshMaterials()
{
    const ofbx::Mesh* pMesh;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        std::vector<const ofbx::Material*> vecMaterial;
        for (int iMaterial = 0; iMaterial < pMesh->getMaterialCount(); iMaterial++)
        {
            vecMaterial.push_back(pMesh->getMaterial(iMaterial));
        }
        m_mapMeshMaterials[pMesh] = vecMaterial;
    }
}

void CFBXScene::CacheMaterials()
{
    const ofbx::Material* const* pMaterial;
    for (int i = 0; i < m_pScene->getMaterialsCount(); i++)
    {
        pMaterial = m_pScene->getMaterials() + i;
        m_materialList[(*pMaterial)->id] = pMaterial;
    }
}

void CFBXScene::FixIndices()
{
    const ofbx::Mesh*     pMesh;
    const ofbx::Geometry* pGeometry;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        pGeometry = pMesh->getGeometry();
        for (int i = 0; i < pGeometry->getIndicesCount(); i++)
        {
            (int)pGeometry->getFaceIndices()[i] = abs(pGeometry->getFaceIndices()[i]);
        }
    }
}

void CFBXScene::GetAllTemplatesIds(std::vector<unsigned int>& vecIds)
{
    for (auto const& pair : m_templateMap)
    {
        vecIds.emplace_back(pair.first);
    }
}

bool CFBXScene::GetMeshName(long long int ulId, SString& strMeshName)
{
    if (m_meshName.count(ulId) != 0)
    {
        strMeshName = m_meshName[ulId];
        return true;
    }
    return false;
}

bool CFBXScene::GetTexturePath(long long int ulId, SString& strTexturePath)
{
    if (m_texturePath.count(ulId) != 0)
    {
        strTexturePath = m_texturePath[ulId];
        return true;
    }
    return false;
}

bool CFBXScene::GetAllTemplatesModelsIds(std::vector<unsigned int>& vecIds, unsigned int uiTemplateId)
{
    if (!IsTemplateValid(uiTemplateId))
        return false;

    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    for (auto const& pair : pTemplate->GetObjectsMap())
    {
        vecIds.emplace_back(pair.first);
    }
    return true;
}

void CFBXScene::RenderScene(IDirect3DDevice9* pDevice)
{
    // for (auto const& pair : m_templateMap)
    //{
    //    pair.second->Render(pDevice, this);
    //}
    // unsigned char ucInterior = static_cast<unsigned char>(g_pCore->GetGame()->GetWorld()->GetCurrentArea());
    CMatrix*      pMatrix = new CMatrix();
    D3DMATRIX*    pObjectMatrix = new D3DMATRIX();
    CFBXTemplate* pTemplate;
    CMatrix*      pCameraMatrix = new CMatrix();
    CVector       vecCameraPosition;
    CVector       vecTemplatePosition;
    CVector       vecTemplateOffset;
    float         fDrawDistance;
    g_pCore->GetGame()->GetCamera()->GetMatrix(pCameraMatrix);
    vecCameraPosition = pCameraMatrix->GetPosition();
    for (auto const& renderItem : m_vecTemporaryRenderLoop)
    {
        if (!IsTemplateValid(renderItem->uiTemplateId))
            continue;

        pTemplate = m_templateMap[renderItem->uiTemplateId];
        pTemplate->GetPosition(vecTemplatePosition);
        pTemplate->GetDrawDistance(fDrawDistance);

        if (((vecTemplatePosition + renderItem->position) - vecCameraPosition).Length() < fDrawDistance)
        {
            pMatrix->SetPosition(renderItem->position);
            pMatrix->SetRotation(renderItem->rotation);
            pMatrix->SetScale(renderItem->scale);

            pMatrix->GetBuffer((float*)pObjectMatrix);
            pTemplate->Render(pDevice, this, pObjectMatrix);
        }
    }

    std::map<unsigned long long, std::vector<CMatrix>> pTemplatesMatrix = pClientFBXInterface->GetTemplatesRenderingMatrix();
    for (auto const& pair : pTemplatesMatrix)
    {
        for (CMatrix matrix : pair.second)
        {
            pTemplate = m_templateMap[pair.first];
            pTemplate->GetPosition(vecTemplatePosition);
            pTemplate->GetDrawDistance(fDrawDistance);
            vecTemplateOffset = matrix.GetPosition();
            if (((vecTemplatePosition + vecTemplateOffset) - vecCameraPosition).Length() < fDrawDistance)
            {
                matrix.GetBuffer((float*)pObjectMatrix);
                pTemplate->Render(pDevice, this, pObjectMatrix);
            }
        }
    }

    ListClearAndReserve(m_vecTemporaryRenderLoop);
}

void CFBXScene::GetTemplatePosition(unsigned int uiTemplateId, CVector& position)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetPosition(position);
}

void CFBXScene::GetTemplateRotation(unsigned int uiTemplateId, CVector& rotation)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetRotation(rotation);
}

void CFBXScene::GetTemplateScale(unsigned int uiTemplateId, CVector& scale)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetScale(scale);
}

void CFBXScene::GetTemplateDrawDistance(unsigned int uiTemplateId, float& drawDistance)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetDrawDistance(drawDistance);
}

void CFBXScene::SetTemplatePosition(unsigned int uiTemplateId, CVector& position)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetPosition(position);
}

void CFBXScene::SetTemplateRotation(unsigned int uiTemplateId, CVector& rotation)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetRotation(rotation);
}

void CFBXScene::SetTemplateScale(unsigned int uiTemplateId, CVector& scale)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetScale(scale);
}

void CFBXScene::SetTemplateDrawDistance(unsigned int uiTemplateId, float fDrawDistance)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetDrawDistance(fDrawDistance);
}

void CFBXScene::GetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetPosition(position);
}

void CFBXScene::GetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetRotation(rotation);
}

void CFBXScene::GetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetScale(scale);
}

void CFBXScene::GetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fDrawDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetDrawDistance(fDrawDistance);
}

void CFBXScene::SetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetPosition(position);
}

void CFBXScene::SetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetRotation(rotation);
}

void CFBXScene::SetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetScale(scale);
}

void CFBXScene::SetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fDrawDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetDrawDistance(fDrawDistance);
}

void CFBXScene::SetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode cullMode)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetCullMode(cullMode);
}

void CFBXScene::GetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode& cullMode)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetCullMode(cullMode);
}

CFBXScene* CFBX::AddScene(ofbx::IScene* pScene, CClientFBXInterface* pClientFBXInterface)
{
    CFBXScene* pFBXScene = new CFBXScene(pScene, pClientFBXInterface);

    m_sceneList.push_back(pFBXScene);
    return pFBXScene;
}

const char* CFBX::GetObjectType(const ofbx::Object const* pObject)
{
    const char* label;
    switch (pObject->getType())
    {
        case ofbx::Object::Type::GEOMETRY:
            label = "geometry";
            break;
        case ofbx::Object::Type::MESH:
            label = "mesh";
            break;
        case ofbx::Object::Type::MATERIAL:
            label = "material";
            break;
        case ofbx::Object::Type::ROOT:
            label = "root";
            break;
        case ofbx::Object::Type::TEXTURE:
            label = "texture";
            break;
        case ofbx::Object::Type::NULL_NODE:
            label = "null";
            break;
        case ofbx::Object::Type::LIMB_NODE:
            label = "limb node";
            break;
        case ofbx::Object::Type::NODE_ATTRIBUTE:
            label = "node attribute";
            break;
        case ofbx::Object::Type::CLUSTER:
            label = "cluster";
            break;
        case ofbx::Object::Type::SKIN:
            label = "skin";
            break;
        case ofbx::Object::Type::ANIMATION_STACK:
            label = "animation stack";
            break;
        case ofbx::Object::Type::ANIMATION_LAYER:
            label = "animation layer";
            break;
        case ofbx::Object::Type::ANIMATION_CURVE:
            label = "animation curve";
            break;
        case ofbx::Object::Type::ANIMATION_CURVE_NODE:
            label = "animation curve node";
            break;
        default:
            label = "unknown";
            break;
    }
    return label;
}

bool CFBX::HasAnyFBXLoaded()
{
    return m_sceneList.size() > 0;
}

CFBX::CFBX()
{
    m_pDevice = nullptr;

    ZeroMemory(&m_globalLight, sizeof(m_globalLight));
    m_globalLight.Type = D3DLIGHT_DIRECTIONAL;
    m_globalLight.Diffuse.r = 0.4f;
    m_globalLight.Diffuse.g = 0.4f;
    m_globalLight.Diffuse.b = 0.4f;
    m_globalLight.Ambient.r = 0.4f;
    m_globalLight.Ambient.g = 0.4f;
    m_globalLight.Ambient.b = 0.4f;
    m_globalLight.Ambient.a = 1.0f;
    m_globalLight.Specular = *(D3DCOLORVALUE*)&CVector4D(0.0f, 1.0f, 0.0f, 0.5f);
    m_globalLight.Direction = *(D3DVECTOR*)&CVector(0.0f, 0, -1.0f);
    m_globalLight.Attenuation0 = 0.2f;
    m_globalLight.Attenuation1 = 0.2f;
    m_globalLight.Attenuation2 = 0.2f;
    m_globalLight.Phi = 0.2f;
}

void CFBX::Initialize()
{
    m_pDevice = g_pCore->GetGraphics()->GetDevice();
}

CFBX::~CFBX()
{
}

void CFBX::RemoveScene(CFBXScene* pScene)
{
}

void CFBX::Render()
{
    if (m_pDevice != nullptr)
    {
        for (auto const& pFBXScene : m_sceneList)
        {
            pFBXScene->RenderScene(m_pDevice);
        }
    }
}
