/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CProxyDirect3DEffect.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

DEFINE_GUID(CProxyDirect3DEffect_GUID, 0x128A025E, 0x0123, 0x04F1, 0x40, 0x60, 0x53, 0x19, 0x44, 0x56, 0x59, 0x42);

class CProxyDirect3DEffect : public ID3DXEffect
{
public:
    // ID3DXBaseEffect
    HRESULT __stdcall QueryInterface(REFIID iid, LPVOID* ppv);
    ULONG __stdcall AddRef() { return m_pOriginal->AddRef(); }
    ULONG __stdcall Release();

    // Descs
    HRESULT __stdcall GetDesc(D3DXEFFECT_DESC* pDesc) { return m_pOriginal->GetDesc(pDesc); }
    HRESULT __stdcall GetParameterDesc(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc) { return m_pOriginal->GetParameterDesc(hParameter, pDesc); }
    HRESULT __stdcall GetTechniqueDesc(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc) { return m_pOriginal->GetTechniqueDesc(hTechnique, pDesc); }
    HRESULT __stdcall GetPassDesc(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc) { return m_pOriginal->GetPassDesc(hPass, pDesc); }
    HRESULT __stdcall GetFunctionDesc(D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc) { return m_pOriginal->GetFunctionDesc(hShader, pDesc); }

    // Handle operations
    D3DXHANDLE __stdcall GetParameter(D3DXHANDLE hParameter, UINT Index) { return m_pOriginal->GetParameter(hParameter, Index); }
    D3DXHANDLE __stdcall GetParameterByName(D3DXHANDLE hParameter, LPCSTR pName) { return m_pOriginal->GetParameterByName(hParameter, pName); }
    D3DXHANDLE __stdcall GetParameterBySemantic(D3DXHANDLE hParameter, LPCSTR pSemantic) { return m_pOriginal->GetParameterBySemantic(hParameter, pSemantic); }
    D3DXHANDLE __stdcall GetParameterElement(D3DXHANDLE hParameter, UINT Index) { return m_pOriginal->GetParameterElement(hParameter, Index); }
    D3DXHANDLE __stdcall GetTechnique(UINT Index) { return m_pOriginal->GetTechnique(Index); }
    D3DXHANDLE __stdcall GetTechniqueByName(LPCSTR pName) { return m_pOriginal->GetTechniqueByName(pName); }
    D3DXHANDLE __stdcall GetPass(D3DXHANDLE hTechnique, UINT Index) { return m_pOriginal->GetPass(hTechnique, Index); }
    D3DXHANDLE __stdcall GetPassByName(D3DXHANDLE hTechnique, LPCSTR pName) { return m_pOriginal->GetPassByName(hTechnique, pName); }
    D3DXHANDLE __stdcall GetFunction(UINT Index) { return m_pOriginal->GetFunction(Index); }
    D3DXHANDLE __stdcall GetFunctionByName(LPCSTR pName) { return m_pOriginal->GetFunctionByName(pName); }
    D3DXHANDLE __stdcall GetAnnotation(D3DXHANDLE hObject, UINT Index) { return m_pOriginal->GetAnnotation(hObject, Index); }
    D3DXHANDLE __stdcall GetAnnotationByName(D3DXHANDLE hObject, LPCSTR pName) { return m_pOriginal->GetAnnotationByName(hObject, pName); }

    // Get/Set Parameters
    HRESULT __stdcall SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) { return m_pOriginal->SetValue(hParameter, pData, Bytes); }
    HRESULT __stdcall GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) { return m_pOriginal->GetValue(hParameter, pData, Bytes); }
    HRESULT __stdcall SetBool(D3DXHANDLE hParameter, BOOL b) { return m_pOriginal->SetBool(hParameter, b); }
    HRESULT __stdcall GetBool(D3DXHANDLE hParameter, BOOL* pb) { return m_pOriginal->GetBool(hParameter, pb); }
    HRESULT __stdcall SetBoolArray(D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) { return m_pOriginal->SetBoolArray(hParameter, pb, Count); }
    HRESULT __stdcall GetBoolArray(D3DXHANDLE hParameter, BOOL* pb, UINT Count) { return m_pOriginal->GetBoolArray(hParameter, pb, Count); }
    HRESULT __stdcall SetInt(D3DXHANDLE hParameter, INT n) { return m_pOriginal->SetInt(hParameter, n); }
    HRESULT __stdcall GetInt(D3DXHANDLE hParameter, INT* pn) { return m_pOriginal->GetInt(hParameter, pn); }
    HRESULT __stdcall SetIntArray(D3DXHANDLE hParameter, CONST INT* pn, UINT Count) { return m_pOriginal->SetIntArray(hParameter, pn, Count); }
    HRESULT __stdcall GetIntArray(D3DXHANDLE hParameter, INT* pn, UINT Count) { return m_pOriginal->GetIntArray(hParameter, pn, Count); }
    HRESULT __stdcall SetFloat(D3DXHANDLE hParameter, FLOAT f) { return m_pOriginal->SetFloat(hParameter, f); }
    HRESULT __stdcall GetFloat(D3DXHANDLE hParameter, FLOAT* pf) { return m_pOriginal->GetFloat(hParameter, pf); }
    HRESULT __stdcall SetFloatArray(D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) { return m_pOriginal->SetFloatArray(hParameter, pf, Count); }
    HRESULT __stdcall GetFloatArray(D3DXHANDLE hParameter, FLOAT* pf, UINT Count) { return m_pOriginal->GetFloatArray(hParameter, pf, Count); }
    HRESULT __stdcall SetVector(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) { return m_pOriginal->SetVector(hParameter, pVector); }
    HRESULT __stdcall GetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector) { return m_pOriginal->GetVector(hParameter, pVector); }
    HRESULT __stdcall SetVectorArray(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count)
    {
        return m_pOriginal->SetVectorArray(hParameter, pVector, Count);
    }
    HRESULT __stdcall GetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
    {
        return m_pOriginal->GetVectorArray(hParameter, pVector, Count);
    }
    HRESULT __stdcall SetMatrix(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) { return m_pOriginal->SetMatrix(hParameter, pMatrix); }
    HRESULT __stdcall GetMatrix(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) { return m_pOriginal->GetMatrix(hParameter, pMatrix); }
    HRESULT __stdcall SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
    {
        return m_pOriginal->SetMatrixArray(hParameter, pMatrix, Count);
    }
    HRESULT __stdcall GetMatrixArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) { return m_pOriginal->GetMatrixArray(hParameter, pMatrix, Count); }
    HRESULT __stdcall SetMatrixPointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count)
    {
        return m_pOriginal->SetMatrixPointerArray(hParameter, ppMatrix, Count);
    }
    HRESULT __stdcall GetMatrixPointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
    {
        return m_pOriginal->GetMatrixPointerArray(hParameter, ppMatrix, Count);
    }
    HRESULT __stdcall SetMatrixTranspose(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) { return m_pOriginal->SetMatrixTranspose(hParameter, pMatrix); }
    HRESULT __stdcall GetMatrixTranspose(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) { return m_pOriginal->GetMatrixTranspose(hParameter, pMatrix); }
    HRESULT __stdcall SetMatrixTransposeArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
    {
        return m_pOriginal->SetMatrixTransposeArray(hParameter, pMatrix, Count);
    }
    HRESULT __stdcall GetMatrixTransposeArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
    {
        return m_pOriginal->GetMatrixTransposeArray(hParameter, pMatrix, Count);
    }
    HRESULT __stdcall SetMatrixTransposePointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count)
    {
        return m_pOriginal->SetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
    }
    HRESULT __stdcall GetMatrixTransposePointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
    {
        return m_pOriginal->GetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
    }
    HRESULT __stdcall SetString(D3DXHANDLE hParameter, LPCSTR pString) { return m_pOriginal->SetString(hParameter, pString); }
    HRESULT __stdcall GetString(D3DXHANDLE hParameter, LPCSTR* ppString) { return m_pOriginal->GetString(hParameter, ppString); }
    HRESULT __stdcall SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture) { return m_pOriginal->SetTexture(hParameter, pTexture); }
    HRESULT __stdcall GetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9* ppTexture) { return m_pOriginal->GetTexture(hParameter, ppTexture); }
    HRESULT __stdcall GetPixelShader(D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9* ppPShader) { return m_pOriginal->GetPixelShader(hParameter, ppPShader); }
    HRESULT __stdcall GetVertexShader(D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9* ppVShader) { return m_pOriginal->GetVertexShader(hParameter, ppVShader); }

    // Set Range of an Array to pass to device
    // Usefull for sending only a subrange of an array down to the device
    HRESULT __stdcall SetArrayRange(D3DXHANDLE hParameter, UINT uStart, UINT uEnd) { return m_pOriginal->SetArrayRange(hParameter, uStart, uEnd); }
    // ID3DXBaseEffect

    // Pool
    HRESULT __stdcall GetPool(LPD3DXEFFECTPOOL* ppPool) { return m_pOriginal->GetPool(ppPool); }

    // Selecting and setting a technique
    HRESULT __stdcall SetTechnique(D3DXHANDLE hTechnique) { return m_pOriginal->SetTechnique(hTechnique); }
    D3DXHANDLE __stdcall GetCurrentTechnique() { return m_pOriginal->GetCurrentTechnique(); }
    HRESULT __stdcall ValidateTechnique(D3DXHANDLE hTechnique) { return m_pOriginal->ValidateTechnique(hTechnique); }
    HRESULT __stdcall FindNextValidTechnique(D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique)
    {
        return m_pOriginal->FindNextValidTechnique(hTechnique, pTechnique);
    }
    BOOL __stdcall IsParameterUsed(D3DXHANDLE hParameter, D3DXHANDLE hTechnique) { return m_pOriginal->IsParameterUsed(hParameter, hTechnique); }

    // Using current technique
    // Begin           starts active technique
    // BeginPass       begins a pass
    // CommitChanges   updates changes to any set calls in the pass. This should be called before
    //                 any DrawPrimitive call to d3d
    // EndPass         ends a pass
    // End             ends active technique
    HRESULT __stdcall Begin(UINT* pPasses, DWORD Flags) { return m_pOriginal->Begin(pPasses, Flags); }
    HRESULT __stdcall BeginPass(UINT Pass) { return m_pOriginal->BeginPass(Pass); }
    HRESULT __stdcall CommitChanges() { return m_pOriginal->CommitChanges(); }
    HRESULT __stdcall EndPass() { return m_pOriginal->EndPass(); }
    HRESULT __stdcall End() { return m_pOriginal->End(); }

    // Managing D3D Device
    HRESULT __stdcall GetDevice(LPDIRECT3DDEVICE9* ppDevice) { return m_pOriginal->GetDevice(ppDevice); }
    HRESULT __stdcall OnLostDevice() { return m_pOriginal->OnLostDevice(); }
    HRESULT __stdcall OnResetDevice() { return m_pOriginal->OnResetDevice(); }

    // Logging device calls
    HRESULT __stdcall SetStateManager(LPD3DXEFFECTSTATEMANAGER pManager) { return m_pOriginal->SetStateManager(pManager); }
    HRESULT __stdcall GetStateManager(LPD3DXEFFECTSTATEMANAGER* ppManager) { return m_pOriginal->GetStateManager(ppManager); }

    // Parameter blocks
    HRESULT __stdcall BeginParameterBlock() { return m_pOriginal->BeginParameterBlock(); }
    D3DXHANDLE __stdcall EndParameterBlock() { return m_pOriginal->EndParameterBlock(); }
    HRESULT __stdcall ApplyParameterBlock(D3DXHANDLE hParameterBlock) { return m_pOriginal->ApplyParameterBlock(hParameterBlock); }
    HRESULT __stdcall DeleteParameterBlock(D3DXHANDLE hParameterBlock) { return m_pOriginal->DeleteParameterBlock(hParameterBlock); }

    // Cloning
    HRESULT __stdcall CloneEffect(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect);

    // Fast path for setting variables directly in ID3DXEffect
    HRESULT __stdcall SetRawValue(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes)
    {
        return m_pOriginal->SetRawValue(hParameter, pData, ByteOffset, Bytes);
    }

    // CProxyDirect3DEffect
    CProxyDirect3DEffect(IDirect3DDevice9* InD3DDevice9, ID3DXEffect* InOriginal);
    virtual ~CProxyDirect3DEffect();

    ID3DXEffect* GetOriginal() { return m_pOriginal; }

protected:
    ID3DXEffect*                            m_pOriginal;
    CProxyDirect3DDevice9::SResourceMemory& m_stats;
};

HRESULT WINAPI MyD3DXCreateEffectFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags,
                                          LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

HRESULT WINAPI MyD3DXCreateEffect(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags,
                                  LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);
