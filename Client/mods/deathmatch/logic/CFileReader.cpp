/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CFileReader.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <fstream>

CFileReader::CFileReader()
{
    m_u32BytesReadFromBuffer = 0;
}

void CFileReader::FreeFileReaderMemory()
{
    SString().swap(m_buffer);
    m_u32BytesReadFromBuffer = 0;
}

void CFileReader::ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
    m_u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, m_buffer.data() + u32ReadOffset, u32BytesToRead);
}

void CFileReader::ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead)
{
    const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
    m_u32BytesReadFromBuffer += u32BytesToRead;
    memcpy(pDestination, m_buffer.data() + u32ReadOffset, u32BytesToRead);
    *(pDestination + (u32BytesToRead - 1)) = '\0';
}

void CFileReader::SkipBytes(const std::uint32_t u32BytesToSkip)
{
    m_u32BytesReadFromBuffer += u32BytesToSkip;
}

bool CFileReader::LoadFromFile(SString filePath) noexcept
{
    return FileLoad(std::nothrow, filePath, m_buffer);
}

bool CFileReader::LoadFromBuffer(SString buffer) noexcept
{
    m_buffer.swap(buffer);
    return true;
}
