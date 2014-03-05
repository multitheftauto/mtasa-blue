/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFileUtilsSA.cpp
*  PURPOSE:     File parsing utilities used by R* Games
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <direct.h>

/*=========================================================
    FileMgr::GetConfigLine

    Arguments:
        file - source stream to read from
    Purpose:
        Reads a line from the stream character by character
        and parses it for sscanf compatibility. Returns the
        string it processed. If there was nothing to read,
        it returns NULL.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00536F80
=========================================================*/
static char tempBuffer[512];

const char* FileMgr::GetConfigLine( CFile *file )
{
    if ( !file->GetString( tempBuffer, sizeof( tempBuffer ) ) )
        return NULL;
    
    char *buf = tempBuffer;

    for ( buf = tempBuffer; *buf; buf++ )
    {
        const char c = *buf;

        if ( c < ' ' || c == ',' )
            *buf = ' ';
    }
    
    // Offset until we start at valid offset
    for ( buf = tempBuffer; *buf && *buf <= ' '; buf++ );

    return buf;
}

static CFileTranslator *curDirTranslator = NULL;

static void OnGTASACurrentDirectoryChange( const char *absPath )
{
    // Make sure we know of the current GTA:SA directory.
    curDirTranslator->ChangeDirectory( absPath );
}

inline void ProcessRelativePath( char *pathBuffer, const char *path )
{
    size_t pathLen = strlen( path );

    char *progOutPtr = pathBuffer + strlen( pathBuffer );

    memcpy( progOutPtr, path, pathLen + 1 );

    if ( path[ pathLen - 1 ] != '\\' )
    {
        progOutPtr += pathLen;

        *(unsigned short*)progOutPtr = '\\\0';
    }
}

static void __cdecl chdirProgramDirAbsolute( const char *path )
{
    char *pathBuffer = FileMgr::GetPathBuffer();

    if ( *path != '\\' )
        strcpy( pathBuffer, FileMgr::GetProgramBaseDir() );

    if ( *path )
    {
        ProcessRelativePath( pathBuffer, path );
    }

    int iRet = _chdir( pathBuffer );

    if ( iRet != -1 )
    {
        // Report change of current directory to our API.
        OnGTASACurrentDirectoryChange( pathBuffer );
    }
}

static void __cdecl chdirProgramDir( const char *path )
{
    char *pathBuffer = FileMgr::GetPathBuffer();
    char *progBaseDir = FileMgr::GetProgramBaseDir();

    strcpy( pathBuffer, progBaseDir );

    if ( *path )
    {
        ProcessRelativePath( pathBuffer, path );
    }

    int iRet = _chdir( pathBuffer );

    if ( iRet != -1 )
    {
        // Report change of current directory to our API.
        OnGTASACurrentDirectoryChange( pathBuffer );
    }
}

CFileTranslator* FileMgr::GetCurDirTranslator( void )
{
    return curDirTranslator;
}

void FileMgr::InitHooks( void )
{
    // Install file system monitoring hooks.
    HookInstall( 0x00538730, (DWORD)chdirProgramDirAbsolute, 5 );
    HookInstall( 0x005387D0, (DWORD)chdirProgramDir, 5 );
    // todo: 0x00538860

    // Create file monitoring utilities.
    curDirTranslator = core->GetFileSystem()->CreateTranslator( "/" );
}

void FileMgr::ShutdownHooks( void )
{
    // Delete the file monitoring utilities.
    delete curDirTranslator;
}