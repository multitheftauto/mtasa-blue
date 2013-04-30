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


DEFINE_GUID(CProxyDirect3DTexture_GUID,   0x128A025E,0x0122,0x04F1,0x40,0x60,0x53,0x19,0x44,0x56,0x59,0x42);


class CProxyDirect3DTexture : public IDirect3DTexture9
{
public:
    /*** IUnknown methods ***/
    HRESULT __stdcall   QueryInterface  ( REFIID riid, void** ppvObj );
    ULONG   __stdcall   AddRef          ( void )                        { return m_pOriginal->AddRef (); }
    ULONG   __stdcall   Release         ( void );

    /*** IDirect3DResource9 methods ***/
    HRESULT __stdcall   GetDevice       ( IDirect3DDevice9** ppDevice ) { return m_pOriginal->GetDevice ( ppDevice ); }
    HRESULT __stdcall   SetPrivateData  ( REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags )   { return m_pOriginal->SetPrivateData ( refguid, pData, SizeOfData, Flags ); }
    HRESULT __stdcall   GetPrivateData  ( REFGUID refguid, void* pData, DWORD* pSizeOfData )                    { return m_pOriginal->GetPrivateData ( refguid, pData, pSizeOfData ); }
    HRESULT __stdcall   FreePrivateData ( REFGUID refguid )             { return m_pOriginal->FreePrivateData ( refguid ); }
    DWORD   __stdcall   SetPriority     ( DWORD PriorityNew )           { return m_pOriginal->SetPriority ( PriorityNew ); }
    DWORD   __stdcall   GetPriority     ( void )                        { return m_pOriginal->GetPriority (); }
    void    __stdcall   PreLoad         ( void )                        { return m_pOriginal->PreLoad (); }
    D3DRESOURCETYPE __stdcall GetType   ( void )                        { return m_pOriginal->GetType (); }

    /*** IDirect3DBaseTexture9 methods ***/
    DWORD   __stdcall   SetLOD                  ( DWORD LODNew )    { return m_pOriginal->SetLOD ( LODNew ); }
    DWORD   __stdcall   GetLOD                  ( void )            { return m_pOriginal->GetLOD (); }
    DWORD   __stdcall   GetLevelCount           ( void )            { return m_pOriginal->GetLevelCount (); }
    HRESULT __stdcall   SetAutoGenFilterType    ( D3DTEXTUREFILTERTYPE FilterType){ return m_pOriginal->SetAutoGenFilterType ( FilterType ); }
    D3DTEXTUREFILTERTYPE
            __stdcall   GetAutoGenFilterType    ( void )            { return m_pOriginal->GetAutoGenFilterType (); }
    void    __stdcall   GenerateMipSubLevels    ( void )            { return m_pOriginal->GenerateMipSubLevels (); }

    /*** IDirect3DTexture9 methods ***/
    HRESULT __stdcall   GetLevelDesc    ( UINT Level,D3DSURFACE_DESC *pDesc)                { return m_pOriginal->GetLevelDesc ( Level, pDesc ); }
    HRESULT __stdcall   GetSurfaceLevel ( UINT Level,IDirect3DSurface9** ppSurfaceLevel)    { return m_pOriginal->GetSurfaceLevel ( Level, ppSurfaceLevel ); }
    HRESULT __stdcall   LockRect        ( UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    HRESULT __stdcall   UnlockRect      ( UINT Level)                                       { return m_pOriginal->UnlockRect ( Level ); }
    HRESULT __stdcall   AddDirtyRect    ( CONST RECT* pDirtyRect)                           { return m_pOriginal->AddDirtyRect ( pDirtyRect ); }

    // CProxyDirect3DTexture
            CProxyDirect3DTexture  ( IDirect3DDevice9* InD3DDevice9, IDirect3DTexture9* InOriginal, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool );
    virtual ~CProxyDirect3DTexture ( void );

    IDirect3DTexture9* GetOriginal ( void )                        { return m_pOriginal; }

protected:
    IDirect3DTexture9*  m_pOriginal;
    int                 m_iMemUsed;
    DWORD               m_dwUsage;
    CProxyDirect3DDevice9::SResourceMemory&     m_stats;
};
