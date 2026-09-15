/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/MockBitStream.h
 *  PURPOSE:     In-memory NetBitStreamInterface for unit testing
 *
 *  Implements a standalone bit-level read/write buffer that satisfies the
 *  NetBitStreamInterface contract. This lets us test SyncStructure
 *  serialization round-trips without the real network dependencies.
 *
 *  IMPORTANT: This mock diverges from RakNet in several ways:
 *
 *  1. Bit ordering: WriteBits/ReadBits use LSB-first ordering within each
 *     byte, while RakNet uses MSB-first. Round-trip tests are self-consistent
 *     (write and read use the same ordering), but do NOT feed real captured
 *     RakNet data into this mock — it will silently produce wrong results.
 *
 *  2. WriteCompressed/ReadCompressed: The mock uses a simplified 1-flag-bit
 *     approach ("is upper half zero?"), while RakNet iterates byte-by-byte
 *     with multiple flag bits. Wire sizes differ between the two, but
 *     round-trip correctness is preserved for testing purposes.
 *
 *  3. NormVector compression uses a simplified algorithm that preserves
 *     direction and magnitude to within 1/32767.5 accuracy (matching the
 *     documented spec). The exact wire format is NOT compatible with RakNet.
 *
 *  These differences are acceptable because we only test round-trip
 *  serialization correctness, not wire-format compatibility.
 *
 *****************************************************************************/

#pragma once

#include <net/bitstream.h>
#include <cassert>
#include <cmath>
#include <cstring>
#include <vector>

class MockBitStream : public NetBitStreamInterface
{
public:
    MockBitStream() : m_usVersion(static_cast<unsigned short>(eBitStreamVersion::Latest)) {}
    explicit MockBitStream(unsigned short version) : m_usVersion(version) {}

    // ------- Offset / Reset -------
    int  GetReadOffsetAsBits() override { return m_iReadBit; }
    void SetReadOffsetAsBits(int iOffset) override { m_iReadBit = iOffset; }
    void Reset() override
    {
        m_data.clear();
        m_iWriteBit = 0;
        m_iReadBit = 0;
    }
    void ResetReadPointer() override { m_iReadBit = 0; }

    // ------- Write primitives -------
    void Write(const unsigned char& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const char& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const unsigned short& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const short& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const unsigned int& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const int& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const float& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const double& input) override { WriteBytes(&input, sizeof(input)); }
    void Write(const char* input, int numberOfBytes) override { WriteBytes(input, numberOfBytes); }

    // ISyncStructure delegation: call the struct's own Write method
    void Write(const ISyncStructure* syncStruct) override { syncStruct->Write(*this); }

    // ------- WriteCompressed -------
    // Simplified compression: differs from RakNet's byte-by-byte algorithm.
    // See file header for details. Only valid for round-trip testing.
    // Writes a 1 bit if the upper half is all zeros (and skips it), else writes 0 + full value.
    void WriteCompressed(const unsigned char& input) override { WriteCompressedT(input); }
    void WriteCompressed(const char& input) override { WriteCompressedT(reinterpret_cast<const unsigned char&>(input)); }
    void WriteCompressed(const unsigned short& input) override { WriteCompressedT(input); }
    void WriteCompressed(const short& input) override { WriteCompressedT(reinterpret_cast<const unsigned short&>(input)); }
    void WriteCompressed(const unsigned int& input) override { WriteCompressedT(input); }
    void WriteCompressed(const int& input) override { WriteCompressedT(reinterpret_cast<const unsigned int&>(input)); }
    void WriteCompressed(const float& input) override { Write(input); }
    void WriteCompressed(const double& input) override { Write(input); }

    // ------- Bit-level writing -------
    void WriteBits(const char* input, unsigned int numbits) override
    {
        if (numbits == 0)
            return;
        EnsureCapacity(m_iWriteBit + numbits);
        for (unsigned int i = 0; i < numbits; ++i)
        {
            // Read bit i from input (LSB-first byte order, LSB-first within each byte)
            int  byteIndex = i / 8;
            int  bitIndex = i % 8;
            bool bit = (input[byteIndex] >> bitIndex) & 1;
            SetBit(m_iWriteBit + i, bit);
        }
        m_iWriteBit += numbits;
    }

    void WriteBit(bool input) override
    {
        EnsureCapacity(m_iWriteBit + 1);
        SetBit(m_iWriteBit, input);
        ++m_iWriteBit;
    }

    // ------- NormVector: simplified compression -------
    // Documented: "at most 4 bytes + 3 bits instead of 12 bytes. Accurate to 1/32767.5"
    // We write each component as a 16-bit signed fixed-point value scaled by 32767.
    // This differs from RakNet's wire format but preserves the accuracy contract.
    void WriteNormVector(float x, float y, float z) override
    {
        // Normalize the input vector
        float len = std::sqrt(x * x + y * y + z * z);
        if (len > 0.0f)
        {
            x /= len;
            y /= len;
            z /= len;
        }
        // Write each component as 16-bit signed integer scaled by 32767
        auto  encode = [](float v) -> short { return static_cast<short>(Clamp(-32767.0f, v * 32767.0f, 32767.0f)); };
        short sx = encode(x);
        short sy = encode(y);
        short sz = encode(z);
        Write(sx);
        Write(sy);
        Write(sz);
    }

    // Low-accuracy vector: "10 bytes instead of 12"
    void WriteVector(float x, float y, float z) override
    {
        // Stub: just write raw floats. Neither sync structure uses this method.
        Write(x);
        Write(y);
        Write(z);
    }

    // Quaternion: "6 bytes + 4 bits instead of 16 bytes"
    void WriteNormQuat(float w, float x, float y, float z) override
    {
        // Stub: no sync structure uses this
        Write(w);
        Write(x);
        Write(y);
        Write(z);
    }

    // Orthogonal matrix: "3 components of quaternion in 2 bytes each"
    void WriteOrthMatrix(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) override
    {
        // Stub: no sync structure uses this
        Write(m00);
        Write(m01);
        Write(m02);
        Write(m10);
        Write(m11);
        Write(m12);
        Write(m20);
        Write(m21);
        Write(m22);
    }

    // ------- Read primitives -------
    bool Read(unsigned char& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(char& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(unsigned short& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(short& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(unsigned int& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(int& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(float& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(double& output) override { return ReadBytes(&output, sizeof(output)); }
    bool Read(char* output, int numberOfBytes) override { return ReadBytes(output, numberOfBytes); }

    // ISyncStructure delegation: call the struct's own Read method
    bool Read(ISyncStructure* syncStruct) override { return syncStruct->Read(*this); }

    // ------- ReadCompressed -------
    bool ReadCompressed(unsigned char& output) override { return ReadCompressedT(output); }
    bool ReadCompressed(char& output) override { return ReadCompressedT(reinterpret_cast<unsigned char&>(output)); }
    bool ReadCompressed(unsigned short& output) override { return ReadCompressedT(output); }
    bool ReadCompressed(short& output) override { return ReadCompressedT(reinterpret_cast<unsigned short&>(output)); }
    bool ReadCompressed(unsigned int& output) override { return ReadCompressedT(output); }
    bool ReadCompressed(int& output) override { return ReadCompressedT(reinterpret_cast<unsigned int&>(output)); }
    bool ReadCompressed(float& output) override { return Read(output); }
    bool ReadCompressed(double& output) override { return Read(output); }

    // ------- Bit-level reading -------
    bool ReadBits(char* output, unsigned int numbits) override
    {
        if (m_iReadBit + static_cast<int>(numbits) > m_iWriteBit)
            return false;
        // Zero the output first to avoid stale bits
        std::memset(output, 0, (numbits + 7) / 8);
        for (unsigned int i = 0; i < numbits; ++i)
        {
            bool bit = GetBit(m_iReadBit + i);
            if (bit)
            {
                int byteIndex = i / 8;
                int bitIndex = i % 8;
                output[byteIndex] |= (1 << bitIndex);
            }
        }
        m_iReadBit += numbits;
        return true;
    }

    bool ReadBit() override
    {
        if (m_iReadBit >= m_iWriteBit)
            return false;
        bool bit = GetBit(m_iReadBit);
        ++m_iReadBit;
        return bit;
    }

    // ------- NormVector read (matches our write format) -------
    bool ReadNormVector(float& x, float& y, float& z) override
    {
        short sx, sy, sz;
        if (!Read(sx) || !Read(sy) || !Read(sz))
            return false;
        x = sx / 32767.0f;
        y = sy / 32767.0f;
        z = sz / 32767.0f;
        return true;
    }

    bool ReadVector(float& x, float& y, float& z) override { return Read(x) && Read(y) && Read(z); }

    bool ReadNormQuat(float& w, float& x, float& y, float& z) override { return Read(w) && Read(x) && Read(y) && Read(z); }

    bool ReadOrthMatrix(float& m00, float& m01, float& m02, float& m10, float& m11, float& m12, float& m20, float& m21, float& m22) override
    {
        return Read(m00) && Read(m01) && Read(m02) && Read(m10) && Read(m11) && Read(m12) && Read(m20) && Read(m21) && Read(m22);
    }

    // ------- Size / position queries -------
    int GetNumberOfBitsUsed() const override { return m_iWriteBit; }
    int GetNumberOfBytesUsed() const override { return (m_iWriteBit + 7) / 8; }
    int GetNumberOfUnreadBits() const override { return m_iWriteBit - m_iReadBit; }

    // ------- Alignment -------
    // m_iWriteBit and m_iReadBit are mutable because the interface declares
    // these methods const (design quirk inherited from RakNet).
    void AlignWriteToByteBoundary() const override
    {
        if (m_iWriteBit % 8 != 0)
            m_iWriteBit = ((m_iWriteBit / 8) + 1) * 8;
    }
    void AlignReadToByteBoundary() const override
    {
        if (m_iReadBit % 8 != 0)
            m_iReadBit = ((m_iReadBit / 8) + 1) * 8;
    }

    // ------- Data access -------
    unsigned char* GetData() const override { return const_cast<unsigned char*>(m_data.data()); }
    unsigned short Version() const override { return m_usVersion; }

private:
    // ------- Internal helpers -------

    void EnsureCapacity(int totalBits)
    {
        int bytesNeeded = (totalBits + 7) / 8;
        if (static_cast<int>(m_data.size()) < bytesNeeded)
            m_data.resize(bytesNeeded, 0);
    }

    void SetBit(int bitPos, bool value)
    {
        int byteIdx = bitPos / 8;
        int bitIdx = bitPos % 8;
        if (value)
            m_data[byteIdx] |= (1 << bitIdx);
        else
            m_data[byteIdx] &= ~(1 << bitIdx);
    }

    bool GetBit(int bitPos) const
    {
        int byteIdx = bitPos / 8;
        int bitIdx = bitPos % 8;
        return (m_data[byteIdx] >> bitIdx) & 1;
    }

    void WriteBytes(const void* data, int numBytes) { WriteBits(static_cast<const char*>(data), numBytes * 8); }

    bool ReadBytes(void* data, int numBytes) { return ReadBits(static_cast<char*>(data), numBytes * 8); }

    // Compressed write: if upper half of bytes is all zeros, write 1 bit + lower half.
    // Otherwise write 0 bit + full value.
    template <typename T>
    void WriteCompressedT(const T& input)
    {
        constexpr int halfBytes = sizeof(T) / 2;
        if constexpr (sizeof(T) == 1)
        {
            // For single-byte types, just write the full byte
            WriteBits(reinterpret_cast<const char*>(&input), 8);
            return;
        }

        // Check if upper half is zero (big-endian: last halfBytes bytes on little-endian)
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&input);
        bool                 upperIsZero = true;
        for (int i = halfBytes; i < static_cast<int>(sizeof(T)); ++i)
        {
            if (bytes[i] != 0)
            {
                upperIsZero = false;
                break;
            }
        }

        if (upperIsZero)
        {
            WriteBit(true);
            WriteBits(reinterpret_cast<const char*>(&input), halfBytes * 8);
        }
        else
        {
            WriteBit(false);
            WriteBits(reinterpret_cast<const char*>(&input), sizeof(T) * 8);
        }
    }

    template <typename T>
    bool ReadCompressedT(T& output)
    {
        constexpr int halfBytes = sizeof(T) / 2;
        if constexpr (sizeof(T) == 1)
        {
            return ReadBits(reinterpret_cast<char*>(&output), 8);
        }

        output = 0;
        if (m_iReadBit >= m_iWriteBit)
            return false;

        bool upperIsZero = ReadBit();
        if (upperIsZero)
        {
            return ReadBits(reinterpret_cast<char*>(&output), halfBytes * 8);
        }
        else
        {
            return ReadBits(reinterpret_cast<char*>(&output), sizeof(T) * 8);
        }
    }

    std::vector<unsigned char> m_data;
    mutable int                m_iWriteBit = 0;
    mutable int                m_iReadBit = 0;
    unsigned short             m_usVersion;
};
