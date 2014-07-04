/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.SysInfo.hpp
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef WIN32
#ifdef MTA_CLIENT

#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")


/////////////////////////////////////////////////////////////////////
//
// QueryWMI
//
//
//
/////////////////////////////////////////////////////////////////////
bool SharedUtil::QueryWMI ( SQueryWMIResult& outResult, const SString& strQuery, const SString& strKeys )
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    // This has already been done somewhere else.

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);

    // Error here can be non fatal
    //if (FAILED(hres))
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

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
                      
    // Error here can be non fatal
//    if (FAILED(hres))
//    {
//        OutputDebugLine ( SString ( "[Error] QueryWMI - Failed to initialize security. Error code = %x", hres ) );
//        return "";
//    }
    
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
#if MTA_DEBUG
        OutputDebugLine ( SString ( "[Error] QueryWMI - Failed to create IWbemLocator object. Error code = %x", hres ) );
#endif
        return false;
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;


    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t( "ROOT\\CIMV2" ), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
   
    if (FAILED(hres))
    {
        pLoc->Release();     
        OutputDebugLine ( SString ( "[Error] QueryWMI - Could not connect. Error code = %x", hres ) );
        return false;
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
        OutputDebugLine ( SString ( "[Error] QueryWMI - Could not set proxy blanket. Error code = %x", hres ) );
        return false;
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t( "SELECT * FROM " ) + bstr_t ( strQuery ),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        OutputDebugLine ( SString ( "[Error] QueryWMI - Query failed. Error code = %x", hres ) );
        return false;
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;

    // Get list of keys to find values for
    std::vector < SString > vecKeys;
    SString ( strKeys ).Split ( ",", vecKeys );

    // Reserve 20 rows for results
    outResult.reserve ( 20 );

    // Fill each row
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Add result row
        outResult.insert ( outResult.end (), std::vector < SString > () );
        outResult.back().reserve ( vecKeys.size () );

        // Fill each cell
        for ( unsigned int i = 0 ; i < vecKeys.size () ; i++ )
        {
            string strKey = vecKeys[i];
            string strValue;

            wstring wstrKey( strKey.begin (), strKey.end () );
            hr = pclsObj->Get ( wstrKey.c_str (), 0, &vtProp, 0, 0 );

            VariantChangeType( &vtProp, &vtProp, 0, VT_BSTR );
            if ( vtProp.vt == VT_BSTR )
                strValue = _bstr_t ( vtProp.bstrVal );
            VariantClear ( &vtProp );

            outResult.back().insert ( outResult.back().end (), strValue );
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
SString SharedUtil::GetWMIOSVersion ( void )
{
    SQueryWMIResult result;

    QueryWMI ( result, "Win32_OperatingSystem", "Version" );

    if ( result.empty () )
        return "";

    const SString& strVersion  = result[0][0];
    return strVersion;
}


/////////////////////////////////////////////////////////////////////
//
// GetWMITotalPhysicalMemory
//
//
//
/////////////////////////////////////////////////////////////////////
long long SharedUtil::GetWMITotalPhysicalMemory ( void )
{
    // This won't change after the first call
    static long long llResult = 0;

    if ( llResult == 0 )
    {
        SQueryWMIResult result;

        QueryWMI ( result, "Win32_ComputerSystem", "TotalPhysicalMemory" );

        if ( result.empty () )
            return 0;

        const SString& strTotalPhysicalMemory  = result[0][0];
        llResult = _atoi64 ( strTotalPhysicalMemory );
    }

    return llResult;
}


/////////////////////////////////////////////////////////////////////
//
// GetWMIVideoAdapterMemorySize
//
//
//
/////////////////////////////////////////////////////////////////////
long long SharedUtil::GetWMIVideoAdapterMemorySize ( const SString& strDisplay )
{
    // This won't change after the first call
    static long long llResult = 0;

    if ( llResult == 0 )
    {
        SString strDeviceId;

        // Find a device id for the display
        for ( int i = 0 ; true ; i++ )
        {
            DISPLAY_DEVICE device;
            device.cb = sizeof(device);

            // Get next DISPLAY_DEVICE from the system
            if( !EnumDisplayDevicesA ( NULL, i, &device, 0 ) )
                break;

            // Calc flags
            bool bAttachedToDesktop = ( device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0;
            bool bMirroringDriver   = ( device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER ) != 0;
            bool bPrimaryDevice     = ( device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0;

            // Only check attached, non mirror displays
            if( bAttachedToDesktop && !bMirroringDriver )
            {
                if ( strDisplay.CompareI ( device.DeviceName ) )
                {
                    // Found a match
                    strDeviceId = device.DeviceID;
                    break;
                }
            }
        }

        // Get WMI info about all video controllers
        SQueryWMIResult result;
        QueryWMI ( result, "Win32_VideoController", "PNPDeviceID,AdapterRAM,Availability" );

        // Check each controller for a device id match
        for ( uint i = 0 ; i < result.size () ; i++ )
        {
            const SString& PNPDeviceID = result[i][0];
            const SString& AdapterRAM = result[i][1];
            const SString& Availability = result[i][2];

            long long llAdapterRAM = _atoi64 ( AdapterRAM );
            int iAvailability = atoi ( Availability );

            if ( llResult == 0 )
                llResult = llAdapterRAM;

            if ( iAvailability == 3 )
                llResult = Max ( llResult, llAdapterRAM );

            if ( llAdapterRAM != 0 )
                if ( PNPDeviceID.BeginsWithI ( strDeviceId ) )
                {
                    llResult = llAdapterRAM;
                    break;  // Found match
                }
        }
    }

    return llResult;
}

#endif
#endif
