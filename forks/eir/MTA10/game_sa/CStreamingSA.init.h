/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.init.h
*  PURPOSE:     Header file for streaming initialization routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_INIT_
#define _STREAMING_INIT_

// IMG archive entries stored in a global array (parallel to model info array)
#define ARRAY_CModelLoadInfo                                0x008E4CC0

#define VAR_StreamerThreadHandle                            0x008E4008

class CBaseModelInfoSAInterface;

// Streaming exports
namespace Streaming
{
    // Important globals.
    extern int biggestResourceBlockCount;
    extern modelId_t streamingWaitModel;

    // Header of a file entry in the IMG archive.
    struct resourceFileHeader
    {
        size_t              offset;                         // 0
        unsigned short      primaryBlockOffset;             // 4
        unsigned short      secondaryBlockOffset;           // 6
        char                name[24];                       // 8
    };

    /*=========================================================
        CMissingModelInfoSA

        This class stores debug information during the GTA:SA archive
        streaming loading (PLAYER.IMG, GTA3.IMG, ...). Every model file
        that is placed into the IMG archive has to have an IDE entry.
        If not, the filename gets added to this item stack. This is not
        debug code, but a cache of resource entries which have no
        configuration representation. These resources can still be
        loaded using RequestSpecialModel.

        The debugging features are just side-effects.
    =========================================================*/
    class CMissingModelInfoSA : public CSimpleItemStack <resourceFileHeader>
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
                header - resource file header to add to this cache
            Purpose:
                Adds a resource file header into this resource cache. 
                If there are too many entries added, it outputs
                a debug message [sic].
            Binary offsets:
                (1.0 US and 1.0 EU): 0x00532310
        =========================================================*/
        inline void Add( resourceFileHeader& header )
        {
            resourceFileHeader *newEntry = Allocate();

            if ( !newEntry )
            {
                OutputDebugString( "Too many objects without modelinfo structures\n" );
                return;
            }

            *newEntry = header;
        }

        /*=========================================================
            CMissingModelInfoSA::FindByName

            Arguments:
                name - name of the model which misses an IDE entry
            Purpose:
                Returns the resourceFileHeader of the entry found
                in this container. If none is found, it returns NULL.
            Binary offsets:
                (1.0 US and 1.0 EU): 0x00532450
        =========================================================*/
        inline resourceFileHeader* FindByName( const char *name )
        {
            for ( unsigned int n = 0; n < m_count; n++ )
            {
                resourceFileHeader *entry = m_stack + n;

                if ( stricmp( entry->name, name ) == 0 )
                    return entry;
            }

            return NULL;
        }

        /*=========================================================
            CMissingModelInfoSA::GetEntry

            Arguments:
                name - name of the model which misses an IDE entry
                offset - file offset of the entry
                blockCount - entry size in IMG chunks
            Purpose:
                Writes information about the resource lacking
                game configuration. If the resource is not found by
                name, nothing is written. Returns whether the operation
                was successful.
            Binary offsets:
                (1.0 US and 1.0 EU): 0x005324A0
        =========================================================*/
        inline bool GetEntry( const char *name, unsigned int& offset, unsigned int& blockCount )
        {
            resourceFileHeader *entry = FindByName( name );

            if ( !entry )
                return false;

            offset = entry->offset;
            blockCount = entry->primaryBlockOffset;
            return true;
        }
    };

    inline CModelLoadInfoSA&    GetModelLoadInfo( modelId_t id )
    {
        return *( (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id );
    }

    inline CModelLoadInfoSA&    GetModelLoadInfo( modelId_t offset, modelId_t id )
    {
        return *( ( (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + offset ) + id );
    }

    inline CModelLoadInfoSA*    GetNextLoadInfo( CModelLoadInfoSA *info )
    {
        if ( info->m_primaryModel == 0xFFFF )
            return NULL;

        return &GetModelLoadInfo( info->m_primaryModel );
    }

    inline CModelLoadInfoSA*    GetPrevLoadInfo( CModelLoadInfoSA *info )
    {
        if ( info->m_secondaryModel == 0xFFFF )
            return NULL;

        return &GetModelLoadInfo( info->m_secondaryModel );
    }

    inline CModelLoadInfoSA*    GetQueuedLoadInfo( void )
    {
        return GetNextLoadInfo( *(CModelLoadInfoSA**)0x008E4C58 );
    }

    inline CModelLoadInfoSA*    GetLastQueuedLoadInfo( void )
    {
        return GetPrevLoadInfo( *(CModelLoadInfoSA**)0x008E4C54 );
    }

    inline CModelLoadInfoSA*    GetGarbageCollectModel( void )
    {
        return GetPrevLoadInfo( *(CModelLoadInfoSA**)0x008E4C5C );
    }

    inline CModelLoadInfoSA*    GetLastGarbageCollectModel( void )
    {
        return *(CModelLoadInfoSA**)0x008E4C60;
    }

    CBaseModelInfoSAInterface* __cdecl  GetModelByHash      ( unsigned int hash, modelId_t *id );
    CBaseModelInfoSAInterface* __cdecl  GetModelByName      ( const char *name, modelId_t *id );
    CBaseModelInfoSAInterface*          GetModelInfoByName  ( const char *name, modelId_t startId, modelId_t endId, modelId_t *id );

    unsigned int __cdecl RegisterCollision( const char *name );
    unsigned int __cdecl RegisterIPLFile( const char *name );

    void __cdecl LoadArchives( void );
};

void StreamingLoader_Init( void );
void StreamingLoader_Shutdown( void );

#endif //_STREAMING_INIT_