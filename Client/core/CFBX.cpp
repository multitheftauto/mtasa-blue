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

#include "StdInc.h"
#include "d3dx9mesh.h"

int CFBXDebugging::iRenderedTemplates = 0;
int CFBXDebugging::iRenderedObjects = 0;
int CFBXDebugging::iRenderedScenes = 0;

void CFBXDebugging::Start()
{
    ResetTemplatesCounter();
    ResetObjectsCounter();
    ResetScenesCounter();
}

bool GetScreenFromWorldPosition(CVector& vecWorld, CVector& vecScreen, float fEdgeTolerance, bool bRelative)
{
    g_pCore->GetGraphics()->CalcScreenCoors(&vecWorld, &vecScreen);

    float fResWidth = static_cast<float>(g_pCore->GetGraphics()->GetViewportWidth());
    float fResHeight = static_cast<float>(g_pCore->GetGraphics()->GetViewportHeight());

    // Calc relative values if required
    float fToleranceX = bRelative ? fEdgeTolerance * fResWidth : fEdgeTolerance;
    float fToleranceY = bRelative ? fEdgeTolerance * fResHeight : fEdgeTolerance;

    // Keep within a reasonable range
    fToleranceX = Clamp(0.f, fToleranceX, fResWidth * 10);
    fToleranceY = Clamp(0.f, fToleranceY, fResHeight * 10);

    if (vecScreen.fX >= -fToleranceX && vecScreen.fX <= fResWidth + fToleranceX && vecScreen.fY >= -fToleranceY && vecScreen.fY <= fResHeight + fToleranceY &&
        vecScreen.fZ > 0.1f)
    {
        return true;
    }

    return false;
}

void CFBXDebugging::DrawBoundingBox(CFBXBoundingBox* pBoundingBox, CMatrix& matrix, SColorRGBA color, float fLineWidth)
{
    static CVector vecCorner[8];

    pBoundingBox->GetBoundingBoxCornersByMatrix(vecCorner, matrix);

    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[0], vecCorner[4], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[1], vecCorner[5], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[2], vecCorner[6], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[3], vecCorner[7], fLineWidth, color, false);

    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[0], vecCorner[1], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[0], vecCorner[2], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[2], vecCorner[3], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[1], vecCorner[3], fLineWidth, color, false);

    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[4], vecCorner[5], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[4], vecCorner[6], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[6], vecCorner[7], fLineWidth, color, false);
    g_pCore->GetGraphics()->DrawLine3DQueued(vecCorner[5], vecCorner[7], fLineWidth, color, false);
}

void CFBXDebugging::DrawBoundingBox(CFBXTemplate* pTemplate, CMatrix& matrix)
{
    static SColorRGBA color(255, 0, 0, 255);
    static CVector    vecCorner[8];
    static float      fLineWidth = 8.0f;

    DrawBoundingBox(pTemplate->GetBoundingBox(), matrix * *pTemplate->GetViewMatrix());
}

void CFBXDebugging::DrawDebuggingInformation(CFBXTemplate* pTemplate, CMatrix& matrix)
{
    CMatrix templateMatrix = matrix * *pTemplate->GetViewMatrix();
    CVector vecWorld = templateMatrix.GetPosition();
    CVector vecScreen;
    g_pCore->GetGraphics()->CalcScreenCoors(&vecWorld, &vecScreen);

    if (GetScreenFromWorldPosition(vecWorld, vecScreen, 0, true))
    {
        SString strDebugInfo(
            "Template id: %i\n"
            "Draw distance: %.2f"
            "",
            pTemplate->m_uiTemplateId, pTemplate->GetDrawDistance());
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX - 1, vecScreen.fY - 1, vecScreen.fX - 1, vecScreen.fY - 1, 0xFF000000, strDebugInfo.c_str(), 1, 1,
                                                 DT_NOCLIP, NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX + 1, vecScreen.fY + 1, vecScreen.fX + 1, vecScreen.fY + 1, 0xFF000000, strDebugInfo.c_str(), 1, 1,
                                                 DT_NOCLIP, NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX - 1, vecScreen.fY + 1, vecScreen.fX - 1, vecScreen.fY + 1, 0xFF000000, strDebugInfo.c_str(), 1, 1,
                                                 DT_NOCLIP, NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX + 1, vecScreen.fY - 1, vecScreen.fX + 1, vecScreen.fY - 1, 0xFF000000, strDebugInfo.c_str(), 1, 1,
                                                 DT_NOCLIP, NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX, vecScreen.fY, vecScreen.fX, vecScreen.fY, 0xFFFF0000, strDebugInfo.c_str(), 1, 1, DT_NOCLIP,
                                                 NULL, true);
    }
}
void CFBXDebugging::DrawDebuggingInformation(CFBXTemplateObject* pTemplateObject, CMatrix& matrix)
{
    CVector vecWorld = matrix.GetPosition();
    CVector vecScreen;

    g_pCore->GetGraphics()->CalcScreenCoors(&vecWorld, &vecScreen);

    if (GetScreenFromWorldPosition(vecWorld, vecScreen, 0, true))
    {
        char szDebug[512] = "";
        snprintf(szDebug, 512, "Object id: %lu\nIndex: %i\nDraw distance: %.2f", pTemplateObject->GetObjectId(), pTemplateObject->m_indexId,
                 pTemplateObject->GetDrawDistance());

        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX - 1, vecScreen.fY - 1, vecScreen.fX - 1, vecScreen.fY - 1, 0xFF000000, szDebug, 1, 1, DT_NOCLIP,
                                                 NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX + 1, vecScreen.fY + 1, vecScreen.fX + 1, vecScreen.fY + 1, 0xFF000000, szDebug, 1, 1, DT_NOCLIP,
                                                 NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX - 1, vecScreen.fY + 1, vecScreen.fX - 1, vecScreen.fY + 1, 0xFF000000, szDebug, 1, 1, DT_NOCLIP,
                                                 NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX + 1, vecScreen.fY - 1, vecScreen.fX + 1, vecScreen.fY - 1, 0xFF000000, szDebug, 1, 1, DT_NOCLIP,
                                                 NULL, true);
        g_pCore->GetGraphics()->DrawStringQueued(vecScreen.fX, vecScreen.fY, vecScreen.fX, vecScreen.fY, 0xFF00FF00, szDebug, 1, 1, DT_NOCLIP, NULL, true);
    }
}

void CFBXBoundingBox::GetBoundingBoxCornersByMatrix(CVector vecCorner[8], CMatrix& matrix)
{
    vecCorner[0] = matrix.TransformVector(CVector(min.fX, min.fY, min.fZ));
    vecCorner[1] = matrix.TransformVector(CVector(min.fX, max.fY, min.fZ));
    vecCorner[2] = matrix.TransformVector(CVector(max.fX, min.fY, min.fZ));
    vecCorner[3] = matrix.TransformVector(CVector(max.fX, max.fY, min.fZ));
    vecCorner[4] = matrix.TransformVector(CVector(min.fX, min.fY, max.fZ));
    vecCorner[5] = matrix.TransformVector(CVector(min.fX, max.fY, max.fZ));
    vecCorner[6] = matrix.TransformVector(CVector(max.fX, min.fY, max.fZ));
    vecCorner[7] = matrix.TransformVector(CVector(max.fX, max.fY, max.fZ));
}

FBXBuffer::FBXBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, unsigned long long ullMaterialId)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid;

    m_pFBXVertexBuffer = new FBXVertexBuffer(vecVertexList, (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

    pDevice->CreateIndexBuffer(vecIndexList.size() * sizeof(int), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    m_pIndexBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vecIndexList.data(), vecIndexList.size() * sizeof(int));
    m_pIndexBuffer->Unlock();

    m_iIndicesCount = vecIndexList.size();
    m_iFacesCount = m_iIndicesCount / 3;
    m_ullMaterialId = ullMaterialId;
}

FBXBuffer::~FBXBuffer()
{
    delete m_pFBXVertexBuffer;
}

template <typename T>
FBXVertexBuffer::FBXVertexBuffer(std::vector<T> vector, int FVF)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    VOID*             pVoid;
    pDevice->CreateVertexBuffer(vector.size() * sizeof(T), D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_pBuffer, NULL);
    m_pBuffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vector.data(), vector.size() * sizeof(T));
    m_pBuffer->Unlock();

    m_iBufferSize = vector.size();
    m_FVF = FVF;
    m_iTypeSize = sizeof(T);
}

FBXVertexBuffer::~FBXVertexBuffer()
{
    m_pBuffer->Release();
    delete m_pBuffer;
}

void FBXVertexBuffer::Select(UINT StreamNumber)
{
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
    pDevice->SetStreamSource(StreamNumber, m_pBuffer, 0, m_iTypeSize);
}

FBXObjectBuffer::FBXObjectBuffer(std::vector<FBXVertex> vecVertexList, std::vector<int> vecIndexList, std::vector<int> vecMaterialList,
                                 const ofbx::Mesh* const* pMesh)
{
    if (vecMaterialList.size() > 0)
    {
        std::unordered_map<int, std::vector<FBXVertex>> fbxBufferMap;
        std::unordered_map<int, std::vector<int>>       fbxIndexMap;
        std::unordered_map<int, unsigned long long>     fbxMaterialMap;

        int i = 0;
        for (int const& iMaterial : vecMaterialList)
        {
            if (fbxBufferMap.count(iMaterial) == 0)
            {
                fbxBufferMap[iMaterial] = std::vector<FBXVertex>();
            }
            if (fbxIndexMap.count(iMaterial) == 0)
            {
                fbxIndexMap[iMaterial] = std::vector<int>();
            }
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3]);
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3 + 1]);
            fbxBufferMap[iMaterial].push_back(vecVertexList[i * 3 + 2]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3 + 1]);
            fbxIndexMap[iMaterial].push_back(vecIndexList[i * 3 + 2]);
            i++;
        }

        const ofbx::Material* pMaterial;
        int                   iCount = 0;
        for (auto const& pair : fbxBufferMap)
        {
            pMaterial = (*pMesh)->getMaterial(pair.first);
            fbxMaterialMap[pair.first] = pMaterial->id;
            iCount++;
        }

        if (iCount > 0)
        {
            for (int i = 0; i < iCount; i++)
            {
                m_bufferList.push_back(new FBXBuffer(fbxBufferMap[i], fbxIndexMap[i], fbxMaterialMap[i]));
            }
        }
    }

    else            // model has 0 or 1 material
    {
        unsigned long long id = 0;
        int                count = (*pMesh)->getMaterialCount();
        if (count != 0)
        {
            id = (*pMesh)->getMaterial(0)->id;
        }
        m_bufferList.push_back(new FBXBuffer(vecVertexList, vecIndexList, id));
    }
}

FBXObjectBuffer::~FBXObjectBuffer()
{
    delete m_pDiffuseBuffer;
    for (auto const buffer : m_bufferList)
        delete buffer;
}

// returns false if model wans't rendered due being out of screen, too far, something went wrong etc.
bool CFBXTemplate::Render(IDirect3DDevice9* pDevice, CFBXScene* pScene, D3DMATRIX& pOffsetMatrix)
{
    bool             bRenderedAtLeastOneMesh = false;
    FBXObjectBuffer* pObjectBuffer;
    CFBXTextureSet*  pTextureSet;
    float            fDrawDistance;
    D3DMATRIX        pTempMatrix;
    CMatrix          pTemplateObjectMatrix;
    CVector          vecTemplatePosition;
    CVector          vecTemplateObjectPosition;
    bool             bRenderDebug = g_pCore->GetFBX()->GetDevelopmentModeEnabled();
    m_pViewMatrix->GetBuffer((float*)m_pObjectMatrix);
    vecTemplatePosition = m_pViewMatrix->GetPosition();

    pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
    pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);
    pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

    g_pCore->GetGame()->GetCamera()->GetMatrix(m_pCameraMatrix);
    CVector vecCameraPosition = m_pCameraMatrix->GetPosition();
    pDevice->SetVertexDeclaration(g_pCore->GetFBX()->GetVertexDeclaration(VERTEX_TYPE_POS_NORMAL_TEXTURE_DIFFUSE));

    pDevice->SetLight(7, g_pCore->GetFBX()->GetGlobalLight());
    pDevice->LightEnable(7, TRUE);

    DWORD         materialDiffuse;
    eCullMode     cullMode;
    D3DMATERIAL9* pMaterial;
    D3DXCOLOR*    globalAmbient = g_pCore->GetFBX()->GetGlobalAmbient();
    DWORD         colorGlobalAmbient =
        D3DCOLOR_ARGB((DWORD)(globalAmbient->a * 255), (DWORD)(globalAmbient->r * 255), (DWORD)(globalAmbient->g * 255), (DWORD)(globalAmbient->b * 255));

    float    globalLighting = g_pCore->GetFBX()->GetGlobalLighting();
    D3DCOLOR d3GlobalLightingColor = (D3DCOLOR)D3DXCOLOR(globalLighting, globalLighting, globalLighting, 1.0f);

    for (auto const& object : m_objectMap)
    {
        // Reset transform for each object each time
        pDevice->SetTransform(D3DTS_TEXTURE0, g_pCore->GetFBX()->GetMatrixUVFlip());

        object.second->GetMaterialDiffuseColor(materialDiffuse);

        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CONSTANT);
        pDevice->SetTextureStageState(1, D3DTSS_CONSTANT, colorGlobalAmbient);

        pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CONSTANT);
        pDevice->SetTextureStageState(2, D3DTSS_CONSTANT, d3GlobalLightingColor);

        pDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_CONSTANT);
        pDevice->SetTextureStageState(3, D3DTSS_CONSTANT, materialDiffuse);

        pDevice->SetTransform(D3DTS_WORLDMATRIX(0), &pOffsetMatrix);

        pObjectBuffer = pScene->GetFBXBuffer(object.second->GetObjectId());
        if (pObjectBuffer != nullptr)
        {
            object.second->GetMatrix(m_pObjectMatrix);
            pDevice->MultiplyTransform(D3DTS_WORLDMATRIX(0), m_pObjectMatrix);
            pDevice->GetTransform(D3DTS_WORLDMATRIX(0), &pTempMatrix);

            pTemplateObjectMatrix = CMatrix((float*)&pTempMatrix.m);

            fDrawDistance = object.second->GetDrawDistance();
            float fCameraDistance = (pTemplateObjectMatrix.GetPosition() - vecCameraPosition).Length();

            pDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            pDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            pDevice->SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);
            pDevice->SetTextureStageState(3, D3DTSS_CONSTANT, D3DCOLOR_ARGB(object.second->GetOpacityFromDistance(fCameraDistance), 255,255,255));

            if (fCameraDistance < fDrawDistance)
            {
                object.second->GetCullMode(cullMode);
                pDevice->SetRenderState(D3DRS_CULLMODE, cullMode);
                if (bRenderDebug)
                {
                    CFBXDebugging::DrawBoundingBox(object.second->GetBoundingBox(), pTemplateObjectMatrix, SColorRGBA(0, 255, 0, 255), 2.0f);
                    CFBXDebugging::DrawDebuggingInformation(object.second, pTemplateObjectMatrix);
                }
                for (auto const& pBuffer : pObjectBuffer->m_bufferList)
                {
                    if (pBuffer->m_ullMaterialId == 0)
                    {
                        pDevice->SetTexture(0, nullptr);
                    }
                    else
                    {
                        pTextureSet = pScene->GetTextureSet(pBuffer->m_ullMaterialId);
                        if (pTextureSet != nullptr)
                        {
                            if (pTextureSet->diffuse != nullptr)
                            {
                                // Set texture addressing mode
                                pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, pTextureSet->diffuse->m_TextureAddress);
                                pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, pTextureSet->diffuse->m_TextureAddress);

                                if (pTextureSet->diffuse->m_TextureAddress == TADDRESS_BORDER)
                                    pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, pTextureSet->diffuse->m_uiBorderColor);
                                pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
                                pDevice->SetTexture(0, pTextureSet->diffuse->m_pD3DTexture);
                            }
                            else
                            {
                                pDevice->SetTexture(0, g_pCore->GetFBX()->GetBlankTexture()->m_pD3DTexture);
                            }

                            if (pTextureSet->normal != nullptr)
                            {
                                // Set texture addressing mode
                                pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, pTextureSet->normal->m_TextureAddress);
                                pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, pTextureSet->normal->m_TextureAddress);

                                if (pTextureSet->normal->m_TextureAddress == TADDRESS_BORDER)
                                    pDevice->SetSamplerState(1, D3DSAMP_BORDERCOLOR, pTextureSet->normal->m_uiBorderColor);
                                pDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
                                pDevice->SetTexture(1, pTextureSet->normal->m_pD3DTexture);
                            }
                            else
                            {
                                pDevice->SetTexture(1, nullptr);
                            }
                        }
                    }

                    object.second->GetMaterial(pMaterial);
                    pDevice->SetMaterial(pMaterial);

                    pBuffer->m_pFBXVertexBuffer->Select(0);
                    pObjectBuffer->m_pDiffuseBuffer->Select(1);
                    pDevice->SetIndices(pBuffer->m_pIndexBuffer);

                    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, pBuffer->m_iFacesCount);
                    bRenderedAtLeastOneMesh = true;
                }
            }
        }
    }

    pDevice->LightEnable(7, FALSE);

    return bRenderedAtLeastOneMesh;
}

CFBXTemplate::CFBXTemplate()
{
    m_pViewMatrix = new CMatrix();
    m_pObjectMatrix = new D3DMATRIX();
    m_pCameraMatrix = new CMatrix();
    m_pBoundingBox = new CFBXBoundingBox();
}

CFBXTemplate::~CFBXTemplate()
{
    for (auto const pair : m_objectMap)
        delete pair.second;

    delete m_pViewMatrix;
    delete m_pObjectMatrix;
    delete m_pCameraMatrix;
    delete m_pBoundingBox;
}

unsigned int CFBXTemplate::AddTemplateObject(CFBXTemplateObject* pObject)
{
    m_objectMap[m_uiNextFreeObjectId] = pObject;
    UpdateBoundingBox();
    pObject->m_indexId = m_uiNextFreeObjectId;
    return m_uiNextFreeObjectId++;
}

bool CFBXTemplate::RemoveObject(unsigned int uiObject)
{
    if (!IsModelValid(uiObject))
        return false;

    delete m_objectMap[uiObject];
    m_objectMap.erase(uiObject);
    UpdateBoundingBox();
    return true;
}

void CFBXTemplate::GetBoundingBoxCornersByMatrix(CVector vecCorner[8], CMatrix& matrix)
{
    CFBXBoundingBox* pBoundingBox = GetBoundingBox();
    pBoundingBox->GetBoundingBoxCornersByMatrix(vecCorner, matrix);
}

void CFBXTemplate::UpdateBoundingBox()
{
    CVector          min;
    CVector          max;
    float            radius;
    CFBXBoundingBox* pBoundingBox;
    CVector          vecMin, vecMax, vecPosition;
    for (const auto& pair : m_objectMap)
    {
        pBoundingBox = pair.second->GetBoundingBox();
        pair.second->GetPosition(vecPosition);
        vecMin = pBoundingBox->min + vecPosition;
        vecMax = pBoundingBox->max + vecPosition;
        min.fX = std::min(vecMin.fX, min.fX);
        min.fY = std::min(vecMin.fY, min.fY);
        min.fZ = std::min(vecMin.fZ, min.fZ);
        max.fX = std::max(vecMax.fX, max.fX);
        max.fY = std::max(vecMax.fY, max.fY);
        max.fZ = std::max(vecMax.fZ, max.fZ);
    }

    float fDistanceX = min.fX - max.fX;
    float fDistanceY = min.fY - max.fY;
    float fDistanceZ = min.fZ - max.fZ;
    m_pBoundingBox->min = min;
    m_pBoundingBox->max = max;
    m_pBoundingBox->radius = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
}

CFBXTemplateObject::CFBXTemplateObject(unsigned long long ullObjectId, CFBXScene* pScene) : m_ullObjectId(ullObjectId), m_pScene(pScene)
{
    m_pViewMatrix = new CMatrix();
    m_pBoundingBox = new CFBXBoundingBox();
    m_pMaterial = new D3DMATERIAL9();
    m_pMaterial->Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    m_pMaterial->Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
    m_pMaterial->Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.8f);            // depends
    m_pMaterial->Emissive = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
    m_pMaterial->Power = 5.0f;

    UpdateBoundingBox();
}

CFBXTemplateObject::~CFBXTemplateObject()
{
    delete m_pViewMatrix;
    delete m_pMaterial;
    delete m_pBoundingBox;
}

void CFBXTemplateObject::GetMaterialDiffuseColor(DWORD& color)
{
    color = D3DCOLOR_ARGB((DWORD)(m_pMaterial->Diffuse.a * 255), (DWORD)(m_pMaterial->Diffuse.r * 255), (DWORD)(m_pMaterial->Diffuse.g * 255),
                          (DWORD)(m_pMaterial->Diffuse.b * 255));
}

ushort CFBXTemplateObject::GetOpacityFromDistance(float fDistance)
{
    if (m_bUseCustomOpacity || fDistance == -1)
        return m_usOpacity;

    if (fDistance < m_fDrawDistance - m_fFadeDistance)
        return 255;

    return (m_fDrawDistance - fDistance) / m_fFadeDistance * 255;
}

void CFBXTemplateObject::UpdateBoundingBox()
{
    CVector min;
    CVector max;
    float   radius;

    m_pScene->GetBoundingBox(m_ullObjectId, min, max, radius);
    m_pBoundingBox->min = min;
    m_pBoundingBox->max = max;
    m_pBoundingBox->radius = radius;
}

unsigned int CFBXScene::AddTemplete(CFBXTemplate* pTemplate)
{
    m_templateMap[m_uiNextFreeTemplateId] = pTemplate;
    return m_uiNextFreeTemplateId++;
}

FBXObjectBuffer* CFBXScene::GetFBXBuffer(unsigned long long ullId)
{
    if (m_mapMeshBuffer.find(ullId) != m_mapMeshBuffer.end())
    {
        return m_mapMeshBuffer[ullId];
    }
    return nullptr;
}

bool CFBXScene::CreateFBXBuffer(const ofbx::Object* const* pObject)
{
    if ((*pObject)->getType() != ofbx::Object::Type::MESH)
        return false;

    const ofbx::Mesh* const* pMesh = (const ofbx::Mesh* const*)pObject;
    const ofbx::Geometry*    pGeometry = (*pMesh)->getGeometry();
    std::vector<FBXVertex>   vecVertices;
    std::vector<int>         vecIndices(pGeometry->getIndicesCount());
    std::vector<int>         vecMaterials(pGeometry->getMaterialCount());
    std::vector<DWORD>       vecColors;

    const CVector*    vertices = pGeometry->getVertices();
    const ofbx::Vec3* normals = pGeometry->getNormals();
    const ofbx::Vec4* colors = pGeometry->getColors();
    const ofbx::Vec2* UVs = pGeometry->getUVs(0);
    const CVector*    vertex;
    const ofbx::Vec3* normal;
    const ofbx::Vec2* uv;
    DWORD             dColor;
    if (pGeometry->getColorCount() == 0)
    {
        if (m_mapMeshMaterials[*pMesh].size() > 0)
        {
            const ofbx::Material* pMaterialFirst = m_mapMeshMaterials[*pMesh].at(0);
            ofbx::Color           diffuseColor = (*pMaterialFirst).getDiffuseColor();
            dColor = D3DCOLOR_XRGB((DWORD)(diffuseColor.r * 255), (DWORD)(diffuseColor.g * 255), (DWORD)(diffuseColor.b * 255));
        }
        else
        {
            dColor = D3DCOLOR_XRGB(255, 255, 255);
        }
        for (int i = 0; i < pGeometry->getVertexCount(); i++)
        {
            vertex = vertices + i;
            normal = normals + i;
            uv = UVs + i;
            vecVertices.emplace_back(vertex->fX, vertex->fY, vertex->fZ, normal->x, normal->y, normal->z, uv->x, uv->y);
            vecColors.emplace_back(dColor);
        }
    }
    else
    {
        const ofbx::Vec4* color;
        for (int i = 0; i < pGeometry->getVertexCount(); i++)
        {
            vertex = vertices + i;
            normal = normals + i;
            color = colors + i;
            uv = UVs + i;
            dColor = D3DCOLOR_XRGB((DWORD)((*color).x * 255), (DWORD)((*color).y * 255), (DWORD)((*color).z * 255), (DWORD)((*color).w * 255));
            vecVertices.emplace_back(vertex->fX, vertex->fY, vertex->fZ, normal->x, normal->y, normal->z, uv->x, uv->y);
            vecColors.push_back(dColor);
        }
    }

    memcpy(vecIndices.data(), pGeometry->getFaceIndices(), sizeof(int) * vecIndices.size());
    memcpy(vecMaterials.data(), pGeometry->getMaterials(), sizeof(int) * vecMaterials.size());

    FBXObjectBuffer* pBuffer = new FBXObjectBuffer(vecVertices, vecIndices, vecMaterials, pMesh);
    pBuffer->m_pDiffuseBuffer = new FBXVertexBuffer(vecColors, D3DFVF_DIFFUSE);
    AddBuffer((*pObject)->id, pBuffer);
    return true;
}

bool CFBXScene::AddBuffer(unsigned long long ullObjectId, FBXObjectBuffer* pBuffer)
{
    if (m_mapMeshBuffer.find(ullObjectId) != m_mapMeshBuffer.end())
        return false;

    m_mapMeshBuffer[ullObjectId] = pBuffer;
    return true;
}

bool CFBXScene::IsTemplateModelValid(unsigned int uiTemplate, unsigned int uiModelId)
{
    if (m_templateMap.count(uiTemplate) == 0)
        return false;

    CFBXTemplate* pTemplate = m_templateMap[uiTemplate];
    if (!pTemplate->IsModelValid(uiModelId))
        return false;

    return true;
}

unsigned int CFBXScene::AddMeshToTemplate(unsigned int uiTemplate, unsigned long long uiModelId)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplate];
    CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject(uiModelId, this);
    unsigned int        uiNewObjectId = pTemplate->AddTemplateObject(pTemplateObject);
    return uiNewObjectId;
}

bool CFBXScene::RemoveObjectFromTemplate(unsigned int uiTemplate, unsigned int uiObjectId)
{
    if (!IsTemplateModelValid(uiTemplate, uiObjectId))
        return false;

    CFBXTemplate* pTemplate = m_templateMap[uiTemplate];
    return pTemplate->RemoveObject(uiObjectId);
}

bool CFBXScene::GetBoundingBox(unsigned long long ullObjectId, CVector& min, CVector& max, float& fRadius)
{
    if (m_geometryBoundingBox.count(ullObjectId) == 0)
        return false;

    CFBXBoundingBox* boundingBox = m_geometryBoundingBox[ullObjectId];

    min = boundingBox->min;
    max = boundingBox->max;
    fRadius = boundingBox->radius;
    return true;
}

CTextureItem* CFBXScene::GetTexture(const ofbx::Texture* pTexture)
{
    if (pTexture == nullptr)
        return nullptr;

    if (m_pClientFBXInterface == nullptr)
        return nullptr;

    if (!m_pClientFBXInterface->IsTextureCreated(pTexture->id))
    {
        char fileName[128];
        pTexture->getFileName().toString(fileName);
        SString  name = fileName;
        CPixels* pTexturePixels = m_textureContentList[name];

        m_pClientFBXInterface->CreateTexture(pTexture->id, pTexturePixels);
    }
    CMaterialItem* pMaterialItem = m_pClientFBXInterface->GetTextureById(pTexture->id);
    if (CShaderItem* pShaderItem = DynamicCast<CShaderItem>(pMaterialItem))
    {
        // If material is a shader, use its current instance
        pMaterialItem = pShaderItem->m_pShaderInstance;
    }
    if (CTextureItem* pTextureItem = DynamicCast<CTextureItem>(pMaterialItem))
    {
        return pTextureItem;
    }
    return nullptr;
}

CFBXTextureSet* CFBXScene::GetTextureSet(unsigned long long ullMaterialId)
{
    const ofbx::Material* const* pMaterial = m_materialList[ullMaterialId];
    if (pMaterial == nullptr)
        return nullptr;

    if (m_mapMaterialTextureSet.count(ullMaterialId) == 0)
    {
        CFBXTextureSet* pTextureSet = new CFBXTextureSet();

        const ofbx::Texture* pDiffuseTexture = (*pMaterial)->getTexture(ofbx::Texture::TextureType::DIFFUSE);
        pTextureSet->diffuse = GetTexture(pDiffuseTexture);

        const ofbx::Texture* pNormalTexture = (*pMaterial)->getTexture(ofbx::Texture::TextureType::NORMAL);
        pTextureSet->normal = GetTexture(pNormalTexture);
        m_mapMaterialTextureSet[ullMaterialId] = pTextureSet;
    }
    return m_mapMaterialTextureSet[ullMaterialId];
}

CFBXScene::CFBXScene(ofbx::IScene* scene, CClientFBXInterface* pClientFBXInterface) : m_pScene(scene)
{
    m_pRoot = m_pScene->getRoot();
    m_pClientFBXInterface = pClientFBXInterface;

    CacheObjects();
    CacheMeshes();
    CacheTextures();
    CacheMaterials();
    CacheMeshMaterials();
    CacheBoundingBoxes();

    m_fUnitScaleFactor = 1 / (m_pScene->getGlobalSettings()->OriginalUnitScaleFactor / m_pScene->getGlobalSettings()->UnitScaleFactor);

    CreateBaseTemplate();
    IDirect3DDevice9* pDevice = g_pCore->GetGraphics()->GetDevice();
}

CFBXScene::~CFBXScene()
{
    for (auto const pair : m_templateMap)
        delete pair.second;

    for (auto const pair : m_geometryBoundingBox)
        delete pair.second;

    for (auto const pair : m_mapMeshBuffer)
        delete pair.second;

    for (auto const pair : m_mapMaterialTextureSet)
        delete pair.second;

    for (auto const pair : m_textureContentList)
        delete pair.second;
    m_pScene->release();

    g_pCore->GetFBX()->RemoveScene(this);
}

// makes as much excact copy of scene as possible
void CFBXScene::CreateBaseTemplate()
{
    unsigned int  uiTemplateId = CreateTemplate();
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetViewMatrix()->SetPosition(CVector(0, 0, 0));
    pTemplate->GetViewMatrix()->SetRotation(CVector(0, 0, 0));
    pTemplate->GetViewMatrix()->SetScale(CVector(1, 1, 1));
    int                i = 0;
    unsigned long long ullObjectId;
    double             m[16] = {};
    CMatrix            matrix;
    float              array[16];

    for (const auto& pair : m_objectList)
    {
        if (CreateFBXBuffer(pair.second))
        {
            ullObjectId = (*pair.second)->id;

            memcpy(m, (*pair.second)->getGlobalTransform().m, sizeof(double) * 16);
            for (char i = 0; i < 16; i++)
                array[i] = (float)m[i];

            matrix = CMatrix(array);

            CFBXTemplateObject* pTemplateObject = new CFBXTemplateObject(ullObjectId, this);
            pTemplateObject->SetPosition(matrix.GetPosition() * GetUnitScaleFactor());
            pTemplateObject->SetRotation(matrix.GetRotation() * GetUnitScaleFactor());
            pTemplateObject->SetScale(matrix.GetScale() * GetUnitScaleFactor());
            pTemplate->AddTemplateObject(pTemplateObject);
            i++;
        }
    }
}

unsigned int CFBXScene::CreateTemplate()
{
    CFBXTemplate* pTemplate = new CFBXTemplate();
    unsigned int  uiTemplateId = AddTemplete(pTemplate);
    pTemplate->m_uiTemplateId = uiTemplateId;
    return uiTemplateId;
}

bool CFBXScene::RemoveTemplate(unsigned int uiTemplateId)
{
    if (!IsTemplateValid(uiTemplateId))
        return false;

    delete m_templateMap[uiTemplateId];
    MapRemove(m_templateMap, uiTemplateId);
    return true;
}

void CFBXScene::AddToRenderQueue(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale)
{
    CMatrix matrix;
    matrix.SetPosition(vecPosition);
    matrix.SetRotation(vecRotation);
    matrix.SetScale(vecScale);

    CFBXRenderItem renderItem;
    renderItem.m_uiTemplateId = uiTemplateId;
    renderItem.m_matrix = matrix;
    m_vecTemporaryRenderLoop.push_back(renderItem);
}

void CFBXScene::CacheObjects()
{
    const ofbx::Object* const* pObject;
    for (int i = 0; i < m_pScene->getAllObjectCount(); i++)
    {
        pObject = m_pScene->getAllObjects() + i;
        m_objectList[(*pObject)->id] = pObject;
        m_objectIdsList.push_back((*pObject)->id);
    }
}

CFBXBoundingBox* CFBX::CalculateBoundingBox(const ofbx::Mesh* pMesh)
{
    const ofbx::Geometry* pGeometry = pMesh->getGeometry();

    CFBXBoundingBox* boundingBox = new CFBXBoundingBox();

    const CVector* vertices = pGeometry->getVertices();
    D3DXVECTOR3    vecMin;
    D3DXVECTOR3    vecMax;
    D3DXVECTOR3    vecCenter;
    float          fRadius;

    D3DXComputeBoundingBox((D3DXVECTOR3*)vertices, pGeometry->getVertexCount(), sizeof(D3DXVECTOR3), &vecMin, &vecMax);
    D3DXComputeBoundingSphere((D3DXVECTOR3*)vertices, pGeometry->getVertexCount(), sizeof(D3DXVECTOR3), &vecCenter, &fRadius);
    boundingBox->min.fX = vecMin.x;
    boundingBox->min.fY = vecMin.y;
    boundingBox->min.fZ = vecMin.z;
    boundingBox->max.fX = vecMax.x;
    boundingBox->max.fY = vecMax.y;
    boundingBox->max.fZ = vecMax.z;
    boundingBox->radius = fRadius;
    boundingBox->center.fX = vecCenter.x;
    boundingBox->center.fY = vecCenter.y;
    boundingBox->center.fZ = vecCenter.z;

    return boundingBox;
}

void CFBXScene::CacheMeshes()
{
    const ofbx::Mesh* pMesh;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        m_meshName[pMesh->id] = pMesh->name;
        m_meshList[pMesh->id] = pMesh;
    }
}

void CFBXScene::CacheBoundingBoxes()
{
    for (const auto& pair : m_meshList)
    {
        m_geometryBoundingBox[pair.second->id] = g_pCore->GetFBX()->CalculateBoundingBox(pair.second);
    }
}

void CFBXScene::CacheTextures()
{
    SString                     name;
    const ofbx::DataView const* pFilePath;
    const ofbx::DataView const* pContent;
    const ofbx::Texture*        pTexture;
    int                         iTextureSize;
    for (int i = 0; i < m_pScene->getTexturesCount(); i++)
    {
        pContent = *(m_pScene->getTextureContent() + i);
        pTexture = *(m_pScene->getTextures() + i);
        pFilePath = *(m_pScene->getTextureFilePath() + i);

        char fileName[120];
        pFilePath->toString(fileName);
        SString strFilePath = fileName;

        iTextureSize = pContent->end - pContent->begin - 4;
        CPixels* pPixels = new CPixels();

        pPixels->SetSize(iTextureSize);
        memcpy(pPixels->GetData(), pContent->begin + 4, pPixels->GetSize());

        m_textureContentList[strFilePath] = pPixels;
        m_texturePath[pTexture->id] = strFilePath;
    }
}

void CFBXScene::CacheMeshMaterials()
{
    const ofbx::Mesh* pMesh;
    for (int i = 0; i < m_pScene->getMeshCount(); i++)
    {
        pMesh = m_pScene->getMesh(i);
        std::vector<const ofbx::Material*> vecMaterial;
        for (int iMaterial = 0; iMaterial < pMesh->getMaterialCount(); iMaterial++)
        {
            vecMaterial.push_back(pMesh->getMaterial(iMaterial));
        }
        m_mapMeshMaterials[pMesh] = vecMaterial;
    }
}

void CFBXScene::CacheMaterials()
{
    const ofbx::Material* const* pMaterial;
    for (int i = 0; i < m_pScene->getMaterialsCount(); i++)
    {
        pMaterial = m_pScene->getMaterials() + i;
        m_materialList[(*pMaterial)->id] = pMaterial;
    }
}

void CFBXScene::GetAllTemplatesIds(std::vector<unsigned int>& vecIds)
{
    for (auto const& pair : m_templateMap)
    {
        vecIds.emplace_back(pair.first);
    }
}

bool CFBXScene::GetMeshName(long long int ulId, SString& strMeshName)
{
    if (m_meshName.count(ulId) != 0)
    {
        strMeshName = m_meshName[ulId];
        return true;
    }
    return false;
}

bool CFBXScene::GetTexturePath(long long int ulId, SString& strTexturePath)
{
    if (m_texturePath.count(ulId) != 0)
    {
        strTexturePath = m_texturePath[ulId];
        return true;
    }
    return false;
}

bool CFBXScene::GetAllTemplatesModelsIds(std::vector<unsigned int>& vecIds, unsigned int uiTemplateId)
{
    if (!IsTemplateValid(uiTemplateId))
        return false;

    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    for (auto const& pair : pTemplate->GetObjectsMap())
    {
        vecIds.emplace_back(pair.first);
    }
    return true;
}

bool CFBXScene::RenderTemplate(CFBXTemplate* pTemplate, CMatrix& pMatrix, CVector& vecCameraPosition)
{
    CVector          vecTemplatePosition;
    CVector          vecTemplateOffset;
    float            fDrawDistance;
    CFBXBoundingBox* pBoundingBox;
    CVector          vecTemp;
    CMatrix          matrix;
    CVector          vecPosition;

    pTemplate->GetPosition(vecTemplatePosition);
    fDrawDistance = pTemplate->GetDrawDistance();

    matrix = pMatrix * *pTemplate->GetViewMatrix();

    vecPosition = pMatrix.GetPosition() + vecTemplatePosition;
    if ((vecPosition - vecCameraPosition).LengthSquared() < fDrawDistance * fDrawDistance)
    {
        pBoundingBox = pTemplate->GetBoundingBox();
        if (g_pCore->GetFBX()->CheckCulling(pBoundingBox, &matrix))
        {
            matrix.GetBuffer(reinterpret_cast<float*>(&m_ObjectMatrix));
            if (pTemplate->Render(m_pDevice, this, m_ObjectMatrix))
            {
                if (bRenderDebug)
                {
                    CFBXDebugging::DrawBoundingBox(pTemplate, matrix);
                    CFBXDebugging::DrawDebuggingInformation(pTemplate, matrix);
                }
            }
            return true;
        }
    }
    return false;
}

bool CFBXScene::RenderScene(IDirect3DDevice9* pDevice, CFrustum* pFrustum, CVector& vecCameraPosition)
{
    if (!m_pClientFBXInterface->IsLoaded())
        return false;

    m_pDevice = pDevice;
    bRenderDebug = g_pCore->GetFBX()->GetDevelopmentModeEnabled() && g_pCore->GetFBX()->GetDevelopmentModeEnabled();
    m_pClientFBXInterface->Render();

    bool          renderedAtLeastOneTemplate = false;
    CFBXTemplate* pTemplate;
    CVector       vecPosition;

    unsigned short usDimension = m_pClientFBXInterface->GetPlayerDimension();
    unsigned char  ucInterior = static_cast<unsigned char>(g_pCore->GetGame()->GetWorld()->GetCurrentArea());

    static std::unordered_map<unsigned long long, std::vector<CMatrix>> pTemplatesMatrix;

    pTemplatesMatrix.clear();
    m_pClientFBXInterface->GetTemplatesRenderingMatrix(pTemplatesMatrix, ucInterior, usDimension);

    for (auto const& renderItem : m_vecTemporaryRenderLoop)
    {
        if (!IsTemplateValid(renderItem.m_uiTemplateId))
            continue;

        vecPosition = renderItem.m_matrix.GetPosition();
        pTemplate = m_templateMap[renderItem.m_uiTemplateId];
        if (RenderTemplate(pTemplate, (CMatrix&)renderItem.m_matrix, vecCameraPosition))
        {
            CFBXDebugging::AddRenderedTemplate();
            renderedAtLeastOneTemplate = true;
        }
    }

    for (auto const& pair : pTemplatesMatrix)
    {
        if (!IsTemplateValid(pair.first))
            continue;
        pTemplate = m_templateMap[pair.first];
        for (CMatrix matrix : pair.second)
        {
            if (RenderTemplate(pTemplate, matrix, vecCameraPosition))
            {
                CFBXDebugging::AddRenderedTemplate();
                renderedAtLeastOneTemplate = true;
            }
        }
    }
    ListClearAndReserve(m_vecTemporaryRenderLoop);
    return renderedAtLeastOneTemplate;
}

void CFBXScene::GetTemplatePosition(unsigned int uiTemplateId, CVector& position)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetPosition(position);
}

void CFBXScene::GetTemplateRotation(unsigned int uiTemplateId, CVector& rotation)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetRotation(rotation);
}

void CFBXScene::GetTemplateScale(unsigned int uiTemplateId, CVector& scale)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->GetScale(scale);
}

void CFBXScene::GetTemplateDrawDistance(unsigned int uiTemplateId, float& drawDistance)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    drawDistance = pTemplate->GetDrawDistance();
}

void CFBXScene::SetTemplatePosition(unsigned int uiTemplateId, CVector& position)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetPosition(position);
}

void CFBXScene::SetTemplateRotation(unsigned int uiTemplateId, CVector& rotation)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetRotation(rotation);
}

void CFBXScene::SetTemplateScale(unsigned int uiTemplateId, CVector& scale)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetScale(scale);
}

void CFBXScene::SetTemplateDrawDistance(unsigned int uiTemplateId, float fDrawDistance)
{
    CFBXTemplate* pTemplate = m_templateMap[uiTemplateId];
    pTemplate->SetDrawDistance(fDrawDistance);
}

void CFBXScene::GetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetPosition(position);
}

void CFBXScene::GetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetRotation(rotation);
}

void CFBXScene::GetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetScale(scale);
}

void CFBXScene::GetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fDrawDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    fDrawDistance = pTemplateObject->GetDrawDistance();
}

void CFBXScene::SetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetPosition(position);
}

void CFBXScene::SetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetRotation(rotation);
}

void CFBXScene::SetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetScale(scale);
}

void CFBXScene::SetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fDrawDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetDrawDistance(fDrawDistance);
}

void CFBXScene::SetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode cullMode)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetCullMode(cullMode);
}

void CFBXScene::GetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode& cullMode)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetCullMode(cullMode);
}

void CFBXScene::SetTemplateModelUseCustomOpacity(unsigned int uiTemplateId, unsigned int uiModelId, bool bUseCustomOpacity)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetUseCustomOpacity(bUseCustomOpacity);
}

void CFBXScene::GetTemplateModelUseCustomOpacity(unsigned int uiTemplateId, unsigned int uiModelId, bool& bUseCustomOpacity)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetUseCustomOpacity(bUseCustomOpacity);
}

void CFBXScene::SetTemplateModelOpacity(unsigned int uiTemplateId, unsigned int uiModelId, ushort usOpacity)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetOpacity(usOpacity);
}

void CFBXScene::GetTemplateModelOpacity(unsigned int uiTemplateId, unsigned int uiModelId, ushort& usOpacity)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetOpacity(usOpacity);
}

void CFBXScene::SetTemplateModelFadeDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fFadeDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->SetFadeDistance(fFadeDistance);
}

void CFBXScene::GetTemplateModelFadeDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fFadeDistance)
{
    CFBXTemplate*       pTemplate = m_templateMap[uiTemplateId];
    CFBXTemplateObject* pTemplateObject = pTemplate->GetObjectById(uiModelId);
    pTemplateObject->GetFadeDistance(fFadeDistance);
}

CFBXScene* CFBX::AddScene(ofbx::IScene* pScene, CClientFBXInterface* pClientFBXInterface)
{
    CFBXScene* pFBXScene = new CFBXScene(pScene, pClientFBXInterface);

    m_sceneList.push_back(pFBXScene);
    return pFBXScene;
}

void CFBX::RemoveScene(CFBXScene* pScene)
{
    ListRemove(m_sceneList, pScene);
}

const char* CFBX::GetObjectType(const ofbx::Object const* pObject)
{
    return ofbx::GetObjectType(pObject);
}

bool CFBX::HasAnyFBXLoaded()
{
    return m_sceneList.size() > 0;
}

CFBX::CFBX()
{
    m_pDevice = nullptr;

    ZeroMemory(&m_globalLight, sizeof(m_globalLight));
    m_globalLight.Type = D3DLIGHT_DIRECTIONAL;
    m_globalLight.Diffuse.r = 0.4f;
    m_globalLight.Diffuse.g = 1.0f;
    m_globalLight.Diffuse.b = 0.4f;
    m_globalLight.Diffuse.a = 1.0f;
    m_globalLight.Ambient.r = 1.0f;
    m_globalLight.Ambient.g = 1.0f;
    m_globalLight.Ambient.b = 1.0f;
    m_globalLight.Ambient.a = 1.0f;
    m_globalLight.Specular = *(D3DCOLORVALUE*)&CVector4D(0.5f, 0.5f, 0.5f, 0.5f);
    m_globalLight.Direction = *(D3DVECTOR*)&CVector(0.0f, 0, -1.0f);
    m_globalLight.Attenuation0 = 0.2f;
    m_globalLight.Attenuation1 = 0.2f;
    m_globalLight.Attenuation2 = 0.2f;
    m_globalLight.Phi = 0.2f;

    m_globalAmbient = new D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
    m_globalLighting = 0.8f;

    CMatrix* matrixFixInvertedUVs = new CMatrix();
    matrixFixInvertedUVs->SetPosition(CVector(0, 0, 0));
    matrixFixInvertedUVs->SetScale(CVector(1, 1, 1));
    matrixFixInvertedUVs->SetRotation(CVector(PI * 1.5, 0, 0));
    m_pMatrixUVFlip = (D3DXMATRIX*)matrixFixInvertedUVs;

    m_pFrustum = new CFrustum();
}

void CFBX::Initialize()
{
    m_pDevice = g_pCore->GetGraphics()->GetDevice();

    D3DVERTEXELEMENT9 dwDeclPosNormalTexColor[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                                   {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
                                                   {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                                                   {1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
                                                   D3DDECL_END()};

    D3DVERTEXELEMENT9 dwDeclPosNormalTex[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                              {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
                                              {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                                              D3DDECL_END()};

    m_pDevice->CreateVertexDeclaration(dwDeclPosNormalTexColor, &m_pVertexDeclaration[VERTEX_TYPE_POS_NORMAL_TEXTURE_DIFFUSE]);
    m_pDevice->CreateVertexDeclaration(dwDeclPosNormalTex, &m_pVertexDeclaration[VERTEX_TYPE_POS_NORMAL_TEXTURE]);

    m_pBlankTexture =
        g_pCore->GetGraphics()->GetRenderItemManager()->CreateTexture(CalcMTASAPath("MTA\\cgui\\images\\blanktexture.jpg"), NULL, false, 2, 2, RFORMAT_DXT1);

    CFBXDebugging::Start();
}

CFBX::~CFBX()
{
}

void CFBX::UpdateFrustum(float screenDepth, D3DXMATRIX projectionMatrix, D3DXMATRIX viewMatrix)
{
    m_pFrustum->ConstructFrustum(screenDepth, projectionMatrix, viewMatrix);
}

bool CFBX::CheckCulling(CFBXBoundingBox* pBoundingBox, CMatrix* pMatrix)
{
    CVector vecPosition = pMatrix->GetPosition();
    if (!m_pFrustum->CheckSphere(pBoundingBox->center + vecPosition, pBoundingBox->radius))
        return false;

    CVector vecCenter = (pBoundingBox->max - pBoundingBox->min) / 2 + vecPosition;

    if (!m_pFrustum->CheckRectangle(pBoundingBox->center + vecPosition, pBoundingBox->max - pBoundingBox->min))
        return false;

    return true;
}

void CFBX::Render()
{
    unsigned char ucHour, ucMin;
    g_pCore->GetGame()->GetClock()->Get(&ucHour, &ucMin);
    if (ucHour > 23 || ucHour < 7)
    {
        m_globalLighting = 0.2f;
    }
    else
    {
        m_globalLighting = 0.8f;
    }

    CFBXDebugging::ResetScenesCounter();
    CFBXDebugging::ResetTemplatesCounter();
    CFBXDebugging::ResetObjectsCounter();

    if (m_pDevice != nullptr)
    {
        m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

        m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
        m_pDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);
        m_pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
        m_pDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);

        m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

        m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
        m_pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

        CVector vecCameraPosition;
        g_pCore->GetGame()->GetCamera()->GetMatrix(&m_pCameraMatrix);
        vecCameraPosition = m_pCameraMatrix.GetPosition();
        for (auto const& pFBXScene : m_sceneList)
        {
            if (pFBXScene->RenderScene(m_pDevice, m_pFrustum, vecCameraPosition))
            {
                CFBXDebugging::AddRenderedScene();
            }
        }
    }
}
