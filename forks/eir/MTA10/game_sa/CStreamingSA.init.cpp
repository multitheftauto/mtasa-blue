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

// IPL files have to be placed in their pool.
void* CIPLFileSA::operator new ( size_t )
{
    return (*ppIPLFilePool)->Allocate();
}

void CIPLFileSA::operator delete ( void *ptr )
{
    (*ppIPLFilePool)->Free( (CIPLFileSA*)ptr );
}

namespace Streaming
{
    /*=========================================================
        CMissingModelInfoSA

        This class stores debug information during the GTA:SA archive
        streaming loading (PLAYER.IMG, GTA3.IMG, ...). Every model file
        that is placed into the IMG archive has to have an IDE entry.
        If not, the filename gets added to this item stack. Yes, Rockstar
        left this debug code inside the engine. This class takes a
        maximum of stack entries, so that you cannot overshoot your
        estimations.
        
        Putting out this stacks content could be useful to modders.
    =========================================================*/
    class CMissingModelInfoSA : public CSimpleItemStack <char[32]>
    {
    public:
        /*=========================================================
            CMissingModelInfoSA::constructor

            Arguments:
                max - maximum number of stack entries
            Purpose:
                Constructs a simple item stack which accepts string
                items.
            Binary offsets (CSimpleItemStack::constructor ?):
                (1.0 US and 1.0 EU): 0x005322A0
        =========================================================*/
        CMissingModelInfoSA( unsigned int max ) : CSimpleItemStack( max )
        {
        }

        /*=========================================================
            CMissingModelInfoSA::Add

            Arguments:
                name - name of the model which misses an IDE entry
            Purpose:
                Adds a name entry of a model which misses its IDE
                entry. If there are too many entries added, it outputs
                a debug message [sic].
            Binary offsets:
                (1.0 US and 1.0 EU): 0x00532310
        =========================================================*/
        inline void Add( const char *name )
        {
            char *alloc = *Allocate();

            if ( !alloc )
            {
                OutputDebugString( "Too many objects without modelinfo structures\n" );
                return;
            }

            memcpy( alloc, name, 32 );
        }
    };

    // Frequently used variables
    static HANDLE *const VAR_UnkFileHandle = (HANDLE*)0x008E4010;
    static size_t *const VAR_NumResourceEntries = (size_t*)0x008E4C68;
    static unsigned short *const VAR_BiggestPrimaryBlockOffset = (unsigned short*)0x008E4CA8;
    static modelId_t *const VAR_LastModelScanIndex = (modelId_t*)0x00AAE948;
    static CModelLoadInfoSA *const VAR_ModelLoadInfo = (CModelLoadInfoSA*)0x008E4CC0;
    static CMissingModelInfoSA *const *VAR_MissingModelInfo = (CMissingModelInfoSA**)0x008E48D0;

    /*=========================================================
        Streaming::GetModelByHash

        Arguments:
            hash - hash key of the model to find
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
        modelId_t n = *VAR_LastModelScanIndex;
        CBaseModelInfoSAInterface *model;

        for ( ; n < DATA_TEXTURE_BLOCK; n++ )
        {
            model = ppModelInfo[n];

            if ( model && model->GetHashKey() == hash )
                goto success;
        }

        n = *VAR_LastModelScanIndex;

        for ( ; n < DATA_TEXTURE_BLOCK; n-- )
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
    static void __cdecl TxdAssignVehiclePaintjob( const char *name, modelId_t id )
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
            ((CVehicleModelInfoSAInterface*)info)->AssignPaintjob( id );
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

    static unsigned int __cdecl RegisterCollision( const char *name )
    {
        CColFileSA *col = new CColFileSA;

        if ( stricmp( name, "procobj" ) == 0 || stricmp( name, "proc_int" ) == 0 || stricmp( name, "proc_int2" ) == 0 )
            col->m_isProcedural = true;

        if ( strnicmp( name, "int_la", 6 ) == 0 ||
             strnicmp( name, "int_sf", 6 ) == 0 ||
             strnicmp( name, "int_veg", 7 ) == 0 ||
             strnicmp( name, "int_cont", 8 ) == 0 ||
             strnicmp( name, "gen_int1", 8 ) == 0 ||
             strnicmp( name, "gen_int2", 8 ) == 0 ||
             strnicmp( name, "gen_int3", 8 ) == 0 ||
             strnicmp( name, "gen_int4", 8 ) == 0 ||
             strnicmp( name, "gen_int5", 8 ) == 0 ||
             strnicmp( name, "gen_intb", 8 ) == 0 ||
             strnicmp( name, "savehous", 8 ) == 0 ||
             stricmp( name, "props" ) == 0 ||
             stricmp( name, "props2" ) == 0 ||      // Okay, I am unsure whether I caught all of the namechecking due to secuROM obfuscation
                                                    // If there is a filename missing, feel free to append it here!
             strnicmp( name, "levelmap", 8 ) == 0 ||
             strnicmp( name, "stadint", 7 ) == 0 )
            col->m_isInterior = true;

        return (*ppColFilePool)->GetIndex( col );
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
        unsigned int n;

        for ( n=0; n<MAX_IPL; n++ )
        {
            CIPLFileSA *ipl = (*ppIPLFilePool)->Get( n );

            if ( ipl && stricmp( ipl->m_name, name ) == 0 )
                return n;
        }

        return 0xFFFFFFFF;
    }

    /*=========================================================
        Streaming::RegisterIPLFile

        Arguments:
            name - string of the IPL filename
        Purpose:
            Allocates an instance in the IPL file pool and returns
            its index. The name of the IPL file is stored in its
            name field.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00405AC0
    =========================================================*/
    static unsigned int __cdecl RegisterIPLFile( const char *name )
    {
        CIPLFileSA *ipl = new CIPLFileSA;

        strcpy( ipl->m_name, name );

        return (*ppIPLFilePool)->GetIndex( ipl );
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
    // Header of a file entry in the IMG archive.
    struct fileHeader
    {
        size_t              offset;                         // 0
        unsigned short      primaryBlockOffset;             // 4
        unsigned short      secondaryBlockOffset;           // 6
        char                name[24];                       // 8
    };

    static void __cdecl LoadArchive( IMGFile& archive, unsigned int imgID )
    {
        CFile *file = OpenGlobalStream( archive.name, "rb" );

        // We should always find the IMG container. Do a security check anyway.
        if ( !file )
            return;

        modelId_t lastID = 0xFFFF;
        union
        {
            char version[4];    // has to be "VER2"
            unsigned int checksum;
        };

        file->Read( version, 1, 4 );

        assert( checksum == '2REV' );

        unsigned int numFiles = 0;
            
        bool success = file->ReadUInt( numFiles );

        if ( success )
        {
            // Load all files one by one
            while ( numFiles-- )
            {
                fileHeader header;

                file->Read( &header, 1, sizeof(header) );

                if ( *VAR_BiggestPrimaryBlockOffset < header.primaryBlockOffset )
                    *VAR_BiggestPrimaryBlockOffset = header.primaryBlockOffset;

                // Zero terminated for safety
                header.name[ sizeof(header.name) - 1 ] = '\0';

                char *dot = strchr( header.name, '.' );

                if ( !dot )
                {
                    lastID = 0xFFFF;
                    continue;
                }

                const char *ext = dot + 1;

                if ( (size_t)( ext - header.name ) > 20 )
                {
                    lastID = 0xFFFF;
                    continue;
                }

                *dot = '\0';

                modelId_t id;

                if ( strnicmp( ext, "DFF", 3 ) == 0 )
                {
                    if ( GetModelByHash( pGame->GetKeyGen()->GetUppercaseKey( header.name ), &id ) )
                    {
                        header.offset |= imgID << 24;

                        // Some sort of debug container
                        (*VAR_MissingModelInfo)->Add( header.name );
                        
                        lastID = -1;
                        continue;
                    }
                }
                else if ( strnicmp( ext, "TXD", 3 ) == 0 )
                {
                    id = pGame->GetTextureManager()->FindTxdEntry( header.name );

                    if ( id == -1 )
                    {
                        id = pGame->GetTextureManager()->CreateTxdEntry( header.name );

                        // Assign the txd to a vehicle if found a valid one
                        TxdAssignVehiclePaintjob( header.name, id );
                    }

                    id += DATA_TEXTURE_BLOCK;
                }
                else if ( strnicmp( ext, "COL", 3 ) == 0 )
                {
                    id = DATA_COLL_BLOCK + RegisterCollision( header.name );
                }
                else if ( strnicmp( ext, "IPL", 3 ) == 0 )
                {
                    id = FindIPLFile( header.name );

                    if ( id == -1 )
                    {
                        id = RegisterIPLFile( header.name );
                    }

                    id += DATA_IPL_BLOCK;
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
                    OutputDebugString( "found unsupported SCM file: " );
                    OutputDebugString( header.name );
                    OutputDebugString( "\n" );
                    continue;
                }
                else
                {
                    *dot = '.';
                    lastID = -1;
                    continue;
                }

                // Prepare the loading of this resource by storing its offset and IMG archive index
                unsigned int offset, count;

                // If this ID slot is already occupied, skip preparing this resource.
                if ( VAR_ModelLoadInfo[id].GetOffset( offset, count ) )
                    continue;

                CModelLoadInfoSA& info = VAR_ModelLoadInfo[id];
                info.m_imgID = imgID;

                if ( header.secondaryBlockOffset != 0 )
                    header.primaryBlockOffset = header.secondaryBlockOffset;

                info.SetOffset( header.offset, header.primaryBlockOffset );
                info.m_flags = 0;

                if ( lastID != -1 )
                    VAR_ModelLoadInfo[lastID].m_lastID = id;

                lastID = id;
            }
        }

        delete file;
    }

    /*=========================================================
        Streaming::Hook_LoadArchives

        Purpose:
            Initializes some global variables and loads all active
            archives into streaming memory except PLAYER.IMG.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x005B82C0
    =========================================================*/
    void __cdecl HOOK_LoadArchives( void )
    {
        *(unsigned int*)0x008E4C90 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C94 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C98 = 0xFFFFFFFF;
        *(unsigned int*)0x008E4C9C = 0xFFFFFFFF;

        *(unsigned int*)0x008E4C8C = 0;

        *(unsigned int*)0x008E4CA0 = 0xFFFFFFFF;

        *VAR_NumResourceEntries = GetFileSize( *VAR_UnkFileHandle, NULL );

        for ( unsigned int n = 0; n < MAX_GTA_IMG_ARCHIVES; n++ )
        {
            IMGFile& file = imgArchives[n];

            if ( file.name[0] == '\0' )
                break;

            if ( !file.isNotPlayerImg )
                continue;

            //LoadArchive( file, n );
            ((void (__cdecl*)( const char *, unsigned int ))0x005B6170)( file.name, n );
        }

        *(unsigned int*)0x008E4C64 = 0;
        *VAR_NumResourceEntries = *VAR_NumResourceEntries / 2048;
    }
};

void StreamingLoader_Init( void )
{
    // Hook the archive loading system
    HookInstall( 0x005B82C0, (DWORD)Streaming::HOOK_LoadArchives, 5 );
}

void StreamingLoader_Shutdown( void )
{
}