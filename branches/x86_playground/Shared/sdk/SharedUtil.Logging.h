/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Logging.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


namespace SharedUtil
{
    //
    // Output timestamped line into the debugger
    //
    #ifdef MTA_DEBUG
        void OutputDebugLine ( const char* szMessage );
        void SetDebugTagHidden ( const SString& strTag, bool bHidden = true );
        bool IsDebugTagHidden ( const SString& strTag );
    #else
        inline void OutputDebugLineDummy ( void ) {}
        inline void SetDebugTagHiddenDummy ( void ) {}
        inline bool IsDebugTagHiddenDummy ( void ) { return false; }
        #define OutputDebugLine(x) OutputDebugLineDummy ()
        #define SetDebugTagHidden(x) SetDebugTagHiddenDummy ()
        #define IsDebugTagHidden(x) IsDebugTagHiddenDummy ()
    #endif

};

using namespace SharedUtil;

