/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CIMGManagerSA.cpp
*  PURPOSE:     Image accelerated archive management
*       The idea of the IMG Management is that we get a closer look at
*       GTA:SA's way of loading resources from files. We want to create an indepedent
*       loader so that we do not encounter limitations of the old system, since it has
*       been optimized for gameplay.
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*  INSPIRATION: fastman92 <http://fastman92.tk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define MAX_GTA_STREAM_HANDLES  128

typedef char streamName[64];

HANDLE gtaStreamHandles[MAX_GTA_STREAM_HANDLES];    // file handles
streamName gtaStreamNames[MAX_GTA_STREAM_HANDLES];

/*=========================================================
    OpenStream_protected

    Arguments:
        path - filename of the stream to open
    Purpose:
        Opens a file instance on the local system. It can be used
        for asynchronous read access (overlapped I/O).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004067B0
=========================================================*/

/* 
    SECUmadness chapter 1: OpenStream

    The GTA:SA engine has been _lightly_ protected by the SecuROM machine code obfuscator.
    I have first-grade experience of what decompiling such code means: a f***ing headache.
    Simple routines become a torturous calling of key-value functions which silently redirect
    to the general-purpose API function. What you can read in the executable is in no-way
    represented in this function-stub.

    Due to the applied protective-layer, we are restricted to 128 streamHandles. Do not wonder
    why fastman92 has raised the limit to that exact value! Here we go, ladies and gentlemen,
    the cleaned-up version of 0x015649F0(US)..!
*/

static unsigned int __cdecl OpenStream_protected( const char *path )
{
    // First we find a free stream handle
    unsigned int n;

    // Bugfix: usually GTA:SA would corrupt it's streamHandle slots if the file was not read-accessible
    // I prevent this from happening by not storing INVALID_HANDLE_VALUE, but a NULL
    for ( n=0; n<MAX_GTA_STREAM_HANDLES; n++ )
    {
        if ( gtaStreamHandles[n] == NULL )
            goto validIndex;
    }

    return 0;

validIndex:
    // Interesting fact: This function was key-val protected..!
    HANDLE hFile = CreateFileA( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_READONLY, NULL );

    // Bugfix: do not write invalid handles to the streamHandles
    if ( hFile == INVALID_HANDLE_VALUE )
        return 0;

    gtaStreamHandles[n] = hFile;

    // Bugfix: clip too-long names
    strncpy( gtaStreamNames[n], path, sizeof(streamName)-1 );

    return n << 24;
}

/*=========================================================
    GetFreeArchive

    Arguments:
        idx - write location if a free archive was found
    Purpose:
        Scans the archive array and returns true once the first
        free IMG archive was found.
    Note:
        This function has been inlined into OpenStandardArchives.
=========================================================*/
IMGFile imgArchives[MAX_GTA_IMG_ARCHIVES];

static inline bool GetFreeArchive( unsigned int& idx )
{
    for ( unsigned int n = 0; n < MAX_GTA_IMG_ARCHIVES; n++ )
    {
        if ( imgArchives[n].name[0] == '\0' )
        {
            idx = n;
            return true;
        }
    }

    return false;
}

/*=========================================================
    OpenStandardArchives

    Purpose:
        Initializes the IMG array and opens GTA3.IMG and GTA_INT.IMG.
        They are assigned the first indices. Their indices are stored
        in globals.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004083C0
=========================================================*/
static unsigned int *const VAR_GTA3IMGIndex = (unsigned int*)0x008E48A8;
static unsigned int *const VAR_GTA_INTIndex = (unsigned int*)0x008E48A4;

// NOTE: This function has been optimized for our usage; i.e. the GTA3IMG and GTA_INT always have the same index 0,1
// In GTA:SA, there is a weird logic with free-object allocation (see above GetFreeArchive!).
static void __cdecl OpenStandardArchives( void )
{
    // First, clear the IMG array
    for ( unsigned int n = 0; n < MAX_GTA_IMG_ARCHIVES; n++ )
    {
        imgArchives[n].name[0] = '\0';
        imgArchives[n].handle = 0;
    }

    // Those might be inlined versions of OpenImgFile.
    // Process GTA3.IMG
    IMGFile& gta3 = imgArchives[0];
    strcpy( gta3.name, "MODELS\\GTA3.IMG" );
    gta3.handle = OpenStream_protected( "MODELS\\GTA3.IMG" );
    gta3.isNotPlayerImg = true;

    *VAR_GTA3IMGIndex = 0;

    // Process GTA_INT.IMG
    IMGFile& gta_int = imgArchives[1];
    strcpy( gta_int.name, "MODELS\\GTA_INT.IMG" );
    gta_int.handle = OpenStream_protected( "MODELS\\GTA_INT.IMG" );
    gta_int.isNotPlayerImg = true;

    *VAR_GTA_INTIndex = 1;
}

/*=========================================================
    CModelLoadInfoSA::GetOffset

    Arguments:
        offset - writes the offset in the first 3 bytes
                 and the handle in the last bytes
        blockCount - writes the number of IMG archive blocks
    Purpose:
        Checks whether this entry is valid and returns its
        file information.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004075A0
=========================================================*/
bool CModelLoadInfoSA::GetOffset( unsigned int& offset, unsigned int& blockCount ) const
{
    if ( m_blockCount == 0 )
        return false;

    offset = GetStreamOffset();
    blockCount = m_blockCount;
    return true;
}

/*=========================================================
    CModelLoadInfoSA::SetOffset

    Arguments:
        offset - offset of the entry in bytes
        blockCount - number of IMG archive blocks
    Purpose:
        Updates the file information of this resource entry.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004075E0
=========================================================*/
void CModelLoadInfoSA::SetOffset( unsigned int offset, unsigned int blockCount )
{
    m_blockOffset = offset;
    m_blockCount = blockCount;
}

/*=========================================================
    CModelLoadInfoSA::GetStreamOffset

    Purpose:
        Returns the location of the resource in the archives
        by mixing the handle and the block offset.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00407570
=========================================================*/
unsigned int CModelLoadInfoSA::GetStreamOffset( void ) const
{
    return (unsigned int)imgArchives[m_imgID].handle + m_blockOffset;
}

/*=========================================================
    GetMainArchiveSize

    Purpose:
        Returns the size in bytes of the first IMG archive
        loaded (GTA3.IMG).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00406360
=========================================================*/
static size_t GetMainArchiveSize( void )
{
    return GetFileSize( gtaStreamHandles[0], NULL );
}

/*=========================================================
    OpenImgFile

    Arguments:
        path - filename of the IMG archive
        isNotPlayerImg - boolean whether the archive should be treated as PLAYER.IMG
                         probably loaded on demand
    Purpose:
        Allocates another IMG archive, so it can be used to
        load resources from.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00407610
=========================================================*/
static unsigned int __cdecl OpenImgFile( const char *path, bool isNotPlayerImg )
{
    unsigned int idx;

    if ( !GetFreeArchive( idx ) )
        return 0;

    IMGFile& archive = imgArchives[idx];
    strncpy( archive.name, path, sizeof(archive.name) - 1 );
    archive.handle = OpenStream_protected( path );
    archive.isNotPlayerImg = isNotPlayerImg;
    return idx;
}

// Another lazy hook; I do not know why fastman92 did it, but whatever
static void __declspec(naked) HOOK_DisableGTAIMGCheck()
{
    __asm
    {
        mov ds:[0x008E3FFC],eax
        mov eax,1
        mov esi,0x00406C52
        jmp esi
    }
}

// Let us decipher the ImgManagement, shall we?
void IMG_Initialize( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        *(unsigned int*)0x015663D7 = (unsigned int)imgArchives + offsetof(IMGFile, handle);

        *(unsigned int*)0x0156C2C8 = (unsigned int)gtaStreamHandles;
        break;
    case VERSION_US_10:
        *(unsigned int*)0x015663E7 = (unsigned int)imgArchives + offsetof(IMGFile, handle);

        *(unsigned int*)0x0156C2E8 = (unsigned int)gtaStreamHandles;
        break;
    }

    HookInstall( 0x004067B0, (DWORD)OpenStream_protected, 5 );
    HookInstall( 0x00407610, (DWORD)OpenImgFile, 5 );
    HookInstall( 0x004083C0, (DWORD)OpenStandardArchives, 5 );
    HookInstall( 0x004075A0, h_memFunc( &CModelLoadInfoSA::GetOffset ), 5 );    // Yes, h_memFunc is necessary to simplex-transform the intermengled class-method-pointer
    HookInstall( 0x004075E0, h_memFunc( &CModelLoadInfoSA::SetOffset ), 5 );
    HookInstall( 0x00407570, h_memFunc( &CModelLoadInfoSA::GetStreamOffset ), 5 );
    HookInstall( 0x00406360, (DWORD)GetMainArchiveSize, 5 );

    // The f'ugly hook :D
    HookInstall( 0x00406C28, (DWORD)HOOK_DisableGTAIMGCheck, 5 );

    // 'Lazy' patches, for now (even though they are combined with important routines..!)
    // Let's make sure we analyze them at some point ;)
    *(unsigned int*)0x00409D5A = *(unsigned int*)0x00408FDC = *(unsigned int*)0x0040CC54 = *(unsigned int*)0x0040CCC7 =
    *(unsigned int*)0x01560E68 =
        (unsigned int)imgArchives + offsetof(IMGFile, handle);

    *(unsigned int*)0x004066C7 = *(unsigned int*)0x0040676C = *(unsigned int*)0x004068DD = *(unsigned int*)0x00406B81 =
        (unsigned int)gtaStreamNames;

    *(unsigned int*)0x00406B98 = (unsigned int)gtaStreamNames + sizeof(gtaStreamNames);

    *(unsigned int*)0x004066D6 = *(unsigned int*)0x004066ED = *(unsigned int*)0x00406737 =
    *(unsigned int*)0x00406797 = *(unsigned int*)0x004068AB = *(unsigned int*)0x004068C2 =
    *(unsigned int*)0x004068D0 = *(unsigned int*)0x00406B7C =
        (unsigned int)gtaStreamHandles;
}

void IMG_Shutdown( void )
{
    // Should we terminate resources here?
}