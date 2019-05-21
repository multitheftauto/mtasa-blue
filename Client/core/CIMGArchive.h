#ifndef CIMGArchive_H
#define CIMGArchive_H

typedef unsigned char		                uchar;
typedef unsigned int		                uint;
typedef unsigned short		                ushort;
typedef unsigned long long	                uint64;

enum eIMGFileOperation
{
    IMG_FILE_READ = 0,
    IMG_FILE_WRITE
};

struct STXDImgArchiveInfo
{
    unsigned int uiOffset;
    unsigned short usSize;
};

#pragma pack(push, 1)
struct EntryHeader
{
    uint					offset;
    ushort					usSize;
    ushort					fSize2;
    char					fileName[24];

    EntryHeader() {}
    EntryHeader(uint theOffset, ushort theSize, ushort theSize2, char* theFileName)
    {
        offset = theOffset;
        usSize = theSize;
        fSize2 = theSize2;
        strncpy_s(fileName, theFileName, sizeof(EntryHeader::fileName));
    }
};
#pragma pack(pop)

struct CIMGArchiveFile
{
    EntryHeader*			fileEntry;
    uint64					actualFileOffset;
    uint64					actualFileSize;
    CBuffer		            fileByteBuffer;
};

class CIMGArchive
{
public:
    CIMGArchive(std::string archiveFilePath, eIMGFileOperation fileOperation);
    ~CIMGArchive();

    uint				  GetFileCount();
    CIMGArchiveFile*	  GetFileByID(uint id);
    CIMGArchiveFile*      GetFileByName(std::string fileName);
    CIMGArchiveFile*     GetFileByTXDImgArchiveInfo(STXDImgArchiveInfo* pTXDImgArchiveInfo);

    std::vector<EntryHeader>& GetArchiveDirEntries();
    void			      ReadEntries();
    void                  WriteEntries(std::vector<CIMGArchiveFile*>& imgEntries);
private:

    FILE * CIMGArchiveFile_;
    std::string			  archiveFilePath_;
    std::vector<EntryHeader> archiveFileEntries_;
};
#endif // CIMGArchive_H
