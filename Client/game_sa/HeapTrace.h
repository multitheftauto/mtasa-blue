/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/HeapTrace.h
 *  PURPOSE:     Heap trace handler
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

void AddTrack(void* pAddress, size_t size, const char* szFile, int iLine);
void RemoveTrack(void* pAddress);
void DumpUnfreed();
