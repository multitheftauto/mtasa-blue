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
class CScriptArgReader;

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
    const ofbx::Object* const* pObject;
    for (unsigned long long ullId : vecIds)
    {
        pObject = m_pFBXScene->GetObjectById(ullId);
        lua_pushnumber(luaVM, ullId);
        lua_pushstring(luaVM, g_pCore->GetFBX()->GetObjectType(*m_pFBXScene->GetObjectById(ullId)));
        lua_settable(luaVM, -3);
    }
}

bool CClientFBX::LuaGetTemplateProperties(lua_State* luaVM, unsigned int uiId, eFBXTemplateProperty eProperty)
{
    if (!m_pFBXScene->IsTemplateValid(uiId))
    {
        return false;
    }

    CVector vector;
    switch (eProperty)
    {
        case FBX_TEMPLATE_PROPERTY_POSITION:
            m_pFBXScene->GetTemplatePosition(uiId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
        case FBX_TEMPLATE_PROPERTY_ROTATION:
            m_pFBXScene->GetTemplateRotation(uiId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
        case FBX_TEMPLATE_PROPERTY_SCALE:
            m_pFBXScene->GetTemplateScale(uiId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
    }
    return true;
}

bool CClientFBX::LuaGetTemplateModelProperties(lua_State* luaVM, unsigned int uiId, unsigned int uiModelId, eFBXTemplateModelProperty eProperty)
{
    if (!m_pFBXScene->IsTemplateValid(uiId))
    {
        return false;
    }

    if (!m_pFBXScene->IsTemplateModelValid(uiId, uiModelId))
    {
        return false;
    }

    CVector vector;
    switch (eProperty)
    {
        case FBX_TEMPLATE_PROPERTY_POSITION:
            m_pFBXScene->GetTemplateModelPosition(uiId, uiModelId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
        case FBX_TEMPLATE_PROPERTY_ROTATION:
            m_pFBXScene->GetTemplateModelRotation(uiId, uiModelId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
        case FBX_TEMPLATE_PROPERTY_SCALE:
            m_pFBXScene->GetTemplateModelScale(uiId, uiModelId, vector);
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vector.fX);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vector.fY);
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vector.fZ);
            lua_settable(luaVM, -3);
            break;
    }
    return true;
}

bool CClientFBX::LuaSetTemplateModelProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, unsigned int uiModelId,
                                               eFBXTemplateModelProperty eProperty)
{
    if (!m_pFBXScene->IsTemplateValid(uiId))
    {
        return false;
    }

    CVector vector;
    switch (eProperty)
    {
        case FBX_TEMPLATE_MODEL_PROPERTY_POSITION:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplateModelPosition(uiId, uiModelId, vector);
        case FBX_TEMPLATE_MODEL_PROPERTY_ROTATION:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplateModelRotation(uiId, uiModelId, vector);
            break;
        case FBX_TEMPLATE_MODEL_PROPERTY_SCALE:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplateModelScale(uiId, uiModelId, vector);
            break;
    }
    return true;
}

bool CClientFBX::LuaSetTemplateProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, eFBXTemplateProperty eProperty)
{
    if (!m_pFBXScene->IsTemplateValid(uiId))
    {
        return false;
    }

    CVector vector;
    switch (eProperty)
    {
        case FBX_TEMPLATE_PROPERTY_POSITION:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplatePosition(uiId, vector);
        case FBX_TEMPLATE_PROPERTY_ROTATION:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplateRotation(uiId, vector);
            break;
        case FBX_TEMPLATE_PROPERTY_SCALE:
            argStream.ReadVector3D(vector);
            if (argStream.HasErrors())
                return false;
            m_pFBXScene->SetTemplateScale(uiId, vector);
            break;
    }
    return true;
}

bool CClientFBX::AddMeshToTemplate(lua_State* luaVM, unsigned int uiId, unsigned long long ullMesh, unsigned long long ullParentMesh, CVector vecPosition,
                                   unsigned int& uiObjectId)
{
    if (!m_pFBXScene->IsTemplateValid(uiId))
    {
        return false;
    }

    const ofbx::Object* const* pObject = m_pFBXScene->GetObjectById(ullMesh);
    if (pObject == nullptr || *pObject == nullptr)
        return false;

    if ((*pObject)->getType() != ofbx::Object::Type::MESH)
        return false;

    uiObjectId = m_pFBXScene->AddMeshToTemplate(uiId, ullMesh);

    m_pFBXScene->SetTemplateModelPosition(uiId, uiObjectId, vecPosition);
    return true;
}

bool CClientFBX::AddTemplate(unsigned int uiCopyFromTemplateId, unsigned int& uiNewTemplateId)
{
    if (uiCopyFromTemplateId != 0)
        if (!m_pFBXScene->IsTemplateValid(uiCopyFromTemplateId))
        {
            return false;
        }

    uiNewTemplateId = m_pFBXScene->CreateTemplate();
    return true;
}

void CClientFBX::RemoveTemplate(unsigned int uiTemplateId)
{
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

bool CClientFBX::LuaGetObjectProperties(lua_State* luaVM, const ofbx::Object* const* pObject, eFBXObjectProperty eProperty)
{
    if (pObject == nullptr)
    {
        lua_pushboolean(luaVM, false);
        return false;
    }
    ofbx::Vec3            vec3;
    const ofbx::Geometry* pGeometry;
    const ofbx::Mesh*     pMesh;
    const ofbx::Material* pMaterial;
    const ofbx::Texture*  pTexture;
    bool                  bSupportsTransform = false;
    ofbx::Object*         pObjectParent;
    ofbx::Object::Type    eType = (*pObject)->getType();
    CVector               min, max;
    float                 radius;
    switch (eProperty)
    {
        case FBX_OBJECT_PROPERTY_TYPE:
            lua_pushstring(luaVM, g_pCore->GetFBX()->GetObjectType(*pObject));
            return true;
        case FBX_OBJECT_PROPERTY_PARENT:
            pObjectParent = (*pObject)->getParent();
            if (pObjectParent != nullptr)
            {
                if (pObjectParent->id != 0)
                    lua_pushnumber(luaVM, pObjectParent->id);
                else
                    lua_pushboolean(luaVM, false);
            }
            return true;
        case FBX_OBJECT_PROPERTY_COLOR_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getColorCount());
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_INDEX_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getIndicesCount());
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_MATERIAL_FACE_ID_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getMaterialCount());
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_NORMAL_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getNormalCount());
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_VERTEX_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getVertexCount());
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_UV_COUNT:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    pGeometry = (const ofbx::Geometry*)(*pObject);
                    lua_pushnumber(luaVM, pGeometry->getUVCount(0));
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_BOUNDING_BOX:
            switch (eType)
            {
                case ofbx::Object::Type::GEOMETRY:
                    m_pFBXScene->GetBoundingBox((*pObject)->id,min, max, radius);
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, min.fX);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, min.fY);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, min.fZ);
                    lua_settable(luaVM, -3);

                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);

                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, max.fX);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, max.fY);
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, max.fZ);
                    lua_settable(luaVM, -3);

                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, radius);
                    lua_settable(luaVM, -3);
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_DIFFUSE_COLOR:
            switch (eType)
            {
                case ofbx::Object::Type::MATERIAL:
                    pMaterial = (const ofbx::Material*)(*pObject);
                    lua_pushnumber(luaVM,
                                   SColorRGBA(pMaterial->getDiffuseColor().r, pMaterial->getDiffuseColor().g, pMaterial->getDiffuseColor().b, 255).ulARGB);
                    return true;
                default:
                    return false;            // unsupported property
            }
        case FBX_OBJECT_PROPERTY_SPECULAR_COLOR:
            switch (eType)
            {
                case ofbx::Object::Type::MATERIAL:
                    pMaterial = (const ofbx::Material*)(*pObject);
                    lua_pushnumber(luaVM,
                                   SColorRGBA(pMaterial->getSpecularColor().r, pMaterial->getSpecularColor().g, pMaterial->getSpecularColor().b, 255).ulARGB);
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_DIFFUSE_TEXTURE:
            switch (eType)
            {
                case ofbx::Object::Type::MATERIAL:
                    pMaterial = (const ofbx::Material*)(*pObject);
                    pTexture = pMaterial->getTexture(ofbx::Texture::DIFFUSE);
                    if (pTexture != nullptr)
                        lua_pushnumber(luaVM, pTexture->id);
                    else
                        lua_pushboolean(luaVM, false);

                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_SPECULAR_TEXTURE:
            switch (eType)
            {
                case ofbx::Object::Type::MATERIAL:
                    pMaterial = (const ofbx::Material*)(*pObject);
                    pTexture = pMaterial->getTexture(ofbx::Texture::SPECULAR);
                    if (pTexture != nullptr)
                        lua_pushnumber(luaVM, pTexture->id);
                    else
                        lua_pushboolean(luaVM, false);
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_NORMAL_TEXTURE:
            switch (eType)
            {
                case ofbx::Object::Type::MATERIAL:
                    pMaterial = (const ofbx::Material*)(*pObject);
                    pTexture = pMaterial->getTexture(ofbx::Texture::NORMAL);
                    if (pTexture != nullptr)
                        lua_pushnumber(luaVM, pTexture->id);
                    else
                        lua_pushboolean(luaVM, false);
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_GEOMETRY:
            switch (eType)
            {
                case ofbx::Object::Type::MESH:
                    pMesh = (const ofbx::Mesh*)(*pObject);
                    pGeometry = pMesh->getGeometry();
                    if (pGeometry == nullptr)
                        lua_pushboolean(luaVM, false);
                    else
                        lua_pushnumber(luaVM, pGeometry->id);
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
        case FBX_OBJECT_PROPERTY_MATERIALS:
            switch (eType)
            {
                case ofbx::Object::Type::MESH:
                    pMesh = (const ofbx::Mesh*)(*pObject);
                    lua_newtable(luaVM);
                    for (int i = 0; i < pMesh->getMaterialCount(); i++)
                    {
                        lua_pushnumber(luaVM, i + 1);
                        lua_pushnumber(luaVM, pMesh->getMaterial(i)->id);
                        lua_settable(luaVM, -3);
                    }
                    return true;
                default:
                    return false;            // unsupported property
            }
            break;
    }

    if (eType == ofbx::Object::Type::GEOMETRY)
    {
        double  m[16] = {};
        float   array[16];
        CMatrix matrix, matrixLocal;
        CVector vector;
        memcpy(m, (*pObject)->getLocalTransform().m, sizeof(double) * 16);
        for (char i = 0; i < 16; i++)
            array[i] = (float)m[i];

        matrixLocal = CMatrix(array);

        memcpy(m, (*pObject)->getGlobalTransform().m, sizeof(double) * 16);
        for (char i = 0; i < 16; i++)
            array[i] = (float)m[i];

        matrix = CMatrix(array);
        switch (eProperty)
        {
            case FBX_OBJECT_PROPERTY_POSITION:
                vector = matrix.GetPosition();
                break;
            case FBX_OBJECT_PROPERTY_ROTATION:
                vector = matrix.GetRotation();
                break;
            case FBX_OBJECT_PROPERTY_SCALE:
                vector = matrix.GetScale();
                break;
            case FBX_OBJECT_PROPERTY_LOCAL_POSITION:
                vector = matrixLocal.GetPosition();
                break;
            case FBX_OBJECT_PROPERTY_LOCAL_ROTATION:
                vector = matrixLocal.GetRotation();
                break;
            case FBX_OBJECT_PROPERTY_LOCAL_SCALE:
                vector = matrixLocal.GetScale();
                break;
            default:
                return false;            // unsupported property
        }

        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, vector.fX);
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, vector.fY);
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, vector.fZ);
        lua_settable(luaVM, -3);
        return true;
    }

    return false;
}

bool CClientFBX::RenderTemplate(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale)
{
    if (g_pCore->IsWindowMinimized())
        return false;

    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
        return false;

    m_pFBXScene->AddToRenderQueue(uiTemplateId, vecPosition, vecRotation, vecScale);
    return true;
}

bool CClientFBX::ApplyTemplateToElement(unsigned int uiTemplateId, CDeathmatchObject* pElement)
{
    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
        return false;

    if (m_mapElementRenderLoop.count(pElement) == 0)
    {
        m_mapElementRenderLoop[pElement] = std::vector<unsigned int>{uiTemplateId};
        return true;
    }
    else
    {
        for (auto const& templateId : m_mapElementRenderLoop[pElement])
            if (templateId == uiTemplateId)
                return false;

        m_mapElementRenderLoop[pElement].push_back(uiTemplateId);
    }

    return true;
}
bool CClientFBX::RemoveTemplateFromElement(unsigned int uiTemplateId, CDeathmatchObject* pElement)
{
    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
        return false;

    if (m_mapElementRenderLoop.count(pElement) == 0)
    {
        return false;
    }

    for (auto const& templateId : m_mapElementRenderLoop[pElement])
        if (templateId == uiTemplateId)
        {
            ListRemove(m_mapElementRenderLoop[pElement], templateId);
            if (m_mapElementRenderLoop[pElement].size() == 0)
            {
                m_mapElementRenderLoop.erase(pElement);
            }
            return true;
        }

    return false;
}

bool CClientFBX::ApplyTemplateToModel(unsigned int uiTemplateId, unsigned long ulModel)
{
    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
        return false;

    if (m_mapModelRenderLoop.count(ulModel) == 0)
    {
        m_mapModelRenderLoop[ulModel] = std::vector<unsigned int>{uiTemplateId};
        return true;
    }
    else
    {
        for (auto const& templateId : m_mapModelRenderLoop[ulModel])
            if (templateId == uiTemplateId)
                return false;

        m_mapModelRenderLoop[ulModel].push_back(uiTemplateId);
    }

    return true;
}

bool CClientFBX::RemoveTemplateFromModel(unsigned int uiTemplateId, unsigned long ulModel)
{
    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
        return false;

    if (m_mapModelRenderLoop.count(ulModel) == 0)
    {
        return false;
    }

    for (auto const& templateId : m_mapModelRenderLoop[ulModel])
        if (templateId == uiTemplateId)
        {
            ListRemove(m_mapModelRenderLoop[ulModel], templateId);
            if (m_mapModelRenderLoop[ulModel].size() == 0)
            {
                m_mapModelRenderLoop.erase(ulModel);
            }
            return true;
        }

    return false;
}

bool CClientFBX::LuaGetAllTemplates(lua_State* luaVM)
{
    std::vector<unsigned int> vecIds;
    m_pFBXScene->GetAllTemplatesIds(vecIds);
    lua_newtable(luaVM);
    for (int i = 0; i < vecIds.size(); i++)
    {
        lua_pushnumber(luaVM, i);
        lua_pushnumber(luaVM, vecIds[i]);
        lua_settable(luaVM, -3);
    }
    return true;
}

bool CClientFBX::LuaGetAllTemplateModels(lua_State* luaVM, unsigned int uiTemplateId)
{
    if (!m_pFBXScene->IsTemplateValid(uiTemplateId))
    {
        return false;
    }

    std::vector<unsigned int> vecIds;
    m_pFBXScene->GetAllTemplatesModelsIds(vecIds, uiTemplateId);
    lua_newtable(luaVM);
    for (int i = 0; i < vecIds.size(); i++)
    {
        lua_pushnumber(luaVM, i + 1);
        lua_pushnumber(luaVM, vecIds[i]);
        lua_settable(luaVM, -3);
    }
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
        m_mapTexture[ullTextureId] =
            g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", pPixels, RDEFAULT, RDEFAULT, RDEFAULT, RFORMAT_UNKNOWN, TADDRESS_WRAP);
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

std::map<unsigned long long, std::vector<CMatrix>> CClientFBX::GetTemplatesRenderingMatrix()
{
    unsigned short                                     usDimension = g_pClientGame->GetLocalPlayer()->GetDimension();
    unsigned char                                      ucInterior = static_cast<unsigned char>(g_pCore->GetGame()->GetWorld()->GetCurrentArea());
    std::map<unsigned long long, std::vector<CMatrix>> templatesMatrix;
    CVector                                            scale;
    std::vector<CClientObject*>                        vecObjects;

    for (const auto& pair : m_mapElementRenderLoop)
    {
        if (pair.first->IsBeingDeleted())
        {
            m_mapElementRenderLoop.erase(pair.first);
        }
        else
        {
            if (pair.first->GetInterior() == ucInterior && pair.first->GetDimension() == usDimension)
            {
                for (const auto& templateId : pair.second)
                {
                    if (templatesMatrix.count(templateId) == 0)
                    {
                        templatesMatrix[templateId] = std::vector<CMatrix>();
                    }
                    CMatrix pMatrix;

                    pair.first->GetMatrix(pMatrix);
                    pair.first->GetScale(scale);
                    pMatrix.SetScale(scale);
                    templatesMatrix[templateId].push_back(pMatrix);
                }
            }
        }
    }

    for (const auto& pair : m_mapModelRenderLoop)
    {
        vecObjects = g_pClientGame->GetObjectManager()->GetObjectsByModel(pair.first);
        for (const auto& templateId : pair.second)
        {
            if (templatesMatrix.count(templateId) == 0)
            {
                templatesMatrix[templateId] = std::vector<CMatrix>();
            }
            for (CClientObject* pObject : vecObjects)
            {
                if (pObject->GetInterior() == ucInterior && pObject->GetDimension() == usDimension)
                {
                    CMatrix pMatrix;
                    pObject->GetMatrix(pMatrix);
                    pObject->GetScale(scale);
                    pMatrix.SetScale(scale);
                    templatesMatrix[templateId].push_back(pMatrix);
                }
            }
        }
    }
    return templatesMatrix;
}
