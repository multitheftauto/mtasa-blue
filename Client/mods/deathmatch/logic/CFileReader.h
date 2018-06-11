#pragma once
#ifndef CFILEREADER_H
#define CFILEREADER_H

#include <Windows.h>
#include <iostream>
#include <fstream>

class CFileReader
{

public:
                       CFileReader                ( void );
                       ~CFileReader               ( void );

    BOOL               CreateLoader               ( const char * szFileToRead );
    BOOL               LoadFile                   ( void );
    void               UnloadFile                 ( void );

    template < class T >
    void ReadBuffer ( T * pDestination )
    {
        const UINT u32ReadOffset = u32BytesReadFromBuffer;
        u32BytesReadFromBuffer += sizeof ( T );
        *pDestination = *reinterpret_cast < T * > ( pFileDataBuffer + u32ReadOffset );
    }

    void               ReadBytes                  ( void * pDestination, const UINT u32BytesToRead );
    std::string        ReadString                 ( UINT u32SizeInBytes );
    void               ReadCString                ( char * pDestination, const UINT u32BytesToRead );
    void               SkipBytes                  ( UINT u32BytesToSkip );

private:
    inline void        SetLoaderFilePath          ( const char * szFileToRead );
    BOOL               OpenFile                   ( void );
    BOOL               GetFileLength              ( void );
    void               AllocateBufferMemory       ( void );
    BOOL               LoadToMemory               ( void );

    const char *       pFilePath;
    FILE *             pFilePointer;
    char *             pFileDataBuffer;
    UINT               u32FileLength;
    UINT               u32BytesReadFromBuffer;
};

#endif
