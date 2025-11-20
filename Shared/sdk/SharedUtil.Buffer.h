/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Buffer.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <vector>
#include "SharedUtil.IntTypes.h"
#include "SharedUtil.Misc.h"
#include "SharedUtil.File.h"
#include "SString.h"
#if defined(MTA_CLIENT) && defined(__cplusplus)
    #include "CrashTelemetry.h"
#endif

namespace SharedUtil
{
    //
    // Heap memory which is auto-deleted on scope exit
    // Designed as a replacement for alloca(), so allocated size is in bytes
    //
    template <class T>
    class CScopeAlloc
    {
        std::vector<char> buffer;
        bool m_bAllocFailed;

    public:
        CScopeAlloc(size_t sizeInBytes) : m_bAllocFailed(false)
        {
            try
            {
                buffer.resize(sizeInBytes);
            }
            catch (const std::exception&)
            {
                m_bAllocFailed = true;
            }
        }

        void resize(size_t newSizeInBytes)
        {
            try
            {
                buffer.resize(newSizeInBytes);
            }
            catch (const std::exception&)
            {
                m_bAllocFailed = true;
            }
        }

        bool AllocationFailed() const { return m_bAllocFailed; }

        operator T*() { return (!buffer.empty() && !m_bAllocFailed) ? reinterpret_cast<T*>(buffer.data()) : nullptr; }
    };

    // Assuming compiled on little endian machine
    #define CBUFFER_LITTLE_ENDIAN
    // #define CBUFFER_BIG_ENDIAN

    //////////////////////////////////////////////////////
    //
    // class CBuffer
    //
    // Wrapped byte array
    //
    //////////////////////////////////////////////////////
    class CBuffer : protected std::vector<char>
    {
    public:
        CBuffer() {}
        CBuffer(const void* pData, uint uiSize) 
        { 
            if (pData != nullptr && uiSize > 0)
                AddBytes(pData, uiSize, 0);
        }

        void ZeroClear() { std::fill(begin(), end(), 0); }
        void Clear() { clear(); }

        bool IsEmpty() const { return empty(); }

        void Reserve(uint uiSize)
        {
    #if defined(MTA_CLIENT) && defined(__cplusplus)
            CrashTelemetry::Scope telemetryScope(uiSize, this, "SharedUtil::CBuffer::Reserve", nullptr);
    #endif
            try
            {
                reserve(uiSize);
            }
            catch (const std::exception&)
            {

            }
        }

        // Comparison
        bool operator==(const CBuffer& other) const { return size() == other.size() && std::equal(begin(), end(), other.begin()); }

        bool operator!=(const CBuffer& other) const { return !operator==(other); }

        // Status
        bool SetSize(uint uiSize, bool bZeroPad = false)
        {
    #if defined(MTA_CLIENT) && defined(__cplusplus)
            CrashTelemetry::Scope telemetryScope(uiSize,
                             this,
                             "SharedUtil::CBuffer::SetSize",
                             bZeroPad ? "ZeroPad" : "NoZeroPad");
    #endif
            uint uiOldSize = (uint)size();
            try
            {
                resize(uiSize);
            }
            catch (const std::exception&)
            {
                return false;
            }

            // Verify resize actually succeeded
            if (size() != uiSize)
                return false;  // Allocation failed

            if (bZeroPad && uiSize > uiOldSize && size() > 0)
            {
                char* pData = GetData(uiOldSize);
                if (pData != nullptr)
                    memset(pData, 0, uiSize - uiOldSize);
            }
            
            return true;  // Success
        }

        uint GetSize() const { return (uint)size(); }

        // Access
        char* GetData(uint uiOffset = 0) 
        { 
            return (uiOffset <= size()) ? (data() + uiOffset) : nullptr;
        }

        const char* GetData(uint uiOffset = 0) const 
        { 
            return (uiOffset <= size()) ? (data() + uiOffset) : nullptr;
        }

        // Joining
        CBuffer operator+(const CBuffer& other) const
        {
            CBuffer temp = *this;
            if (other.GetSize() > 0)
            {
                uint uiOldSize = temp.GetSize();
                uint uiOtherSize = other.GetSize();
                
                // Check for overflow before concatenation
                if (uiOldSize > UINT_MAX - uiOtherSize)
                    return CBuffer();  // Overflow would occur
                
                temp += other;

                if (temp.GetSize() != uiOldSize + uiOtherSize)
                {
                    return CBuffer();
                }
            }
            return temp;
        }

        CBuffer& operator+=(const CBuffer& other)
        {
            // Only append if other buffer has data
            if (other.GetSize() > 0)
                AddBytes(other.GetData(), other.GetSize(), GetSize());
            return *this;
        }

        // Splitting
        CBuffer Mid(int iOffset, int iSize) const
        {
            uint uiSize = GetSize();
            // Clamp offset and size with proper types
            uint uiOffset = (iOffset < 0) ? 0 : (uint)iOffset;
            if (uiOffset > uiSize)
                uiOffset = uiSize;
            
            uint uiMaxSize = uiSize - uiOffset;
            uint uiMidSize = (iSize < 0) ? 0 : ((uint)iSize > uiMaxSize ? uiMaxSize : (uint)iSize);
            
            // Zero-length slice is valid, but only copy if we have a buffer base
            if (uiSize > 0)
            {
                const char* pMidData = GetData(uiOffset);
                if (pMidData != nullptr && uiMidSize > 0)
                    return CBuffer(pMidData, uiMidSize);
            }
            return CBuffer();
        }

        CBuffer Head(uint uiAmount) const { return Mid(0, uiAmount); }

        CBuffer Tail(uint uiAmount) const
        {
            uint uiSize = GetSize();
            if (uiSize == 0)
                return CBuffer();  // Empty buffer, return empty tail
            uiAmount = std::min(uiAmount, uiSize);
            return Mid(uiSize - uiAmount, uiAmount);
        }

        // Util
        bool LoadFromFile(const char* szFilename) { return FileLoad(szFilename, *this); }
        bool SaveToFile(const char* szFilename) const
        {
            const char* pData = GetData();
            if (GetSize() > 0 && pData == nullptr)
                return false;
            return FileSave(szFilename, pData, GetSize());
        }

    protected:
        void AddBytes(const void* pData, uint uiLength, int iOffset, bool bToFromNetwork = false)
        {
            if (pData == nullptr || uiLength == 0)
                return;  // Gracefully handle null/empty input


            if (iOffset < 0)
                return;  // Invalid offset

            uint uiOffset = (uint)iOffset;

            // More room required? Check for overflow
            uint uiNewSize = uiOffset + uiLength;
            if (uiNewSize < uiOffset || uiNewSize < uiLength)
                return;

            if (uiNewSize > GetSize())
            {
                if (!SetSize(uiNewSize))
                    return;
            }

            char* pDest = GetData(uiOffset);
            if (pDest == nullptr)
                return;

#ifdef CBUFFER_LITTLE_ENDIAN
            if (bToFromNetwork)
            {
                pDest += uiLength;
                const char* pSrc = (const char*)pData;
                while (uiLength--)
                    *--pDest = *pSrc++;
            }
            else
#endif
                memcpy(pDest, pData, uiLength);
        }

        bool GetBytes(void* pData, uint uiLength, int iOffset, bool bToFromNetwork = false) const
        {

            if (iOffset < 0)
                return false;  // Invalid offset

            uint uiOffset = (uint)iOffset;

            // Not enough data to get? Check with proper types
            uint uiRequired = uiOffset + uiLength;
            if (uiRequired < uiOffset || uiRequired < uiLength)  // Overflow check
                return false;  // Overflow detected
            if (uiRequired > GetSize())
                return false;  // Not enough data

            // Zero-length read is always valid
            if (uiLength == 0)
                return true;

            // Non-zero read requires valid output buffer and buffer data
            if (pData == nullptr)
                return false;

            const char* pBase = GetData(uiOffset);
            if (pBase == nullptr)
                return false;

#ifdef CBUFFER_LITTLE_ENDIAN
            if (bToFromNetwork)
            {
                char*       pDest = (char*)pData;
                const char* pSrc = pBase + uiLength;
                while (uiLength--)
                    *pDest++ = *--pSrc;
            }
            else
#endif
                memcpy(pData, pBase, uiLength);
            return true;
        }

        friend class CBufferWriteStream;
        friend class CBufferReadStream;
    };

    //////////////////////////////////////////////////////
    //
    // class CBufferStream
    //
    // Buffer access
    //
    //////////////////////////////////////////////////////
    class CBufferStream
    {
    public:
        CBufferStream(bool bToFromNetwork) : m_iPos(0), m_uiVersion(0), m_bToFromNetwork(bToFromNetwork) {}
        void        Seek(int iPos) { m_iPos = Clamp(0, iPos, GetSize()); }
        int         Tell() const { return m_iPos; }
        virtual int GetSize() const = 0;
        bool        AtEnd(int iOffset = 0) const
        {
            // Safely check if at end without integer overflow
            int iCheckPos = Tell();
            if (iOffset < 0)
            {
                // Negative offset: check if Tell() would go negative
                if (iCheckPos < -iOffset)
                    return true;  // Would underflow
                iCheckPos += iOffset;
            }
            else
            {
                // Positive offset: check for positive overflow
                if (iCheckPos > INT_MAX - iOffset)
                    return true;  // Would overflow
                iCheckPos += iOffset;
            }
            return iCheckPos >= GetSize();
        }
        void        SetVersion(uint uiVersion) { m_uiVersion = uiVersion; }
        uint        Version() const { return m_uiVersion; }

    protected:
        int  m_iPos;
        int  m_uiVersion;
        bool m_bToFromNetwork;
    };

    //////////////////////////////////////////////////////
    //
    // class CBufferReadStream
    //
    // Used to stream bytes from a buffer
    //
    //////////////////////////////////////////////////////
    class CBufferReadStream : public CBufferStream
    {
        const CBuffer* pBuffer;

    public:
        CBufferReadStream(const CBuffer& source, bool bToFromNetwork = false) : CBufferStream(bToFromNetwork), pBuffer(&source) {}

        virtual int         GetSize() const { return pBuffer->GetSize(); }
        virtual const char* GetData() const { return pBuffer->GetData(); }

        // Return true if enough bytes left in the buffer
        bool CanReadNumberOfBytes(int iLength)
        {
            Seek(Tell());
            // Validate length is non-negative
            if (iLength < 0)
                return false;
            // Safe comparison: convert to unsigned for comparison
            uint uiLength = (uint)iLength;
            uint uiPos = (uint)Tell();
            uint uiSize = (uint)GetSize();
            // Check for underflow in size calculation
            if (uiSize < uiPos)
                return false;  // Corrupted state
            uint uiAvailable = uiSize - uiPos;
            return uiLength <= uiAvailable;
        }

        bool ReadBytes(void* pData, int iLength, bool bToFromNetwork = false)
        {
            if (iLength < 0)
                return false;

            // Validate pos
            Seek(Tell());

            if (!pBuffer->GetBytes(pData, (uint)iLength, Tell(), bToFromNetwork))
                return false;            // Not enough bytes left to fill request

            // Adjust pos
            Seek(Tell() + iLength);
            return true;
        }

        void Read(SString&);            // Not defined as it won't work
        void Read(CBuffer&);            // Not defined as it won't work
        bool ReadString(SString& result, bool bByteLength = false, bool bDoesLengthIncludeLengthOfLength = false)
        {
            result = "";

            // Get the length
            ushort usLength = 0;
            if (bByteLength)
            {
                uchar ucLength = 0;
                if (!Read(ucLength))
                    return false;
                usLength = ucLength;
            }
            else if (!Read(usLength))
                return false;

            if (bDoesLengthIncludeLengthOfLength && usLength)
            {
                uint uiSubtract = bByteLength ? 1 : 2;
                if (usLength >= uiSubtract)
                    usLength -= uiSubtract;
                else
                    usLength = 0;  // Prevent underflow
            }

            if (usLength)
            {
                // Check has enough data
                if (!CanReadNumberOfBytes((int)usLength))
                    return false;
                // Read the data
                CScopeAlloc<char> buffer(usLength);
                if (buffer.AllocationFailed())
                    return false;
                if (!ReadBytes(buffer, (int)usLength, false))
                    return false;

                result = std::string(buffer, usLength);
            }
            return true;
        }

        bool ReadBuffer(CBuffer& outResult)
        {
            outResult.Clear();

            // Get the length
            ushort usLength = 0;
            if (!Read(usLength))
                return false;

            uint uiLength = usLength;
            if (uiLength == 65535)
            {
                int iLength = 0;
                if (!Read(iLength))
                    return false;
                if (iLength < 0)
                    return false;  // Invalid length
                uiLength = (uint)iLength;
            }

            if (uiLength)
            {
                // Ensure length fits in int
                if (uiLength > INT_MAX)
                    return false;
                // Check has enough data
                if (!CanReadNumberOfBytes((int)uiLength))
                    return false;
                // Read the data
                if (!outResult.SetSize(uiLength))
                {
                    outResult.Clear();
                    return false;
                }
                if (!ReadBytes(outResult.GetData(), (int)uiLength, false))
                {
                    outResult.Clear();
                    return false;
                }
            }
            return true;
        }

        template <class T>
        bool Read(T& e)
        {
            size_t szSize = sizeof(e);
            // Ensure size fits in int and is reasonable
            if (szSize == 0 || szSize > INT_MAX)
                return false;
            return ReadBytes(&e, (int)szSize, m_bToFromNetwork);
        }

#if defined(ANY_x64) || defined(ANY_arm64)
        // Force all these types to use 4 bytes
        bool Read(unsigned long& e)
        {
            uint temp;
            bool bResult = Read(temp);
            e = temp;
            return bResult;
        }
        bool Read(long& e)
        {
            int  temp;
            bool bResult = Read(temp);
            e = temp;
            return bResult;
        }
    #ifdef WIN_x64
        bool Read(size_t& e)
        {
            uint temp;
            bool bResult = Read(temp);
            e = temp;
            return bResult;
        }
    #endif
#endif
    };

    //////////////////////////////////////////////////////
    //
    // class CBufferReadStream
    //
    // Used to stream bytes into a buffer
    //
    //////////////////////////////////////////////////////
    class CBufferWriteStream : public CBufferStream
    {
        CBuffer* pBuffer;

    public:
        CBufferWriteStream(CBuffer& source, bool bToFromNetwork = false) : CBufferStream(bToFromNetwork), pBuffer(&source) {}

        virtual int GetSize() const { return pBuffer->GetSize(); }

        void WriteBytes(const void* pData, int iLength, bool bToFromNetwork = false)
        {
            if (iLength < 0)
                return;
            
            // Validate pos
            Seek(Tell());
            
            // Add data
            pBuffer->AddBytes(pData, (uint)iLength, Tell(), bToFromNetwork);
            
            // Only advance position if allocation succeeded
            int iSizeAfter = (int)pBuffer->GetSize();
            if (iSizeAfter >= Tell() + iLength)
                Seek(Tell() + iLength);
            // else: allocation failed, stream position unchanged
        }

        void Write(const SString&);            // Not defined as it won't work
        void Write(const CBuffer&);            // Not defined as it won't work
        void WriteString(const SString& str, bool bByteLength = false, bool bDoesLengthIncludeLengthOfLength = false)
        {
            size_t length = str.length();
            ushort usLength = (length > 65535) ? 65535 : (ushort)length;
            if (bDoesLengthIncludeLengthOfLength && usLength)
            {
                uint uiAdd = bByteLength ? 1 : 2;
                if (usLength > 65535 - uiAdd)
                    usLength = 65535;
                else
                    usLength += (ushort)uiAdd;
            }

            int iPosBeforeLength = Tell();
            if (bByteLength)
                Write((uchar)usLength);
            else
                Write(usLength);

            // Verify length was written before writing string data
            if (Tell() == iPosBeforeLength)
                return;  // Write failed, don't write string data

            if (usLength)
            {
                // Use safe string access instead of .at() which can throw
                const char* pStr = str.c_str();
                if (pStr != nullptr)
                    WriteBytes(pStr, (int)usLength, false);
            }
        }

        void WriteBuffer(const CBuffer& inBuffer)
        {
            uint uiLength = (uint)inBuffer.GetSize();
            int iPosBeforeLength = Tell();
            
            if (uiLength > 65534)
            {
                Write((ushort)65535);
                // Verify first write succeeded
                if (Tell() == iPosBeforeLength)
                    return;
                Write(uiLength);
                // Verify second write succeeded
                if (Tell() <= iPosBeforeLength + 2)
                    return;
            }
            else
            {
                Write((ushort)uiLength);
                // Verify write succeeded
                if (Tell() == iPosBeforeLength)
                    return;
            }

            // Write the data
            if (uiLength)
            {
                const char* pData = inBuffer.GetData();
                if (pData != nullptr)
                    WriteBytes(pData, uiLength, false);
            }
        }

        template <class T>
        void Write(const T& e)
        {
            size_t szSize = sizeof(e);
            if (szSize > 0 && szSize <= INT_MAX)
                WriteBytes(&e, (int)szSize, m_bToFromNetwork);
        }

#if defined(ANY_x64) || defined(ANY_arm64)
        // Force all these types to use 4 bytes
        void Write(unsigned long e) { Write((uint)e); }
        void Write(long e) { Write((int)e); }
    #ifdef WIN_x64
        void Write(size_t e) { Write((uint)e); }
    #endif
#endif
    };

}            // namespace SharedUtil
