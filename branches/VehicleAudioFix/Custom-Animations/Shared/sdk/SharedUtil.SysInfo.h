/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.SysInfo.h
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    struct SQueryWMIResult : public std::vector < std::vector < SString > >
    {
    };

    bool        QueryWMI                        ( SQueryWMIResult& outResult, const SString& strQuery, const SString& strKeys );
    SString     GetWMIOSVersion                 ( void );
    long long   GetWMIVideoAdapterMemorySize    ( const SString& strDisplay );
    long long   GetWMITotalPhysicalMemory       ( void );
}
