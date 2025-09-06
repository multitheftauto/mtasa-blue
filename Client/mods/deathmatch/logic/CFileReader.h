/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CFileReader.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

class CFileReader
{
public:
    enum eIFSTREAM
    {
        SIZE_ERROR = -1
    };

    CFileReader();

    bool LoadFromFile(SString filePath) noexcept;
    bool LoadFromBuffer(SString buffer) noexcept;

    // Do not call any file reader functions after calling this function
    void FreeFileReaderMemory();

    template <class T>
    void ReadBuffer(T* pDestination)
    {
        const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
        m_u32BytesReadFromBuffer += sizeof(T);
        *pDestination = *reinterpret_cast<T*>(m_buffer.data() + u32ReadOffset);
    }

    void ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead);
    void ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead);
    void SkipBytes(const std::uint32_t u32BytesToSkip);

    std::uint32_t GetRemainingBytesCount() const noexcept
    {
        if (static_cast<std::uint32_t>(m_buffer.size()) > m_u32BytesReadFromBuffer)
            return static_cast<std::uint32_t>(m_buffer.size()) - m_u32BytesReadFromBuffer;
        else
            return 0;
    }

private:
    SString       m_buffer;
    std::uint32_t m_u32BytesReadFromBuffer;
};
