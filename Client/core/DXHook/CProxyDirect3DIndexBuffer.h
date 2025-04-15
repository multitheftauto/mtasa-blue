/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

DEFINE_GUID(CProxyDirect3DIndexBuffer_GUID, 0x128A025E, 0x0121, 0x04F1, 0x40, 0x60, 0x53, 0x19, 0x44, 0x56, 0x59, 0x42);

class CProxyDirect3DIndexBuffer : public IDirect3DIndexBuffer9
{
public:
    /*** IUnknown methods ***/
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
    ULONG __stdcall AddRef() { return m_pOriginal->AddRef(); }
    ULONG __stdcall Release();

    /*** IDirect3DResource9 methods ***/
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice) { return m_pOriginal->GetDevice(ppDevice); }
    HRESULT __stdcall SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags)
    {
        return m_pOriginal->SetPrivateData(refguid, pData, SizeOfData, Flags);
    }
    HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return m_pOriginal->GetPrivateData(refguid, pData, pSizeOfData); }
    HRESULT __stdcall FreePrivateData(REFGUID refguid) { return m_pOriginal->FreePrivateData(refguid); }
    DWORD __stdcall SetPriority(DWORD PriorityNew) { return m_pOriginal->SetPriority(PriorityNew); }
    DWORD __stdcall GetPriority() { return m_pOriginal->GetPriority(); }
    void __stdcall PreLoad() { return m_pOriginal->PreLoad(); }
    D3DRESOURCETYPE __stdcall GetType() { return m_pOriginal->GetType(); }

    /*** IDirect3DIndexBuffer9 methods ***/
    HRESULT __stdcall Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
    HRESULT __stdcall Unlock() { return m_pOriginal->Unlock(); }
    HRESULT __stdcall GetDesc(D3DINDEXBUFFER_DESC* pDesc) { return m_pOriginal->GetDesc(pDesc); }

    // CProxyDirect3DIndexBuffer
    CProxyDirect3DIndexBuffer(IDirect3DDevice9* InD3DDevice9, IDirect3DIndexBuffer9* InOriginal, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool);
    virtual ~CProxyDirect3DIndexBuffer();

    IDirect3DIndexBuffer9* GetOriginal() { return m_pOriginal; }

protected:
    IDirect3DIndexBuffer9*                  m_pOriginal;
    int                                     m_iMemUsed;
    DWORD                                   m_dwUsage;
    D3DFORMAT                               m_format;
    D3DPOOL                                 m_pool;
    CProxyDirect3DDevice9::SResourceMemory& m_stats;
};
