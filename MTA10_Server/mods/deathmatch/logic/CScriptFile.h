/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptFile.h
*  PURPOSE:     Script file element class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFILE_H
#define __CFILE_H

#include "CElement.h"
#include <stdio.h>
#include <string>

class CScriptFile: public CElement
{
public:
    enum eMode
    {
        MODE_READ,
        MODE_READWRITE,
        MODE_CREATE,
    };

                            CScriptFile             ( CResource* pResource, const char* szFilename, unsigned long ulMaxSize );
                            ~CScriptFile            ( void );

    // Functions required by CElement
    void                    Unlink                  ( void ) {};
    bool                    ReadSpecialData         ( void )    { return true; };

    // Load and unload routines
    bool                    Load                    ( eMode Mode );
    void                    Unload                  ( void );
    bool                    IsLoaded                ( void )    { return m_pFile != NULL; };

    // Only call functions belw this if you're sure that the file is loaded.
    // Or you will crash.
    bool                    IsEOF                   ( void );
    long                    GetPointer              ( void );
    long                    GetSize                 ( void );

    long                    SetPointer              ( unsigned long ulPosition );
    void                    SetSize                 ( unsigned long ulNewSize );

    void                    Flush                   ( void );
    long                    Read                    ( unsigned long ulSize, char* pData );
    long                    Write                   ( unsigned long ulSize, const char* pData );

private:
    FILE*                   m_pFile;
    CResource*              m_pResource;
    std::string             m_strFilename;
    unsigned long           m_ulMaxSize;
};

#endif
