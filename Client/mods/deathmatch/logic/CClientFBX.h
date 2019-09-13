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

// from unknown reason fbx global transform position/rotation/scale are ~39,37007874015748 smaller than should be
#define FBX_MAGIC_TRANSFORM (1 / 0.0254)

class CScriptArgReader;
enum eFBXTemplateProperty;
enum eFBXTemplateModelProperty;
enum eFBXObjectProperty;
class CDeathmatchObject;

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

    bool LoadFBX(const SString& strFile, bool bIsRawData, lua_State* luaVM, SString strError);
    void LoadScene();
    void Render();

    bool         IsLoaded() { return m_bLoaded; };

    void LuaGetLoadingStatus(lua_State* luaVM);

    static bool IsFBXData(const SString& strData);

    const ofbx::Object* const* GetObjectById(unsigned long long ullId) { return m_pFBXScene->GetObjectById(ullId); }
    bool                       IsObjectValid(unsigned long long ullId) { return GetObjectById(ullId) != nullptr; }

    void LuaGetTextures(lua_State* luaVM);
    bool LuaGetAllTemplates(lua_State* luaVM);
    bool LuaGetObjectProperties(lua_State* luaVM, const ofbx::Object* const* pObject, eFBXObjectProperty eProperty);
    bool LuaRawGetVertices(lua_State* luaVM, const ofbx::Object* const* pObject);
    bool LuaRawGetIndices(lua_State* luaVM, const ofbx::Object* const* pObject);
    bool LuaGetAllTemplateModels(lua_State* luaVM, unsigned int uiTemplateId);
    void LuaGetAllObjectsIds(lua_State* luaVM, eFBXObjectType eObjectType);
    bool LuaSetTemplateProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, eFBXTemplateProperty eProperty);
    bool LuaGetTemplateProperties(lua_State* luaVM, unsigned int uiId, eFBXTemplateProperty eProperty);
    bool LuaSetTemplateModelProperties(lua_State* luaVM, CScriptArgReader argStream, unsigned int uiId, unsigned int uiModelId,
                                       eFBXTemplateModelProperty eProperty);
    bool LuaGetTemplateModelProperties(lua_State* luaVM, unsigned int uiId, unsigned int uiModelId, eFBXTemplateModelProperty eProperty);

    bool AddMeshToTemplate(lua_State* luaVM, unsigned int uiId, unsigned long long ullMesh, unsigned long long ullParentMesh, CVector vecPosition,
                           unsigned int& uiObjectId);
    bool AddTemplate(unsigned int uiCopyFromTemplateId, unsigned int& uiNewTemplateId);
    void RemoveTemplate(unsigned int uiTemplateId);
    bool RenderTemplate(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale);
    bool ApplyTemplateToElement(unsigned int uiTemplateId, CDeathmatchObject* pElement);
    bool RemoveTemplateFromElement(unsigned int uiTemplateId, CDeathmatchObject* pElement);
    bool ApplyTemplateToModel(unsigned int uiTemplateId, unsigned long ulModel);
    bool RemoveTemplateFromModel(unsigned int uiTemplateId, unsigned long ulModel);

    void                                               CreateTexture(unsigned long long stTextureName, CPixels* pPixels);
    CMaterialItem*                                     GetTextureById(unsigned long long strTextureName);
    bool                                               IsTextureCreated(unsigned long long strTextureName);
    std::map<unsigned long long, std::vector<CMatrix>> GetTemplatesRenderingMatrix();

    CBuffer* GetBuffer() { return m_RawDataBuffer; }

private:
    CFBXSceneInterface* m_pFBXScene;
    SString             m_strFbxFilename;
    CBuffer*            m_RawDataBuffer;
    bool                m_bIsRawData;
    CClientFBXManager*  m_pFBXManager;

    unsigned int          nextFreeId = 0;
    const ofbx::Geometry* pTempGeometry;
    const ofbx::Vec3*     tempVertexPosition[3];
    CVector               tempVecPos[3];
    bool                  m_bLoaded = false;
    bool                  m_bEventCalled = false;
    std::thread           threadAsyncLoad;
    CFBXLoading*          pLoadingState;

    std::map<unsigned long long, CClientTexture*>           m_mapTexture;
    std::map<CDeathmatchObject*, std::vector<unsigned int>> m_mapElementRenderLoop;
    std::map<unsigned long, std::vector<unsigned int>>      m_mapModelRenderLoop;
};
