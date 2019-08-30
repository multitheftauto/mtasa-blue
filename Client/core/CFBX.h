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
    CVector   pos;
    CVector   normal;
    D3DCOLOR  diffuse;
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
    void GetPosition(CVector& pos);
    void GetRotation(CVector& rot);
    void GetScale(CVector& scale);

    CMatrix* pViewMatrix;

    D3DMATERIAL9 material;
    D3DLIGHT9    light;
};

class CFBXTemplate
{
public:
    CFBXTemplate();
    void         Render(IDirect3DDevice9* pDevice, CFBXScene* pScene);
    unsigned int AddTemplateObject(CFBXTemplateObject* pObject);

    void SetPosition(CVector& position);
    void SetRotation(CVector& rotation);
    void SetScale(CVector& scale);
    void GetPosition(CVector& position);
    void GetRotation(CVector& rotation);
    void GetScale(CVector& scale);

    CMatrix* pViewMatrix;

    CFBXTemplateObject*                         GetObjectById(unsigned int uiModelId) { return IsModelValid(uiModelId) ? m_objectMap[uiModelId] : nullptr; }
    std::map<unsigned int, CFBXTemplateObject*> GetObjectsMap() { return m_objectMap; }
    bool                                        IsModelValid(unsigned int uiModelId) { return m_objectMap.count(uiModelId) != 0; }

private:
    std::map<unsigned int, CFBXTemplateObject*> m_objectMap;

    unsigned int uiNextFreeObjectId = 1;
};

class CFBXScene : public CFBXSceneInterface
{
public:
    CFBXScene(ofbx::IScene* scene, CClientFBXInterface* pClientFBXInterface);

    bool                       IsMeshValid(const SString& strHierarchyMesh) { return m_meshList.find(strHierarchyMesh) != m_meshList.end(); }
    bool                       IsObjectValid(unsigned long long ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    bool                       IsTemplateValid(unsigned int uiId) { return m_templateMap.count(uiId) != 0; }
    const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) { return IsMeshValid(strHierarchyMesh) ? m_meshList[strHierarchyMesh] : nullptr; }
    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectValid(ulId) ? m_objectList[ulId] : nullptr; }
    void                       GetAllObjectsIds(std::vector<unsigned long long>& vecIds) { vecIds = m_objectIdsList; };
    void                       GetAllTemplatesIds(std::vector<unsigned int>& vecIds);
    bool                       GetAllTemplatesModelsIds(std::vector<unsigned int>& vecIds, unsigned int uiTemplateId);
    void                       RenderScene(IDirect3DDevice9* pDevice);
    FBXObjectBuffer*           GetFBXBuffer(unsigned long long ullId);
    unsigned int               AddTemplete(CFBXTemplate* pTemplate);
    CTextureItem*              GetTexture(unsigned long long ullMaterialId);
    bool                       IsTemplateModelValid(unsigned int uiTemplate, unsigned int uiModelId);
    unsigned int               AddMeshToTemplate(unsigned int uiTemplate, unsigned long long uiModelId);

    void GetTemplateScale(unsigned int uiTemplateId, CVector& scale);
    void GetTemplatePosition(unsigned int uiTemplateId, CVector& position);
    void GetTemplateRotation(unsigned int uiTemplateId, CVector& rotation);
    void SetTemplateScale(unsigned int uiTemplateId, CVector& scale);
    void SetTemplatePosition(unsigned int uiTemplateId, CVector& position);
    void SetTemplateRotation(unsigned int uiTemplateId, CVector& rotation);
    void GetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale);
    void GetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position);
    void GetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation);
    void SetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale);
    void SetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position);
    void SetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation);


    D3DMATRIX* GetMatrixUVFlip() { return m_pMatrixUVFlip; }

private:
    const ofbx::IScene*                   m_pScene;
    std::map<unsigned int, CFBXTemplate*> m_templateMap;
    CClientFBXInterface*                  pClientFBXInterface;

    void FixIndices();
    void CacheObjects();
    void CacheMeshes();
    void CacheTextures();
    void CacheMaterials();
    void CacheMeshMaterials();
    void GetMeshPath(const ofbx::Mesh* pObject, SString& name);
    bool CreateFBXBuffer(const ofbx::Object* const* pObject);
    bool AddBuffer(unsigned long long ullObjectId, FBXObjectBuffer* pBuffer);

    unsigned int                                                    uiNextFreeTemplateId = 1;            // 0 is special
    const ofbx::Object*                                             m_pRoot;
    std::vector<unsigned long long>                                 m_objectIdsList;
    std::map<unsigned long long, const ofbx::Object* const*>        m_objectList;
    std::map<SString, const ofbx::Mesh*>                            m_meshList;
    std::map<SString, CPixels*>                                     m_textureContentList;
    std::map<unsigned long long, const ofbx::Material* const*>      m_materialList;
    std::map<unsigned long long, FBXObjectBuffer*>                  m_mapMeshBuffer;
    std::map<const ofbx::Mesh*, std::vector<const ofbx::Material*>> m_mapMeshMaterials;

    D3DXMATRIX* m_pMatrixUVFlip;
};

class CFBX : public CFBXInterface
{
public:
    CFBX();
    ~CFBX();

    CFBXScene*  AddScene(ofbx::IScene* pScene, CClientFBXInterface* pInterface);
    void        RemoveScene(CFBXScene* pScene);
    void        Render();
    void        Initialize();
    bool        HasAnyFBXLoaded();
    const char* GetObjectType(const ofbx::Object const* pObject);

private:
    std::vector<CFBXScene*> m_sceneList;
    IDirect3DDevice9*       m_pDevice;
};
