/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.init.cpp
*  PURPOSE:     Streaming initialization routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CBaseModelInfoSAInterface **ppModelInfo;

namespace Streaming
{
    // Frequently used variables
    static HANDLE *const VAR_UnkFileHandle = (HANDLE*)0x008E4010;
    static size_t *const VAR_NumResourceEntries = (size_t*)0x008E4C68;
    int biggestResourceBlockCount = 0;              // Binary offsets: (1.0 US and 1.0 EU): 0x008E4CA8
    static int *const VAR_LastModelScanIndex = (int*)0x00AAE948;
    static CModelLoadInfoSA *const VAR_ModelLoadInfo = (CModelLoadInfoSA*)0x008E4CC0;
    static CMissingModelInfoSA *const *VAR_MissingModelInfo = (CMissingModelInfoSA**)0x008E48D0;

    /*=========================================================
        Streaming::GetModelByName

        Arguments:
            name - name string to check case-insensitively with
            id (optional) - index of the model info (if found)
        Purpose:
            Returns the model info associated with the hash key. If
            found and id pointer != NULL, it writes its id there.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x004C5940
    =========================================================*/
    CBaseModelInfoSAInterface* __cdecl GetModelByHash( unsigned int hash, modelId_t *id )
    {
        // Use the last model index (performance improvement?)
        int n = *VAR_LastModelScanIndex;
        CBaseModelInfoSAInterface *model;

        for ( ; n < MAX_MODELS; n++ )
        {
            model = ppModelInfo[n];

            if ( model && model->GetHashKey() == hash )
                goto success;
        }

        n = *VAR_LastModelScanIndex;

        for ( ; n >= 0; n-- )
        {
            model = ppModelInfo[n];

            if ( model && model->GetHashKey() == hash )
                goto success;
        }

        return NULL;

success:
        if ( id )
            *id = n;

        *VAR_LastModelScanIndex = n;
        return model;
    }

    CBaseModelInfoSAInterface* __cdecl GetModelByName( const char *name, modelId_t *id )
    {
        return GetModelByHash( pGame->GetKeyGen()->GetUppercaseKey( name ), id );
    }

    /*=========================================================
        Streaming::GetModelInfoByName

        Arguments:
            name - string which contains the model info name
            startId - model info range start
            endId - model info range end
            id (optional) - pointer to store model info id at (if found)
        Purpose:
            Returns the model info associated with the given name.
            It scans a region from startId to endId and inclused startId
            and endId model infos. If found and id pointer is specified,
            it stores the model info id there.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x004C5A20
        Note:
            The id argument is an extension which the original
            function does not have.
    =========================================================*/
    CBaseModelInfoSAInterface* GetModelInfoByName( const char *name, modelId_t startId, modelId_t endId, modelId_t *id )
    {
        unsigned int hash = pGame->GetKeyGen()->GetUppercaseKey( name );

        for ( modelId_t n = startId; n <= endId; n++ )
        {
            CBaseModelInfoSAInterface *info = ppModelInfo[n];

            if ( info && info->GetHashKey() == hash )
            {
                if ( id )
                    *id = n;

                return info;
            }
        }

        return NULL;
    }

    /*=========================================================
        Streaming::TxdAssignVehiclePaintjob

        Arguments:
            name - string which contains the model info name
            id - TXD instance container index
        Purpose:
            Attempts to register another paintjob TXD instance id
            to a model described by name.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x004C9360
    =========================================================*/
    static void __cdecl TxdAssignVehiclePaintjob( const char *name, int txdId )
    {
        unsigned char len = (unsigned char)strlen( name ) - 1;
        unsigned char lastchr = name[len];

        if ( !isdigit( name[len] ) )
            return;

        // Filter out the numbers
        char buf[24];

        // Bugfix: if a name consists of numbers entirely, we reject it
        for (;;)
        {
            if ( !( --len ) )
                return;

            if ( !isdigit( name[len] ) )
                break;
        }

        // Store the result
        strncpy( buf, name, ++len );
        buf[len] = '\0';

        // Yep, paintjobs only ever work for ids 400 to 630.
        CBaseModelInfoSAInterface *info = GetModelInfoByName( buf, 400, 630, NULL );

        // It only works for vehicle models
        if ( info && info->GetModelType() == MODEL_VEHICLE )
        {
            // Put the paintjob into our modelinfo
            ((CVehicleModelInfoSAInterface*)info)->AssignPaintjob( txdId );
        }
    }

    /*=========================================================
        Streaming::RegisterCollision

        Arguments:
            name - string of the collision descriptor file
        Purpose:
            Allocates a named COL library entry into its pool. If
            its name describes an interior or a procedural setting,
            it is flagged accordingly.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00411140
    =========================================================*/

    /*
        SECUmadness chapter 2: Streaming::RegisterCollision

        While the previous secuROM encounter with OpenImgFile had been a mad ride, this function
        appears to have less protective measures. The annoying thing about reading it is the constant
        anti-trace methods considering that this function is nowhere worthy of such security.

        Apparrently this is one of the many routines which add complexity to other code-areas by referencing
        chunks of bytes as dword-values, while in reality there dword-value are parts of routines themselves!
        Luckily all pointers are within secuROM borders.

        But I have learned from my jonders: Spectating two versions of GTA:SA at a time, utilizing different
        obfuscation methods, is very helpful..!
    */

    unsigned int __cdecl RegisterCollision( const char *name )
    {
        return Streaming::GetCOLEnvironment().RegisterInstance( name );
    }

    /*=========================================================
        Streaming::FindIPLFile

        Arguments:
            name - string of the IPL filename
        Purpose:
            Returns the index of the IPL file which matches the given
            filename. Returns 0xFFFFFFFF if not found.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00404AC0
    =========================================================*/
    static unsigned int __cdecl FindIPLFile( const char *name )
    {
        IPLEnv_t::pool_t *pool = Streaming::GetIPLEnvironment().m_pool;

        for ( unsigned int n = 0; n < pool->GetMax(); n++ )
        {
            CIPLFileSA *ipl = pool->Get( n );

            if ( ipl && stricmp( ipl->m_name, name ) == 0 )
                return n;
        }

        return 0xFFFFFFFF;
    }

    /*=========================================================
        Streaming::LoadArchive

        Arguments:
            archive - pointer to the IMG archive entry (probably just filename)
            imgID - index of the loading IMG archive
        Purpose:
            Loads an IMG archive into the streaming management memory.
            This allocates entries for COL libraries and IPL sectors.
            Paintjobs are allocated and models validated.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x005B6170
    =========================================================*/
    static void __cdecl LoadArchive( IMGFile& archive, unsigned int imgID )
    {
        CFile *file = OpenGlobalStream( archive.name, "rb" );

        // We should always find the IMG container. Do a security check anyway.
        // NOTE: Rockstar did this too.
        if ( !file )
            return;

        modelId_t lastID = 0xFFFFFFFF;
        union
        {
            char version[8];    // has to be "VER2"
            unsigned int checksum;
        };

        memset( version, 0, 5 );

        file->Read( version, 1, 4 );

        assert( checksum == '2REV' );
            
        int numFiles;
        
        bool successful = file->ReadInt( numFiles );

        // Bugfix: make sure we read the file entry count properly.
        if ( successful )
        {
            // Load all files one by one
            while ( numFiles )
            {
                numFiles--;

                resourceFileHeader header;

                file->Read( &header, 1, sizeof(header) );

                if ( header.primaryBlockOffset > biggestResourceBlockCount )
                    biggestResourceBlockCount = header.primaryBlockOffset;

                // Zero terminated for safety
                header.name[ sizeof(header.name) - 1 ] = '\0';

                char *dot = strchr( header.name, '.' );

                if ( !dot || (size_t)( dot - header.name ) > 20 )
                {
                    header.name[ sizeof(header.name) - 1 ] = '\0';
                    goto failureAdd;
                }

                const char *ext = dot + 1;

                *dot = '\0';

                modelId_t id;

                if ( strnicmp( ext, "DFF", 3 ) == 0 )
                {
                    if ( !GetModelByName( header.name, &id ) )
                    {
                        // We found a structure that has no representation in the game configuration files.
                        // Native GTA:SA supports about 550 of such models.
                        // We can still load such data through .SCM there (RequestSpecialModel).
                        // That is done by storing the resourceFileHeader in a special stack.
                        header.offset = Streaming::GetFileHandle( imgID, header.offset );

                        // Some sort of debug container
                        (*VAR_MissingModelInfo)->Add( header );
                        
                        goto failureAdd;
                    }
                }
                else if ( strnicmp( ext, "TXD", 3 ) == 0 )
                {
                    int txdId = pGame->GetTextureManager()->FindTxdEntry( header.name );

                    if ( txdId == -1 )
                    {
                        txdId = pGame->GetTextureManager()->CreateTxdEntry( header.name );

                        // Assign the txd to a vehicle if found a valid one
                        TxdAssignVehiclePaintjob( header.name, txdId );
                    }

                    id = (modelId_t)( txdId + DATA_TEXTURE_BLOCK );
                }
                else if ( strnicmp( ext, "COL", 3 ) == 0 )
                {
                    id = DATA_COLL_BLOCK + RegisterCollision( header.name );
                }
                else if ( strnicmp( ext, "IPL", 3 ) == 0 )
                {
                    unsigned int iplIndex = FindIPLFile( header.name );

                    if ( iplIndex == 0xFFFFFFFF )
                    {
                        iplIndex = GetIPLEnvironment().RegisterInstance( header.name );
                    }

                    id = iplIndex + DATA_IPL_BLOCK;
                }
                else if ( strnicmp( ext, "DAT", 3 ) == 0 )
                {
                    sscanf( header.name + 5, "%d", &id );
                    
                    id += DATA_PATHFIND_BLOCK;
                }
                else if ( strnicmp( ext, "IFP", 3 ) == 0 )
                {
                    id = DATA_ANIM_BLOCK + pGame->GetAnimManager()->RegisterAnimBlock( header.name );
                }
                else if ( strnicmp( ext, "RRR", 3 ) == 0 )
                {
                    id = DATA_RECORD_BLOCK + pGame->GetRecordings()->Register( header.name );
                }
                else if ( strnicmp( ext, "SCM", 3 ) == 0 )
                {
                    // For now we do not need these script files.
                    // If we ever need them, contact midnightStar/Martin.
#ifdef _DEBUG
                    OutputDebugString( "found unsupported SCM file: " );
                    OutputDebugString( header.name );
                    OutputDebugString( "\n" );
#endif //_DEBUG
                    goto failureAdd;
                }
                else
                {
                    *dot = '.';
                    goto failureAdd;
                }

                // Prepare the loading of this resource by storing its offset and IMG archive index
                unsigned int offset, count;

                // If this ID slot is already occupied, skip preparing this resource.
                CModelLoadInfoSA& info = Streaming::GetModelLoadInfo( id );

                if ( info.GetOffset( offset, count ) )
                    goto failureAdd;

                info.m_imgID = imgID;

                if ( header.secondaryBlockOffset != 0 )
                    header.primaryBlockOffset = header.secondaryBlockOffset;

                info.SetOffset( header.offset, header.primaryBlockOffset );
                info.m_flags = 0;

                // This id is used to optimize loading from disk.
                // The streaming system knows which data is next to each other.
                // It can be loaded in a swipe.
                if ( lastID != 0xFFFFFFFF )
                    Streaming::GetModelLoadInfo( lastID ).m_lastID = id;

                lastID = id;
                continue;

failureAdd:
                lastID = 0xFFFFFFFF;
            }
        }

        delete file;
    }

    /*=========================================================
        Streaming::LoadArchives

        Purpose:
            Initializes some global variables and loads all active
            archives into streaming memory except PLAYER.IMG.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x005B82C0
    =========================================================*/
    void __cdecl LoadArchives( void )
    {
        // Initialize some unknown globals.
        *(unsigned int*)0x008E4C90 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C94 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C98 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C9C = 0xFFFFFFFF;

        *(unsigned int*)0x008E4C8C = 0;

        *(unsigned int*)0x008E4CA0 = 0xFFFFFFFF;

        // ???
        *VAR_NumResourceEntries = GetMainArchiveSize();

        for ( unsigned int n = 0; n < MAX_GTA_IMG_ARCHIVES; n++ )
        {
            IMGFile& file = imgArchives[n];

            if ( file.name[0] == '\0' )
                break;

            // We do not load player IMGs.
            if ( !file.isNotPlayerImg )
                continue;

            // Load the IMG archive.
            LoadArchive( file, n );
        }

        *(unsigned int*)0x008E4C64 = 0;
        *VAR_NumResourceEntries = *VAR_NumResourceEntries / 2048;
    }
};

void StreamingLoader_Init( void )
{
}

void StreamingLoader_Shutdown( void )
{
}