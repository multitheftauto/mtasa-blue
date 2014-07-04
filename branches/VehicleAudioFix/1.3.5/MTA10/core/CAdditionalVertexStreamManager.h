/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  D3DOCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// SAdditionalStreamInfo
//
struct SAdditionalStreamInfo
{
    IDirect3DVertexDeclaration9*    pVertexDeclaration;
    IDirect3DVertexBuffer9*         pStreamData;
    UINT                            Stride;
    CRanges                         ConvertedRanges;
};


struct SCurrentStateInfo
{
private:
    SCurrentStateInfo ( const SCurrentStateInfo& other );
    SCurrentStateInfo& operator= ( const SCurrentStateInfo& other );
public:

    SCurrentStateInfo ( void )
    {
        ZERO_POD_STRUCT( this );
    }

    ~SCurrentStateInfo ( void )
    {
        SAFE_RELEASE( stream1.pStreamData );
        SAFE_RELEASE( pIndexData );
        SAFE_RELEASE( decl.pVertexDeclaration );
    }

    // Info to DrawIndexPrimitive
    struct
    {
        D3DPRIMITIVETYPE                PrimitiveType;
        INT                             BaseVertexIndex;
        UINT                            MinVertexIndex;
        UINT                            NumVertices;
        UINT                            startIndex;
        UINT                            primCount;
    } args;

    // Render state
    struct
    {
        IDirect3DVertexBuffer9*         pStreamData;
        UINT                            OffsetInBytes;
        UINT                            Stride;
    } stream1;

    IDirect3DIndexBuffer9*         pIndexData;

    struct
    {
        IDirect3DVertexDeclaration9*    pVertexDeclaration;
        D3DVERTEXELEMENT9               elements[MAXD3DDECLLENGTH];
        UINT                            numElements;
        D3DVERTEXBUFFER_DESC            VertexBufferDesc1;
    } decl;
};


//
// CAdditionalVertexStreamManager
//
class CAdditionalVertexStreamManager
{
public:
    ZERO_ON_NEW
                            CAdditionalVertexStreamManager      ( void );
    virtual                 ~CAdditionalVertexStreamManager     ( void );

    void                    OnDeviceCreate                      ( IDirect3DDevice9* pDevice );
    bool                    MaybeSetAdditionalVertexStream      ( D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    void                    MaybeUnsetAdditionalVertexStream    ( void );
    void                    OnVertexBufferDestroy               ( IDirect3DVertexBuffer9* pStreamData1 );
    void                    OnVertexBufferRangeInvalidated      ( IDirect3DVertexBuffer9* pStreamData, uint Offset, uint Size );

    static CAdditionalVertexStreamManager* GetSingleton         ( void );
protected:
    void                    SetAdditionalVertexStream           ( SCurrentStateInfo& renderState );
    bool                    UpdateCurrentStateInfo              ( SCurrentStateInfo& state );
    bool                    UpdateAdditionalStreamContent       ( SCurrentStateInfo& state, SAdditionalStreamInfo* pAdditionalStreamInfo, uint ReadOffsetStart, uint ReadSize, uint WriteOffsetStart, uint WriteSize );
    bool                    CheckCanDoThis                      ( const SCurrentStateInfo& state );
    SAdditionalStreamInfo*  GetAdditionalStreamInfo             ( IDirect3DVertexBuffer9* pStreamData1 );
    SAdditionalStreamInfo*  CreateAdditionalStreamInfo          ( const SCurrentStateInfo& state );


    IDirect3DDevice9*                           m_pDevice;
    IDirect3DVertexDeclaration9*                m_pOldVertexDeclaration;
    std::map < void*, SAdditionalStreamInfo >   m_AdditionalStreamInfoMap;
    static CAdditionalVertexStreamManager*      ms_Singleton;
};
