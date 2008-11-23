/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemHook.cpp
*  PURPOSE:     Detouring and hooking of file system functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "detours/include/detours.h"

/*
#define _UNICODE
#include <tchar.h>

#include "CFileSystemHook.h"
#include "detours/include/detours.h"
#include "CDirect3DHook9.h"
#include "CDirectInputHook8.h"
#include "CLogger.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <d3d9.h>
*/

template<> CFileSystemHook * CSingleton< CFileSystemHook >::m_pSingleton = NULL;

CFileSystemHook::CFileSystemHook ( )
{
    m_pMemoryHandleManager = new CFileSystemMemoryHandleManager;

    WriteDebugEvent ( "CFileSystemHook::CFileSystemHook" );
}

CFileSystemHook::~CFileSystemHook ( )
{
    delete m_pMemoryHandleManager;

	RemoveHook ( );

    WriteDebugEvent ( "CFileSystemHook::~CFileSystemHook" );
}

CFileSystemMemoryHandleManager* CFileSystemHook::GetFileSystemMemoryHandleManager ( void )
{
    return m_pMemoryHandleManager;
}

void CFileSystemHook::ApplyHook ( )
{
    // Hook createfile.
	m_pfnCreateFileA = reinterpret_cast < pCreateFileA > ( DetourFunction ( DetourFindFunction ( "Kernel32.dll", "CreateFileA" ), 
																			reinterpret_cast < PBYTE > ( API_CreateFileA ) ) );

    // Hook readfile
    m_pfnReadFile = reinterpret_cast < pReadFile > ( DetourFunction ( DetourFindFunction( "Kernel32.dll", "ReadFile" ), 
																			      reinterpret_cast < PBYTE > ( API_ReadFile ) ) );

    m_pfnGetFileSize = reinterpret_cast < pGetFileSize > ( DetourFunction ( DetourFindFunction( "Kernel32.dll", "GetFileSize" ), 
																			      reinterpret_cast < PBYTE > ( API_GetFileSize ) ) );

    // Hook setfilepointer.
	m_pfnSetFilePointer = reinterpret_cast < pSetFilePointer > ( DetourFunction ( DetourFindFunction( "Kernel32.dll", "SetFilePointer" ), 
																			      reinterpret_cast < PBYTE > ( API_SetFilePointer ) ) );

    // Hook closehandle
	m_pfnCloseHandle = reinterpret_cast < pCloseHandle > ( DetourFunction ( DetourFindFunction( "Kernel32.dll", "CloseHandle" ), 
																			      reinterpret_cast < PBYTE > ( API_CloseHandle ) ) );

	WriteDebugEvent ( "File system hooks applied." );
}

void CFileSystemHook::RemoveHook ( )
{
    // UnHook createfile.
	if ( m_pfnCreateFileA )
	{
		DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnCreateFileA ), 
					   reinterpret_cast < PBYTE > ( API_CreateFileA  ) );
	}

    // UnHook readfile
    if ( m_pfnReadFile )
    {
        DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnReadFile ), 
					   reinterpret_cast < PBYTE > ( API_ReadFile ) );
    }

    // UnHook getfilesize
    if ( m_pfnGetFileSize )
    {
        DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnGetFileSize ), 
					   reinterpret_cast < PBYTE > ( API_GetFileSize ) );
    }

    // UnHook setfilepointer
	if ( m_pfnSetFilePointer )
	{
		DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnSetFilePointer ), 
					   reinterpret_cast < PBYTE > ( API_SetFilePointer  ) );
	}

    // UnHook closehandle
	if ( m_pfnCloseHandle )
	{
		DetourRemove ( reinterpret_cast < PBYTE > ( m_pfnCloseHandle ), 
					   reinterpret_cast < PBYTE > ( API_CloseHandle  ) );
	}



	WriteDebugEvent ( "File system hooks removed." );
}


HANDLE WINAPI CFileSystemHook::API_CreateFileA ( LPCTSTR               lpFileName,
                                                 DWORD                 dwDesiredAccess,
                                                 DWORD                 dwShareMode,
                                                 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                                 DWORD                 dwCreationDisposition,
                                                 DWORD                 dwFlagsAndAttributes,
                                                 HANDLE                hTemplateFile )
{	
    CFileSystemHook *   pThis;
    string              strRedirectedFile;
    bool                bRedirectToFileBuffer;
    void*               pFilebufferOut;
    size_t              size;
    bool                bReadOnly;

    // Get our self-pointer.
    pThis = CFileSystemHook::GetSingletonPtr ( );

    // See if we have a redirect for this file.  If not, just do the normal call.
    if ( pThis->GetRedirectedFile ( lpFileName, strRedirectedFile, bRedirectToFileBuffer, &pFilebufferOut, size, bReadOnly ) )
    {
        if ( !bRedirectToFileBuffer )
        {
            return pThis->m_pfnCreateFileA ( strRedirectedFile.c_str ( ),
                                            dwDesiredAccess,
                                            dwShareMode,
                                            lpSecurityAttributes,
                                            dwCreationDisposition,
                                            dwFlagsAndAttributes,
                                            hTemplateFile );
        }
        else
        {
            // Create a filebuffer for it
            CFileSystemMemoryHandle* pHandle = NULL;
            if ( !pFilebufferOut )
            {
                pHandle = new CFileSystemMemoryHandle ( (HANDLE)13556 );
            }
            else
            {
                pHandle = new CFileSystemMemoryHandle ( (HANDLE)13556, pFilebufferOut, size );
            }

            pThis->m_pMemoryHandleManager->Add ( pHandle );
            SetLastError ( NO_ERROR );
            return (HANDLE)13556;
        }
    }

    // Return the created file.
	return pThis->m_pfnCreateFileA ( lpFileName,
                                     dwDesiredAccess,
                                     dwShareMode,
                                     lpSecurityAttributes,
                                     dwCreationDisposition,
                                     dwFlagsAndAttributes,
                                     hTemplateFile );
}

BOOL WINAPI CFileSystemHook::API_ReadFile           ( HANDLE hFile,
                                                      LPVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped )
{
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ();
    if ( pThis )
    {
        CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
        if ( pHandle )
        {
            return pHandle->ReadFile ( lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead );
        }
        else
        {
            return pThis->m_pfnReadFile ( hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped );
        }
    }

    return FALSE;
}

DWORD WINAPI CFileSystemHook::API_WriteFile         ( HANDLE hFile,
                                                      LPCVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped )
{
    return 0;
}

DWORD WINAPI CFileSystemHook::API_GetFileSize ( HANDLE hFile, LPDWORD lpFileSizeHigh )
{
    CFileSystemHook * pThis;

    // Get our self-pointer.
    pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
    if ( pHandle )
    {
        return pHandle->GetFileSize ( lpFileSizeHigh );
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnGetFileSize ( hFile, lpFileSizeHigh );
    }
}

DWORD WINAPI CFileSystemHook::API_SetFilePointer  ( HANDLE   hFile,
                                                    LONG     lDistanceToMove,
                                                    PLONG    lpDistanceToMoveHigh,
                                                    DWORD    dwMoveMethod )
{
    // Get our self-pointer.
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
    if ( pHandle )
    {
        return pHandle->SetFilePointer ( lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnSetFilePointer ( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
    }
}

BOOL CFileSystemHook::API_CloseHandle ( HANDLE hObject )
{
    // Get our self-pointer.
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hObject );
    if ( pHandle )
    {
        pThis->m_pMemoryHandleManager->Remove ( pHandle );
        delete pHandle;
        return TRUE;
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnCloseHandle ( hObject );
    }
}

void CFileSystemHook::RedirectFile       ( string NativeFile, string RedirectedFile )
{
    FILEREDIRECT RedirectedFileEntry;

    // Copy in our parameters.
    RedirectedFileEntry.NativeFile      = NativeFile;
    RedirectedFileEntry.RedirectedFile  = RedirectedFile;
    RedirectedFileEntry.bRedirectToBuffer = false;
    RedirectedFileEntry.pBuffer = NULL;
    RedirectedFileEntry.sizeBufferSize = 0;
    RedirectedFileEntry.bBufferReadOnly = false;
    
    // Add this item to the list.
    m_RedirectList.insertAtFront ( RedirectedFileEntry );
}

void CFileSystemHook::RedirectFile	( string RedirectedFileName, void* pFilebufferOut, size_t size, bool bReadOnly )
{
    FILEREDIRECT RedirectedFileEntry;

    // Copy in our parameters.
    RedirectedFileEntry.NativeFile              = RedirectedFileName;
    RedirectedFileEntry.RedirectedFile          = "";
    RedirectedFileEntry.bRedirectToBuffer   = true;
    RedirectedFileEntry.pBuffer             = pFilebufferOut;
    RedirectedFileEntry.sizeBufferSize          = size;
    RedirectedFileEntry.bBufferReadOnly               = bReadOnly;
    
    // Add this item to the list.
    m_RedirectList.insertAtFront ( RedirectedFileEntry );
}

void CFileSystemHook::RemoveRedirect     ( string NativeFileToRemove )
{
	CLinkedList< FILEREDIRECT >::CIterator iterator;

    // Find the entry we're looking for.
	for ( iterator = m_RedirectList.getBegin (); 
          iterator != m_RedirectList.getEnd (); 
          iterator++ ) 
    {
        // Check to see if this is the variable.
        if ( iterator->NativeFile == NativeFileToRemove )
        { 
            // Remove this item.
            m_RedirectList.remove ( iterator );

			if ( m_RedirectList.isEmpty() ) break;
			iterator = m_RedirectList.getBegin();
        }
	}
}

bool CFileSystemHook::GetRedirectedFile   ( string NativeFileToCheck, string & RedirectedFileOut, bool& bRedirectToFileBuffer, void** pFilebufferOut, size_t& size, bool& bReadOnly )
{
	CLinkedList< FILEREDIRECT >::CIterator iterator;

    // Find the entry we're looking for.
	for ( iterator = m_RedirectList.getBegin (); 
          iterator != m_RedirectList.getEnd (); 
          iterator++ ) 
    {
        // Check to see if this is the variable.
        if ( iterator->NativeFile.find ( NativeFileToCheck ) != string::npos )
        {
            RedirectedFileOut = iterator->RedirectedFile;
            bRedirectToFileBuffer = iterator->bRedirectToBuffer;
            *pFilebufferOut = iterator->pBuffer;
            size = iterator->sizeBufferSize;
            bReadOnly = iterator->bBufferReadOnly;
            return true;
        }
	}

	return false;
}