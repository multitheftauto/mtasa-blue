#pragma once

#include <fstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>

typedef unsigned char      uchar;
typedef unsigned int       uint;
typedef unsigned short     ushort;
typedef unsigned long long uint64;

enum eIMGFileOperation
{
    IMG_FILE_READ = 0,
    IMG_FILE_WRITE
};

struct STXDImgArchiveInfo
{
    unsigned int   uiOffset;
    unsigned short usSize;
};

struct SIMGFileHeader
{
    char version[4];
    uint entryCount;
};

#pragma pack(push, 1)
struct EntryHeader
{
    uint   offset;
    ushort usSize;
    ushort fSize2;
    char   fileName[24];

    EntryHeader()
    {
        offset = 0;
        usSize = 0;
        fSize2 = 0;
        memset(fileName, 0, sizeof(EntryHeader::fileName));
    }

    EntryHeader(uint theOffset, ushort theSize, ushort theSize2, char* theFileName)
    {
        offset = theOffset;
        usSize = theSize;
        fSize2 = theSize2;
        strncpy_s(fileName, theFileName, std::min(sizeof(EntryHeader::fileName), strlen(theFileName) + 1));
    }
};
#pragma pack(pop)

struct CIMGArchiveFile
{
    EntryHeader    fileEntry;
    uint64         actualFileOffset;
    uint64         actualFileSize;
    unsigned char* pFileData;            // points to a location in m_vecImgArchiveFilesBuffer if fileByteBuffer size is zero,
                                         // otherwise this is nullptr
    CBuffer fileByteBuffer;              // If the size is zero, then pFileData is used

    CIMGArchiveFile()
    {
        actualFileOffset = 0;
        actualFileSize = 0;
        pFileData = nullptr;
    }

    unsigned char* GetData()
    {
        if (fileByteBuffer.GetSize() == 0)
        {
            return pFileData;
        }
        return (unsigned char*)fileByteBuffer.GetData();
    }
};

class CIMGArchive
{
public:
    CIMGArchive(int padding);
    CIMGArchive();
    ~CIMGArchive();

    CIMGArchive(const CIMGArchive&) {}
    CIMGArchive& operator=(const CIMGArchive&) {}

    std::vector<CIMGArchiveFile>* GetNextImgFiles(unsigned int imgReadWriteOperationSize);
    std::vector<CIMGArchiveFile>* GetAllImgFiles();
    uint                          GetFileCount();
    bool                          GetFileByID(uint id, CIMGArchiveFile& archiveFile);
    bool                          GetFileByName(std::string fileName, CIMGArchiveFile& archiveFile);
    bool                          GetFileByTXDImgArchiveInfo(STXDImgArchiveInfo* pTXDImgArchiveInfo, CIMGArchiveFile& archiveFile);

    void                      WriteEntries(std::vector<EntryHeader>& vecFinalOutputIMGEntries);
    void                      AppendArchiveFiles(std::vector<CIMGArchiveFile>& vecOutputIMGArchiveFiles, std::vector<EntryHeader>& vecFinalOutputIMGEntries);
    CIMGArchiveFile*          InsertArchiveFile(unsigned int actualFileSizeInBytes, unsigned int imgReadWriteOperationSizeInBytes);
    void                      AllocateSpace(size_t space);
    void                      FreeMemory();
    std::vector<EntryHeader>& GetArchiveDirEntries();

    bool         CreateTheFile(const SString& filePath, eIMGFileOperation fileOperation);
    void         CloseFile();
    unsigned int ReadEntries();
    void         FlushEntriesToFile();

private:
    eIMGFileOperation            m_fileOperation;
    std::ifstream                fileStream;
    std::ofstream                outputStream;
    std::string                  archiveFilePath_;
    unsigned int                 totalImgFilesRead;
    unsigned int                 totalIMGFilesWritten;
    unsigned int                 totalOutputDataSizeInBytes;
    std::vector<CIMGArchiveFile> imgArchiveFiles;
    std::vector<char>            m_vecImgArchiveFilesBuffer;
    std::vector<EntryHeader>     archiveFileEntries_;
};
