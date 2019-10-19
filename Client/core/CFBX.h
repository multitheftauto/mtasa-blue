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

#include "CFrustum.h"
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class CFBXTemplate;
class CFBXTemplateObject;

enum eVertexType
{
    VERTEX_TYPE_POS_NORMAL_TEXTURE_DIFFUSE,
    VERTEX_TYPE_POS_NORMAL_TEXTURE,
    COUNT,
};

static class CFBXDebugging
{
public:
    static void DrawBoundingBox(CFBXBoundingBox* pBoundingBox, CMatrix& matrix, SColorRGBA color = SColorRGBA(255, 0, 0, 255), float fLineWidth = 2.0f);
    static void DrawBoundingBox(CFBXTemplate* pTemplate, CMatrix& matrix);
    static void DrawDebuggingInformation(CFBXTemplate* pTemplate, CMatrix& matrix);
    static void DrawDebuggingInformation(CFBXTemplateObject* pTemplateObject, CMatrix& matrix);
    static void AddRenderedTemplate() { iRenderedTemplates++; };
    static void AddRenderedObject() { iRenderedObjects++; };
    static void AddRenderedScene() { iRenderedScenes++; };
    static void ResetTemplatesCounter() { iRenderedTemplates = 0; };
    static void ResetObjectsCounter() { iRenderedObjects = 0; };
    static void ResetScenesCounter() { iRenderedScenes = 0; };

    static void Start();

    // last frame
    static int iRenderedTemplates;
    static int iRenderedObjects;
    static int iRenderedScenes;
};

class CFBXTextureSet
{
public:
    CTextureItem* diffuse;
    CTextureItem* normal;
};

struct CFBXRenderItem
{
    unsigned int m_uiTemplateId;
    CMatrix      m_matrix;
};

struct CFBXBoundingBox
{
    CVector min;
    CVector max;
    CVector center;
    float   radius;
    void    GetBoundingBoxCornersByMatrix(CVector vecCorner[8], CMatrix& matrix);
};

struct FBXVertex
{
    CVector   position;
    CVector   normal;
    CVector2D uv;
    FBXVertex() {}
    FBXVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
    {
        position.fX = x;
        position.fY = y;
        position.fZ = z;
        normal.fX = nx;
        normal.fY = ny;
        normal.fZ = nz;
        uv.fX = u;
        uv.fY = v;
    }
};

class FBXVertexBuffer
{
public:
    template <typename T>
    FBXVertexBuffer(std::vector<T> vector, int FVF);
    ~FBXVertexBuffer();
    void Select(UINT StreamNumber);

private:
    IDirect3DVertexBuffer9* m_pBuffer;
    int                     m_iBufferSize;
    int                     m_iTypeSize;
    int                     m_FVF;
};

class FBXBuffer
{
public:
    FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, unsigned long long ullMterialId);
    ~FBXBuffer();

    FBXVertexBuffer*        m_pFBXVertexBuffer;
    LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    int                     m_iBufferSize;
    int                     m_iIndicesCount;
    int                     m_iFacesCount;
    int                     m_iVertexCount;
    unsigned long long      m_ullMaterialId;
};

class FBXObjectBuffer
{
public:
    FBXObjectBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, std::vector<int> vecMaterialList, const ofbx::Mesh* const* pMesh);
    ~FBXObjectBuffer();
    std::vector<FBXBuffer*> m_bufferList;
    FBXVertexBuffer*        m_pDiffuseBuffer;
};

class CFBXTemplateObject
{
public:
    CFBXTemplateObject(unsigned long long ullObjectId, CFBXScene* pScene);
    CFBXTemplateObject::~CFBXTemplateObject();

    void SetPosition(CVector& position) { m_pViewMatrix->SetPosition(position); };
    void SetRotation(CVector& rotation)
    {
        m_pViewMatrix->SetRotation(rotation);
        UpdateBoundingBox();
    };
    void SetScale(CVector& scale)
    {
        m_pViewMatrix->SetScale(scale);
        UpdateBoundingBox();
    };
    void               SetDrawDistance(float fDrawDistance) { this->m_fDrawDistance = fDrawDistance; };
    void               GetPosition(CVector& position) { position = m_pViewMatrix->GetPosition(); };
    void               GetRotation(CVector& rotation) { rotation = m_pViewMatrix->GetRotation(); };
    void               GetScale(CVector& scale) { scale = m_pViewMatrix->GetScale(); };
    float              GetDrawDistance() { return m_fDrawDistance; };
    void               SetCullMode(eCullMode cullMode) { m_eCullMode = cullMode; };
    void               GetCullMode(eCullMode& cullMode) { cullMode = m_eCullMode; };
    void               SetUseCustomOpacity(bool bUseCustomOpacity) { m_bUseCustomOpacity = bUseCustomOpacity; };
    void               GetUseCustomOpacity(bool& bUseCustomOpacity) { bUseCustomOpacity = this->m_bUseCustomOpacity; };
    void               SetOpacity(ushort usOpacity) { m_usOpacity = usOpacity; };
    void               GetOpacity(ushort& usOpacity) { usOpacity = m_usOpacity; };
    void               SetFadeDistance(float fFadeDistance) { m_fFadeDistance = fFadeDistance; };
    void               GetFadeDistance(float& fFadeDistance) { fFadeDistance = m_fFadeDistance; };

    unsigned long long GetObjectId() { return m_ullObjectId; };
    void               GetMatrix(D3DMATRIX* pMatrix) { m_pViewMatrix->GetBuffer((float*)pMatrix); };
    void               GetMatrix(CMatrix& pMatrix) { pMatrix = *m_pViewMatrix; };
    void               GetMaterial(D3DMATERIAL9*& pMaterial) const { pMaterial = m_pMaterial; };
    void               GetMaterialDiffuseColor(DWORD& pMaterial);
    ushort             GetOpacityFromDistance(float fDistance);
    CFBXBoundingBox*   GetBoundingBox() const { return m_pBoundingBox; }
    // void               GetLight(D3DLIGHT9*& pLight) const { pLight = m_pLight; };

    // require better name
    unsigned int m_indexId;

private:
    void UpdateBoundingBox();

    // D3DLIGHT9*         m_pLight;
    CMatrix*           m_pViewMatrix;
    D3DMATERIAL9*      m_pMaterial;
    float              m_fDrawDistance = 500.0f;
    float              m_fFadeDistance = 20.0f;
    ushort             m_usOpacity = 255;
    bool               m_bUseCustomOpacity = false;
    eCullMode          m_eCullMode = (eCullMode)2;
    unsigned long long m_ullObjectId;
    CFBXBoundingBox*   m_pBoundingBox;
    CFBXScene*         m_pScene;
};

class CFBXTemplate
{
public:
    CFBXTemplate();
    ~CFBXTemplate();
    bool         Render(IDirect3DDevice9* pDevice, CFBXScene* pScene, D3DMATRIX& pOffsetMatrix);
    unsigned int AddTemplateObject(CFBXTemplateObject* pObject);
    bool         RemoveObject(unsigned int uiObject);

    void SetPosition(CVector& position) const { m_pViewMatrix->SetPosition(position); };
    void SetRotation(CVector& rotation) const { m_pViewMatrix->SetRotation(rotation); };
    void SetScale(CVector& scale) const { m_pViewMatrix->SetScale(scale); };
    void SetDrawDistance(float fDrawDistance) { m_fDrawDistance = fDrawDistance; };
    void GetPosition(CVector& position) const { position = m_pViewMatrix->GetPosition(); };
    void GetRotation(CVector& rotation) const { rotation = m_pViewMatrix->GetRotation(); };
    void GetScale(CVector& scale) const { scale = m_pViewMatrix->GetScale(); };
    float GetDrawDistance() { return m_fDrawDistance; };

    CFBXTemplateObject* GetObjectById(unsigned int uiModelId) { return IsModelValid(uiModelId) ? m_objectMap[uiModelId] : nullptr; }
    std::unordered_map<unsigned int, CFBXTemplateObject*> GetObjectsMap() { return m_objectMap; }
    bool                                                  IsModelValid(unsigned int uiModelId) { return m_objectMap.count(uiModelId) != 0; }
    unsigned int                                          GetInterior() { return m_uiInterior; };
    unsigned int                                          GetDimension() { return m_uiDimension; };
    CMatrix*                                              GetViewMatrix() const { return m_pViewMatrix; };
    CFBXBoundingBox*                                      GetBoundingBox() const { return m_pBoundingBox; };
    void                                                  GetBoundingBoxCornersByMatrix(CVector vecCorner[8], CMatrix& matrix);

    unsigned int m_uiTemplateId;
private:
    void UpdateBoundingBox();

    std::unordered_map<unsigned int, CFBXTemplateObject*> m_objectMap;
    unsigned int                                          m_uiInterior = 0;
    unsigned int                                          m_uiDimension = 0;
    float                                                 m_fDrawDistance = 10000.0f;
    unsigned int                                          m_uiNextFreeObjectId = 1;
    CMatrix*                                              m_pViewMatrix;
    D3DMATRIX*                                            m_pObjectMatrix;
    CMatrix*                                              m_pCameraMatrix;
    CFBXBoundingBox*                                      m_pBoundingBox;
};

class CFBXScene : public CFBXSceneInterface
{
public:
    CFBXScene(ofbx::IScene* scene, CClientFBXInterface* pClientFBXInterface);
    ~CFBXScene();

    const ofbx::Object* const* GetObjectById(long long int ulId) { return IsObjectValid(ulId) ? m_objectList[ulId] : nullptr; }

    bool             IsObjectValid(unsigned long long ulId) { return m_objectList.find(ulId) != m_objectList.end(); }
    bool             IsTemplateValid(unsigned int uiId) { return m_templateMap.count(uiId) != 0; }
    CFBXBoundingBox* GetMeshBoundingBox(long long int ulId) { return (m_geometryBoundingBox.count(ulId) != 0) ? m_geometryBoundingBox[ulId] : nullptr; }
    bool             GetMeshName(long long int ulId, SString& strMeshName);
    bool             GetTexturePath(long long int ulId, SString& strTexturePath);
    void             GetAllObjectsIds(std::vector<unsigned long long>& vecIds) { vecIds = m_objectIdsList; };
    void             GetAllTemplatesIds(std::vector<unsigned int>& vecIds);
    bool             GetAllTemplatesModelsIds(std::vector<unsigned int>& vecIds, unsigned int uiTemplateId);
    bool             RenderScene(IDirect3DDevice9* pDevice, CFrustum* pFrustum, CVector& vecCameraPosition);
    bool             RenderTemplate(CFBXTemplate* pTemplate, CMatrix& pMatrix, CVector& vecCameraPosition);
    FBXObjectBuffer* GetFBXBuffer(unsigned long long ullId);
    unsigned int     AddTemplete(CFBXTemplate* pTemplate);
    CTextureItem*    GetTexture(const ofbx::Texture* pTexture);
    CFBXTextureSet*  GetTextureSet(unsigned long long ullMaterialId);
    bool             IsTemplateModelValid(unsigned int uiTemplate, unsigned int uiModelId);
    unsigned int     AddMeshToTemplate(unsigned int uiTemplate, unsigned long long uiModelId);
    unsigned int     CreateTemplate();
    bool             RemoveTemplate(unsigned int uiTemplateId);
    bool             RemoveObjectFromTemplate(unsigned int uiTemplate, unsigned int uiObjectId);

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
    void AddToRenderQueue(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale);
    bool GetBoundingBox(unsigned long long ullObjectId, CVector& min, CVector& max, float& fRadius);
    void GetTemplateDrawDistance(unsigned int uiTemplateId, float& drawDistance);
    void SetTemplateDrawDistance(unsigned int uiTemplateId, float drawDistance);
    void GetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fDrawDistance);
    void SetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fDrawDistance);
    void SetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode cullMode);
    void GetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode& cullMode);
    void SetTemplateModelUseCustomOpacity(unsigned int uiTemplateId, unsigned int uiModelId, bool bUseCustomOpacity);
    void GetTemplateModelUseCustomOpacity(unsigned int uiTemplateId, unsigned int uiModelId, bool& bUseCustomOpacity);
    void SetTemplateModelOpacity(unsigned int uiTemplateId, unsigned int uiModelId, ushort usOpacity);
    void GetTemplateModelOpacity(unsigned int uiTemplateId, unsigned int uiModelId, ushort& usOpacity);
    void SetTemplateModelFadeDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fFadeDistance);
    void GetTemplateModelFadeDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fFadeDistance);

    float GetUnitScaleFactor() { return m_fUnitScaleFactor; }

private:
    void CacheObjects();
    void CacheMeshes();
    void CacheTextures();
    void CacheMaterials();
    void CacheMeshMaterials();
    void CacheBoundingBoxes();
    void CreateBaseTemplate();
    bool CreateFBXBuffer(const ofbx::Object* const* pObject);
    bool AddBuffer(unsigned long long ullObjectId, FBXObjectBuffer* pBuffer);

    const ofbx::IScene*                             m_pScene;
    CClientFBXInterface*                            m_pClientFBXInterface;
    float                                           m_fUnitScaleFactor;
    unsigned int                                    m_uiNextFreeTemplateId = 0;            // user defined templates starts from 1, below are special ones
    const ofbx::Object*                             m_pRoot;
    std::vector<unsigned long long>                 m_objectIdsList;
    std::unordered_map<unsigned int, CFBXTemplate*> m_templateMap;
    std::unordered_map<unsigned long long, const ofbx::Object* const*>        m_objectList;
    std::unordered_map<unsigned long long, const ofbx::Mesh*>                 m_meshList;
    std::unordered_map<unsigned long long, SString>                           m_meshName;
    std::unordered_map<unsigned long long, SString>                           m_texturePath;
    std::unordered_map<unsigned long long, CFBXBoundingBox*>                  m_geometryBoundingBox;
    std::unordered_map<unsigned long long, const ofbx::Material* const*>      m_materialList;
    std::unordered_map<unsigned long long, FBXObjectBuffer*>                  m_mapMeshBuffer;
    std::unordered_map<unsigned long long, CFBXTextureSet*>                   m_mapMaterialTextureSet;
    std::unordered_map<SString, CPixels*>                                     m_textureContentList;
    std::unordered_map<const ofbx::Mesh*, std::vector<const ofbx::Material*>> m_mapMeshMaterials;
    std::vector<CFBXRenderItem>                                               m_vecTemporaryRenderLoop;
    CMatrix                                                                   m_Matrix;
    D3DMATRIX                                                                 m_ObjectMatrix;
    CMatrix                                                                   m_CameraMatrix;

    IDirect3DDevice9*                                                         m_pDevice;

    bool                                                                      bRenderDebug;
};

class CFBX : public CFBXInterface
{
public:
    CFBX();
    ~CFBX();

    CFBXScene*                   AddScene(ofbx::IScene* pScene, CClientFBXInterface* pInterface);
    void                         RemoveScene(CFBXScene* pScene);
    void                         Render();
    void                         UpdateFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix);
    bool                         CheckCulling(CFBXBoundingBox* pBoundingBox, CMatrix* pMatrix);
    void                         Initialize();
    bool                         HasAnyFBXLoaded();
    CFBXBoundingBox*             CalculateBoundingBox(const ofbx::Mesh* pGeometry);
    const char*                  GetObjectType(const ofbx::Object const* pObject);
    D3DLIGHT9*                   GetGlobalLight() { return &m_globalLight; }
    D3DXCOLOR*                   GetGlobalAmbient() { return m_globalAmbient; }
    float                        GetGlobalLighting() { return m_globalLighting; }
    D3DMATRIX*                   GetMatrixUVFlip() { return m_pMatrixUVFlip; }
    IDirect3DVertexDeclaration9* GetVertexDeclaration(eVertexType index) { return m_pVertexDeclaration[index]; }
    void                         SetDevelopmentModeEnabled(bool bEnabled) { m_pDevelopmentModeEnabled = bEnabled; }
    bool                         GetDevelopmentModeEnabled() { return m_pDevelopmentModeEnabled && GetShowFBXEnabled(); }
    void                         SetShowFBXEnabled(bool bEnabled) { m_pShowFBX = bEnabled; }
    bool                         GetShowFBXEnabled() { return m_pShowFBX; }
    CTextureItem*                GetBlankTexture() { return m_pBlankTexture; }

private:
    std::vector<CFBXScene*>      m_sceneList;
    IDirect3DDevice9*            m_pDevice;
    D3DLIGHT9                    m_globalLight;
    D3DXCOLOR*                   m_globalAmbient;
    float                        m_globalLighting;            // how bright are objects, 0.0f - 1.0f
    D3DXMATRIX*                  m_pMatrixUVFlip;
    CFrustum*                    m_pFrustum;
    bool                         m_pShowFBX = false;
    bool                         m_pDevelopmentModeEnabled = false;
    CMatrix                      m_pCameraMatrix;
    IDirect3DVertexDeclaration9* m_pVertexDeclaration[eVertexType::COUNT];
    CTextureItem*                m_pBlankTexture;            // used if original doesn't exists and is enabled
};
