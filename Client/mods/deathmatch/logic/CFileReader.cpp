#include <StdInc.h>
#include <fstream>

CFileReader::CFileReader(void)
{
    u32BytesReadFromBuffer = 0;
}

void CFileReader::FreeFileReaderMemory(void)
{
    std::vector<char>().swap( vecFileDataBuffer );
    u32BytesReadFromBuffer = 0;
}

void CFileReader::ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = u32BytesReadFromBuffer;
    u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, &vecFileDataBuffer[u32ReadOffset], u32BytesToRead);
}

void CFileReader::ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = u32BytesReadFromBuffer;
    u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, &vecFileDataBuffer[u32ReadOffset], u32BytesToRead);
    *(pDestination + (u32BytesToRead - 1)) = '\0';
}

void CFileReader::SkipBytes(const std::uint32_t u32BytesToSkip)
{
    u32BytesReadFromBuffer += u32BytesToSkip;
}

bool CFileReader::LoadFileToMemory(const SString& strFilePath)
{
    std::ifstream fileStream(strFilePath, std::ios::binary | std::ios::ate); 
    std::streamsize iFileSize = fileStream.tellg();
    if (iFileSize == eIFSTREAM::SIZE_ERROR)
    {
        return false;
    }

    fileStream.seekg(0, std::ios::beg);
    vecFileDataBuffer.reserve (static_cast < size_t > ( iFileSize ));
    if (fileStream.read(vecFileDataBuffer.data(), iFileSize))
    {
        return true;
    }
    return false;
}
