/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        launcher/CDynamicLibrary.h
*  PURPOSE:     Dynamic library handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDYNAMICLIBRARY_H
#define __CDYNAMICLIBRARY_H

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class CDynamicLibrary
{
public:
                    CDynamicLibrary         ( void );
                    ~CDynamicLibrary        ( void );

    bool            Load                    ( const char* szFilename );
    void            Unload                  ( void );
    bool            IsLoaded                ( void );

    void*           GetProcedureAddress     ( const char* szProcName );

private:
    #ifdef WIN32
    HMODULE         m_hModule;
    #else
    void*           m_hModule;
    #endif
};

#endif
