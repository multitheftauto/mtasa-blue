/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#include <list>
#include "CClientEntity.h"
#include "CClientFBXManager.h"
#include "fbx/ofbx.h"

class CClientFBX : public CClientEntity
{
    DECLARE_CLASS(CClientFBX, CClientEntity)
public:
    CClientFBX(class CClientManager* pManager, ElementID ID);
    ~CClientFBX(void);

    eClientEntityType GetType(void) const { return CCLIENTFBX; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(void){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    bool        LoadFBX(const SString& strFile, bool bIsRawData);

    bool        IsMeshExists(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    static bool IsFBXData(const SString& strData);

    const ofbx::Mesh const* GetMeshByName(const SString& strHierarchyMesh) { return IsMeshExists(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }

    void DrawPreview(const ofbx::Mesh* pMesh, CVector vecPosition, SColor color, float fWidth, bool bPostGUI);

    void LuaGetMeshes(lua_State* luaVM, const SString& strFilter);
    bool LuaGetMeshProperties(lua_State* luaVM, const ofbx::Mesh const* pMesh);
    bool LuaRawGetVertices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaRawGetIndices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);

private:
    void               GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    void               CacheMeshes();
    void               FixIndices();
    const char*        GetObjectType(const ofbx::Object const* pObject);
    SString            m_strFbxFilename;
    CBuffer            m_RawDataBuffer;
    bool               m_bIsRawData;
    CClientFBXManager* m_pFBXManager;

    const ofbx::Object*                              m_pRoot;
    ofbx::IScene*                                    m_pScene;
    std::map<SString, const ofbx::Mesh const*>       m_meshList;

    const ofbx::Geometry* pTempGeometry;
    const ofbx::Vec3* tempVertexPosition[3];
    CVector           tempVecPos[3];
};
