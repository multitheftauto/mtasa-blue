/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDynamicLibrary.h
*  PURPOSE:     Header for dynamic library class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDYNAMICLIBRARY_H
#define __CDYNAMICLIBRARY_H

class CDynamicLibrary
{
public:
                    CDynamicLibrary();
                    ~CDynamicLibrary();

    bool            Load( const char *path );
    void            Unload();
    bool            IsLoaded();

    void*           GetProcedureAddress( const char *name );

private:
#ifdef WIN32
    HMODULE         m_module;
#else
    void*           m_module;
#endif
};

#endif
