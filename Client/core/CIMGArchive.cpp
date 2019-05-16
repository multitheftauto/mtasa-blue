#include "StdInc.h"
#include "CIMGArchive.h"


CIMGArchive::CIMGArchive(std::string archiveFilePath, eIMGFileOperation fileOperation)
{
    CIMGArchiveFile_ = NULL;
    archiveFilePath_ = archiveFilePath;

    if (fileOperation == IMG_FILE_READ)
    {
        fopen_s(&CIMGArchiveFile_, &archiveFilePath_[0], "rb");
    }
    else if (fileOperation == IMG_FILE_WRITE)
    {
        //fopen_s(&CIMGArchiveFile_, &archiveFilePath_[0], "w");
    }
}

CIMGArchive::~CIMGArchive()
{
    archiveFileEntries_.clear();
    if (CIMGArchiveFile_ != NULL)
    {
        fclose(CIMGArchiveFile_);
    }
}

void CIMGArchive::ReadEntries()
{
    if (CIMGArchiveFile_ != NULL)
    {
        char ver[4];
        fread(ver, 1, 4, CIMGArchiveFile_);
        if (ver[0] == 'V' && ver[3] == '2')
        {
            uint entryCount;
            fread(&entryCount, sizeof(uint), 1, CIMGArchiveFile_);
            for (int i = 0; i < entryCount; i++)
            {
                EntryHeader newEntry;
                fread(&newEntry, 1, 32, CIMGArchiveFile_);

                //std::printf("READ: newEntry.fileName: %s\n", newEntry.fileName);
                archiveFileEntries_.push_back(newEntry);
            }
        }
    }
}

DWORD GetTotalBlockSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1); // Round up to block size
    return blockSize / IMG_BLOCK_SIZE;
}

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>

void CIMGArchive::WriteEntries(std::vector<CIMGArchiveFile*>& imgEntries)
{
    //if (CIMGArchiveFile_ != NULL)
    //{/
        DWORD dwTotalEntries = imgEntries.size();
        const char * version = "VER2";

        std::vector <EntryHeader> vecEntryHeaders;
        vecEntryHeaders.reserve(dwTotalEntries);

        std::ofstream outputStream(archiveFilePath_, std::ofstream::out | std::ios::binary | std::ios::trunc);

        using std::ofstream;
        using std::strerror;
        using std::cerr;
        

        //outputStream.open(archiveFilePath_, std::ofstream::out | std::ios::binary);

        if (outputStream.fail())
        {
            std::printf("WriteEntries: ostream failed to open: %s\n", strerror(errno));
        }

        outputStream.write((char*)version, 4);
        outputStream.write((char*)&dwTotalEntries, 4);

        // write the entry headers
        uint64_t currentBlockOffset = GetTotalBlockSize(8 + dwTotalEntries * sizeof(EntryHeader));
        for (int i = 0; i < dwTotalEntries; i++)
        {
            CIMGArchiveFile* pArchiveFile = imgEntries[i];
            DWORD fileBlockSize = pArchiveFile->fileEntry->fSize;
            vecEntryHeaders.emplace_back(currentBlockOffset, fileBlockSize, 0, pArchiveFile->fileEntry->fileName);
            currentBlockOffset += fileBlockSize;
        }


        outputStream.write((char*)vecEntryHeaders.data(), 32 * dwTotalEntries);


        // now write the buffers
        for (DWORD i = 0; i < dwTotalEntries; i++)
        {
            CIMGArchiveFile* pArchiveFile = imgEntries[i];
            DWORD actualFileOffset = vecEntryHeaders[i].offset * 2048;
            DWORD actualFileSize = vecEntryHeaders[i].fSize * 2048;

            outputStream.seekp(actualFileOffset, std::ios::beg);

            //std::printf("WriteEntries: actualFileSize = %d\n", actualFileSize);
            outputStream.write((char*)pArchiveFile->fileByteBuffer.GetData(), actualFileSize);
        }

        std::printf("WriteEntries: okay done writing\n");
    //}
}


uint CIMGArchive::GetFileCount()
{
    return archiveFileEntries_.size();
}

std::vector<EntryHeader> CIMGArchive::GetArchiveDirEntries()
{
    return archiveFileEntries_;
}

CIMGArchiveFile* CIMGArchive::GetFileByID(uint id)
{
    if (archiveFileEntries_.size() <= id || id < 0)
    {
        return NULL;
    }
    else
    {
        if (CIMGArchiveFile_ != NULL)
        {
            CIMGArchiveFile* newArchiveFile = new CIMGArchiveFile;
            newArchiveFile->fileEntry = &archiveFileEntries_[id];
            newArchiveFile->actualFileOffset = archiveFileEntries_[id].offset * 2048;
            newArchiveFile->actualFileSize = archiveFileEntries_[id].fSize * 2048;
            newArchiveFile->fileByteBuffer.SetSize((size_t)newArchiveFile->actualFileSize);
            fseek(CIMGArchiveFile_, newArchiveFile->actualFileOffset, SEEK_SET);
            // std::printf("archiveFileEntries_.size() = %d | id: %d | fileBYteBuffer Size: %d | actual file size: %d\n",
            //    archiveFileEntries_.size(), id, newArchiveFile->fileByteBuffer.size(), newArchiveFile->actualFileSize);
            char * pData = newArchiveFile->fileByteBuffer.GetData();
            fread(pData, 1, newArchiveFile->actualFileSize, CIMGArchiveFile_);
            return newArchiveFile;
        }
        return NULL;
    }
}

CIMGArchiveFile* CIMGArchive::GetFileByName(std::string fileName)
{
    for (int i = 0; i < archiveFileEntries_.size(); i++)
    {
        if ((std::string)archiveFileEntries_[i].fileName == fileName)
        {
            if (CIMGArchiveFile_ != NULL)
            {
                CIMGArchiveFile* newArchiveFile = new CIMGArchiveFile;
                newArchiveFile->fileEntry = &archiveFileEntries_[i];
                newArchiveFile->actualFileOffset = archiveFileEntries_[i].offset * 2048;
                newArchiveFile->actualFileSize = archiveFileEntries_[i].fSize * 2048;
                newArchiveFile->fileByteBuffer.SetSize(newArchiveFile->actualFileSize);
                fseek(CIMGArchiveFile_, newArchiveFile->actualFileOffset, SEEK_SET);
                fread(newArchiveFile->fileByteBuffer.GetData(), 1, newArchiveFile->actualFileSize, CIMGArchiveFile_);
                return newArchiveFile;
            }
            return NULL;
        }
    }
    return NULL;
}
