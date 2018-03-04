#pragma once
#ifndef FILELOADER_H
#define FILELOADER_H

#include <Windows.h>
#include <iostream>
#include <fstream>

class FileLoader
{
private:

    const char * FilePath;
    FILE *       FilePointer;
    char *       FileDataBuffer;
    UINT         FileLength = 0;
    UINT         BytesReadFromBuffer = 0;

    void setLoaderFilePath(const char * FileToRead)
    {
        FilePath = FileToRead;
    }

    BOOL getFileHandle(void)
    {
        errno_t FileOpen = fopen_s(&FilePointer, FilePath, "rb");

        if (FileOpen != 0)
        {
            std::cout << "Failed to open file";
            return FALSE;
        }

        return TRUE;
    }

    BOOL getFileLength(void)
    {
        if (!FilePointer)
            return FALSE;

        if (fseek(FilePointer, 0, SEEK_END) != 0)
        {
            std::cout << "Failed to set cursor to end of file." << std::endl;
            return FALSE;
        }

        FileLength = ftell(FilePointer);
        if (FileLength == -1L)
        {
            std::cout << "Failed to get length of file (ftell)" << std::endl;
            return FALSE;
        }

        rewind(FilePointer);

        return TRUE;
    }

    void AllocateBufferMemory(void)
    {
        if (!FilePointer)
            return;

        FileDataBuffer = new char[FileLength + 1];
    }

    BOOL loadToMemory(void)
    {
        if (!FilePointer)
            return FALSE;

        UINT ReadSize = fread(FileDataBuffer, sizeof(char), FileLength, FilePointer);

        if (ReadSize != FileLength)
        {
            std::cout << "loadToMemory Failed. ReadSize != FileLength";
            return FALSE;
        }

        return TRUE;
    }


public:

    ~FileLoader(void)
    {
        unloadFile ();
    }

    FileLoader(void)
    {

    }

    BOOL createLoader(const char * FileToRead)
    {
        setLoaderFilePath(FileToRead);

        if (!getFileHandle() || !getFileLength())
            return FALSE;

        AllocateBufferMemory();

        FileDataBuffer[FileLength] = '\0';

        return TRUE;
    }

    BOOL loadFile(void)
    {
        if (!loadToMemory())
            return FALSE;

        std::cout << "Length: " << FileLength << std::endl;
        return TRUE;
    }

    // This function does not unload IFP data, to unload IFP call unloadIFP function
    void unloadFile(void)
    {
        if (FileDataBuffer != nullptr)
        {
            delete[] FileDataBuffer;
            fclose(FilePointer);

            FileDataBuffer = nullptr;
        } 
    }

    void PrintReadOffset(void)
    {
        std::cout << "Bytes read from buffer: " << BytesReadFromBuffer << std::endl;
    }


    template < class T >
    void readBuffer ( T * Destination )
    {
        const UINT ReadOffset = BytesReadFromBuffer;
        BytesReadFromBuffer += sizeof ( T );

        *Destination = *reinterpret_cast < T * > (FileDataBuffer + ReadOffset);

    }

    void readBytes(void * Destination, const UINT BytesToRead)
    {
        const UINT ReadOffset = BytesReadFromBuffer;
        BytesReadFromBuffer += BytesToRead;

        memcpy(Destination, FileDataBuffer + ReadOffset, BytesToRead);
    }

    std::string readString(UINT StringSizeInBytes)
    {
        std::string String;

        String.resize(StringSizeInBytes);

        for (UINT i = 0; i < StringSizeInBytes; i++)
        {
            const UINT ReadOffset = BytesReadFromBuffer;

            String[i] = FileDataBuffer[ReadOffset];
            BytesReadFromBuffer++;
        }
        return String;
    }

    void readCString (char * Destination, const UINT BytesToRead)
    {
        const UINT ReadOffset = BytesReadFromBuffer;
        BytesReadFromBuffer += BytesToRead;

        memcpy(Destination, FileDataBuffer + ReadOffset, BytesToRead);
        
        *(Destination + (BytesToRead - 1)) = '\0';
    }

    void skipBytes(UINT TotalBytesToSkip)
    {
        BytesReadFromBuffer += TotalBytesToSkip;
    }
};

#endif