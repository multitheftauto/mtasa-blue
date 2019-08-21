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

CFBXScene::CFBXScene(ofbx::IScene* scene) : m_pScene(scene)
{
    m_pRoot = m_pScene->getRoot();
    FixIndices();
    CacheObjects();
    CacheMeshes();
    CacheTextures();
    CacheMaterials();
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

CFBX::CFBX()
{

}

CFBX::~CFBX()
{

}

void CFBX::RemoveScene(CFBXScene* pScene)
{
}

void CFBX::Render()
{
    int i = 0;
    for (auto const& pFBXScene : m_sceneList)
    {
        g_pCore->GetConsole()->Printf("render! %i", i++);
    }
}
