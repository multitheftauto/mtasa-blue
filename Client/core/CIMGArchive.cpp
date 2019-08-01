#include "StdInc.h"
#include "CIMGArchive.h"

CIMGArchive::CIMGArchive()
{
    totalImgFilesRead = 0;
    totalOutputDataSizeInBytes = 0;
}

CIMGArchive::~CIMGArchive()
{
    archiveFileEntries_.clear();
}

bool CIMGArchive::CreateTheFile(const SString& filePath, eIMGFileOperation fileOperation)
{
    totalOutputDataSizeInBytes = 0;
    archiveFilePath_ = filePath;
    m_fileOperation = fileOperation;
    if (fileOperation == IMG_FILE_READ)
    {
        fileStream.open(FromUTF8(archiveFilePath_), std::ios::binary | std::ios::ate);
        if (fileStream.fail())
        {
            printf("CreateTheFile failed: %s\n", filePath.c_str());
        }
        return fileStream.fail() == false;
    }
    else if (fileOperation == IMG_FILE_WRITE)
    {
        outputStream.open(archiveFilePath_, std::ofstream::out | std::ios::binary | std::ios::trunc);
        if (outputStream.fail())
        {
            printf("CreateTheFile failed: %s\n", filePath.c_str());
        }
        return outputStream.fail() == false;
    }
    return false;
}

void CIMGArchive::CloseFile()
{
    if (m_fileOperation == IMG_FILE_READ)
    {
        fileStream.close();
    }
    else
    {
        outputStream.close();
    }
}

unsigned int CIMGArchive::ReadEntries()
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
    return archiveFileEntries_.size();
}

DWORD GetTotalBlockSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1);            // Round up to block size
    return blockSize / IMG_BLOCK_SIZE;
}

void CIMGArchive::FlushEntriesToFile()
{
    if (imgArchiveFiles.size() <= 0)
    {
        return;
    }

    DWORD       dwTotalEntries = imgArchiveFiles.size();
    const char* version = "VER2";

    archiveFileEntries_.reserve(dwTotalEntries);

    using std::cerr;
    using std::strerror;

    outputStream.write((char*)version, 4);
    outputStream.write((char*)&dwTotalEntries, 4);

    // write the entry headers
    uint64_t currentBlockOffset = GetTotalBlockSize(8 + dwTotalEntries * sizeof(EntryHeader));
    for (size_t i = 0; i < dwTotalEntries; i++)
    {
        CIMGArchiveFile* pArchiveFile = &imgArchiveFiles[i];
        DWORD            fileBlockSize = pArchiveFile->fileEntry.usSize;
        archiveFileEntries_.emplace_back(currentBlockOffset, fileBlockSize, 0, pArchiveFile->fileEntry.fileName);
        currentBlockOffset += fileBlockSize;
    }

    outputStream.write((char*)archiveFileEntries_.data(), 32 * dwTotalEntries);

    // now write the buffers
    for (DWORD i = 0; i < dwTotalEntries; i++)
    {
        CIMGArchiveFile* pArchiveFile = &imgArchiveFiles[i];
        DWORD            actualFileOffset = archiveFileEntries_[i].offset * 2048;
        DWORD            actualFileSize = archiveFileEntries_[i].usSize * 2048;

        outputStream.seekp(actualFileOffset, std::ios::beg);

        // std::printf("WriteEntries: actualFileSize = %d\n", actualFileSize);
        outputStream.write((char*)pArchiveFile->GetData(), actualFileSize);
    }

    std::printf("WriteEntries: okay done writing\n");

    imgArchiveFiles.clear();
    std::vector<EntryHeader>().swap(archiveFileEntries_);
}

CIMGArchiveFile* CIMGArchive::InsertArchiveFile(unsigned int actualFileSizeInBytes, unsigned int imgReadWriteOperationSizeInBytes)
{
    if (actualFileSizeInBytes < imgReadWriteOperationSizeInBytes)
    {
        if ((totalOutputDataSizeInBytes + actualFileSizeInBytes) >= imgReadWriteOperationSizeInBytes)
        {
            return nullptr;
        }
    }

    totalOutputDataSizeInBytes += actualFileSizeInBytes;
    CIMGArchiveFile& archiveFile = imgArchiveFiles.emplace_back();
    return &archiveFile;
}

void CIMGArchive::AllocateSpace(size_t space)
{
    imgArchiveFiles.reserve(space);
}

void CIMGArchive::FreeMemory()
{
    std::vector<EntryHeader>().swap(archiveFileEntries_);
    std::vector<CIMGArchiveFile>().swap(imgArchiveFiles);
    std::vector<char>().swap(m_vecImgArchiveFilesBuffer);
}

std::vector<EntryHeader>& CIMGArchive::GetArchiveDirEntries()
{
    return archiveFileEntries_;
}

uint CIMGArchive::GetFileCount()
{
    return archiveFileEntries_.size();
}

std::vector<CIMGArchiveFile>* CIMGArchive::GetNextImgFiles(unsigned int imgReadWriteOperationSizeInBytes)
{
    if (totalImgFilesRead >= archiveFileEntries_.size())
    {
        return nullptr;
    }

    // Free the container memory
    std::vector<CIMGArchiveFile>().swap(imgArchiveFiles);
    std::vector<char>().swap(m_vecImgArchiveFilesBuffer);

    fileStream.seekg(archiveFileEntries_[totalImgFilesRead].offset * 2048, std::ios::beg);

    unsigned int bytesToRead = 0;
    unsigned int filesToRead = 0;
    size_t       entryHeaderIndex = totalImgFilesRead;            // 9
    while (true)
    {
        EntryHeader& entryHeader = archiveFileEntries_[entryHeaderIndex];
        unsigned int actualFileSize = entryHeader.usSize * 2048;
        bytesToRead += actualFileSize;
        filesToRead++;
        entryHeaderIndex++;

        if (bytesToRead > imgReadWriteOperationSizeInBytes)
        {
            bytesToRead -= actualFileSize;
            filesToRead--;
            entryHeaderIndex--;
            break;
        }

        if (entryHeaderIndex >= archiveFileEntries_.size())
        {
            entryHeaderIndex--;
            break;
        }
    }

    m_vecImgArchiveFilesBuffer.resize(bytesToRead);
    imgArchiveFiles.resize(filesToRead);

    fileStream.read(m_vecImgArchiveFilesBuffer.data(), bytesToRead);

    unsigned char* pFilesData = (unsigned char*)m_vecImgArchiveFilesBuffer.data();
    for (unsigned int i = 0; i < filesToRead; i++)
    {
        size_t       entryHeaderIndex = totalImgFilesRead + i;
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

    return &imgArchiveFiles;
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
