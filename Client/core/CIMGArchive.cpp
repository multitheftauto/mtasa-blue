#include "StdInc.h"
#include "CIMGArchive.h"

CIMGArchive::CIMGArchive(std::string archiveFilePath, eIMGFileOperation fileOperation)
{
    archiveFilePath_ = archiveFilePath;

    if (fileOperation == IMG_FILE_READ)
    {
        fileStream.open(FromUTF8(archiveFilePath_), std::ios::binary | std::ios::ate);
    }
    else if (fileOperation == IMG_FILE_WRITE)
    {
    }
}

CIMGArchive::~CIMGArchive()
{
    archiveFileEntries_.clear();
}

void CIMGArchive::ReadEntries()
{
    if (!fileStream.fail())
    {
        fileStream.seekg(0, std::ios::beg);
        SIMGFileHeader imgHeader;
        fileStream.read((char*)&imgHeader, sizeof(SIMGFileHeader));
        if (imgHeader.version[0] == 'V' && imgHeader.version[3] == '2')
        {
            archiveFileEntries_.resize(imgHeader.entryCount);
            fileStream.read((char*)archiveFileEntries_.data(), sizeof(EntryHeader) * imgHeader.entryCount);
        }
    }
}

DWORD GetTotalBlockSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1);            // Round up to block size
    return blockSize / IMG_BLOCK_SIZE;
}

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>

void CIMGArchive::WriteEntries(std::vector<CIMGArchiveFile*>& imgEntries)
{
    DWORD       dwTotalEntries = imgEntries.size();
    const char* version = "VER2";

    std::vector<EntryHeader> vecEntryHeaders;
    vecEntryHeaders.reserve(dwTotalEntries);

    std::ofstream outputStream(archiveFilePath_, std::ofstream::out | std::ios::binary | std::ios::trunc);

    using std::cerr;
    using std::strerror;

    // outputStream.open(archiveFilePath_, std::ofstream::out | std::ios::binary);

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
        DWORD            fileBlockSize = pArchiveFile->fileEntry->usSize;
        vecEntryHeaders.emplace_back(currentBlockOffset, fileBlockSize, 0, pArchiveFile->fileEntry->fileName);
        currentBlockOffset += fileBlockSize;
    }

    outputStream.write((char*)vecEntryHeaders.data(), 32 * dwTotalEntries);

    // now write the buffers
    for (DWORD i = 0; i < dwTotalEntries; i++)
    {
        CIMGArchiveFile* pArchiveFile = imgEntries[i];
        DWORD            actualFileOffset = vecEntryHeaders[i].offset * 2048;
        DWORD            actualFileSize = vecEntryHeaders[i].usSize * 2048;

        outputStream.seekp(actualFileOffset, std::ios::beg);

        // std::printf("WriteEntries: actualFileSize = %d\n", actualFileSize);
        outputStream.write((char*)pArchiveFile->fileByteBuffer.GetData(), actualFileSize);
    }

    std::printf("WriteEntries: okay done writing\n");
}

std::vector<EntryHeader>& CIMGArchive::GetArchiveDirEntries()
{
    return archiveFileEntries_;
}

uint CIMGArchive::GetFileCount()
{
    return archiveFileEntries_.size();
}

CIMGArchiveFile* CIMGArchive::GetFileByID(uint id)
{
    if (archiveFileEntries_.size() <= id || id < 0)
    {
        return NULL;
    }
    else
    {
        if (!fileStream.fail())
        {
            CIMGArchiveFile* newArchiveFile = new CIMGArchiveFile;
            newArchiveFile->fileEntry = &archiveFileEntries_[id];
            newArchiveFile->actualFileOffset = archiveFileEntries_[id].offset * 2048;
            newArchiveFile->actualFileSize = archiveFileEntries_[id].usSize * 2048;
            newArchiveFile->fileByteBuffer.SetSize((size_t)newArchiveFile->actualFileSize);
            fileStream.seekg(newArchiveFile->actualFileOffset, std::ios::beg);
            char* pData = newArchiveFile->fileByteBuffer.GetData();
            fileStream.read(pData,newArchiveFile->actualFileSize);
            return newArchiveFile;
        }
        return NULL;
    }
}

/*
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
                newArchiveFile->actualFileSize = archiveFileEntries_[i].usSize * 2048;
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
*/

CIMGArchiveFile* CIMGArchive::GetFileByTXDImgArchiveInfo(STXDImgArchiveInfo* pTXDImgArchiveInfo)
{
    if (!fileStream.fail())
    {
        CIMGArchiveFile* newArchiveFile = new CIMGArchiveFile;
        newArchiveFile->fileEntry = nullptr;
        newArchiveFile->actualFileOffset = pTXDImgArchiveInfo->uiOffset * 2048;
        newArchiveFile->actualFileSize = pTXDImgArchiveInfo->usSize * 2048;
        newArchiveFile->fileByteBuffer.SetSize((size_t)newArchiveFile->actualFileSize);
        fileStream.seekg(newArchiveFile->actualFileOffset, std::ios::beg);
        char* pData = newArchiveFile->fileByteBuffer.GetData();
        fileStream.read(pData, newArchiveFile->actualFileSize);
        return newArchiveFile;
    }
    return NULL;
}
