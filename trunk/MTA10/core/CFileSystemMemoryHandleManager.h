/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemMemoryHandleManager.h
*  PURPOSE:     Header file for memory buffer filesystem manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFILESYSTEMMEMORYHANDLEMANAGER_H
#define __CFILESYSTEMMEMORYHANDLEMANAGER_H

#include "CFileSystemMemoryHandle.h"
#include <list>

using namespace std;

class CFileSystemMemoryHandleManager
{
public:
                                        CFileSystemMemoryHandleManager          ( void );
                                        ~CFileSystemMemoryHandleManager         ( void );

    CFileSystemMemoryHandle*            Add                                     ( CFileSystemMemoryHandle* pFileSystemMemoryHandle );
    CFileSystemMemoryHandle*            Get                                     ( HANDLE hFile );

    void                                Remove                                  ( CFileSystemMemoryHandle* pFileSystemMemoryHandle );
    void                                Remove                                  ( HANDLE hFile );

private:
    list < CFileSystemMemoryHandle* >   m_List;
};

#endif
