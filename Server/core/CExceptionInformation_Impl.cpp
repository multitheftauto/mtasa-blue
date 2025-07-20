/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CExceptionInformation_Impl.cpp
 *  PURPOSE:     Exception information parser
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CExceptionInformation_Impl.h"

#define MAX_MODULE_PATH 512

CExceptionInformation_Impl::CExceptionInformation_Impl()
{
    m_uiCode = 0;
    m_pAddress = NULL;
    m_szModulePathName = NULL;
    m_szModuleBaseName = NULL;
    m_uiAddressModuleOffset = 0;
    m_ulEAX = 0;
    m_ulEBX = 0;
    m_ulECX = 0;
    m_ulEDX = 0;
    m_ulESI = 0;
    m_ulEDI = 0;
    m_ulEBP = 0;
    m_ulESP = 0;
    m_ulEIP = 0;
    m_ulCS = 0;
    m_ulDS = 0;
    m_ulES = 0;
    m_ulFS = 0;
    m_ulGS = 0;
    m_ulSS = 0;
    m_ulEFlags = 0;
}

CExceptionInformation_Impl::~CExceptionInformation_Impl()
{
    if (m_szModulePathName)
        delete m_szModulePathName;
}

void CExceptionInformation_Impl::Set(unsigned int iCode, _EXCEPTION_POINTERS* pException)
{
    m_uiCode = iCode;
    m_pAddress = pException->ExceptionRecord->ExceptionAddress;
#ifdef WIN_x86
    m_ulEAX = pException->ContextRecord->Eax;
    m_ulEBX = pException->ContextRecord->Ebx;
    m_ulECX = pException->ContextRecord->Ecx;
    m_ulEDX = pException->ContextRecord->Edx;
    m_ulESI = pException->ContextRecord->Esi;
    m_ulEDI = pException->ContextRecord->Edi;
    m_ulEBP = pException->ContextRecord->Ebp;
    m_ulESP = pException->ContextRecord->Esp;
    m_ulEIP = pException->ContextRecord->Eip;
    m_ulCS = pException->ContextRecord->SegCs;
    m_ulDS = pException->ContextRecord->SegDs;
    m_ulES = pException->ContextRecord->SegEs;
    m_ulFS = pException->ContextRecord->SegFs;
    m_ulGS = pException->ContextRecord->SegGs;
    m_ulSS = pException->ContextRecord->SegSs;
    m_ulEFlags = pException->ContextRecord->EFlags;
#endif

    void* pModuleBaseAddress = NULL;
    m_szModulePathName = new char[MAX_MODULE_PATH];
    GetModule(m_szModulePathName, MAX_MODULE_PATH, &pModuleBaseAddress);
    m_szModuleBaseName = strrchr(m_szModulePathName, '\\');
    m_szModuleBaseName = m_szModuleBaseName ? m_szModuleBaseName + 1 : m_szModulePathName;
    m_uiAddressModuleOffset = static_cast<BYTE*>(GetAddress()) - static_cast<BYTE*>(pModuleBaseAddress);
}

/**
 * Returns the name of the module in which the exception occured, or
 * an emtpy string if the module could not be found.
 *
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 */
bool CExceptionInformation_Impl::GetModule(char* szOutputBuffer, int nOutputNameLength, void** ppModuleBaseAddress)
{
    HMODULE hModule;

    if (szOutputBuffer == NULL)
        return false;

    /*
     * NUL out the first char in the output buffer.
     */
    szOutputBuffer[0] = 0;

    if (nOutputNameLength == 0)
        return false;

    /*
     * GetModuleHandleExA isn't supported under Windows 2000.
     */
    typedef BOOL(WINAPI * _pfnGetModuleHandleExA)(DWORD, LPCSTR, HMODULE*);

    /*
     * Get kernel32.dll's HMODULE.
     */
    HMODULE hKern32 = GetModuleHandle("kernel32.dll");
    if (NULL == hKern32)
        return false;

    /*
     * See if we're able to use GetModuleHandleExA.  According to Microsoft,
     * this API is only available on Windows XP and Vista.
     */
    _pfnGetModuleHandleExA pfnGetModuleHandleExA = (_pfnGetModuleHandleExA)GetProcAddress(hKern32, "GetModuleHandleExA");

    /*
     * TODO:  Possibly use our own code to do this for other systems.
     * It is possible to enumerate all modules and get their starting/ending
     * offsets, so it would just be a simple comparison of addresses to
     * do this...
     */
    if (NULL == pfnGetModuleHandleExA)
        return false;

    if (0 == pfnGetModuleHandleExA(0x00000004 /*GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS*/, (LPCSTR)m_pAddress, &hModule))
    {
        return false;
    }

    *ppModuleBaseAddress = hModule;

    if (0 != GetModuleFileNameA(hModule, szOutputBuffer, nOutputNameLength))
    {
        /*
         * GetModuleFileNameA will return nOutputNameLength to us
         * if the buffer is too small.  NUL term the buffer.
         *
         * TODO:  Check GetLastError() and actually grow the buffer
         * and retry if it is too small.
         */
        if (nOutputNameLength)
            szOutputBuffer[nOutputNameLength - 1] = 0;

        return true;
    }

    szOutputBuffer[0] = 0;

    return false;
}
