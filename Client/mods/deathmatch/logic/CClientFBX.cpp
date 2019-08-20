/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFBX.cpp
 *  PURPOSE:     FBX entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

CClientFBX::CClientFBX(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pFBXManager = pManager->GetFBXManager();

    SetTypeName("FBX");

    // Add us to FBX manager's list
    m_pFBXManager->AddToList(this);

    CClientFBXRenderTemplate* pTemplate = new CClientFBXRenderTemplate();
    m_templateMap[nextFreeId] = pTemplate;
}

CClientFBX::~CClientFBX(void)
{
    // Remove us from FBX manager's list
    m_pFBXManager->RemoveFromList(this);
}

bool CClientFBX::LoadFBX(const SString& strFile, bool bIsRawData)
{
    // Should only be called once, directly after construction
    m_bIsRawData = bIsRawData;
    if (!m_bIsRawData)            // If we have actual file
    {
        assert(m_strFbxFilename.empty());

        m_strFbxFilename = strFile;
        if (m_strFbxFilename.empty())
            return false;

        if (!FileExists(m_strFbxFilename))
            return false;

        // if (!g_pCore->GetNetwork()->CheckFile("fbx", m_strDffFilename))
        //    return false;
    }
    else
    {
        m_RawDataBuffer = CBuffer(strFile, strFile.length());
        if (!g_pCore->GetNetwork()->CheckFile("fbx", "", m_RawDataBuffer.GetData(), m_RawDataBuffer.GetSize()))
            return false;
    }

    m_pScene = ofbx::load((ofbx::u8*)m_RawDataBuffer.GetData(), m_RawDataBuffer.GetSize(), (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
    if (m_pScene == nullptr)
        return false;

    m_pRoot = m_pScene->getRoot();
    FixIndices();
    CacheObjects();
    CacheMeshes();
    CacheTextures();
    CacheMaterials();
    return true;
}

void CClientFBX::FixIndices()
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

// Return true if data looks like FBX file contents
bool CClientFBX::IsFBXData(const SString& strData)
{
    // return strData.length() > 128 && memcmp(strData, "\x10\x00\x00\x00", 4) == 0;
    return strData.length() > 128;
}

const char* CClientFBX::GetObjectType(const ofbx::Object const* pObject)
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

void CClientFBX::CacheObjects()
{
    const ofbx::Object* const* pObject;
    for (int i = 0; i < m_pScene->getAllObjectCount(); i++)
    {
        pObject = m_pScene->getAllObjects() + i;
        m_objectList[(*pObject)->id] = pObject;
    }
}

void CClientFBX::CacheMeshes()
{
    if (m_bMeshesCached)
        return;

    m_bMeshesCached = true;

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

void CClientFBX::CacheTextures()
{
    if (m_bTexturesCached)
        return;

    m_bTexturesCached = true;

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
void CClientFBX::CacheMaterials()
{
    if (m_bMaterialsCached)
        return;

    m_bMaterialsCached = true;

    SString                      name;
    const ofbx::Material* const* pMaterial;
    for (int i = 0; i < m_pScene->getMaterialsCount(); i++)
    {
        pMaterial = m_pScene->getMaterials() + i;
        m_materialList.push_back(pMaterial);
    }
}

void CClientFBX::GetMeshPath(const ofbx::Mesh* pMesh, SString& name)
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

void CClientFBX::DrawPreview(const ofbx::Mesh* pMesh, CVector vecPosition, SColor color, float fWidth, bool bPostGUI)
{
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
    pTempGeometry = pMesh->getGeometry();

    int               size = pTempGeometry->getIndicesCount() - 3;
    const ofbx::Vec3* pVertices = pTempGeometry->getVertices();
    const int*        pFaceIndices = pTempGeometry->getFaceIndices();
    for (int i = 0; i < size; i += 3)
    {
        tempVertexPosition[0] = pVertices + pFaceIndices[i];
        tempVertexPosition[1] = pVertices + pFaceIndices[i + 1];
        tempVertexPosition[2] = pVertices + pFaceIndices[i + 2];
        tempVecPos[0].fX = tempVertexPosition[0]->x + vecPosition.fX;
        tempVecPos[0].fY = tempVertexPosition[0]->y + vecPosition.fY;
        tempVecPos[0].fZ = tempVertexPosition[0]->z + vecPosition.fZ;

        tempVecPos[1].fX = tempVertexPosition[1]->x + vecPosition.fX;
        tempVecPos[1].fY = tempVertexPosition[1]->y + vecPosition.fY;
        tempVecPos[1].fZ = tempVertexPosition[1]->z + vecPosition.fZ;

        tempVecPos[2].fX = tempVertexPosition[2]->x + vecPosition.fX;
        tempVecPos[2].fY = tempVertexPosition[2]->y + vecPosition.fY;
        tempVecPos[2].fZ = tempVertexPosition[2]->z + vecPosition.fZ;

        pGraphics->DrawLine3DQueued(tempVecPos[0], tempVecPos[1], fWidth, color, bPostGUI);
        pGraphics->DrawLine3DQueued(tempVecPos[0], tempVecPos[2], fWidth, color, bPostGUI);
        pGraphics->DrawLine3DQueued(tempVecPos[1], tempVecPos[2], fWidth, color, bPostGUI);
    }
}

void CClientFBX::Render()
{
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
    for (auto const& pair : m_templateMap)
    {
        /*
                const void* pVertexStreamZeroData = &primitive.pVecVertices->at(0);
        uint        uiVertexStreamZeroStride = sizeof(PrimitiveVertice);

        DrawPrimitive(primitive.eType, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
        */
        std::vector<CVector> asd;
        asd.emplace_back(0, 0, 0);
        asd.emplace_back(10, 0, 0);
        asd.emplace_back(0, 10, 0);
        pGraphics->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 3, &asd.at(0), sizeof(CVector));
        g_pCore->GetConsole()->Printf("draw id %i", pair.first);
    }
}

void CClientFBX::LuaGetAllObjectsIds(lua_State* luaVM)
{
    for (auto const& pair : m_objectList)
    {
        lua_pushnumber(luaVM, pair.first);
        lua_pushstring(luaVM, GetObjectType(*pair.second));
        lua_settable(luaVM, -3);
    }
}

void CClientFBX::LuaGetTextures(lua_State* luaVM)
{
    for (auto const& pTexture : m_textureList)
    {
        lua_pushnumber(luaVM, pTexture.second->id);
        lua_pushstring(luaVM, pTexture.first.c_str());
        lua_settable(luaVM, -3);
    }
}

void CClientFBX::LuaGetMaterials(lua_State* luaVM)
{
    for (auto const& pMaterial : m_materialList)
    {
        lua_pushnumber(luaVM, (*pMaterial)->id);
        lua_pushstring(luaVM, (*pMaterial)->name);
        lua_settable(luaVM, -3);
    }
}

void CClientFBX::LuaGetMeshes(lua_State* luaVM)
{
    for (auto const& pair : m_meshList)
    {
        lua_pushnumber(luaVM, pair.second->id);
        lua_pushstring(luaVM, pair.first.c_str());
        lua_settable(luaVM, -3);
    }
}

bool CClientFBX::LuaGetObjectProperties(lua_State* luaVM, const ofbx::Object* const* pObject)
{
    lua_newtable(luaVM);
    lua_pushstring(luaVM, "id");
    lua_pushnumber(luaVM, (*pObject)->id);
    lua_settable(luaVM, -3);

    return true;
}

bool CClientFBX::LuaGetMeshProperties(lua_State* luaVM, const ofbx::Mesh const* pMesh)
{
    lua_newtable(luaVM);
    lua_pushstring(luaVM, "id");
    lua_pushnumber(luaVM, pMesh->id);
    lua_settable(luaVM, -3);

    const ofbx::Geometry* pGeometry = pMesh->getGeometry();
    lua_pushstring(luaVM, "vertexCount");
    lua_pushnumber(luaVM, pGeometry->getVertexCount());
    lua_settable(luaVM, -3);
    lua_pushstring(luaVM, "normalCount");
    lua_pushnumber(luaVM, pGeometry->getNormalCount());
    lua_settable(luaVM, -3);
    lua_pushstring(luaVM, "UVCount");

    lua_newtable(luaVM);
    lua_pushnumber(luaVM, 1);
    lua_pushnumber(luaVM, pGeometry->getUVCount(0));
    lua_settable(luaVM, -3);
    lua_pushnumber(luaVM, 2);
    lua_pushnumber(luaVM, pGeometry->getUVCount(2));
    lua_settable(luaVM, -3);
    lua_pushnumber(luaVM, 3);
    lua_pushnumber(luaVM, pGeometry->getUVCount(3));
    lua_settable(luaVM, -3);
    lua_pushnumber(luaVM, 4);
    lua_pushnumber(luaVM, pGeometry->getUVCount(4));
    lua_settable(luaVM, -3);
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "colorCount");
    lua_pushnumber(luaVM, pGeometry->getColorCount());
    lua_settable(luaVM, -3);
    lua_pushstring(luaVM, "tangentCount");
    lua_pushnumber(luaVM, pGeometry->getTangentCount());
    lua_settable(luaVM, -3);
    lua_pushstring(luaVM, "triangleCount");
    lua_pushnumber(luaVM, pGeometry->getIndicesCount() / 3);
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "materialCount");
    lua_pushnumber(luaVM, pMesh->getMaterialCount());
    lua_settable(luaVM, -3);
    return true;
}

bool CClientFBX::LuaRawGetVertices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop)
{
    const ofbx::Geometry* pGeometry = pMesh->getGeometry();
    const ofbx::Vec3*     pVertex;

    if (iStart < 0 && iStop < 0)            // get all
    {
        lua_newtable(luaVM);

        for (int i = 0; i < pGeometry->getVertexCount(); i++)
        {
            pVertex = pGeometry->getVertices() + i;
            lua_pushnumber(luaVM, i + 1);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, pVertex->x);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, pVertex->y);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, pVertex->z);
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);
        }
        return true;
    }
    if (iStart >= 0 && iStop < 0)            // select only 1 vertex
    {
        if (pGeometry->getVertexCount() > iStart)
        {
            pVertex = pGeometry->getVertices() + iStart;
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, pVertex->x);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, pVertex->y);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, pVertex->z);
            lua_settable(luaVM, -3);
            return true;
        }
    }
    else if (iStart >= 0 && iStop >= 0)            // get range
    {
        if (iStart < iStop)
        {
            if (pGeometry->getVertexCount() > iStart && pGeometry->getVertexCount() > iStop)
            {
                lua_newtable(luaVM);

                int id = 0;
                for (int i = iStart; i <= iStop; i++)
                {
                    pVertex = pGeometry->getVertices() + i;
                    lua_pushnumber(luaVM, ++id);
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, pVertex->x);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, pVertex->y);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, pVertex->z);
                    lua_settable(luaVM, -3);
                    lua_settable(luaVM, -3);
                }
                return true;
            }
        }
    }

    return false;
}

bool CClientFBX::LuaRawGetMaterials(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop)
{
    const ofbx::Material* pMaterial;
    const ofbx::Texture*  pTexture;
    const ofbx::Vec3*     pVertex;
    ofbx::DataView        file;

    if (iStart < 0 && iStop < 0)            // get all
    {
        lua_newtable(luaVM);
        SString strFileName;
        for (int i = 0; i < pMesh->getMaterialCount(); i++)
        {
            pMaterial = pMesh->getMaterial(i);
            int diffuseColor =
                pMaterial->getDiffuseColor().r * 255 * 255 * 255 + pMaterial->getDiffuseColor().g * 255 * 255 + pMaterial->getDiffuseColor().b * 255;
            int specularColor =
                pMaterial->getSpecularColor().r * 255 * 255 * 255 + pMaterial->getSpecularColor().g * 255 * 255 + pMaterial->getSpecularColor().b * 255;

            lua_pushnumber(luaVM, i + 1);
            lua_newtable(luaVM);
            lua_pushstring(luaVM, "id");
            lua_pushnumber(luaVM, pMaterial->id);
            lua_settable(luaVM, -3);
            lua_pushstring(luaVM, "name");
            lua_pushstring(luaVM, pMaterial->name);
            lua_settable(luaVM, -3);
            lua_pushstring(luaVM, "diffuseColor");
            lua_pushnumber(luaVM, diffuseColor);
            lua_settable(luaVM, -3);
            lua_pushstring(luaVM, "specularColor");
            lua_pushnumber(luaVM, specularColor);
            lua_settable(luaVM, -3);
            pTexture = pMaterial->getTexture(ofbx::Texture::TextureType::DIFFUSE);
            if (pTexture != nullptr)
            {
                lua_pushstring(luaVM, "diffuseTexture");
                strFileName = SString("%s", pTexture->getFileName().begin);
                strFileName = strFileName.substr(0, strFileName.size() - strlen((const char*)pTexture->getFileName().end));
                lua_pushstring(luaVM, strFileName.c_str());
                lua_settable(luaVM, -3);

                /*             lua_pushstring(luaVM, "diffuseTextureContent");
                             strFileName = SString("%s", pTexture->getContent().begin);
                             strFileName = strFileName.substr(0, strFileName.size() - strlen((const char*)pTexture->getContent().end));
                             lua_pushstring(luaVM, strFileName.c_str());
                             lua_settable(luaVM, -3);*/
                // Content
            }

            lua_pushstring(luaVM, "normalTexture");
            pTexture = pMaterial->getTexture(ofbx::Texture::TextureType::NORMAL);
            if (pTexture != nullptr)
            {
                strFileName = SString("%s", pTexture->getFileName().begin);
                strFileName = strFileName.substr(0, strFileName.size() - strlen((const char*)pTexture->getFileName().end));
                lua_pushstring(luaVM, strFileName.c_str());
            }
            else
                lua_pushboolean(luaVM, false);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "specularTexture");
            pTexture = pMaterial->getTexture(ofbx::Texture::TextureType::SPECULAR);
            if (pTexture != nullptr)
            {
                strFileName = SString("%s", pTexture->getFileName().begin);
                strFileName = strFileName.substr(0, strFileName.size() - strlen((const char*)pTexture->getFileName().end));
                lua_pushstring(luaVM, strFileName.c_str());
            }
            else
                lua_pushboolean(luaVM, false);

            lua_settable(luaVM, -3);

            lua_settable(luaVM, -3);
        }
        return true;
    }
    /*
    if (iStart >= 0 && iStop < 0)            // select only 1 vertex
    {
        lua_pushnumber(luaVM, pGeometry->getFaceIndices()[iStart]);
    }
    else if (iStart >= 0 && iStop >= 0)            // get range
    {
        if (iStart < iStop)
        {
            if (pMesh->getMaterialCount() > iStart && pMesh->getMaterialCount() > iStop)
            {
                lua_newtable(luaVM);

                int id = 0;
                for (int i = iStart; i <= iStop; i++)
                {
                    lua_pushnumber(luaVM, ++id);
                    lua_pushnumber(luaVM, abs(pGeometry->getFaceIndices()[i]));
                    lua_settable(luaVM, -3);
                }
            }
        }
    }
    */
    return false;
}

bool CClientFBX::LuaRawGetIndices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop)
{
    const ofbx::Geometry* pGeometry = pMesh->getGeometry();
    const ofbx::Vec3*     pVertex;
    if (iStart < 0 && iStop < 0)            // get all
    {
        lua_newtable(luaVM);

        for (int i = 0; i < pGeometry->getIndicesCount() - 1; i++)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_pushnumber(luaVM, pGeometry->getFaceIndices()[i]);
            lua_settable(luaVM, -3);
        }
    }
    if (iStart >= 0 && iStop < 0)            // select only 1 vertex
    {
        lua_pushnumber(luaVM, pGeometry->getFaceIndices()[iStart]);
    }
    else if (iStart >= 0 && iStop >= 0)            // get range
    {
        if (iStart < iStop)
        {
            if (pGeometry->getIndicesCount() > iStart && pGeometry->getIndicesCount() > iStop)
            {
                lua_newtable(luaVM);

                int id = 0;
                for (int i = iStart; i <= iStop; i++)
                {
                    lua_pushnumber(luaVM, ++id);
                    lua_pushnumber(luaVM, abs(pGeometry->getFaceIndices()[i]));
                    lua_settable(luaVM, -3);
                }
            }
        }
    }

    return false;
}
