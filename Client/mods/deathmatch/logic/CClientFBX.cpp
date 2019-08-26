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

    ofbx::IScene* pScene = ofbx::load((ofbx::u8*)m_RawDataBuffer.GetData(), m_RawDataBuffer.GetSize(), (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
    if (pScene == nullptr)
        return false;

    m_pFBXScene = (CFBXSceneInterface*)g_pCore->GetFBX()->AddScene(pScene, this);

    return true;
}

// Return true if data looks like FBX file contents
bool CClientFBX::IsFBXData(const SString& strData)
{
    // return strData.length() > 128 && memcmp(strData, "\x10\x00\x00\x00", 4) == 0;
    return strData.length() > 128;
}

//
// void CClientFBX::DrawPreview(const ofbx::Mesh* pMesh, CVector vecPosition, SColor color, float fWidth, bool bPostGUI)
//{
//    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
//    pTempGeometry = pMesh->getGeometry();
//
//    int               size = pTempGeometry->getIndicesCount() - 3;
//    const ofbx::Vec3* pVertices = pTempGeometry->getVertices();
//    const int*        pFaceIndices = pTempGeometry->getFaceIndices();
//    for (int i = 0; i < size; i += 3)
//    {
//        tempVertexPosition[0] = pVertices + pFaceIndices[i];
//        tempVertexPosition[1] = pVertices + pFaceIndices[i + 1];
//        tempVertexPosition[2] = pVertices + pFaceIndices[i + 2];
//        tempVecPos[0].fX = tempVertexPosition[0]->x + vecPosition.fX;
//        tempVecPos[0].fY = tempVertexPosition[0]->y + vecPosition.fY;
//        tempVecPos[0].fZ = tempVertexPosition[0]->z + vecPosition.fZ;
//
//        tempVecPos[1].fX = tempVertexPosition[1]->x + vecPosition.fX;
//        tempVecPos[1].fY = tempVertexPosition[1]->y + vecPosition.fY;
//        tempVecPos[1].fZ = tempVertexPosition[1]->z + vecPosition.fZ;
//
//        tempVecPos[2].fX = tempVertexPosition[2]->x + vecPosition.fX;
//        tempVecPos[2].fY = tempVertexPosition[2]->y + vecPosition.fY;
//        tempVecPos[2].fZ = tempVertexPosition[2]->z + vecPosition.fZ;
//
//        pGraphics->DrawLine3DQueued(tempVecPos[0], tempVecPos[1], fWidth, color, bPostGUI);
//        pGraphics->DrawLine3DQueued(tempVecPos[0], tempVecPos[2], fWidth, color, bPostGUI);
//        pGraphics->DrawLine3DQueued(tempVecPos[1], tempVecPos[2], fWidth, color, bPostGUI);
//    }
//}
//
// void CClientFBX::Render()
//{
//    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();
//    for (auto const& pair : m_templateMap)
//    {
//        /*
//                const void* pVertexStreamZeroData = &primitive.pVecVertices->at(0);
//        uint        uiVertexStreamZeroStride = sizeof(PrimitiveVertice);
//
//        DrawPrimitive(primitive.eType, primitive.pVecVertices->size(), pVertexStreamZeroData, uiVertexStreamZeroStride);
//        */
//        std::vector<CVector> asd;
//        asd.emplace_back(0, 0, 0);
//        asd.emplace_back(10, 0, 0);
//        asd.emplace_back(0, 10, 0);
//        pGraphics->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 3, &asd.at(0), sizeof(CVector));
//        g_pCore->GetConsole()->Printf("draw id %i", pair.first);
//    }
//}

void CClientFBX::LuaGetAllObjectsIds(lua_State* luaVM)
{
    std::vector<unsigned long long> vecIds;
    m_pFBXScene->GetAllObjectsIds(vecIds);
    int i = 1;
    for (unsigned long long iId : vecIds)
    {
        lua_pushnumber(luaVM, i++);
        lua_pushnumber(luaVM, iId);
        lua_settable(luaVM, -3);
    }
}

void CClientFBX::LuaGetTextures(lua_State* luaVM)
{
    for (auto const& pTexture : m_mapTexture)
    {
        lua_pushnumber(luaVM, pTexture.first);
        lua_pushelement(luaVM, pTexture.second);
        lua_settable(luaVM, -3);
    }
}

void CClientFBX::LuaGetMaterials(lua_State* luaVM)
{
    // for (auto const& pMaterial : m_materialList)
    //{
    //    lua_pushnumber(luaVM, (*pMaterial)->id);
    //    lua_pushstring(luaVM, (*pMaterial)->name);
    //    lua_settable(luaVM, -3);
    //}
}

void CClientFBX::LuaGetMeshes(lua_State* luaVM)
{
    // for (auto const& pair : m_meshList)
    //{
    //    lua_pushnumber(luaVM, pair.second->id);
    //    lua_pushstring(luaVM, pair.first.c_str());
    //    lua_settable(luaVM, -3);
    //}
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

void CClientFBX::CreateTexture(unsigned long long ullTextureId, CPixels* pPixels)
{
    if (m_mapTexture.find(ullTextureId) == m_mapTexture.end())
        m_mapTexture[ullTextureId] = g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", pPixels, RDEFAULT, RDEFAULT, RDEFAULT, RFORMAT_UNKNOWN, TADDRESS_WRAP);
}

bool CClientFBX::IsTextureCreated(unsigned long long ullTextureId)
{
    return m_mapTexture.find(ullTextureId) != m_mapTexture.end();
}

CMaterialItem* CClientFBX::GetTextureById(unsigned long long ullTextureId)
{
    if (m_mapTexture.find(ullTextureId) == m_mapTexture.end())
    {
        return nullptr;
    }

    CClientTexture* pTexture = m_mapTexture[ullTextureId];
    if (pTexture != nullptr)
        return pTexture->GetMaterialItem();
    return nullptr;
}
