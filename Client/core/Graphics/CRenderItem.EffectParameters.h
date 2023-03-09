/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CRenderItem.EffectParameters.h
 *  PURPOSE:     Handles and settings for a compiled effect
 *
 *****************************************************************************/

enum EStateGroup
{
    STATE_GROUP_RENDER,
    STATE_GROUP_STAGE,
    STATE_GROUP_SAMPLER,
    STATE_GROUP_MATERIAL,
    STATE_GROUP_TRANSFORM,
    STATE_GROUP_TEXTURE,
    STATE_GROUP_LIGHT,
    STATE_GROUP_LIGHT_ENABLE,
    STATE_GROUP_DEVICE_CAPS,
    STATE_GROUP_VERTEX_DECL,
};
DECLARE_ENUM(EStateGroup)

enum EReadableAsType
{
    TYPE_INT,                      // int
    TYPE_D3DCOLOR,                 // Can be read as float4 or DWORD color
    TYPE_IFLOAT,                   // Can be read as float or as int on float memory
    TYPE_D3DCOLORVALUE,            // float4
    TYPE_FLOAT,                    // float
    TYPE_VECTOR3,                  // float3
    TYPE_MATRIX,                   // float4x4
    TYPE_TEXTURE,
};

namespace RegMap
{
    // Vector is 3 floats
    // Color is 4 floats
    // Matrix is 16 floats
    enum ETranslateType
    {
        Int2Int,
        Int2Color,
        Int2Float,
        Float2Float,
        Color2Color,
        Vector2Vector,
        Matrix2Matrix,
        Texture2Texture,
    };
}            // namespace RegMap

// Map effect parameter to something
struct STypeMapping
{
    EReadableAsType        readableAsType;
    D3DXPARAMETER_CLASS    ParamMatchClass;
    D3DXPARAMETER_TYPE     ParamMatchType;
    RegMap::ETranslateType OutType;
    int                    OutSize;
};

struct SStateVar
{
    EStateGroup            stateGroup;
    D3DXHANDLE             hHandle;
    int                    iStage;
    int                    iRegister;
    RegMap::ETranslateType iType;
    int                    iSize;
};

// Info about one D3D register
struct SRegisterInfo
{
    int             iOffset;
    const char*     szGroupName;
    const char*     szRegName;
    EReadableAsType eReadableAsType;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectParameters
//
// Handles and settings for a compiled effect
//
class CEffectParameters : public CRenderItem
{
    DECLARE_CLASS(CEffectParameters, CRenderItem)
    CEffectParameters() : ClassInit(this) {}
    bool                        ApplyCommonHandles();
    bool                        ApplyMappedHandles();
    HRESULT                     Begin(UINT* pPasses, DWORD Flags, bool bWorldRender = true);
    HRESULT                     End();
    void                        NotifyModifiedParameter(D3DXHANDLE hParameter);
    const std::set<D3DXHANDLE>& GetModifiedParameters();
    void                        RestoreParametersDefaultValue(const std::vector<D3DXHANDLE>& parameterList);

protected:
    void                 ReadParameterHandles();
    bool                 TryParseSpecialParameter(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc);
    bool                 TryMappingParameterToRegister(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc);
    void                 ReadCommonHandles();
    const SRegisterInfo* GetRegisterInfo(EStateGroup stateGroup, const SString& strName);
    const STypeMapping*  MatchTypeMapping(EReadableAsType readableAsType, const D3DXPARAMETER_DESC& desc);
    void                 AddStateMappedParameter(EStateGroup stateGroup, const SStateVar& var);
    bool                 AddStandardParameter(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc);
    SString              GetAnnotationNameAndValue(D3DXHANDLE hParameter, uint uiIndex, SString& strOutValue);
    bool                 IsSecondaryRenderTarget(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc);

    std::vector<SStateVar> m_renderStateVarList;
    std::vector<SStateVar> m_stageStateVarList;
    std::vector<SStateVar> m_samplerStateVarList;
    std::vector<SStateVar> m_materialStateVarList;
    std::vector<SStateVar> m_transformStateVarList;
    std::vector<SStateVar> m_textureStateVarList;
    std::vector<SStateVar> m_lightStateVarList;
    std::vector<SStateVar> m_lightEnableStateVarList;
    std::vector<SStateVar> m_deviceCapsVarList;
    std::vector<SStateVar> m_vertexDeclStateVarList;

    struct
    {
        D3DXHANDLE hWorld, hView, hProjection, hWorldView, hWorldViewProj;
        D3DXHANDLE hViewProj, hViewInv, hWorldInvTr, hViewInvTr;
        D3DXHANDLE hCamPos, hCamDir;
        D3DXHANDLE hTime;
        D3DXHANDLE hLightAmbient, hLightDiffuse, hLightSpecular, hLightDirection;
        D3DXHANDLE hDepthBuffer;
        D3DXHANDLE hViewMainScene, hWorldMainScene, hProjectionMainScene;
    } m_CommonHandles;

    bool m_bUsesMappedHandles;
    bool m_bUsesCommonHandles;
    bool m_bSkipUnusedParameters;

public:
    ID3DXEffect*                            m_pD3DEffect;
    std::map<SString, D3DXHANDLE>           m_textureHandleMap;
    std::map<SString, D3DXHANDLE>           m_valueHandleMap;
    std::vector<D3DXHANDLE>                 m_SecondaryRenderTargetList;
    D3DXHANDLE                              m_hFirstTexture;
    bool                                    m_bRequiresNormals;
    bool                                    m_bUsesVertexShader;
    bool                                    m_bUsesDepthBuffer;
    std::map<D3DXHANDLE, std::vector<char>> m_defaultValues;
    std::set<D3DXHANDLE>                    m_ModifiedParametersList;
    uint                                    m_uiModifiedParametersRevision;
};
