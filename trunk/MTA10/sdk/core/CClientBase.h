/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CClientBase.h
*  PURPOSE:     Client base interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTBASE_H
#define __CCLIENTBASE_H

#include "CCoreInterface.h"
#include "CExceptionInformation.h"

class CClientBase 
{
public:
    virtual int     ClientInitialize            ( const char* szArguments, CCoreInterface* pCoreInterface ) = 0;
    virtual void    ClientShutdown              ( void ) = 0;  

    virtual void    PreFrameExecutionHandler    ( void ) = 0;
    virtual void    PreHUDRenderExecutionHandler ( void ) = 0;
    virtual void    PostFrameExecutionHandler   ( void ) = 0;

    virtual bool    ProcessCommand              ( const char* szCommandLine ) = 0;
    //virtual bool    ProcessInput                ( CInputMessage* pInputMessage ) = 0  *TODO*

    virtual bool    HandleException             ( CExceptionInformation* pExceptionInformation ) = 0;
};

#endif
