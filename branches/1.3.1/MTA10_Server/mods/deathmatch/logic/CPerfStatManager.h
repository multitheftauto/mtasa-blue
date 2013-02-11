/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStatManager.h
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CPerfStatModule.h"

//
// CPerfStatManager
//
class CPerfStatManager
{
public:
    virtual             ~CPerfStatManager   ( void ) {}

    virtual void        DoPulse             ( void ) = 0;
    virtual void        GetStats            ( CPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter ) = 0;

    // Utility
    static SString      GetScaledByteString ( long long Amount );
    static SString      GetScaledBitString  ( long long Amount );
    static SString      GetScaledMsString   ( float fMs );
    static long long    GetPerSecond        ( long long llValue, long long llDeltaTickCount );
    static void         ToPerSecond         ( long long& llValue, long long llDeltaTickCount );
    static SString      GetPerSecondString  ( long long llValue, double dDeltaTickCount );
    static SString      GetPercentString    ( long long llValue, long long llTotal );

    static CPerfStatManager* GetSingleton ( void );
};
