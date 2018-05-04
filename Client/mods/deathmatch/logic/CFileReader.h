/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CFileReader.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef CFILEREADER_H
#define CFILEREADER_H
#pragma once

#include <cstdint>

class CFileReader
{
public:
    enum eIFSTREAM
    {
        SIZE_ERROR = -1
    };

    CFileReader(void);
    bool LoadFileToMemory(const SString& strFilePath);
    // Do not call any file reader functions after calling this function
    void FreeFileReaderMemory(void);

    template <class T>
    void ReadBuffer(T* pDestination)
    {
        const std::uint32_t u32ReadOffset = m_u32BytesReadFromBuffer;
        m_u32BytesReadFromBuffer += sizeof(T);
        *pDestination = *reinterpret_cast<T*>(&m_vecFileDataBuffer[u32ReadOffset]);
    }

    void ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead);
    void ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead);
    void SkipBytes(const std::uint32_t u32BytesToSkip);

private:
    std::vector<char> m_vecFileDataBuffer;
    std::uint32_t     m_u32BytesReadFromBuffer;
};

#endif
