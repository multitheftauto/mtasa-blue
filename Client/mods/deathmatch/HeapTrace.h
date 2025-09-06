/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/HeapTrace.h
 *  PURPOSE:     Heap trace
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

void AddTrack(void* pAddress, size_t size, const char* szFile, int iLine);
void RemoveTrack(void* pAddress);
void DumpUnfreed();
