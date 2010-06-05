/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemMemoryHandle.h
*  PURPOSE:     Header file for memory filesystem class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFILESYSTEMMEMORYHANDLE_H
#define __CFILESYSTEMMEMORYHANDLE_H

#include <windows.h>

class CFileSystemMemoryHandle
{
public:
                CFileSystemMemoryHandle     ( HANDLE hFile );
                CFileSystemMemoryHandle     ( HANDLE hFile, size_t InitialBufferSize );
                CFileSystemMemoryHandle     ( HANDLE hFile, void* pBuffer, size_t BufferSize );
                ~CFileSystemMemoryHandle    ( void );

    HANDLE      GetHandle                   ( void );

    BOOL        ReadFile                    ( LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead );
    BOOL        WriteFile                   ( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten );
    DWORD       GetFileSize                 ( LPDWORD lpFileSizeHigh );
    DWORD       SetFilePointer              ( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );

private:
    HANDLE      m_hFile;
    bool        m_bDeleteBuffer;

    char*       m_pFileBuffer;
    size_t      m_sizeBuffer;
    size_t      m_sizeMaxBufferSize;

    size_t      m_sizeFilePointer;
};

#endif
