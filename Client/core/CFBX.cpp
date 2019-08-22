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

FBXBuffer::FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList)
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
}

void CFBXTemplate::Render(IDirect3DDevice9* pDevice, CFBXScene* pScene)
{
    // select which vertex format we are using
    pDevice->SetFVF(CUSTOMFVF);
    FBXBuffer* pBuffer;
    for (auto const& object : m_objectList)
    {
        pBuffer = pScene->GetFBXBuffer(object->ullObjectId);
        if (pBuffer != nullptr)
        {
            // select the vertex buffer to display
            pDevice->SetStreamSource(0, pBuffer->v_buffer, 0, sizeof(FBXVertex));
            pDevice->SetIndices(pBuffer->i_buffer);

            // copy the vertex buffer to the back buffer
            pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pBuffer->indicesCount / 3);
            //pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pBuffer->indicesCount, 0, pBuffer->indicesCount / 3);
        }
    }
}

void CFBXTemplate::AddTemplateObject(CFBXTemplateObject* pObject)
{
    m_objectList.push_back(pObject);
}

unsigned int CFBXScene::AddTemplete(CFBXTemplate* pTemplate)
{
    m_templateMap[uiNextFreeTemplateId] = pTemplate;
    return uiNextFreeTemplateId++;
}

FBXBuffer* CFBXScene::GetFBXBuffer(unsigned long long ullId)
{
    if (m_mapMeshBuffer.find(ullId) != m_mapMeshBuffer.end())
    {
        return m_mapMeshBuffer[ullId];
    }
    return nullptr;
}

bool CFBXScene::CreateFBXBuffer(const ofbx::Object* const* pObject)
{
    if ((*pObject)->getType() != ofbx::Object::Type::GEOMETRY)
        return false;

    const ofbx::Geometry* const* pGeometry = (const ofbx::Geometry* const*)pObject;
    std::vector<FBXVertex>       vecVertices;
    std::vector<int>             vecIndices((*pGeometry)->getIndicesCount());

    const ofbx::Vec3* vertices = (*pGeometry)->getVertices();
    const ofbx::Vec3* vertex;
    for (int i = 0; i < (*pGeometry)->getVertexCount(); i++)
    {
        vertex = vertices + i;
        vecVertices.emplace_back(vertex->x, vertex->y, vertex->z, D3DCOLOR_XRGB((i % 2 == 0) ? 0 : 255, (i % 2 == 0) ? 255 : 0, 0));
    }
    memcpy(vecIndices.data(), (*pGeometry)->getFaceIndices(), sizeof(int) * vecIndices.size());

    FBXBuffer* pBuffer = new FBXBuffer(vecVertices, vecIndices);
    AddBuffer((*pObject)->id, pBuffer);
    return true;
}

bool CFBXScene::AddBuffer(unsigned long long ullObjectId, FBXBuffer* pBuffer)
{
    if (m_mapMeshBuffer.find(ullObjectId) != m_mapMeshBuffer.end())
        return false;

    m_mapMeshBuffer[ullObjectId] = pBuffer;
    return true;
}

CFBXScene::CFBXScene(ofbx::IScene* scene) : m_pScene(scene)
{
    m_pRoot = m_pScene->getRoot();
    FixIndices();
    CacheObjects();
    CacheMeshes();
    CacheTextures();
    CacheMaterials();

    // std::vector<FBXVertex> vertices = {
    //    {0, 0, 3, D3DCOLOR_XRGB(255, 0, 0)}, {5, 0, 3, D3DCOLOR_XRGB(255, 0, 0)}, {5, 5, 3, D3DCOLOR_XRGB(255, 0, 0)},
    //    {3, 0, 5, D3DCOLOR_XRGB(255, 0, 0)}, {5, 0, 5, D3DCOLOR_XRGB(255, 0, 0)}, {5, 2, 5, D3DCOLOR_XRGB(255, 0, 0)},
    //};
    // std::vector<int> indices;
    // indices.emplace_back(0);
    // indices.emplace_back(1);
    // indices.emplace_back(2);
    // indices.emplace_back(3);
    // indices.emplace_back(4);
    // indices.emplace_back(5);
    // indices.emplace_back(1);
    // indices.emplace_back(4);
    // indices.emplace_back(5);

    // std::vector<FBXVertex> vertices;
    // std::vector<int>       indices;
    // for (int x = 0; x < 100; x++)
    //{
    //    for (int y = 0; y < 100; y++)
    //    {
    //        for (int z = 0; z < 100; z++)
    //        {
    //            indices.emplace_back(vertices.size());
    //            vertices.emplace_back(x + y, x, 5 + z, D3DCOLOR_XRGB(255, 0, 0));
    //            indices.emplace_back(vertices.size());
    //            vertices.emplace_back(x + y, x + 1, 5 + z, D3DCOLOR_XRGB(0, 255, 0));
    //            indices.emplace_back(vertices.size());
    //            vertices.emplace_back(x + 1 + y, x + 1, 5 + z, D3DCOLOR_XRGB(0, 0, 255));
    //        }
    //    }
    //}

    CFBXTemplate* pTemplate = new CFBXTemplate();
    for (const auto& pair : m_objectList)
    {
        if (CreateFBXBuffer(pair.second))
        {
            CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject((*pair.second)->id);
            pTemplate->AddTemplateObject(pTemplateObject);
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
    }
}
void CFBXScene::CacheMaterials()
{
    SString                      name;
    const ofbx::Material* const* pMaterial;
    for (int i = 0; i < m_pScene->getMaterialsCount(); i++)
    {
        pMaterial = m_pScene->getMaterials() + i;
        m_materialList.push_back(pMaterial);
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

CFBXScene* CFBX::AddScene(ofbx::IScene* pScene)
{
    CFBXScene* pFBXScene = new CFBXScene(pScene);
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
