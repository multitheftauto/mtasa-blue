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


struct CUSTOMVERTEX
{
    float x, y, z;
    DWORD color;
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)            // | D3DFVF_TEX1;


class CFBXTemplate
{

};

class CFBXScene : public CFBXSceneInterface
{
public:
    CFBXScene(ofbx::IScene* scene);

    bool IsMeshValid(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    bool IsObjectValid(long long int ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) { return IsMeshValid(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectValid(ulId) ? m_objectList[ulId] : nullptr; }
    void                       GetAllObjectsIds(std::vector<unsigned long long>& vecIds) { vecIds = m_objectIdsList; };

private:
    const ofbx::IScene*                   m_pScene;
    std::map<unsigned int, CFBXTemplate*> m_templateList;
    
private:
    void        FixIndices();
    void        CacheObjects();
    void        CacheMeshes();
    void        CacheTextures();
    void        CacheMaterials();
    void        GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    const char* GetObjectType(const ofbx::Object const* pObject);

    unsigned int        uiNextFreeTemplateId = 0;
    const ofbx::Object*                                      m_pRoot;
    std::map<unsigned long long, const ofbx::Object* const*> m_objectList;
    std::vector<unsigned long long>                          m_objectIdsList;
    std::map<SString, const ofbx::Mesh*>                     m_meshList;
    std::map<SString, const ofbx::Texture*>                  m_textureList;
    std::map<unsigned long long, std::vector<char>>          m_textureContentList;
    std::vector<const ofbx::Material* const*>                m_materialList;
};

class CFBX : public CFBXInterface
{
public:
    CFBX();
    ~CFBX();

    CFBXScene*          AddScene(ofbx::IScene* pScene);
    void                RemoveScene(CFBXScene* pScene);
    void                Render();
    void                Initialize();
    bool                HasAnyFBXLoaded();

protected:
    std::vector<CFBXScene*> m_sceneList;

    std::vector<CUSTOMVERTEX> vertices;
    std::vector<int>          indices;
    LPDIRECT3DVERTEXBUFFER9   v_buffer = NULL;
    LPDIRECT3DINDEXBUFFER9    i_buffer = NULL;
    VOID*                     pVoid;
    IDirect3DDevice9*         m_pDevice;
};
