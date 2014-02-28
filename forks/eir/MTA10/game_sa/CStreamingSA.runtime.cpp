/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.runtime.cpp
*  PURPOSE:     GTA:SA internal FileSystem streaming interface
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace Streaming;

HANDLE gtaStreamHandles[MAX_GTA_STREAM_HANDLES];                // file handles
streamName gtaStreamNames[MAX_GTA_STREAM_HANDLES];

static unsigned int numActiveStreams = 0;                       // Binary offsets: (1.0 US and 1.0 EU): 0x008E4094
                                                                // fun fact: this variable is an unused left-over of Rockstar's development.
                                                                // we all know that GTA:SA for PC is a quick&dirty port.

static DWORD streamOpenFlags = 0;                               // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FE0
static bool isSupportingOverlapped = false;                     // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FE8
static bool enableThreading = false;                            // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FE4

static syncSemaphore *syncSemaphores;                           // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FFC
static unsigned int numSyncSemaphores = 0;                      // Binary offsets: (1.0 US and 1.0 EU): 0x008E4090
static unsigned int *semaphoreQueue = NULL;                     // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FEC
static unsigned int semaphoreQueueSizeCount = 0;                // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FF8
static volatile unsigned int semaphoreQueueReadIndex = 0;       // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FF4

static unsigned int nextStreamReadOffset = 0;                   // Binary offsets: (1.0 US and 1.0 EU): 0x008E4898

static HANDLE globalStreamingSemaphore;                         // Binary offsets: (1.0 US and 1.0 EU): 0x008E4004

static HANDLE streamingThread;                                  // Binary offsets: (1.0 US and 1.0 EU): 0x008E4008
static DWORD streamingThreadId;                                 // Binary offsets: (1.0 US and 1.0 EU): 0x008E4000
static volatile unsigned int semaphoreThreadProcessIndex = 0;   // Binary offsets: (1.0 US and 1.0 EU): 0x008E3FF0

// Use this assert to debug crappy bugs. It performs even in release mode.
// This will force feedback from our users ;)
#ifdef _DEBUG
#define streaming_assert( x )   { assert( x ); }
#else
#define streaming_assert( x )   { if ( !( x ) ) *(unsigned int*)0x00000000 = 0xDEADBEEF; }
#endif //_DEBUG

namespace Streaming
{
    /*=========================================================
        OpenStream

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

    unsigned int __cdecl OpenStream( const char *path )
    {
        // First we find a free stream handle
        unsigned int n;

        // Bugfix: usually GTA:SA would corrupt it's streamHandle slots if the file was not read-accessible
        // I prevent this from happening by not storing INVALID_HANDLE_VALUE, but a NULL
        for ( n = 0; n < MAX_GTA_STREAM_HANDLES; n++ )
        {
            if ( gtaStreamHandles[n] == NULL )
                goto validIndex;
        }

        return 0;

validIndex:
        // Interesting fact: This function was key-val protected..!
        HANDLE hFile = CreateFileA( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            streamOpenFlags | FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_READONLY, NULL );

        // Bugfix: do not write invalid handles to the streamHandles
        if ( hFile == INVALID_HANDLE_VALUE )
            return 0;

        gtaStreamHandles[n] = hFile;

        // Bugfix: clip too-long names
        strncpy( gtaStreamNames[n], path, sizeof(streamName)-1 );

        return n << 24;
    }

    /*=========================================================
        ReadStream

        Arguments:
            syncIdx - index of the semaphore to assign this operation to
            buffer - destination write buffer
            offset - descriptor of the source stream
            blockCount - number of IMG blocks to read
        Purpose:
            Reads from an opened GTA:SA stream handle and puts the
            contents into the given buffer. The operation can either
            be blocking or non-blocking depending isSupportingOverlapped.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406A20
    =========================================================*/
    // If this returns true, then there is a pending operation.
    inline bool ReadStreamInternal( syncSemaphore& sync )
    {
        HANDLE file = sync.file;
        unsigned int blockOffset = sync.blockOffset * 2048;

        if ( isSupportingOverlapped )
        {
            DWORD transferCount;
            sync.overlapped.Offset = blockOffset;

            // Perform an overlapped I/O operation
            return ReadFile( file, sync.buffer, sync.blockCount * 2048, NULL, &sync.overlapped ) == TRUE ||
                GetLastError() == ERROR_IO_PENDING && GetOverlappedResult( file, &sync.overlapped, &transferCount, true ) == TRUE;
        }

        // The slowest and crappiest method :D
        SetFilePointer( file, blockOffset, NULL, FILE_BEGIN );

        return ReadFile( file, sync.buffer, sync.blockCount * 2048, NULL, NULL ) == TRUE;
    }

    int __cdecl ReadStream( unsigned int syncIdx, void *buffer, unsigned int offset, unsigned int blockCount )
    {
        syncSemaphore& sync = syncSemaphores[syncIdx];

        // Reset any Windows error
        SetLastError( 0 );

        // Write the offset of the next read
        nextStreamReadOffset = offset + blockCount;

        // Get the real block offset without stream descriptor
        unsigned int blockOffset = offset & 0x00FFFFFF;

        HANDLE file = sync.file = gtaStreamHandles[offset >> 24];     // grab the appropriate stream

        if ( enableThreading )
        {
            if ( sync.blockCount || sync.threadActive )
                return false;

            // Initialize the sync semaphore for a new request
            sync.resultCode = 0;
            sync.blockCount = blockCount;
            sync.blockOffset = blockOffset;
            sync.buffer = buffer;
            sync.terminating = false;

            // List it into our semaphore operations
            semaphoreQueue[semaphoreQueueReadIndex] = syncIdx;

            // Instead of module, we optimize :3
            if ( semaphoreQueueReadIndex == semaphoreQueueSizeCount - 1 )
                semaphoreQueueReadIndex = 0;
            else
                semaphoreQueueReadIndex++;

            // Notify the streaming thread.
            if ( !ReleaseSemaphore( globalStreamingSemaphore, 1, NULL ) )
                OutputDebugString( "global streaming semaphore release failed\n" );

            return true;
        }
        else if ( isSupportingOverlapped )
        {
            sync.overlapped.Offset = blockOffset * 2048;

            // Perform an overlapped I/O operation
            return ReadFile( file, buffer, blockCount * 2048, NULL, &sync.overlapped ) != FALSE || GetLastError() == ERROR_IO_PENDING;
        }

        // The slowest and crappiest method :D
        SetFilePointer( file, blockOffset * 2048, NULL, FILE_BEGIN );

        return ReadFile( file, buffer, blockCount * 2048, NULL, NULL ) == TRUE;
    }

    /*=========================================================
        GetStreamNextReadOffset

        Purpose:
            Returns the next stream read offset which has been established
            in a prior call to ReadStream. This allows for consecutive stream
            reading.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406450
    =========================================================*/
    unsigned int __cdecl GetStreamNextReadOffset( void )
    {
        return nextStreamReadOffset;
    }

    /*=========================================================
        GetSyncSemaphoreStatus

        Arguments:
            index - index of the syncSemaphore
        Purpose:
            Returns the runtime status code of the specified sync
            semaphore. GTA:SA uses this to know where a sync semaphore
            is being operated on.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x004063E0
    =========================================================*/
    unsigned int __cdecl GetSyncSemaphoreStatus( unsigned int index )
    {
        syncSemaphore& sync = syncSemaphores[index];

        if ( enableThreading )
        {
            if ( sync.threadActive )
                return 0xFF;    // thread is processing it

            if ( sync.blockCount )
                return 0xFA;    // has to be processed yet

            unsigned int result = sync.resultCode;

            if ( result )
                sync.resultCode = 0;

            return result;
        }
        else if ( isSupportingOverlapped )
        {
            return WaitForSingleObjectEx( sync.file, 0, true ) ? 0xFF : 0;
        }

        // No parallel activity on this syncSemaphore.
        return 0;
    }

    /*=========================================================
        CancelSyncSemaphore

        Arguments:
            index - index of the syncSemaphore to cancel
        Purpose:
            Finishes streaming activity on the sync semaphore given by
            index. Returns a status code of the sync object if successful.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406460
    =========================================================*/
    unsigned int __cdecl CancelSyncSemaphore( unsigned int index )
    {
        syncSemaphore& sync = syncSemaphores[index];

        if ( enableThreading )
        {
            // If a sync semaphore is being worked on, we need to wait till finished
            if ( sync.blockCount )
            {
                sync.terminating = true;

                // The_GTA: wait in steps of 5000ms to fix lock-up bug.
                while ( sync.blockCount )
                    WaitForSingleObject( sync.semaphore, 5000 );
            }

            sync.threadActive = false;
            return sync.resultCode;
        }
        else if ( isSupportingOverlapped && sync.file )
        {
            DWORD bytesTransfered;
            return GetOverlappedResult( sync.file, &sync.overlapped, &bytesTransfered, true ) ? 254 : 0;
        }

        return 0;
    }

    /*=========================================================
        CancelAllStreaming

        Purpose:
            Terminates all stream activity by the MTA resource streaming
            system. All handles which were acquired by the engine will
            turn invalid.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406690
    =========================================================*/
    void __cdecl CancelAllStreaming( void )
    {
        for ( unsigned int n = 0; n < numSyncSemaphores; n++ )
            CancelSyncSemaphore( n );

        // The_GTA: fixed the termination of GTA:SA streams here.
        for ( unsigned int n = 0; n < MAX_GTA_STREAM_HANDLES; n++ )
        {
            if ( HANDLE file = gtaStreamHandles[n] )
            {
                SetLastError( 0 );

                CloseHandle( file );

                gtaStreamHandles[n] = NULL;
            }

            gtaStreamNames[n][0] = '\0';
        }
    }

    /*=========================================================
        StreamingThreadProc

        Purpose:
            Runtime procedure of the threaded streaming resource loader.
            This thread runs all the time and allocates contents from 
            IMG file into the engine. It uses semaphores to securely
            write to request buffers inside of "syncSemaphores". We might
            harvest this technique to load resources ourselves. It would
            be fruitful to write our own loader instead and keep this
            as performant as possible.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406560
    =========================================================*/
    static DWORD WINAPI StreamingThreadProc( LPVOID param )
    {
        for (;;)
        {
            // Only run this thread if necessary
            WaitForSingleObject( globalStreamingSemaphore, INFINITE );

            // Execute the current request
            unsigned int curIdx = semaphoreThreadProcessIndex;

            syncSemaphore& sync = syncSemaphores[semaphoreQueue[curIdx]];
            sync.threadActive = true;

            // Only read from the stream if we have not processed this request yet
            if ( sync.resultCode == 0 )
                sync.resultCode = ReadStreamInternal( sync ) ? 0 : 254;

            // Update the request index (if necessary)
            if ( curIdx != semaphoreQueueReadIndex )
            {
                // Optimize without modulo
                if ( curIdx == semaphoreQueueSizeCount - 1 )
                    semaphoreThreadProcessIndex = 0;
                else
                    semaphoreThreadProcessIndex++;
            }

            // We no longer load on this syncSemaphore
            sync.blockCount = 0;

            if ( sync.terminating )
                ReleaseSemaphore( sync.semaphore, 1, NULL );

            sync.threadActive = false;
        }
    }

    /*=========================================================
        CreateStreamingThread

        Purpose:
            Initializes the environment required by the threaded streaming
            resource loader. It initializes the syncSemaphores. Finally,
            it runs the streaming thread.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x004068F0
    =========================================================*/
    static void __cdecl CreateStreamingThread( void )
    {
        // Start with a fresh error info
        SetLastError( 0 );

        for ( unsigned int n = 0; n < numSyncSemaphores; n++ )
        {
            HANDLE semaphore = CreateSemaphore( NULL, 0, 2, NULL );

            syncSemaphores[n].semaphore = semaphore;

            if ( !semaphore )
            {
                OutputDebugString( "cdvd_stream: failed to create sync semaphore\n" );
                return;
            }
        }

        // Allocate the queue
        unsigned int queueCount = numSyncSemaphores + 1;

        // There should be an additional slot for security reasons
        semaphoreQueue = (unsigned int*)LocalAlloc( LMEM_ZEROINIT, sizeof(void*) * numSyncSemaphores );
        semaphoreQueueSizeCount = numSyncSemaphores;

        // Setup the thread variables
        semaphoreThreadProcessIndex = 0;
        semaphoreQueueReadIndex = 0;

        // Allocate the global streaming semaphore
        globalStreamingSemaphore = CreateSemaphore( NULL, 0, numSyncSemaphores, "CdStream" );

        if ( !globalStreamingSemaphore )
        {
            OutputDebugString( "cdvd_stream: failed to create stream semaphore\n" );
            return;
        }

        // Create the streaming thread!
        streamingThread = CreateThread( NULL, 0x10000, (LPTHREAD_START_ROUTINE)StreamingThreadProc, NULL, CREATE_SUSPENDED, &streamingThreadId );

        if ( !streamingThread )
        {
            OutputDebugString( "cdvd_stream: failed to create streaming thread\n" );
            return;
        }

        // Inherit main thread priority
        SetThreadPriority( streamingThread, GetThreadPriority( GetCurrentThread() ) );

        // Run!
        ResumeThread( streamingThread );
    }

    /*=========================================================
        Init

        Arguments:
            numSync - amount of parallel streaming tasks.
                      describes the amount of parallel filesystem reading requests.
                      increasing this value does not improve performance, as
                      syncSymaphores are slot-based.
        Purpose:
            Initializes the streaming runtime environment. It is responsible
            for reading filesystem contents and offering them to the game
            engine. Data can be read multi-threaded (depending on architecture).
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406B70
    =========================================================*/
    void __cdecl Init( unsigned int numSync )
    {
        // Initialize managed variables
        memset( gtaStreamHandles, 0, sizeof(void*) * MAX_GTA_STREAM_HANDLES );

        for ( unsigned int n = 0; n < MAX_GTA_STREAM_HANDLES; n++ )
            gtaStreamNames[n][0] = '\0';

        DWORD sectorsPerCluster;
        DWORD bytesPerSector;
        DWORD numberOfFreeClusters;
        DWORD totalNumberOfClusters;

        // Obtain harddisk information so we decide about buffering
        GetDiskFreeSpaceA( NULL, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters );

        // Adjust the CreateFile FILE_FLAG_* attributes
        DWORD creationFlags = FILE_FLAG_OVERLAPPED;

        if ( bytesPerSector <= 2048 )   // since the sector byte count is fitting into IMG chunks, we need no buffering
            creationFlags |= FILE_FLAG_NO_BUFFERING;

        streamOpenFlags = creationFlags;

        // Test overlapped I/O
        isSupportingOverlapped = true;
        enableThreading = false;    // for the initial testing phase we cannot use the threading API (not initialized yet)

        void *buf = _aligned_malloc( 2048, bytesPerSector );

        // Reset the last Windows error
        SetLastError( 0 );

        *(unsigned int*)0x008E4094 = 0; // unknown
        numSyncSemaphores = numSync;

        // Allocate the semaphore array
        syncSemaphores = (syncSemaphore*)LocalAlloc( LMEM_ZEROINIT, sizeof(syncSemaphore) * numSync );

        // Do the operation
        OpenStream( "MODELS\\GTA3.IMG" );

        unsigned int res = ReadStream( 0, buf, 0, 1 );

        CancelAllStreaming();

        // Testing phase is over; re-enable threading API
        // Fun fact: we could add an option to not use threads, for the user ;)
        enableThreading = true;

        if ( res != TRUE )
        {
            isSupportingOverlapped = false;
            streamOpenFlags &= ~FILE_FLAG_OVERLAPPED;
        }

        // Initialize the streaming thread :)
        CreateStreamingThread();

        // Get rid of the temporary memory
        _aligned_free( buf );
    }

    /*=========================================================
        Terminate

        Purpose:
            Frees all resources which are associated with the streaming
            resource loader.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00406370
    =========================================================*/
    void __cdecl Terminate( void )
    {
        if ( enableThreading )
        {
            LocalFree( semaphoreQueue );
            CloseHandle( globalStreamingSemaphore );
            CloseHandle( streamingThread );

            for ( unsigned int n = 0; n < numSyncSemaphores; n++ )
                CloseHandle( syncSemaphores[n].semaphore );
        }

        LocalFree( syncSemaphores );
    }
};

void StreamingRuntime_Init( void )
{
    using namespace Streaming;

    // Install our code instead of Rockstar's.
    HookInstall( 0x004067B0, (DWORD)OpenStream, 5 );
    HookInstall( 0x00406A20, (DWORD)ReadStream, 5 );
    HookInstall( 0x00406450, (DWORD)GetStreamNextReadOffset, 5 );
    HookInstall( 0x004063E0, (DWORD)GetSyncSemaphoreStatus, 5 );
    HookInstall( 0x00406460, (DWORD)CancelSyncSemaphore, 5 );
    HookInstall( 0x00406690, (DWORD)CancelAllStreaming, 5 );
    HookInstall( 0x00406B70, (DWORD)Init, 5 );
    HookInstall( 0x00406370, (DWORD)Terminate, 5 );

    // Practically, these patches are useless as that code is never called.
    // We leave this in for reference anyway.
    MemPut( (unsigned int*)0x0040676C, gtaStreamNames );
    MemPut( (unsigned int*)0x004068DD, gtaStreamNames );

    MemPut( (unsigned int*)0x00406737, gtaStreamHandles );
    MemPut( (unsigned int*)0x00406797, gtaStreamHandles );
    MemPut( (unsigned int*)0x004068AB, gtaStreamHandles );
    MemPut( (unsigned int*)0x004068C2, gtaStreamHandles );
    MemPut( (unsigned int*)0x004068D0, gtaStreamHandles );
}

void StreamingRuntime_Shutdown( void )
{
    Streaming::Terminate();
}