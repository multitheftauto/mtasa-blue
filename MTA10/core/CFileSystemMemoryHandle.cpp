/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemMemoryHandle.cpp
*  PURPOSE:     File to memory buffer filesystem abstraction
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CFileSystemMemoryHandle::CFileSystemMemoryHandle ( HANDLE hFile )
{
    // Init
    m_hFile = hFile;

    m_bDeleteBuffer = true;
    m_pFileBuffer = NULL;
    m_sizeBuffer = 0;
    m_sizeMaxBufferSize = 0;
    m_sizeFilePointer = 0;
}


CFileSystemMemoryHandle::CFileSystemMemoryHandle ( HANDLE hFile, size_t InitialBufferSize )
{
    // Init
    m_hFile = hFile;
    m_bDeleteBuffer = true;
    m_sizeMaxBufferSize = 0;
    m_sizeFilePointer = 0;

    // Init our filebuffer
    m_pFileBuffer = new char [ InitialBufferSize ];
    m_sizeBuffer = InitialBufferSize;
}


CFileSystemMemoryHandle::CFileSystemMemoryHandle ( HANDLE hFile, void* pBuffer, size_t BufferSize )
{
    // Init
    m_hFile = hFile;

    m_bDeleteBuffer = false;
    m_pFileBuffer = static_cast < char* > ( pBuffer );
    m_sizeBuffer = BufferSize;
    m_sizeMaxBufferSize = BufferSize;
    m_sizeFilePointer = 0;
}


CFileSystemMemoryHandle::~CFileSystemMemoryHandle ( void )
{
    // Delete our filebuffer
    if ( m_bDeleteBuffer && m_pFileBuffer )
    {
        delete [] m_pFileBuffer;
        m_pFileBuffer = NULL;
    }
}


HANDLE CFileSystemMemoryHandle::GetHandle ( void )
{
    return m_hFile;
}


BOOL CFileSystemMemoryHandle::ReadFile ( LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead )
{
    // Got a buffer
    if ( m_pFileBuffer )
    {
        // Determine how many bytes we can read
        unsigned int uiBytesAvailable = m_sizeBuffer - m_sizeFilePointer;
        unsigned int uiBytesToRead = 0;
        if ( uiBytesAvailable > nNumberOfBytesToRead )
        {
            uiBytesToRead = nNumberOfBytesToRead;
        }
        else
        {
            uiBytesToRead = m_sizeBuffer - m_sizeFilePointer;
        }

        // Memcpy them off the buffer and inc the file pointer
        if ( uiBytesToRead > 0 )
        {
            memcpy ( lpBuffer, m_pFileBuffer + m_sizeFilePointer, uiBytesToRead );
            m_sizeFilePointer += uiBytesToRead;
        }

        // Succeeded
        SetLastError ( NO_ERROR );
        *lpNumberOfBytesRead = uiBytesToRead;
        return TRUE;
    }

    // Failed
    SetLastError ( ERROR_OUTOFMEMORY );     // Blame it on out-of-memory
    *lpNumberOfBytesRead = 0;
    return FALSE;
}


BOOL CFileSystemMemoryHandle::WriteFile ( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten )
{
    // ## TODO ## : Ability to increase the buffersize
    // Got a buffer
    if ( m_pFileBuffer )
    {
        // Determine how many bytes we can write
        unsigned int uiBytesLeft = m_sizeBuffer - m_sizeFilePointer;
        unsigned int uiBytesToWrite = 0;
        if ( uiBytesLeft > nNumberOfBytesToWrite )
        {
            uiBytesToWrite = nNumberOfBytesToWrite;
        }
        else
        {
            uiBytesToWrite = m_sizeBuffer - m_sizeFilePointer;
        }

        // Memcpy them off the buffer and inc the file pointer
        if ( uiBytesToWrite > 0 )
        {
            memcpy ( m_pFileBuffer + m_sizeFilePointer, lpBuffer, uiBytesToWrite );
            m_sizeFilePointer += uiBytesToWrite;
        }

        // Succeeded
        SetLastError ( NO_ERROR );
        *lpNumberOfBytesWritten = uiBytesToWrite;
        return TRUE;
    }

    // Failed
    SetLastError ( ERROR_OUTOFMEMORY );
    *lpNumberOfBytesWritten = 0;
    return FALSE;
}


DWORD CFileSystemMemoryHandle::GetFileSize ( LPDWORD lpFileSizeHigh )
{
    SetLastError ( NO_ERROR );
    *lpFileSizeHigh = 0;
    return static_cast < DWORD > ( m_sizeBuffer );
}


DWORD CFileSystemMemoryHandle::SetFilePointer ( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod )
{
    switch ( dwMoveMethod )
    {
        case FILE_BEGIN:
        {
            m_sizeFilePointer = lDistanceToMove;
            if ( m_sizeFilePointer > m_sizeBuffer )
            {
                m_sizeFilePointer = m_sizeBuffer;
            }

            break;
        }

        case FILE_CURRENT:
        {
            m_sizeFilePointer += lDistanceToMove;
            if ( m_sizeFilePointer > m_sizeBuffer )
            {
                m_sizeFilePointer = m_sizeBuffer;
            }

            break;
        }

        case FILE_END:
        {
            m_sizeFilePointer = m_sizeBuffer - lDistanceToMove;
            if ( m_sizeFilePointer > m_sizeBuffer )
            {
                m_sizeFilePointer = m_sizeBuffer;
            }

            break;
        }
    }

    if ( lpDistanceToMoveHigh )
    {
        *lpDistanceToMoveHigh = 0;
    }

    SetLastError ( NO_ERROR );
    return m_sizeFilePointer;
}
