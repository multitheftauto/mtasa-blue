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
    CVector  normal;
    D3DCOLOR diffuse;
    CVector2D uv;
    FBXVertex() {}
    FBXVertex(float x, float y, float z, float nx, float ny, float nz, D3DCOLOR diffuse, float u, float v) : diffuse(diffuse)
    {
        pos.fX = x;
        pos.fY = y;
        pos.fZ = z;
        normal.fX = nx;
        normal.fY = ny;
        normal.fZ = nz;
        uv.fX = u;
        uv.fY = v;
    }
};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class FBXBuffer
{
public:
    FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, unsigned long long ullMterialId);

    LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
    LPDIRECT3DINDEXBUFFER9  i_buffer = NULL;
    int                     indicesCount;
    int                     vertexCount;
    unsigned long long      ullMaterialId;
};

class FBXObjectBuffer
{
public:
    FBXObjectBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, std::vector<int> vecMaterialList, const ofbx::Mesh* const* pMesh);
    std::vector<FBXBuffer*> bufferList;
};

class CFBXTemplateObject
{
public:
    CFBXTemplateObject(unsigned long long ullObjectId);

    unsigned long long ullObjectId;

    void SetPosition(CVector& pos);
    void SetRotation(CVector& rot);
    void SetScale(CVector& scale);

    CMatrix* pViewMatrix;

    D3DMATERIAL9 material;
    D3DLIGHT9    light;
};

class CFBXTemplate
{
public:
    CFBXTemplate();
    void Render(IDirect3DDevice9* pDevice, CFBXScene* pScene);
    void AddTemplateObject(CFBXTemplateObject* pObject);

    void SetPosition(CVector& pos);
    void SetRotation(CVector& rot);
    void SetScale(CVector& scale);

    CMatrix* pViewMatrix;

private:
    std::vector<CFBXTemplateObject*> m_objectList;
};

class CFBXScene : public CFBXSceneInterface
{
public:
    CFBXScene(ofbx::IScene* scene, CClientFBXInterface* pClientFBXInterface);

    bool                       IsMeshValid(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    bool                       IsObjectValid(long long int ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) { return IsMeshValid(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectValid(ulId) ? m_objectList[ulId] : nullptr; }
    void                       GetAllObjectsIds(std::vector<unsigned long long>& vecIds) { vecIds = m_objectIdsList; };
    void                       Render(IDirect3DDevice9* pDevice);
    FBXObjectBuffer*           GetFBXBuffer(unsigned long long ullId);
    unsigned int               AddTemplete(CFBXTemplate* pTemplate);
    CTextureItem*              GetTexture(unsigned long long ullMaterialId);

private:
    const ofbx::IScene*                   m_pScene;
    std::map<unsigned int, CFBXTemplate*> m_templateMap;
    CClientFBXInterface*                  pClientFBXInterface;

    void                   FixIndices();
    void                   CacheObjects();
    void                   CacheMeshes();
    void                   CacheTextures();
    void                   CacheMaterials();
    void                   CacheMeshMaterials();
    void                   GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    const char*            GetObjectType(const ofbx::Object const* pObject);
    bool                   CreateFBXBuffer(const ofbx::Object* const* pObject);
    bool                   AddBuffer(unsigned long long ullObjectId, FBXObjectBuffer* pBuffer);

    unsigned int                                                    uiNextFreeTemplateId = 0;
    const ofbx::Object*                                             m_pRoot;
    std::map<unsigned long long, const ofbx::Object* const*>        m_objectList;
    std::vector<unsigned long long>                                 m_objectIdsList;
    std::map<SString, const ofbx::Mesh*>                            m_meshList;
    std::map<SString, const ofbx::Texture*>                         m_textureList;
    std::map<unsigned long long, SString>                           m_textureNamesList;
    std::map<unsigned long long, std::vector<char>>                 m_textureContentList;
    std::map<unsigned long long, const ofbx::Material* const*>      m_materialList;
    std::map<unsigned long long, FBXObjectBuffer*>                  m_mapMeshBuffer;
    std::map<const ofbx::Mesh*, std::vector<const ofbx::Material*>> m_mapMeshMaterials;
};

class CFBX : public CFBXInterface
{
public:
    CFBX();
    ~CFBX();

    CFBXScene* AddScene(ofbx::IScene* pScene, CClientFBXInterface* pInterface);
    void       RemoveScene(CFBXScene* pScene);
    void       Render();
    void       Initialize();
    bool       HasAnyFBXLoaded();

private:
    std::vector<CFBXScene*> m_sceneList;
    IDirect3DDevice9*       m_pDevice;
};
