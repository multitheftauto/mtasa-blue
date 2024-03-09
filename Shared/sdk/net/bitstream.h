/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/net/bitstream.h
 *  PURPOSE:     Network bitstream interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "../Common.h"
#include <string>
#include "SharedUtil.IntTypes.h"
#include "SharedUtil.Math.h"
#include "SharedUtil.Misc.h"
#include "SharedUtil.Logging.h"
#include <vector>
#ifndef WIN32
    #include <alloca.h>
#endif

#ifdef __cpp_lib_string_view
#include <string_view>
#endif

struct ISyncStructure;
class NetBitStreamInterface;

class NetBitStreamInterfaceNoVersion : public CRefCountable
{
public:
    virtual operator NetBitStreamInterface&() = 0;

    virtual int  GetReadOffsetAsBits() = 0;
    virtual void SetReadOffsetAsBits(int iOffset) = 0;

    virtual void Reset() = 0;
    virtual void ResetReadPointer() = 0;

    // Don't use this, it screws up randomly in certain conditions causing packet misalign
    virtual void Write(const unsigned char& input) = 0;
    virtual void Write(const char& input) = 0;
    virtual void Write(const unsigned short& input) = 0;
    virtual void Write(const short& input) = 0;
    virtual void Write(const unsigned int& input) = 0;
    virtual void Write(const int& input) = 0;
    virtual void Write(const float& input) = 0;
    virtual void Write(const double& input) = 0;
    virtual void Write(const char* input, int numberOfBytes) = 0;
    virtual void Write(const ISyncStructure* syncStruct) = 0;

public:            // Use char functions only when they will be 0 most times
    virtual void WriteCompressed(const unsigned char& input) = 0;
    virtual void WriteCompressed(const char& input) = 0;

public:
    virtual void WriteCompressed(const unsigned short& input) = 0;
    virtual void WriteCompressed(const short& input) = 0;
    virtual void WriteCompressed(const unsigned int& input) = 0;
    virtual void WriteCompressed(const int& input) = 0;

private:            // Float functions not used because they only cover -1 to +1 and are lossy
    virtual void WriteCompressed(const float& input) = 0;
    virtual void WriteCompressed(const double& input) = 0;

public:
    virtual void WriteBits(const char* input, unsigned int numbits) = 0;
    virtual void WriteBit(bool input) = 0;

    // Write a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12 bytes.  Will further compress y or z axis aligned vectors. Accurate to
    // 1/32767.5.
    virtual void WriteNormVector(float x, float y, float z) = 0;

    // Write a vector, using 10 bytes instead of 12. Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
    virtual void WriteVector(float x, float y, float z) = 0;

    // Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
    virtual void WriteNormQuat(float w, float x, float y, float z) = 0;

    // Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each for 6 bytes instead of 36
    virtual void WriteOrthMatrix(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) = 0;

    virtual bool Read(unsigned char& output) = 0;
    virtual bool Read(char& output) = 0;
    virtual bool Read(unsigned short& output) = 0;
    virtual bool Read(short& output) = 0;
    virtual bool Read(unsigned int& output) = 0;
    virtual bool Read(int& output) = 0;
    virtual bool Read(float& output) = 0;
    virtual bool Read(double& output) = 0;
    virtual bool Read(char* output, int numberOfBytes) = 0;
    virtual bool Read(ISyncStructure* syncStruct) = 0;

public:            // Use char functions only when they will be 0 most times
    virtual bool ReadCompressed(unsigned char& output) = 0;
    virtual bool ReadCompressed(char& output) = 0;

public:
    virtual bool ReadCompressed(unsigned short& output) = 0;
    virtual bool ReadCompressed(short& output) = 0;
    virtual bool ReadCompressed(unsigned int& output) = 0;
    virtual bool ReadCompressed(int& output) = 0;

private:            // Float functions not used because they only cover -1 to +1 and are lossy
    virtual bool ReadCompressed(float& output) = 0;
    virtual bool ReadCompressed(double& output) = 0;

public:
    virtual bool ReadBits(char* output, unsigned int numbits) = 0;
    virtual bool ReadBit() = 0;

    virtual bool ReadNormVector(float& x, float& y, float& z) = 0;
    virtual bool ReadVector(float& x, float& y, float& z) = 0;
    virtual bool ReadNormQuat(float& w, float& x, float& y, float& z) = 0;
    virtual bool ReadOrthMatrix(float& m00, float& m01, float& m02, float& m10, float& m11, float& m12, float& m20, float& m21, float& m22) = 0;
    // GetNumberOfBitsUsed appears to round up to the next byte boundary, when reading
    virtual int GetNumberOfBitsUsed() const = 0;
    virtual int GetNumberOfBytesUsed() const = 0;
    // GetNumberOfUnreadBits appears to round up to the next byte boundary, when reading
    virtual int GetNumberOfUnreadBits() const = 0;

    virtual void AlignWriteToByteBoundary() const = 0;
    virtual void AlignReadToByteBoundary() const = 0;

    virtual unsigned char* GetData() const = 0;

    // Force long types to use 4 bytes
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
    bool ReadCompressed(unsigned long& e)
    {
        uint temp;
        bool bResult = ReadCompressed(temp);
        e = temp;
        return bResult;
    }
    bool ReadCompressed(long& e)
    {
        int  temp;
        bool bResult = ReadCompressed(temp);
        e = temp;
        return bResult;
    }

    void Write(unsigned long e) { Write((uint)e); }
    void Write(long e) { Write((int)e); }
    void WriteCompressed(unsigned long e) { WriteCompressed((uint)e); }
    void WriteCompressed(long e) { WriteCompressed((int)e); }

#ifdef WIN_x64
    void Write(const size_t& input);
    void WriteCompressed(const size_t& input);
    bool Read(size_t& output);
    bool ReadCompressed(size_t& output);
#endif

    // Helper template methods that are not actually part
    // of the interface but get inline compiled.

    template <typename T>
    void WriteBits(T* input, unsigned int numbits)
    {
        WriteBits(reinterpret_cast<const char*>(input), numbits);
    }

    template <typename T>
    bool ReadBits(T* output, unsigned int numbits)
    {
        return ReadBits(reinterpret_cast<char*>(output), numbits);
    }

    // Read a single bit
    bool ReadBit(bool& output)
    {
        unsigned char ucTemp = 0;
        if (ReadBits(&ucTemp, 1))
        {
            output = ucTemp & 1;
            return true;
        }
        return false;
    }

    // Return true if enough bytes left in the bitstream
    bool CanReadNumberOfBytes(int iLength) const { return iLength >= 0 && iLength <= (GetNumberOfUnreadBits() + 7) / 8; }

    // For whatever stupid reason sdk/net gets included in Multiplayer SA and Game SA
    // And since those projects are c++14 we need this stuff.
    // TODO: Rip sdk/net out of these projects...
#ifdef __cpp_lib_string_view
    // Write characters in `value`
    void WriteStringCharacters(std::string_view value)
    {
        if (!value.empty())
            Write(value.data(), (int)value.length());
    }
    // Write `n` characters from `value`
    void WriteStringCharacters(std::string_view value, size_t n)
    {
        dassert(n <= value.length());
        if (n)
            Write(value.data(), (int)n);
    }

    // Write all characters in `value` (incl. length as `SizeType`)
    template <typename SizeType = unsigned short>
    void WriteString(std::string_view value)
    {
        // Write the length
        Write(static_cast<SizeType>(value.length()));

        // Write the characters
        return WriteStringCharacters(value);
    }

    // Write a string (incl. variable size header)
    void WriteStr(std::string_view value)
    {
        WriteLength(value.length());
        return WriteStringCharacters(value, value.length());
    }
#else
    // Write characters from a std::string
    void WriteStringCharacters(const std::string& value, uint uiLength)
    {
        dassert(uiLength <= value.length());
        // Send the data
        if (uiLength)
            Write(&value.at(0), uiLength);
    }

    // Write a string (incl. ushort size header)
    template <typename SizeType = unsigned short>
    void WriteString(const std::string& value)
    {
        // Write the length
        auto length = static_cast<SizeType>(value.length());
        Write(length);

        // Write the characters
        return WriteStringCharacters(value, length);
    }

    // Write a string (incl. variable size header)
    void WriteStr(const std::string& value)
    {
        WriteLength(value.length());
        return WriteStringCharacters(value, value.length());
    }
#endif

    // Read characters into a std::string
    bool ReadStringCharacters(std::string& result, uint uiLength)
    {
        result = "";
        if (uiLength)
        {
            if (!CanReadNumberOfBytes(uiLength))
                return false;
            // Read the data
            std::vector<char> bufferArray;
            bufferArray.resize(uiLength);
            char* buffer = &bufferArray[0];
            if (!Read(buffer, uiLength))
                return false;
            result = std::string(buffer, uiLength);
        }
        return true;
    }

    // Read a string (incl. ushort size header)
    template <typename SizeType = unsigned short>
    bool ReadString(std::string& result)
    {
        result = "";

        // Read the length
        SizeType length = 0;
        if (!Read(length))
            return false;

        // Read the characters
        return ReadStringCharacters(result, length);
    }

    // Write variable size length
    void WriteLength(uint uiLength)
    {
        if (uiLength <= 0x7F)            // One byte for length up to 127
            Write((uchar)uiLength);
        else if (uiLength <= 0x7EFF)
        {            // Two bytes for length from 128 to 32511
            Write((uchar)((uiLength >> 8) + 128));
            Write((uchar)(uiLength & 0xFF));
        }
        else
        {            // Five bytes for length 32512 and up
            Write((uchar)255);
            Write(uiLength);
        }
    }

    // Read variable size length
    bool ReadLength(uint& uiOutLength)
    {
        uiOutLength = 0;
        // Read the length
        uchar ucValue = 0;
        if (!Read(ucValue))
            return false;

        if (ucValue <= 0x7F)
        {            // One byte for length up to 127
            uiOutLength = ucValue;
        }
        else if (ucValue != 255)
        {            // Two bytes for length from 128 to 32511
            uchar ucValue2 = 0;
            if (!Read(ucValue2))
                return false;
            uiOutLength = ((ucValue - 128) << 8) + ucValue2;
        }
        else
        {            // Five bytes for length 32512 and up
            if (!Read(uiOutLength))
                return false;
        }
        return true;
    }

    // Read a string (incl. variable size header)
    bool ReadStr(std::string& result)
    {
        result = "";
        uint uiLength = 0;
        if (!ReadLength(uiLength))
            return false;
        return ReadStringCharacters(result, uiLength);
    }

    #ifdef MTA_CLIENT
        #define MAX_ELEMENTS    MAX_CLIENT_ELEMENTS
    #else
        #define MAX_ELEMENTS    MAX_SERVER_ELEMENTS
    #endif

    // Write an element ID
    void Write(const ElementID& ID)
    {
        static const unsigned int bitcount = NumberOfSignificantBits<(MAX_ELEMENTS - 1)>::COUNT;
        const unsigned int&       IDref = ID.Value();
        #ifdef MTA_CLIENT
        if (IDref != INVALID_ELEMENT_ID && IDref >= MAX_SERVER_ELEMENTS)
        {
            dassert("Sending client side element id to server" && 0);
            uint uiInvalidId = INVALID_ELEMENT_ID;
            WriteBits(reinterpret_cast<const unsigned char*>(&uiInvalidId), bitcount);
            return;
        }
        #endif
        WriteBits(reinterpret_cast<const unsigned char*>(&IDref), bitcount);
    }

    // Read an element ID
    bool Read(ElementID& ID)
    {
        static const unsigned int bitcount = NumberOfSignificantBits<(MAX_ELEMENTS - 1)>::COUNT;
        unsigned int&             IDref = ID.Value();
        IDref = 0;
        bool bResult = ReadBits(reinterpret_cast<unsigned char*>(&IDref), bitcount);

        // If max value, change to INVALID_ELEMENT_ID
        static const unsigned int maxValue = (1 << bitcount) - 1;
        if (IDref == maxValue)
            IDref = INVALID_ELEMENT_ID;

        return bResult;
    }
};

// eBitStreamVersion allows us to track what BitStream version is being used without placing magic numbers everywhere.
// It also helps us know what code branches can be removed when we increment a major version of MTA.
// Make sure you only add new items to the end of the list, above the "Latest" entry.
enum class eBitStreamVersion : unsigned short
{
    Unk = 0x062,

    //
    // 1.5.0 UNSTABLE - 2015-01-17
    //

    // Add "quickstand" to setGlitchEnabled
    // 2015-07-13 0x063 209837dcdc30d267519abc12e1361a1d18cd1553
    QuickStandGlitch,

    //
    // 1.5.0 RC RELEASED - 2015-07-15
    //

    //
    // 1.5.1 RELEASED - 2015-11-05
    //

    // Update fix #9038 (bugged shotgun with bullet sync) to only work if all connected clients support it
    // 2015-10-17 0x064 edbc6d37a734914b7349c693edf9a087a5a78a3d
    ShotgunDamageFix,

    //
    // 1.5.2 RELEASED - 2016-01-24
    //

    // Add blend parameter to setPedAnimation (#62)
    // 2016-09-05 0x065 f51983c3e3385b4de8d754e11efe329acaee9301
    SetPedAnimation_Blend,

    // Update net module version
    // 2016-09-24 0x066 3de7e5bd2d425747617a24350f2974e02cddc6dc
    NetUpdate_0x09E,

    //
    // 1.5.3 RELEASED - 2016-10-20
    //

    // Fix player nametag unicode characters missing on player join
    // 2016-12-09 0x067 2e582453b476c1183bd9fae5363a7cffdb531834
    UnicodeNametags,

    // Add -1 parameter to setElementDimension (only to objects) (#111)
    // 2017-02-22 0x068 2e319aa823929360da9e1f48c7eb233f1d6f29e5
    DimensionOmnipresence,

    // Add support for more special detections
    // 2017-02-26 0x069 9b6187b3c2eaa655624254f8d83acb35b31243e7
    MoreSpecialDetections_Nice69,

    // Add option to enable fakelag command for testing sync issues
    // 2017-03-08 0x06A a99fa0afa3b55e84f15aed335ab542520f39126d
    FakeLagCommand,

    //
    // 1.5.4 RELEASED - 2017-04-17
    //

    // Add player element for onClientChatMessage (#138)
    // 2017-07-04 0x06B 8c7095599c6d54784692bf93a1e6c7f56392c323
    OnClientChatMessage_PlayerSource,

    //
    // 1.5.5 RELEASED - 2017-08-07
    //

    // Add bShallow argument for server-side water as well (#240)
    // 2018-08-05 0x06C 1321b538559efe6d70deb5b784c2d392d52658f5
    Water_bShallow_ServerSide,

    //
    // 1.5.6 RELEASED - 2018-09-07
    // 1.5.7 RELEASED - 2019-08-31
    //

    // Add option to disable spawning components by setVehicleDoorState
    // 2019-10-11 0x06D e79d97195439f70ac66ece1859152b4c4896af31
    SetVehicleDoorState_SpawnFlyingComponent,

    // Increment BitStream version for Discord update (#1330)
    // 2020-03-27 0x06E a0ce68f284487ba636e839b06c103bc2442d95e0
    Discord_InitialImplementation,

    // Add analog control sync for accelerate and brake_reverse (#1164)
    // 2020-04-02 0x06F 41e36cc67520dded2a5203727a726c4261c65e31
    AnalogControlSync_AccelBrakeReverse,

    //
    // 1.5.8 RELEASED - 2020-10-11
    //

    // setWaterLevel: add bIncludeWorldSeaLevel and bIncludeOutsideWorldLevel
    // 2020-11-03 0x70
    SetWaterLevel_ChangeOutsideWorldLevel,

    // Implement entering/exiting/jacking for peds #1748
    // 2020-11-10 0x71
    PedEnterExit,

    // Add height for colpolygon (#1908)
    // 2021-01-16 0x72
    SetColPolygonHeight,

    // Support for vehicle blow without explosion and blow state synchronisation
    // 2021-02-26 0x73
    VehicleBlowStateSupport,

    // Implement messageType parameter to onClientChatMessage (#1020)
    // 2021-05-15 0x74
    OnClientChatMessage_MessageType,

    // Add serverside event "onPlayerResourceStart" (#2150)
    // 2021-08-30 0x75
    OnPlayerResourceStart,

    //
    // 1.5.9 RELEASED - 2021-10-01
    //

    // Remove "old" Discord implementation (#2499)
    // 2022-01-16 0x76
    Discord_Cleanup,

    //
    // 1.6.0 RELEASED - 2023-04-07
    //

    CEntityAddPacket_ObjectBreakable,

    // Add serverside setWorldSpecialPropertyEnabled
    // 2023-08-17
    WorldSpecialProperties,

    // Add "fireballdestruct" to setWorldSpecialPropertyEnabled
    // 2023-09-09
    WorldSpecialProperty_FireballDestruct,

    // Send server name to player in CPlayerJoinCompletePacket
    // 2023-10-12
    CPlayerJoinCompletePacket_ServerName,

    // Add serverside isPedReloadingWeapon
    // 2024-01-16
    CPed_isReloadingWeapon,

    // This allows us to automatically increment the BitStreamVersion when things are added to this enum.
    // Make sure you only add things above this comment.
    Next,
    Latest = Next - 1,
};

class NetBitStreamInterface : public NetBitStreamInterfaceNoVersion
{
    NetBitStreamInterface(const NetBitStreamInterface&);
    const NetBitStreamInterface& operator=(const NetBitStreamInterface&);

protected:
    NetBitStreamInterface() { DEBUG_CREATE_COUNT("NetBitStreamInterface"); }
    virtual ~NetBitStreamInterface() { DEBUG_DESTROY_COUNT("NetBitStreamInterface"); }

public:
    virtual                operator NetBitStreamInterface&() { return *this; }
    virtual unsigned short Version() const = 0;

    bool Can(eBitStreamVersion query) { return static_cast<eBitStreamVersion>(Version()) >= query; }
};

// Interface for all sync structures
struct ISyncStructure
{
    virtual ~ISyncStructure() {}
    virtual bool Read(NetBitStreamInterface& bitStream) = 0;
    virtual void Write(NetBitStreamInterface& bitStream) const = 0;
};
