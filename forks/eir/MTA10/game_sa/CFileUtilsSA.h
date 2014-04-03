/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFileUtilsSA.h
*  PURPOSE:     File parsing utilities used by R* Games
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILE_UTILS_
#define _FILE_UTILS_

namespace FileMgr
{
    const char*         GetConfigLine           ( CFile *file );
    const char*         GetConfigLineFromBuffer ( const char **buf, int *bufSize );

    void                InitHooks               ( void );
    void                ShutdownHooks           ( void );

    inline char*        GetProgramBaseDir       ( void )        { return (char*)0x00B71AE0; }
    inline char*        GetPathBuffer           ( void )        { return (char*)0x00B71A60; }

    CFileTranslator*    GetCurDirTranslator     ( void );
};

#endif //_FILE_UTILS_