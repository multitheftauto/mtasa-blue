/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CFileReader.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <fstream>

CFileReader::CFileReader(void)
{
    m_u32BytesReadFromBuffer = 0;
}

void CFileReader::FreeFileReaderMemory(void)
{
    std::vector<char>().swap(m_vecFileDataBuffer);
    m_u32BytesReadFromBuffer = 0;
}

void CFileReader::ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
    m_u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, &m_vecFileDataBuffer[u32ReadOffset], u32BytesToRead);
}

void CFileReader::ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
    m_u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, &m_vecFileDataBuffer[u32ReadOffset], u32BytesToRead);
    *(pDestination + (u32BytesToRead - 1)) = '\0';
}

void CFileReader::SkipBytes(const std::uint32_t u32BytesToSkip)
{
    m_u32BytesReadFromBuffer += u32BytesToSkip;
}

bool CFileReader::LoadFileToMemory(const SString& strFilePath)
{
    std::ifstream   fileStream(FromUTF8(strFilePath), std::ios::binary | std::ios::ate);
    std::streamsize iFileSize = fileStream.tellg();
    if (iFileSize == eIFSTREAM::SIZE_ERROR)
    {
        return false;
    }

    fileStream.seekg(0, std::ios::beg);
    m_vecFileDataBuffer.reserve(static_cast<size_t>(iFileSize));
    if (fileStream.read(m_vecFileDataBuffer.data(), iFileSize))
    {
        return true;
    }
    return false;
}

bool CFileReader::LoadDataBufferToMemory(const SString& buffer)
{
    std::streamsize iBufferSize = buffer.size();
    if (iBufferSize == eIFSTREAM::SIZE_ERROR)
    {
        return false;
    }

    m_vecFileDataBuffer.reserve(static_cast<size_t>(iBufferSize));
    if (std::copy(buffer.begin(), buffer.end(), m_vecFileDataBuffer.data()))
    {
        return true;
    }
    return false;
}
