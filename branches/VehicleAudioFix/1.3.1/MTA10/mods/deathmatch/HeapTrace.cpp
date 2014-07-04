/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/HeapTrace.cpp
*  PURPOSE:     Heap trace
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#ifdef DETECT_LEAKS
using namespace std;

struct ALLOC_INFO
{
    void*           pAddress;
    unsigned int    uiSize;
    char            szFile [64];
    int             iLine;
};

static list < ALLOC_INFO* >* pAllocList;
static unsigned int uiTotalNews = 0;
static unsigned int uiTotalFrees = 0;


void AddTrack ( void* pAddress, size_t size, const char* szFile, int iLine )
{
    // Prevent an infinite loop (self-hooks)
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn)
    {
        bAlreadyIn = true;

        // Create a struct and copy over the info
        ALLOC_INFO* pTemp = new ALLOC_INFO;
        pTemp->pAddress = pAddress;
        pTemp->uiSize = static_cast < unsigned int > ( size );
        strncpy ( pTemp->szFile, szFile, 63 );
        pTemp->szFile [63] = 0;
        pTemp->iLine = iLine;

        // Make sure we got a list
        if ( !pAllocList )
        {
            pAllocList = new list < ALLOC_INFO* >;
        }

        // Put it in
        pAllocList->insert ( pAllocList->begin (), pTemp );

        // Inc the new counter
        ++uiTotalNews;

        // Done adding
        bAlreadyIn = false;
    }
};

void RemoveTrack ( void* pAddress )
{
    // Make sure we got an alloc list
    if ( !pAllocList )
    {
        return;
    }

    // Prevent an infinite loop (self-hooks)
    static bool bAlreadyIn = false;
    if ( !bAlreadyIn )
    {
        bAlreadyIn = true;

        // Inc the free counter
        ++uiTotalFrees;

        // Look for the address in our list
        ALLOC_INFO* pTemp;
        list < ALLOC_INFO* > ::iterator iter = pAllocList->begin ();
        for( ; iter != pAllocList->end() ; iter++ )
        {
            pTemp = *iter;
            if ( pTemp->pAddress == pAddress )
            {
                pAllocList->remove ( pTemp );
                delete pTemp;
                break;
            }
        }

        // Done
        bAlreadyIn = false;
    }
}


void DumpUnfreed ( void )
{
    // Make sure we got a list
    if ( !pAllocList )
    {
        return;
    }

    // Create a dump file
    FILE* pFile = fopen ( "memoryleaks_race.txt", "w+" );
    if ( pFile )
    {
        // Any unfreed items?
        if ( pAllocList->size () > 0 )
        {
            // Dump each unfreed item to a file
            unsigned int uiTotalSize = 0;
            list < ALLOC_INFO* > ::iterator iter = pAllocList->begin ();
            for ( ; iter != pAllocList->end (); iter++ )
            {
                // Grab the item and append the size to the total size
                ALLOC_INFO* pInfo = *iter;
                uiTotalSize += pInfo->uiSize;

                // Write the current info to the file
                fprintf ( pFile, "Address: %p\n"
                                 "Size:    %u\n"
                                 "File:    %s\n"
                                 "Line:    %i\n\n",
                                 pInfo->pAddress,
                                 pInfo->uiSize,
                                 pInfo->szFile,
                                 pInfo->iLine );
            }

            // Total size
            fprintf ( pFile, "----------------\n"
                             "Total 'new' allocs: %u\n"
                             "Total 'delete' frees: %u\n"
                             "Total bytes leaked: %u\n",
                             uiTotalNews,
                             uiTotalFrees,
                             uiTotalSize );
        }
        else
        {
            fprintf ( pFile, "%s", "No memoryleaks\n" );
        }

        // Close the file
        fclose ( pFile );
    }

    // Delete our alloc list
    delete pAllocList;
};

#else
void AddTrack ( void* pAddress, size_t size, const char* szFile, int iLine ) {}
void RemoveTrack ( void* pAddress ) {}
void DumpUnfreed ( void ) {}
#endif
