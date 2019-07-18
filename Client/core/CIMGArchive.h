#ifndef CIMGArchive_H
#define CIMGArchive_H

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
        strncpy_s(fileName, theFileName, std::min(sizeof(EntryHeader::fileName) , strlen(theFileName) + 1));
    }
};
#pragma pack(pop)

struct CIMGArchiveFile
{
    EntryHeader    fileEntry;
    uint64         actualFileOffset;
    uint64         actualFileSize;
    unsigned char* pFileData;        // points to a location in m_vecImgArchiveFilesBuffer if fileByteBuffer size is zero,
                                     // otherwise this is nullptr
    CBuffer        fileByteBuffer;   // If the size is zero, then pFileData is used

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
    CIMGArchive(std::string archiveFilePath, eIMGFileOperation fileOperation);
    ~CIMGArchive();

    std::vector<CIMGArchiveFile>& GetNextImgFiles(unsigned int imgReadWriteOperationSize);
    uint                          GetFileCount();
    bool                          GetFileByID(uint id, CIMGArchiveFile& archiveFile);
    bool                          GetFileByName(std::string fileName, CIMGArchiveFile& archiveFile);
    bool                          GetFileByTXDImgArchiveInfo(STXDImgArchiveInfo* pTXDImgArchiveInfo, CIMGArchiveFile& archiveFile);

    std::vector<EntryHeader>& GetArchiveDirEntries();
    void                      ReadEntries();
    void                      WriteEntries(std::vector<CIMGArchiveFile*>& imgEntries);

private:
    std::ifstream                fileStream;
    std::string                  archiveFilePath_;
    unsigned int                 totalImgFilesRead;
    std::vector<CIMGArchiveFile> imgArchiveFiles;
    std::vector<char>            m_vecImgArchiveFilesBuffer;
    std::vector<EntryHeader>     archiveFileEntries_;
};
#endif            // CIMGArchive_H
