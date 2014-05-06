/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientPerfStatManager.h
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CClientPerfStatModule.h"

//
// CClientPerfStatManager
//
class CClientPerfStatManager
{
public:
    virtual             ~CClientPerfStatManager   ( void ) {}

    virtual void        DoPulse             ( void ) = 0;
    virtual void        GetStats            ( CClientPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter ) = 0;

    static CClientPerfStatManager* GetSingleton ( void );
};
