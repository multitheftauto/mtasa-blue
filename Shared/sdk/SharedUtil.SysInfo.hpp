/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.SysInfo.hpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifdef MTA_CLIENT

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Version.lib")

namespace
{
    HRESULT CallConnectServer(IWbemLocator*               pLoc,
                              /* [in] */ const BSTR       strNetworkResource,
                              /* [in] */ const BSTR       strUser,
                              /* [in] */ const BSTR       strPassword,
                              /* [in] */ const BSTR       strLocale,
                              /* [in] */ long             lSecurityFlags,
                              /* [in] */ const BSTR       strAuthority,
                              /* [in] */ IWbemContext*    pCtx,
                              /* [out] */ IWbemServices** ppNamespace)
    {
        __try
        {
            return pLoc->ConnectServer(strNetworkResource, strUser, strPassword, strLocale, lSecurityFlags, strAuthority, pCtx, ppNamespace);
        }
        __except (GetExceptionCode() == 0xC000041D)
        {
            *ppNamespace = NULL;
            return 0xC000041D;
        }
    }

    HRESULT CallNext(IEnumWbemClassObject*                                                                          pEnumerator,
                     /* [in] */ long                                                                                lTimeout,
                     /* [in] */ ULONG                                                                               uCount,
                     /* [length_is][size_is][out] */ __RPC__out_ecount_part(uCount, *puReturned) IWbemClassObject** apObjects,
                     /* [out] */ __RPC__out ULONG* puReturned)
    {
        __try
        {
            return pEnumerator->Next(lTimeout, uCount, apObjects, puReturned);
        }
        __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
        {
            *puReturned = 0;
            return STATUS_ACCESS_VIOLATION;
        }
    }
}            // namespace

/////////////////////////////////////////////////////////////////////
//
// QueryWMI
//
//
//
/////////////////////////////////////////////////////////////////////
bool SharedUtil::QueryWMI(SQueryWMIResult& outResult, const SString& strQuery, const SString& strKeys, const SString& strNamespace)
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    // This has already been done somewhere else.

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);

    // Error here can be non fatal
    // if (FAILED(hres))
    //{
    //    OutputDebugLine ( SString ( "[Error] Failed to initialize COM library. Error code = %x", hres ) );
    //    return "";
    //}

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres = CoInitializeSecurity(NULL,
                                -1,                                     // COM authentication
                                NULL,                                   // Authentication services
                                NULL,                                   // Reserved
                                RPC_C_AUTHN_LEVEL_DEFAULT,              // Default authentication
                                RPC_C_IMP_LEVEL_IMPERSONATE,            // Default Impersonation
                                NULL,                                   // Authentication info
                                EOAC_NONE,                              // Additional capabilities
                                NULL                                    // Reserved
    );

    // Error here can be non fatal
    //    if (FAILED(hres))
    //    {
    //        OutputDebugLine ( SString ( "[Error] QueryWMI - Failed to initialize security. Error code = %x", hres ) );
    //        return "";
    //    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        AddReportLog(9130, SString("QueryWMI - Failed to create IWbemLocator object. Error code = %x (%s)", hres, *strQuery));
        return false;
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices* pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = CallConnectServer(pLoc,
                             _bstr_t(SStringX("ROOT\\") + strNamespace),            // Object path of WMI namespace
                             NULL,                                                  // User name. NULL = current user
                             NULL,                                                  // User password. NULL = current
                             0,                                                     // Locale. NULL indicates current
                             NULL,                                                  // Security flags.
                             0,                                                     // Authority (e.g. Kerberos)
                             0,                                                     // Context object
                             &pSvc                                                  // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        pLoc->Release();
        AddReportLog(9135, SString("QueryWMI - Could not connect. Error code = %x (%s)", hres, *strQuery));
        return false;
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(pSvc,                                   // Indicates the proxy to set
                             RPC_C_AUTHN_WINNT,                      // RPC_C_AUTHN_xxx
                             RPC_C_AUTHZ_NONE,                       // RPC_C_AUTHZ_xxx
                             NULL,                                   // Server principal name
                             RPC_C_AUTHN_LEVEL_CALL,                 // RPC_C_AUTHN_LEVEL_xxx
                             RPC_C_IMP_LEVEL_IMPERSONATE,            // RPC_C_IMP_LEVEL_xxx
                             NULL,                                   // client identity
                             EOAC_NONE                               // proxy capabilities
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        AddReportLog(9136, SString("QueryWMI - Could not set proxy blanket. Error code = %x (%s)", hres, *strQuery));
        return false;
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----
    IEnumWbemClassObject* pEnumerator = NULL;
    hres =
        pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM ") + bstr_t(strQuery), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        AddReportLog(9137, SString("QueryWMI - Query failed. Error code = %x (%s)", hres, *strQuery));
        return false;
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    // Get list of keys to find values for
    std::vector<SString> vecKeys;
    SString(strKeys).Split(",", vecKeys);

    // Reserve 20 rows for results
    outResult.reserve(20);

    // Fill each row
    while (pEnumerator)
    {
        IWbemClassObject* pclsObj = NULL;
        ULONG             uReturn = 0;

        HRESULT hr = CallNext(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (hr == STATUS_ACCESS_VIOLATION)
        {
            AddReportLog(9130, SString("QueryWMI pEnumerator->Next returned STATUS_ACCESS_VIOLATION for %s", *strQuery));
            break;
        }

        if (0 == uReturn)
        {
            break;
        }

        if (hr != WBEM_S_NO_ERROR)
        {
            AddReportLog(9131, SString("QueryWMI pEnumerator->Next returned %08x for %s", hr, *strQuery));
            break;
        }

        if (pclsObj == NULL)
        {
            AddReportLog(9132, SString("QueryWMI pclsObj == NULL returned %08x for %s", hr, *strQuery));
            break;
        }

        VARIANT vtProp;

        // Add result row
        outResult.insert(outResult.end(), std::vector<SString>());
        outResult.back().reserve(vecKeys.size());

        // Fill each cell
        for (unsigned int i = 0; i < vecKeys.size(); i++)
        {
            std::string strKey = vecKeys[i];
            std::string strValue;

            std::wstring wstrKey(strKey.begin(), strKey.end());
            hr = pclsObj->Get(wstrKey.c_str(), 0, &vtProp, 0, 0);

            if (hr == WBEM_S_NO_ERROR)
            {
                VariantChangeType(&vtProp, &vtProp, 0, VT_BSTR);
                if (vtProp.vt == VT_BSTR)
                    strValue = _bstr_t(vtProp.bstrVal);
                VariantClear(&vtProp);
            }
            else
            {
                AddReportLog(9133, SString("QueryWMI pclsObj->Get returned %08x for key %d in %s", hr, i, *strQuery));
            }

            outResult.back().insert(outResult.back().end(), strValue);
        }

        pclsObj->Release();
    }

    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();

    return true;
}

/////////////////////////////////////////////////////////////////////
//
// GetWMIOSVersion
//
//
//
/////////////////////////////////////////////////////////////////////
SString SharedUtil::GetWMIOSVersion()
{
    SQueryWMIResult result;

    QueryWMI(result, "Win32_OperatingSystem", "Version");

    if (result.empty())
        return "";

    const SString& strVersion = result[0][0];
    return strVersion;
}

/////////////////////////////////////////////////////////////////////
//
// GetWMITotalPhysicalMemory
//
//
//
/////////////////////////////////////////////////////////////////////
long long SharedUtil::GetWMITotalPhysicalMemory()
{
    // This won't change after the first call
    static long long llResult = 0;

    if (llResult == 0)
    {
        SQueryWMIResult result;

        QueryWMI(result, "Win32_ComputerSystem", "TotalPhysicalMemory");

        if (!result.empty())
        {
            const SString& strTotalPhysicalMemory = result[0][0];
            llResult = _atoi64(strTotalPhysicalMemory);
        }
    }

    if (llResult == 0)
    {
        llResult = 2LL * 1024 * 1024 * 1024;            // 2GB
    }
    return llResult;
}

/////////////////////////////////////////////////////////////////////
//
// GetWMIVideoAdapterMemorySize
//  Note that this will never return more than 4 GB of video memory
//
//
/////////////////////////////////////////////////////////////////////
unsigned int SharedUtil::GetWMIVideoAdapterMemorySize(const unsigned long ulVen, const unsigned long ulDev)
{
    unsigned int uiResult = 0;

    SString DevVen;
    DevVen.Format("VEN_%04X&DEV_%04X", ulVen, ulDev);

    // Get WMI info about all video controllers
    SQueryWMIResult result;
    QueryWMI(result, "Win32_VideoController", "PNPDeviceID,AdapterRAM,Availability");

    // Check each controller for a device id match
    for (uint i = 0; i < result.size(); i++)
    {
        const SString& PNPDeviceID = result[i][0];
        const SString& AdapterRAM = result[i][1];
        const SString& Availability = result[i][2];

        unsigned int uiAdapterRAM = atoi(AdapterRAM);
        int          iAvailability = atoi(Availability);

        if ((iAvailability == 8 || iAvailability == 3) && PNPDeviceID.Contains(DevVen))
        {
            uiResult = uiAdapterRAM;
            break;            // Found match
        }
    }

    return uiResult;
}

/////////////////////////////////////////////////////////////////////
//
// GetWMIAntiVirusStatus
//
// Returns a list of enabled AVs and disabled AVs
//
/////////////////////////////////////////////////////////////////////
void SharedUtil::GetWMIAntiVirusStatus(std::vector<SString>& outEnabledList, std::vector<SString>& outDisabledList)
{
    SQueryWMIResult result;

    QueryWMI(result, "AntiVirusProduct", "displayName,productState", "SecurityCenter2");

    if (!result.empty())
    {
        // Vista and up
        for (uint i = 0; i < result.size(); i++)
        {
            const SString& displayName = result[i][0];
            uint           uiProductState = atoi(result[i][1]);
            SString        strComboName("%s[%05x]", *displayName, uiProductState);
            if (uiProductState & 0x1000)
                outEnabledList.push_back(strComboName);
            else
                outDisabledList.push_back(strComboName);
        }
    }
    else
    {
        // XP
        QueryWMI(result, "AntiVirusProduct", "displayName,onAccessScanningEnabled", "SecurityCenter");

        for (uint i = 0; i < result.size(); i++)
        {
            const SString& displayName = result[i][0];
            const SString& onAccessScanningEnabled = result[i][1];
            SString        strComboName("%s[%s]", *displayName, *onAccessScanningEnabled);
            if (onAccessScanningEnabled != "0")
                outEnabledList.push_back(strComboName);
            else
                outDisabledList.push_back(strComboName);
        }
    }
}

/////////////////////////////////////////////////////////////////////
//
// GetInstalledHotFixList
//
// Returns a list of installed Windows hot fixes
//
/////////////////////////////////////////////////////////////////////
void SharedUtil::GetInstalledHotFixList(std::vector<SString>& outInstalledList)
{
    SQueryWMIResult result;
    QueryWMI(result, "Win32_QuickFixEngineering", "HotFixID");
    if (!result.empty())
    {
        for (const auto& row : result)
        {
            const SString& strHotFixId = row[0];
            outInstalledList.push_back(strHotFixId);
        }
    }
}

/////////////////////////////////////////////////////////////////////
//
// IsHotFixInstalled
//
// Return true if supplied Windows hot fix is installed.
// Not thread safe.
//
/////////////////////////////////////////////////////////////////////
bool SharedUtil::IsHotFixInstalled(const SString& strHotFixId)
{
    static std::vector<SString> installedList;
    if (installedList.empty())
        GetInstalledHotFixList(installedList);
    return ListContains(installedList, strHotFixId);
}

///////////////////////////////////////////////////////////////
//
// GetLibVersionInfo
//
// Get version info of a file
//
///////////////////////////////////////////////////////////////
bool SharedUtil::GetLibVersionInfo(const SString& strLibName, SLibVersionInfo* pOutLibVersionInfo)
{
    DWORD dwHandle, dwLen;
    dwLen = GetFileVersionInfoSizeW(FromUTF8(strLibName), &dwHandle);
    if (!dwLen)
        return FALSE;

    LPTSTR lpData = (LPTSTR)malloc(dwLen);
    if (!lpData)
        return FALSE;

    SetLastError(0);
    if (!GetFileVersionInfoW(FromUTF8(strLibName), dwHandle, dwLen, lpData))
    {
        free(lpData);
        return FALSE;
    }

    DWORD dwError = GetLastError();
    if (dwError)
    {
        free(lpData);
        return FALSE;
    }

    UINT              BufLen;
    VS_FIXEDFILEINFO* pFileInfo;
    if (VerQueryValueA(lpData, "\\", (LPVOID*)&pFileInfo, (PUINT)&BufLen))
    {
        *(VS_FIXEDFILEINFO*)pOutLibVersionInfo = *pFileInfo;

        // Nab some strings as well
        WORD* langInfo;
        UINT  cbLang;
        if (VerQueryValueA(lpData, "\\VarFileInfo\\Translation", (LPVOID*)&langInfo, &cbLang))
        {
            SString strFirstBit("\\StringFileInfo\\%04x%04x\\", langInfo[0], langInfo[1]);

            LPVOID lpt;
            UINT   cbBufSize;
            if (VerQueryValueA(lpData, strFirstBit + "CompanyName", &lpt, &cbBufSize))
                pOutLibVersionInfo->strCompanyName = SStringX((const char*)lpt);
            if (VerQueryValueA(lpData, strFirstBit + "ProductName", &lpt, &cbBufSize))
                pOutLibVersionInfo->strProductName = SStringX((const char*)lpt);
        }

        free(lpData);
        return true;
    }

    free(lpData);
    return FALSE;
}

///////////////////////////////////////////////////////////////
//
// Is64BitOS
//
// Return true if is Windows 64 bit OS
//
///////////////////////////////////////////////////////////////
bool SharedUtil::Is64BitOS()
{
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    static bool                bDoneGetProcAddress = false;
    static bool                bIs64BitOS = false;

    if (!bDoneGetProcAddress)
    {
        // Find 'IsWow64Process'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle("Kernel32.dll");
        fnIsWow64Process = static_cast<LPFN_ISWOW64PROCESS>(static_cast<PVOID>(GetProcAddress(hModule, "IsWow64Process")));

        if (fnIsWow64Process)
        {
            // We know current process is 32 bit. See if it is running under WOW64
            BOOL bIsWow64 = FALSE;
            BOOL bOk = fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
            if (bOk)
            {
                // Must be 64bit O/S if current (32 bit) process is running under WOW64
                if (bIsWow64)
                    bIs64BitOS = true;
            }
        }
    }

    return bIs64BitOS;
}

#endif  // MTA_CLIENT
