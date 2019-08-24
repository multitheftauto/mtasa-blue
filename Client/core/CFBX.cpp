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
    IDirect3DDevice9* m_pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid;
    m_pDevice->CreateVertexBuffer(vecVertexList.size() * sizeof(FBXVertex), D3DUSAGE_WRITEONLY, CUSTOMFVF, D3DPOOL_MANAGED, &v_buffer, NULL);
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecVertexList.data(), vecVertexList.size() * sizeof(FBXVertex));
    v_buffer->Unlock();

    m_pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &i_buffer, NULL);
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    i_buffer->Unlock();

    vertexCount = vecVertexList.size();
    indicesCount = vecIndexList.size();
    this->ullMaterialId = ullMaterialId;
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
        if ((*pMesh)->getMaterialCount() != 0)
        {
            id = (*pMesh)->getMaterial(0)->id;
        }
        bufferList.push_back(new FBXBuffer(vecVertexList, vecIndexList, id));
    }
}

void CFBXTemplate::Render(IDirect3DDevice9* pDevice, CFBXScene* pScene)
{
    IDirect3DStateBlock9* pSavedStateBlock = nullptr;
    pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedStateBlock);

    // select which vertex format we are using
    pDevice->SetFVF(CUSTOMFVF);
    FBXObjectBuffer* pObjectBuffer;
    D3DMATRIX*       pObjectMatrix = new D3DMATRIX();
    CTextureItem*    pTextureItem;
    // pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    // pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    // pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
    // pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DRS_DIFFUSEMATERIALSOURCE);
    // pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    // pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));
    for (auto const& object : m_objectList)
    {
        pViewMatrix->GetBuffer((float*)pObjectMatrix);
        pDevice->SetTransform(D3DTS_WORLDMATRIX(0), pObjectMatrix);
        pObjectBuffer = pScene->GetFBXBuffer(object->ullObjectId);
        if (pObjectBuffer != nullptr)
        {
            object->pViewMatrix->GetBuffer((float*)pObjectMatrix);
            // pDevice->SetMaterial(&object->material);
            pDevice->MultiplyTransform(D3DTS_WORLDMATRIX(0), pObjectMatrix);
            for (auto const& pBuffer : pObjectBuffer->bufferList)
            {
                if (pBuffer->ullMaterialId != 0)
                {
                    pTextureItem = pScene->GetTexture(pBuffer->ullMaterialId);
                    // Set texture addressing mode
                    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pTextureItem->m_TextureAddress);
                    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pTextureItem->m_TextureAddress);

                    if (pTextureItem->m_TextureAddress == TADDRESS_BORDER)
                        pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pTextureItem->m_uiBorderColor);

                    pDevice->SetTexture(0, pTextureItem->m_pD3DTexture);
                }
                // pDevice->SetLight(0, &object->light);
                // pDevice->LightEnable(0, TRUE);

                pDevice->SetStreamSource(0, pBuffer->v_buffer, 0, sizeof(FBXVertex));
                pDevice->SetIndices(pBuffer->i_buffer);

                pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pBuffer->indicesCount / 3);
            }
        }
    }
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

void CFBXTemplate::AddTemplateObject(CFBXTemplateObject* pObject)
{
    m_objectList.push_back(pObject);
}

CFBXTemplateObject::CFBXTemplateObject(unsigned long long ullObjectId) : ullObjectId(ullObjectId)
{
    pViewMatrix = new CMatrix();

    ZeroMemory(&material, sizeof(D3DMATERIAL9));                      // clear out the struct for use
    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);             // set diffuse color to white
    material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);             // set ambient color to white
    material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);            // set ambient color to white

    ZeroMemory(&light, sizeof(light));                            // clear out the light struct for use
    light.Type = D3DLIGHT_DIRECTIONAL;                            // make the light type 'directional light'
    light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);            // set the light's color
    light.Direction = D3DXVECTOR3(1.0f, 0.3f, 1.0f);
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
            vecVertices.emplace_back(vertex->x, vertex->y, vertex->z, normal->x, normal->y, normal->z, dColor, uv->x, uv->y);
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
            vecVertices.emplace_back(vertex->x, vertex->y, vertex->z, normal->x, normal->y, normal->z, dColor, uv->x, uv->y);
        }
    }

    memcpy(vecIndices.data(), pGeometry->getFaceIndices(), sizeof(int) * vecIndices.size());
    memcpy(vecMaterials.data(), pGeometry->getMaterials(), sizeof(int) * vecMaterials.size());

    FBXObjectBuffer* pBuffer = new FBXObjectBuffer(vecVertices, vecIndices, vecMaterials, pMesh);
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

CTextureItem* CFBXScene::GetTexture(unsigned long long ullMaterialId)
{
    const ofbx::Material* const* pMaterial = m_materialList[ullMaterialId];
    if (pMaterial == nullptr)
        return nullptr;

    const ofbx::Texture* pTexture = (*pMaterial)->getTexture(ofbx::Texture::TextureType::DIFFUSE);
    if (pTexture == nullptr)
        return nullptr;

    SString strTextureName = m_textureNamesList[pTexture->id];
    if (!pClientFBXInterface->IsTextureCreated(strTextureName))
    {
        std::vector<char> textureRawData = m_textureContentList[pTexture->id];
        CPixels*          pPixels = new CPixels();

        pPixels->SetSize(textureRawData.size());
        memcpy(pPixels->GetData(), textureRawData.data(), pPixels->GetSize());
        pClientFBXInterface->CreateTexture(strTextureName, pPixels);
        delete pPixels;
    }
    CMaterialItem* pMaterialItem = pClientFBXInterface->GetTextureByName(strTextureName);
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

    // test code, remove later
    CFBXTemplate* pTemplate = new CFBXTemplate();
    pTemplate->pViewMatrix->SetPosition(CVector(0, 0, 50));
    pTemplate->pViewMatrix->SetRotation(CVector(0, 0, 0));
    pTemplate->pViewMatrix->SetScale(CVector(1, 1, 1));
    int i = 0;
    for (const auto& pair : m_objectList)
    {
        if (CreateFBXBuffer(pair.second))
        {
            CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject((*pair.second)->id);
            pTemplateObject->pViewMatrix->SetPosition(CVector(i * 20, 0, 0));
            pTemplateObject->pViewMatrix->SetRotation(CVector(i * 20, 0, 0));
            pTemplateObject->pViewMatrix->SetScale(CVector(1, 1, 1));
            pTemplate->AddTemplateObject(pTemplateObject);
            i++;
        }
        if ((*pair.second)->getType() == ofbx::Object::Type::TEXTURE)
        {
        }
    }
    AddTemplete(pTemplate);
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

void CFBXScene::CacheMeshes()
{
    SString           name;
    const ofbx::Mesh* pMesh;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        GetMeshPath(pMesh, name);
        if (name.compare("") != 0)            // eliminate some unnamed objects like BaseLayers
            m_meshList[name] = pMesh;
    }
}

void CFBXScene::GetMeshPath(const ofbx::Mesh* pMesh, SString& name)
{
    std::vector<SString> vecNames;

    int uknownId = 0;

    const ofbx::Object const* pObject = (const ofbx::Object const*)pMesh;
    vecNames.push_back(pObject->name);
    while (pObject = pObject->getParent())
    {
        if (pObject == nullptr)
            break;

        vecNames.push_back(pObject->name);
    }

    std::reverse(std::begin(vecNames), std::end(vecNames));
    name = SString::Join("/", vecNames, 1 /* 1 = ignore RootNode */, vecNames.size());
}

void CFBXScene::CacheTextures()
{
    SString                     name;
    const ofbx::DataView const* pFilePath;
    const ofbx::DataView const* pContent;
    const ofbx::Texture*        pTexture;
    for (int i = 0; i < m_pScene->getTexturesCount(); i++)
    {
        pFilePath = *(m_pScene->getTextureFilePath() + i);
        pContent = *(m_pScene->getTextureContent() + i);
        pTexture = *(m_pScene->getTextures() + i);

        SString strFilePath = (const char*)pFilePath->begin;
        strFilePath = strFilePath.substr(0, pFilePath->end - pFilePath->begin);

        std::vector<char> vecContent;
        vecContent.resize(pContent->end - pContent->begin - 4);
        memcpy(vecContent.data(), pContent->begin + 4, vecContent.size());

        m_textureContentList[pTexture->id] = vecContent;
        m_textureList[strFilePath] = pTexture;
        m_textureNamesList[pTexture->id] = strFilePath;
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
    SString                      name;
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

void CFBXScene::Render(IDirect3DDevice9* pDevice)
{
    for (auto const& pair : m_templateMap)
    {
        pair.second->Render(pDevice, this);
    }
}

const char* CFBXScene::GetObjectType(const ofbx::Object const* pObject)
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

CFBXScene* CFBX::AddScene(ofbx::IScene* pScene, CClientFBXInterface* pClientFBXInterface)
{
    CFBXScene* pFBXScene = new CFBXScene(pScene, pClientFBXInterface);

    m_sceneList.push_back(pFBXScene);
    return pFBXScene;
}

bool CFBX::HasAnyFBXLoaded()
{
    return m_sceneList.size() > 0;
}

CFBX::CFBX()
{
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
    for (auto const& pFBXScene : m_sceneList)
    {
        pFBXScene->Render(m_pDevice);
    }
}
