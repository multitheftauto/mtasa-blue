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


DEFINE_GUID(CProxyDirect3DVertexDeclaration_GUID,   0x128A025E,0x0120,0x04F1,0x40,0x60,0x53,0x19,0x44,0x56,0x59,0x42);


class CProxyDirect3DVertexDeclaration : public IDirect3DVertexDeclaration9
{
public:
    /*** IUnknown methods ***/
    HRESULT __stdcall   QueryInterface  ( REFIID riid, void** ppvObj );
    ULONG   __stdcall   AddRef          ( void )                        { return m_pOriginal->AddRef (); }
    ULONG   __stdcall   Release         ( void );

    /*** IDirect3DVertexDeclaration9 methods ***/
    HRESULT __stdcall   GetDevice       ( IDirect3DDevice9** ppDevice )                         { return m_pOriginal->GetDevice ( ppDevice ); }
    HRESULT __stdcall   GetDeclaration  ( D3DVERTEXELEMENT9* pElement,UINT* pNumElements )      { return m_pOriginal->GetDeclaration ( pElement, pNumElements ); }

    // CProxyDirect3DVertexDeclaration
            CProxyDirect3DVertexDeclaration  ( IDirect3DDevice9* InD3DDevice9, IDirect3DVertexDeclaration9* InOriginal, CONST D3DVERTEXELEMENT9* pVertexElements );
    virtual ~CProxyDirect3DVertexDeclaration ( void );

    IDirect3DVertexDeclaration9* GetOriginal ( void )                        { return m_pOriginal; }

protected:
    IDirect3DVertexDeclaration9* m_pOriginal;
};
