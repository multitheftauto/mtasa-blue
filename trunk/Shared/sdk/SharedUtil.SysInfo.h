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
    SString     GetWMIOSVersion                 ( void );
    long long   GetWMIVideoAdapterMemorySize    ( void );
    SString     GetWMIVideoAdapterName          ( void );
    uint        GetMinStreamingMemory           ( void );
    uint        GetMaxStreamingMemory           ( void );
}
