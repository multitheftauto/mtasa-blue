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
#include <array>
#include <cstring>
#include <new>

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
    CAdditionalVertexStreamManager::GetSingleton()->OnVertexBufferDestroy(m_pOriginal);
    CVertexStreamBoundingBoxManager::GetSingleton()->OnVertexBufferDestroy(m_pOriginal);

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
    *ppvObj = NULL;

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
        CAdditionalVertexStreamManager::GetSingleton()->OnVertexBufferRangeInvalidated(m_pOriginal, OffsetToLock, SizeToLock);
        CVertexStreamBoundingBoxManager::GetSingleton()->OnVertexBufferRangeInvalidated(m_pOriginal, OffsetToLock, SizeToLock);
    }

    ClearFallbackLock();

    *ppbData = NULL;

    const UINT clampedSize = ClampLockSize(OffsetToLock, SizeToLock);

    HRESULT hr = DoLock(OffsetToLock, clampedSize, ppbData, Flags);
    if (SUCCEEDED(hr) && *ppbData != NULL)
        return hr;

    HRESULT recoveryHr = TryRecoverLock(OffsetToLock, clampedSize, ppbData, Flags, hr);
    if (SUCCEEDED(recoveryHr) && *ppbData != NULL)
        return recoveryHr;

    HRESULT finalHr = FAILED(recoveryHr) ? recoveryHr : hr;
    HRESULT fallbackHr = ActivateFallbackLock(OffsetToLock, clampedSize, ppbData, Flags, finalHr);
    if (SUCCEEDED(fallbackHr) && *ppbData != NULL)
        return fallbackHr;

    if (FAILED(fallbackHr))
        finalHr = fallbackHr;

    struct
    {
        const char* szText;
        uint        uiReportId;
        uint        uiLogEventId;
    } info;
    if (finalHr == D3D_OK)
        info = {"result NULL", 8621, 621};
    else if (finalHr == STATUS_ARRAY_BOUNDS_EXCEEDED)
        info = {"offset out of range", 8622, 622};
    else if (finalHr == STATUS_ACCESS_VIOLATION)
        info = {"access violation", 8623, 623};
    else
        info = {"fail", 8620, 620};

    SString strMessage("Lock VertexBuffer [%s] hr:%x Length:%x Usage:%x FVF:%x Pool:%x OffsetToLock:%x SizeToLock:%x Flags:%x", info.szText, finalHr, m_iMemUsed,
                       m_dwUsage, m_dwFVF, m_pool, OffsetToLock, clampedSize, Flags);
    WriteDebugEvent(strMessage);
    AddReportLog(info.uiReportId, strMessage);
    CCore::GetSingleton().LogEvent(info.uiLogEventId, "Lock VertexBuffer", "", strMessage);

    return finalHr;
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
    if (OffsetToLock >= m_iMemUsed)
        return STATUS_ARRAY_BOUNDS_EXCEEDED;

    UINT adjustedSize = ClampLockSize(OffsetToLock, SizeToLock);
    if (adjustedSize == 0)
        return STATUS_ARRAY_BOUNDS_EXCEEDED;

    return LockInternal(OffsetToLock, adjustedSize, ppbData, Flags);
}

HRESULT CProxyDirect3DVertexBuffer::Unlock()
{
    if (!m_fallbackLock.active)
        return m_pOriginal->Unlock();

    HRESULT copyHr = D3D_OK;

    if ((m_fallbackLock.flags & D3DLOCK_READONLY) == 0 && m_fallbackLock.size > 0 && !m_fallbackLock.buffer.empty())
    {
        void* pDestination = NULL;
        copyHr = LockInternal(m_fallbackLock.offset, m_fallbackLock.size, &pDestination, 0);
        if (SUCCEEDED(copyHr) && pDestination != NULL)
        {
            std::memcpy(pDestination, m_fallbackLock.buffer.data(), m_fallbackLock.size);
            HRESULT unlockHr = m_pOriginal->Unlock();
            if (FAILED(unlockHr))
            {
                copyHr = unlockHr;
                SString strMessage("Unlock VertexBuffer [fallback unlock failed] hr:%x Length:%x Usage:%x FVF:%x Pool:%x Offset:%x Size:%x", unlockHr, m_iMemUsed,
                                   m_dwUsage, m_dwFVF, m_pool, m_fallbackLock.offset, m_fallbackLock.size);
                WriteDebugEvent(strMessage);
                AddReportLog(8627, strMessage);
                CCore::GetSingleton().LogEvent(627, "Unlock VertexBuffer", "", strMessage);
            }
        }
        else
        {
            if (SUCCEEDED(copyHr) && pDestination == NULL)
                copyHr = D3DERR_INVALIDCALL;

            SString strMessage("Unlock VertexBuffer [fallback copy failed] hr:%x Length:%x Usage:%x FVF:%x Pool:%x Offset:%x Size:%x", copyHr, m_iMemUsed, m_dwUsage,
                               m_dwFVF, m_pool, m_fallbackLock.offset, m_fallbackLock.size);
            WriteDebugEvent(strMessage);
            AddReportLog(8626, strMessage);
            CCore::GetSingleton().LogEvent(626, "Unlock VertexBuffer", "", strMessage);
        }
    }

    ClearFallbackLock();
    return D3D_OK;
}

UINT CProxyDirect3DVertexBuffer::ClampLockSize(UINT OffsetToLock, UINT SizeToLock) const
{
    if (OffsetToLock >= m_iMemUsed)
        return 0;

    const UINT available = static_cast<UINT>(m_iMemUsed - OffsetToLock);
    if (available == 0)
        return 0;

    if (SizeToLock == 0)
        return available;

    return std::min(SizeToLock, available);
}

HRESULT CProxyDirect3DVertexBuffer::LockInternal(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
    if (ppbData == NULL)
        return E_INVALIDARG;

    *ppbData = NULL;

    __try
    {
        return m_pOriginal->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
    {
        *ppbData = NULL;
        return STATUS_ACCESS_VIOLATION;
    }
}

HRESULT CProxyDirect3DVertexBuffer::TryRecoverLock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, HRESULT originalHr)
{
    HRESULT lastHr = originalHr;

    struct SLockAttempt
    {
        DWORD       flags;
        UINT        offset;
        UINT        size;
        bool        adjustPointer;
    };

    std::array<SLockAttempt, 7> attempts = {
        SLockAttempt{Flags & ~D3DLOCK_DONOTWAIT, OffsetToLock, SizeToLock, false},
        SLockAttempt{Flags & ~D3DLOCK_NOOVERWRITE, OffsetToLock, SizeToLock, false},
        SLockAttempt{Flags & ~D3DLOCK_DISCARD, OffsetToLock, SizeToLock, false},
        SLockAttempt{Flags & ~(D3DLOCK_DONOTWAIT | D3DLOCK_NOOVERWRITE), OffsetToLock, SizeToLock, false},
        SLockAttempt{Flags & ~(D3DLOCK_DONOTWAIT | D3DLOCK_NOOVERWRITE | D3DLOCK_NO_DIRTY_UPDATE), OffsetToLock, SizeToLock, false},
        SLockAttempt{0u, OffsetToLock, SizeToLock, false},
        SLockAttempt{0u, 0u, static_cast<UINT>(m_iMemUsed), true},
    };

    for (const SLockAttempt& attempt : attempts)
    {
        if (attempt.flags == Flags && attempt.offset == OffsetToLock && attempt.size == SizeToLock)
            continue;

        if (attempt.size > m_iMemUsed)
            continue;

        UINT clampedSize = ClampLockSize(attempt.offset, attempt.size);
        if (clampedSize == 0)
            continue;

        void* pRecovered = NULL;
        HRESULT hr = LockInternal(attempt.offset, clampedSize, &pRecovered, attempt.flags);
        if (SUCCEEDED(hr) && pRecovered != NULL)
        {
            if (attempt.adjustPointer)
            {
                UINT pointerOffset = 0;
                if (OffsetToLock > attempt.offset)
                    pointerOffset = OffsetToLock - attempt.offset;
                if (pointerOffset >= clampedSize)
                {
                    lastHr = hr;
                    continue;
                }
                pRecovered = static_cast<void*>(static_cast<std::uint8_t*>(pRecovered) + pointerOffset);
            }

            *ppbData = pRecovered;

            SString strMessage("Lock VertexBuffer [retry] hr:%x->%x Length:%x Usage:%x FVF:%x Pool:%x Offset:%x Size:%x Flags:%x->%x", originalHr, hr, m_iMemUsed,
                               m_dwUsage, m_dwFVF, m_pool, OffsetToLock, SizeToLock, Flags, attempt.flags);
            WriteDebugEvent(strMessage);
            AddReportLog(8625, strMessage);
            CCore::GetSingleton().LogEvent(625, "Lock VertexBuffer", "", strMessage);

            return hr;
        }

        lastHr = hr;
    }

    return lastHr;
}

HRESULT CProxyDirect3DVertexBuffer::ActivateFallbackLock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, HRESULT originalHr)
{
    if (ppbData == NULL)
        return originalHr;

    UINT clampedSize = ClampLockSize(OffsetToLock, SizeToLock);
    if (clampedSize == 0)
        return originalHr;

    try
    {
        m_fallbackLock.buffer.resize(clampedSize);
    }
    catch (const std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }

    m_fallbackLock.active = true;
    m_fallbackLock.offset = OffsetToLock;
    m_fallbackLock.size = clampedSize;
    m_fallbackLock.flags = Flags;

    *ppbData = m_fallbackLock.buffer.data();

    SString strMessage("Lock VertexBuffer [fallback] hr:%x Length:%x Usage:%x FVF:%x Pool:%x OffsetToLock:%x SizeToLock:%x Flags:%x", originalHr, m_iMemUsed,
                       m_dwUsage, m_dwFVF, m_pool, OffsetToLock, clampedSize, Flags);
    WriteDebugEvent(strMessage);
    AddReportLog(8624, strMessage);
    CCore::GetSingleton().LogEvent(624, "Lock VertexBuffer", "", strMessage);

    return D3D_OK;
}

void CProxyDirect3DVertexBuffer::ClearFallbackLock()
{
    m_fallbackLock.active = false;
    m_fallbackLock.offset = 0;
    m_fallbackLock.size = 0;
    m_fallbackLock.flags = 0;
}
