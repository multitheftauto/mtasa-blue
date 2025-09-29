/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CProxyDirect3DVertexBuffer.cpp
 *  PURPOSE:     Direct3D 9 vertex buffer function hooking proxy
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProxyDirect3DVertexBuffer.h"
#include "CAdditionalVertexStreamManager.h"
#include "CVertexStreamBoundingBoxManager.h"
#include <algorithm>
#include <cstring>

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::CProxyDirect3DVertexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexBuffer::CProxyDirect3DVertexBuffer(IDirect3DDevice9* InD3DDevice9, IDirect3DVertexBuffer9* pOriginal, UINT Length, DWORD Usage, DWORD FVF,
                                                       D3DPOOL Pool)
    : m_stats(Usage & D3DUSAGE_DYNAMIC ? g_pDeviceState->MemoryState.DynamicVertexBuffer : g_pDeviceState->MemoryState.StaticVertexBuffer)
{
    m_pOriginal = pOriginal;
    m_iMemUsed = Length;
    m_dwUsage = Usage;
    m_dwFVF = FVF;
    m_pool = Pool;
    m_bFallbackActive = false;
    m_fallbackOffset = 0;
    m_fallbackSize = 0;
    m_fallbackFlags = 0;
    m_fallbackStorage.clear();

    m_stats.iCurrentCount++;
    m_stats.iCurrentBytes += m_iMemUsed;
    m_stats.iCreatedCount++;
    m_stats.iCreatedBytes += m_iMemUsed;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::~CProxyDirect3DVertexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexBuffer::~CProxyDirect3DVertexBuffer()
{
    if (CAdditionalVertexStreamManager* pManager = CAdditionalVertexStreamManager::GetExistingSingleton())
        pManager->OnVertexBufferDestroy(m_pOriginal);
    if (CVertexStreamBoundingBoxManager* pBoundingBoxManager = CVertexStreamBoundingBoxManager::GetExistingSingleton())
        pBoundingBoxManager->OnVertexBufferDestroy(m_pOriginal);

    m_stats.iCurrentCount--;
    m_stats.iCurrentBytes -= m_iMemUsed;
    m_stats.iDestroyedCount++;
    m_stats.iDestroyedBytes += m_iMemUsed;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::QueryInterface(REFIID riid, void** ppvObj)
{
    *ppvObj = nullptr;

    // Looking for me?
    if (riid == CProxyDirect3DVertexBuffer_GUID)
    {
        *ppvObj = this;
        return S_OK;
    }

    return m_pOriginal->QueryInterface(riid, ppvObj);
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DVertexBuffer::Release()
{
    // Call original function
    ULONG ulRefCount = m_pOriginal->Release();
    if (ulRefCount == 0)
        delete this;
    return ulRefCount;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::Lock
//
// If lock is writable, tell manager that range content will change
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    m_stats.iLockedCount++;

    if ((Flags & D3DLOCK_READONLY) == 0)
    {
        if (CAdditionalVertexStreamManager* pManager = CAdditionalVertexStreamManager::GetExistingSingleton())
            pManager->OnVertexBufferRangeInvalidated(m_pOriginal, OffsetToLock, SizeToLock);
        if (CVertexStreamBoundingBoxManager* pBoundingBoxManager = CVertexStreamBoundingBoxManager::GetExistingSingleton())
            pBoundingBoxManager->OnVertexBufferRangeInvalidated(m_pOriginal, OffsetToLock, SizeToLock);
    }

    SharedUtil::CAutoCSLock fallbackGuard(m_fallbackCS);

    if (m_bFallbackActive)
    {
        m_bFallbackActive = false;
        m_fallbackOffset = 0;
        m_fallbackSize = 0;
        m_fallbackFlags = 0;
    }

    *ppbData = nullptr;
    HRESULT hr = DoLock(OffsetToLock, SizeToLock, ppbData, Flags);
    HRESULT originalHr = hr;

    bool  bPointerNullEvent = SUCCEEDED(hr) && (*ppbData == nullptr);
    bool  bRetryAttempted = false;
    bool  bRetrySucceeded = false;
    bool  bFallbackUsed = false;
    bool  bUnlockedAfterNull = false;
    DWORD retryFlags = Flags;

    if (bPointerNullEvent)
    {
        WriteDebugEvent(SString("Lock VertexBuffer: initial pointer null (Usage:%08x Flags:%08x Offset:%x Size:%x)", m_dwUsage, Flags, OffsetToLock,
                                 SizeToLock));

        // Retry once for dynamic buffers using DISCARD
        if ((m_dwUsage & D3DUSAGE_DYNAMIC) && (Flags & D3DLOCK_READONLY) == 0)
        {
            bRetryAttempted = true;

            retryFlags &= ~(DWORD)(D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_READONLY);
            retryFlags |= D3DLOCK_DISCARD;

            // Release the bogus lock result before retrying
            m_pOriginal->Unlock();
            bUnlockedAfterNull = true;

            void* pRetryData = nullptr;
            hr = DoLock(OffsetToLock, SizeToLock, &pRetryData, retryFlags);

            if (SUCCEEDED(hr) && pRetryData != nullptr)
            {
                *ppbData = pRetryData;
                bPointerNullEvent = false;
                bRetrySucceeded = true;
                WriteDebugEvent(SString("Lock VertexBuffer: retry with DISCARD succeeded (Flags:%08x)", retryFlags));
            }
            else
            {
                // Ensure we unlock on success with null pointer to avoid leaving the resource locked
                if (SUCCEEDED(hr))
                {
                    m_pOriginal->Unlock();
                    bUnlockedAfterNull = true;
                }
            }
        }

        if (bPointerNullEvent)
        {
            if (!bUnlockedAfterNull && SUCCEEDED(hr))
            {
                m_pOriginal->Unlock();
                bUnlockedAfterNull = true;
            }
            // Fall back to artificial buffer
            UINT clampedOffset = std::min(OffsetToLock, m_iMemUsed);
            UINT clampedSize = SizeToLock;
            if (clampedOffset + clampedSize > m_iMemUsed)
                clampedSize = m_iMemUsed - clampedOffset;
            if (clampedSize == 0 && clampedOffset < m_iMemUsed)
                clampedSize = m_iMemUsed - clampedOffset;

            // Ensure we have some storage to hand back (even zero-sized locks receive a valid pointer)
            size_t requiredStorage = std::max<size_t>(static_cast<size_t>(clampedSize), static_cast<size_t>(1));
            if (m_fallbackStorage.size() < requiredStorage)
                m_fallbackStorage.resize(requiredStorage);

            if (clampedSize > 0 && (Flags & D3DLOCK_READONLY))
                memset(m_fallbackStorage.data(), 0, clampedSize);

            *ppbData = m_fallbackStorage.data();

            m_bFallbackActive = true;
            m_fallbackOffset = clampedOffset;
            m_fallbackSize = clampedSize;
            m_fallbackFlags = Flags;

            bFallbackUsed = true;
            bPointerNullEvent = true;

            hr = D3D_OK;

            WriteDebugEvent(SString("Lock VertexBuffer: engaged fallback buffer (Offset:%x Size:%x Flags:%08x)", m_fallbackOffset, m_fallbackSize,
                                     m_fallbackFlags));
        }
    }

    // Report problems
    if (FAILED(hr) || bPointerNullEvent)
    {
        struct
        {
            const char* szText;
            uint        uiReportId;
            uint        uiLogEventId;
        } info;
        HRESULT reportHr = FAILED(hr) ? hr : D3DERR_INVALIDCALL;
        if (bPointerNullEvent && originalHr == D3D_OK)
            info = {"result NULL", 8621, 621};
        else if (reportHr == STATUS_ARRAY_BOUNDS_EXCEEDED)
            info = {"offset out of range", 8622, 622};
        else if (reportHr == STATUS_ACCESS_VIOLATION)
            info = {"access violation", 8623, 623};
        else
            info = {"fail", 8620, 620};

        SString strMessage(
            "Lock VertexBuffer [%s] hr:%x origHr:%x returnHr:%x pointerNull:%u retryAttempted:%u retrySucceeded:%u fallback:%u fallbackSize:%x fallbackFlags:%x"
            " unlockedAfterNull:%u Length:%x Usage:%x FVF:%x Pool:%x OffsetToLock:%x SizeToLock:%x Flags:%x retryFlags:%x",
            info.szText, reportHr, originalHr, hr, static_cast<uint>(bPointerNullEvent), static_cast<uint>(bRetryAttempted),
            static_cast<uint>(bRetrySucceeded), static_cast<uint>(bFallbackUsed), m_fallbackSize, m_fallbackFlags, static_cast<uint>(bUnlockedAfterNull),
            m_iMemUsed, m_dwUsage, m_dwFVF, m_pool, OffsetToLock, SizeToLock, Flags, retryFlags);
        WriteDebugEvent(strMessage);
        AddReportLog(info.uiReportId, strMessage);
        CCore::GetSingleton().LogEvent(info.uiLogEventId, "Lock VertexBuffer", "", strMessage);
    }
    return hr;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::Unlock
//
// Apply fallback data if we had to hand out an artificial buffer
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::Unlock()
{
    SharedUtil::CAutoCSLock fallbackGuard(m_fallbackCS);

    if (!m_bFallbackActive)
        return m_pOriginal->Unlock();

    HRESULT copyResult = D3D_OK;
    bool    bShouldRetryLater = false;

    if ((m_fallbackFlags & D3DLOCK_READONLY) == 0 && m_fallbackSize > 0)
    {
        UINT offset = std::min(m_fallbackOffset, m_iMemUsed);
        UINT size = m_fallbackSize;
        if (offset + size > m_iMemUsed)
            size = (offset < m_iMemUsed) ? (m_iMemUsed - offset) : 0;

        if (size > 0)
        {
            DWORD writeFlags = m_fallbackFlags;
            writeFlags &= ~(DWORD)(D3DLOCK_NOOVERWRITE | D3DLOCK_READONLY);
            if (m_dwUsage & D3DUSAGE_DYNAMIC)
                writeFlags |= D3DLOCK_DISCARD;

            void*   pReal = nullptr;
            HRESULT lockHr = DoLock(offset, size, &pReal, writeFlags);

            if (SUCCEEDED(lockHr) && pReal != nullptr)
            {
                if (size > 0 && size <= m_fallbackStorage.size())
                {
                    memcpy(pReal, m_fallbackStorage.data(), size);
                }
                else
                {
                    WriteDebugEvent(SString("Unlock VertexBuffer: fallback copy size mismatch (size:%x storage:%x)", size, m_fallbackStorage.size()));
                    copyResult = D3DERR_INVALIDCALL;
                }

                HRESULT unlockHr = m_pOriginal->Unlock();
                if (FAILED(unlockHr))
                    copyResult = unlockHr;
            }
            else
            {
                if (SUCCEEDED(lockHr))
                    m_pOriginal->Unlock();

                WriteDebugEvent(SString("Unlock VertexBuffer: failed to copy fallback data (lockHr:%x offset:%x size:%x flags:%08x)", lockHr, offset, size,
                                           writeFlags));
                copyResult = FAILED(lockHr) ? lockHr : D3DERR_INVALIDCALL;
                bShouldRetryLater = true;
            }
        }
        else
        {
            bShouldRetryLater = true;
        }
    }
    else if (m_fallbackSize == 0)
    {
        // No bytes were mapped, keep fallback around in case caller retries
        bShouldRetryLater = true;
    }

    WriteDebugEvent(SString("Unlock VertexBuffer: fallback completed (offset:%x size:%x flags:%08x retryLater:%u result:%x)", m_fallbackOffset, m_fallbackSize,
                             m_fallbackFlags, static_cast<uint>(bShouldRetryLater), copyResult));

    m_bFallbackActive = bShouldRetryLater ? m_bFallbackActive : false;
    if (!m_bFallbackActive)
    {
        m_fallbackOffset = 0;
        m_fallbackSize = 0;
        m_fallbackFlags = 0;
    }

    return copyResult;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::DoLock
//
// With validation and exception catcher
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::DoLock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    // Validate sizes because gta can give invalid values (reason unknown)
    if (OffsetToLock + SizeToLock > m_iMemUsed)
    {
        if (OffsetToLock > m_iMemUsed)
        {
            return STATUS_ARRAY_BOUNDS_EXCEEDED;
        }
        SizeToLock = m_iMemUsed - OffsetToLock;
    }

    __try
    {
        return m_pOriginal->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
    {
        return STATUS_ACCESS_VIOLATION;
    }
}
