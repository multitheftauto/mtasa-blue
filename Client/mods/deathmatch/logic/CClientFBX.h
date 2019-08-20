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

class CClientFBXRenderTemplate
{

};

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

    bool LoadFBX(const SString& strFile, bool bIsRawData);

    bool        IsMeshExists(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    bool        IsObjectWithId(long long int ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    static bool IsFBXData(const SString& strData);

    const ofbx::Mesh const* GetMeshByName(const SString& strHierarchyMesh) { return IsMeshExists(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectWithId(ulId) ? m_objectList[ulId] : nullptr; }

    void DrawPreview(const ofbx::Mesh* pMesh, CVector vecPosition, SColor color, float fWidth, bool bPostGUI);
    void Render();

    void LuaGetMeshes(lua_State* luaVM);
    void LuaGetTextures(lua_State* luaVM);
    void LuaGetMaterials(lua_State* luaVM);
    bool LuaGetObjectProperties(lua_State* luaVM, const ofbx::Object* const* pObject);
    bool LuaGetMeshProperties(lua_State* luaVM, const ofbx::Mesh const* pMesh);
    bool LuaRawGetVertices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaRawGetIndices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaRawGetMaterials(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    void LuaGetAllObjectsIds(lua_State* luaVM);

private:
    void        GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    void        CacheObjects();
    void        CacheMeshes();
    void        CacheTextures();
    void        CacheMaterials();
    void        FixIndices();
    const char* GetObjectType(const ofbx::Object const* pObject);

    SString            m_strFbxFilename;
    CBuffer            m_RawDataBuffer;
    bool               m_bIsRawData;
    CClientFBXManager* m_pFBXManager;

    const ofbx::Object* m_pRoot;
    ofbx::IScene*       m_pScene;

    std::map<unsigned long long, const ofbx::Object* const*> m_objectList;
    bool                                                     m_bMeshesCached = false;
    std::map<SString, const ofbx::Mesh*>                     m_meshList;
    bool                                                     m_bTexturesCached = false;
    std::map<SString, const ofbx::Texture*>                  m_textureList;
    std::map<unsigned long long, std::vector<char>>          m_textureContentList;
    bool                                                     m_bMaterialsCached = false;
    std::vector<const ofbx::Material* const*>                m_materialList;

    std::map<unsigned int, CClientFBXRenderTemplate*> m_templateMap;

    
    unsigned int          nextFreeId = 0;
    const ofbx::Geometry* pTempGeometry;
    const ofbx::Vec3*     tempVertexPosition[3];
    CVector               tempVecPos[3];
};
