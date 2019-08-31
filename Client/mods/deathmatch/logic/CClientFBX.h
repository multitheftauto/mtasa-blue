/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFBX.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#include <list>
#include "CClientEntity.h"
#include "CClientFBXManager.h"
#include "CClientFBXInterface.h"

class CScriptArgReader;
enum eFBXTemplateProperty;
enum eFBXTemplateModelProperty;
enum eFBXObjectProperty;

class CClientFBX : public CClientEntity, CClientFBXInterface
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

    bool        IsMeshValid(const SString& strHierarchyMesh) { return m_pFBXScene->IsMeshValid(strHierarchyMesh); }
    bool        IsObjectValid(long long int ulId) { return m_pFBXScene->IsObjectValid(ulId); }
    static bool IsFBXData(const SString& strData);

    const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) { return m_pFBXScene->GetMeshByName(strHierarchyMesh); }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return m_pFBXScene->GetObjectById(ulId); }

    void LuaGetMeshes(lua_State* luaVM);
    void LuaGetTextures(lua_State* luaVM);
    bool LuaGetAllTemplates(lua_State* luaVM);
    void LuaGetMaterials(lua_State* luaVM);
    bool LuaGetObjectProperties(lua_State* luaVM, const ofbx::Object* const* pObject, eFBXObjectProperty eProperty);
    bool LuaRawGetVertices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaRawGetIndices(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaRawGetMaterials(lua_State* luaVM, const ofbx::Mesh const* pMesh, int iStart, int iStop);
    bool LuaGetAllTemplateModels(lua_State* luaVM, unsigned int uiTemplateId);
    void LuaGetAllObjectsIds(lua_State* luaVM);
    bool LuaSetTemplateProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, eFBXTemplateProperty eProperty);
    bool LuaGetTemplateProperties(lua_State* luaVM, unsigned int uiId, eFBXTemplateProperty eProperty);
    bool LuaSetTemplateModelProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, unsigned int uiModelId,
                                       eFBXTemplateModelProperty eProperty);
    bool LuaGetTemplateModelProperties(lua_State* luaVM, unsigned int uiId, unsigned int uiModelId, eFBXTemplateModelProperty eProperty);
    
    bool AddMeshToTemplate(lua_State* luaVM, unsigned int uiId, unsigned long long ullMesh, unsigned long long ullParentMesh, CVector vecPosition,
                           unsigned int& uiObjectId);
    bool             AddTemplate(unsigned int uiCopyFromTemplateId, unsigned int& uiNewTemplateId);
    void             RemoveTemplate(unsigned int uiTemplateId);

    void             CreateTexture(unsigned long long stTextureName, CPixels* pPixels);
    CMaterialItem*   GetTextureById(unsigned long long strTextureName);
    bool             IsTextureCreated(unsigned long long strTextureName);

private:
    CFBXSceneInterface* m_pFBXScene;
    SString             m_strFbxFilename;
    CBuffer             m_RawDataBuffer;
    bool                m_bIsRawData;
    CClientFBXManager*  m_pFBXManager;

    unsigned int          nextFreeId = 0;
    const ofbx::Geometry* pTempGeometry;
    const ofbx::Vec3*     tempVertexPosition[3];
    CVector               tempVecPos[3];

    std::map<unsigned long long, CClientTexture*> m_mapTexture;
};
