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

struct FBXVertex
{
    CVector  pos;
    D3DCOLOR color;
    FBXVertex() {}
    FBXVertex(float x, float y, float z, D3DCOLOR color) : color(color)
    {
        pos.fX = x;
        pos.fY = y;
        pos.fZ = z;
    }
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)            // | D3DFVF_TEX1;

class FBXBuffer
{
public:
    FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList);

    LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
    LPDIRECT3DINDEXBUFFER9  i_buffer = NULL;
    int                     indicesCount;
    int                     vertexCount;
};

class CFBXTemplateObject
{
public:
    CMatrix Matrix;
    unsigned long long ullObjectId;
    CFBXTemplateObject(unsigned long long ullObjectId) : ullObjectId(ullObjectId) {}
};

class CFBXTemplate
{
public:
    void Render(IDirect3DDevice9* pDevice, CFBXScene* pScene);
    void AddTemplateObject(CFBXTemplateObject * pObject);

private:
    std::vector<CFBXTemplateObject*> m_objectList;
};

class CFBXScene : public CFBXSceneInterface
{
public:
    CFBXScene(ofbx::IScene* scene);

    bool                       IsMeshValid(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    bool                       IsObjectValid(long long int ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) { return IsMeshValid(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectValid(ulId) ? m_objectList[ulId] : nullptr; }
    void                       GetAllObjectsIds(std::vector<unsigned long long>& vecIds) { vecIds = m_objectIdsList; };
    void                       Render(IDirect3DDevice9* pDevice);
    FBXBuffer*                 GetFBXBuffer(unsigned long long ullId);
    unsigned int               AddTemplete(CFBXTemplate* pTemplate);

private:

    const ofbx::IScene*                   m_pScene;
    std::map<unsigned int, CFBXTemplate*> m_templateMap;

    void        FixIndices();
    void        CacheObjects();
    void        CacheMeshes();
    void        CacheTextures();
    void        CacheMaterials();
    void        GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    const char* GetObjectType(const ofbx::Object const* pObject);
    bool        CreateFBXBuffer(const ofbx::Object* const* pObject);
    bool        AddBuffer(unsigned long long ullObjectId, FBXBuffer* pBuffer);

    unsigned int                                             uiNextFreeTemplateId = 0;
    const ofbx::Object*                                      m_pRoot;
    std::map<unsigned long long, const ofbx::Object* const*> m_objectList;
    std::vector<unsigned long long>                          m_objectIdsList;
    std::map<SString, const ofbx::Mesh*>                     m_meshList;
    std::map<SString, const ofbx::Texture*>                  m_textureList;
    std::map<unsigned long long, std::vector<char>>          m_textureContentList;
    std::vector<const ofbx::Material* const*>                m_materialList;
    std::map<unsigned long long, FBXBuffer*>                 m_mapMeshBuffer;
};

class CFBX : public CFBXInterface
{
public:
    CFBX();
    ~CFBX();

    CFBXScene* AddScene(ofbx::IScene* pScene);
    void       RemoveScene(CFBXScene* pScene);
    void       Render();
    void       Initialize();
    bool       HasAnyFBXLoaded();

private:

    std::vector<CFBXScene*> m_sceneList;
    IDirect3DDevice9*       m_pDevice;
};
