/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CModManagerInterface.h
*  PURPOSE:     Game mod manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMODMANAGERINTERFACE_H
#define __CMODMANAGERINTERFACE_H

class CModManagerInterface
{
public:
    virtual void        RequestLoad             ( const char* szModName, const char* szArguments ) = 0;
    virtual void        RequestLoadDefault      ( const char* szArguments ) = 0;
    virtual void        RequestUnload           ( void ) = 0;

    virtual bool        IsLoaded                ( void ) = 0;
};

#endif
