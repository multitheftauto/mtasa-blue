/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/CClient.h
*  PURPOSE:     Header file for Client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oliver Brown <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENT_H
#define __CCLIENT_H

#include <core/CClientBase.h>

class CClient : public CClientBase
{
public:
    int         ClientInitialize                ( const char* szArguments, CCoreInterface* pCore );
    void        ClientShutdown                  ( void );  

    void        PreFrameExecutionHandler        ( void );
    void        PostFrameExecutionHandler       ( void );

    bool        ProcessCommand                  ( const char* szCommandLine );
    //bool        ProcessInput                    ( CInputMessage* pInputMessage );

    bool        HandleException                 ( CExceptionInformation* pExceptionInformation );
};

#endif