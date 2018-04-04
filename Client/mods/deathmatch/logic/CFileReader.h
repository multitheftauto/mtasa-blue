#ifndef CFILEREADER_H
#define CFILEREADER_H

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
        const std::uint32_t u32ReadOffset = u32BytesReadFromBuffer;
        u32BytesReadFromBuffer += sizeof(T);
        *pDestination = *reinterpret_cast<T*>(&vecFileDataBuffer[u32ReadOffset]);
    }

    void        ReadBytes(void* pDestination, const std::uint32_t u32BytesToRead);
    void        ReadStringNullTerminated(char* pDestination, const std::uint32_t u32BytesToRead);
    void        SkipBytes(const std::uint32_t u32BytesToSkip);

private:
    std::vector<char> vecFileDataBuffer;
    std::uint32_t     u32BytesReadFromBuffer;
};

#endif
