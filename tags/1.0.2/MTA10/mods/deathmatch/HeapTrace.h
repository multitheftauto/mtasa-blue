/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/HeapTrace.h
*  PURPOSE:     Heap trace
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __HEAPTRACE_H
#define __HEAPTRACE_H

void AddTrack ( void* pAddress, size_t size, const char* szFile, int iLine );
void RemoveTrack ( void* pAddress );
void DumpUnfreed ( void );

#endif
