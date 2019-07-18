#include "StdInc.h"
#include "CIMGArchive.h"

CIMGArchive::CIMGArchive(std::string archiveFilePath, eIMGFileOperation fileOperation)
{
    archiveFilePath_ = archiveFilePath;
    totalImgFilesRead = 0;
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
    for (size_t i = 0; i < dwTotalEntries; i++)
    {
        CIMGArchiveFile* pArchiveFile = imgEntries[i];
        DWORD            fileBlockSize = pArchiveFile->fileEntry.usSize;
        vecEntryHeaders.emplace_back(currentBlockOffset, fileBlockSize, 0, pArchiveFile->fileEntry.fileName);
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

std::vector<CIMGArchiveFile>& CIMGArchive::GetNextImgFiles(unsigned int imgReadWriteOperationSizeInBytes)
{
    // Free the container memory
    std::vector<CIMGArchiveFile>().swap(imgArchiveFiles);
    std::vector<char>().swap(m_vecImgArchiveFilesBuffer);

    fileStream.seekg(archiveFileEntries_[totalImgFilesRead].offset * 2048, std::ios::beg);

    unsigned int bytesToRead = 0;
    unsigned int filesToRead = 0;
    size_t       entryHeaderIndex = totalImgFilesRead;
    while (true)
    {
        EntryHeader& entryHeader = archiveFileEntries_[entryHeaderIndex];
        unsigned int actualFileSize = entryHeader.usSize * 2048;
        bytesToRead += actualFileSize;

        if (bytesToRead > imgReadWriteOperationSizeInBytes)
        {
            bytesToRead -= actualFileSize;
            break;
        }

        filesToRead++;
        entryHeaderIndex++;
    }

    m_vecImgArchiveFilesBuffer.resize(bytesToRead);
    imgArchiveFiles.resize(filesToRead);

    fileStream.read(m_vecImgArchiveFilesBuffer.data(), bytesToRead);

    unsigned char* pFilesData = (unsigned char*)m_vecImgArchiveFilesBuffer.data();
    for (unsigned int i = 0; i < filesToRead; i++)
    {
        size_t entryHeaderIndex = totalImgFilesRead + i;
        EntryHeader& entryHeader = archiveFileEntries_[entryHeaderIndex];
        unsigned int actualFileSize = entryHeader.usSize * 2048;

        CIMGArchiveFile& archiveFile = imgArchiveFiles[i];
        archiveFile.fileEntry = entryHeader;
        archiveFile.actualFileOffset = entryHeader.offset * 2048;
        archiveFile.actualFileSize = actualFileSize;
        archiveFile.pFileData = pFilesData;

        pFilesData += actualFileSize;
    }

    totalImgFilesRead += filesToRead;

    return imgArchiveFiles;
}


bool CIMGArchive::GetFileByID(uint id, CIMGArchiveFile& archiveFile)
{
    if (archiveFileEntries_.size() <= id || id < 0)
    {
        return false;
    }
    else
    {
        if (!fileStream.fail())
        {
            archiveFile.fileEntry = archiveFileEntries_[id];
            archiveFile.actualFileOffset = archiveFileEntries_[id].offset * 2048;
            archiveFile.actualFileSize = archiveFileEntries_[id].usSize * 2048;
            archiveFile.fileByteBuffer.SetSize((size_t)archiveFile.actualFileSize);
            fileStream.seekg(archiveFile.actualFileOffset, std::ios::beg);
            char* pData = archiveFile.fileByteBuffer.GetData();
            fileStream.read(pData, archiveFile.actualFileSize);
            return true;
        }
        return false;
    }
}


bool CIMGArchive::GetFileByName(std::string fileName, CIMGArchiveFile& archiveFile)
{
    for (size_t i = 0; i < archiveFileEntries_.size(); i++)
    {
        if ((std::string)archiveFileEntries_[i].fileName == fileName)
        {
            if (!fileStream.fail())
            {
                archiveFile.fileEntry = archiveFileEntries_[i];
                archiveFile.actualFileOffset = archiveFileEntries_[i].offset * 2048;
                archiveFile.actualFileSize = archiveFileEntries_[i].usSize * 2048;
                archiveFile.fileByteBuffer.SetSize((size_t)archiveFile.actualFileSize);
                fileStream.seekg(archiveFile.actualFileOffset, std::ios::beg);
                char* pData = archiveFile.fileByteBuffer.GetData();
                fileStream.read(pData, archiveFile.actualFileSize);
                return true;
            }
            return false;
        }
    }
    return false;
}

bool CIMGArchive::GetFileByTXDImgArchiveInfo(STXDImgArchiveInfo* pTXDImgArchiveInfo, CIMGArchiveFile& archiveFile)
{
    if (!fileStream.fail())
    {
        archiveFile.actualFileOffset = pTXDImgArchiveInfo->uiOffset * 2048;
        archiveFile.actualFileSize = pTXDImgArchiveInfo->usSize * 2048;
        archiveFile.fileByteBuffer.SetSize((size_t)archiveFile.actualFileSize);
        fileStream.seekg(archiveFile.actualFileOffset, std::ios::beg);
        char* pData = archiveFile.fileByteBuffer.GetData();
        fileStream.read(pData, archiveFile.actualFileSize);
        return true;
    }
    return false;
}
