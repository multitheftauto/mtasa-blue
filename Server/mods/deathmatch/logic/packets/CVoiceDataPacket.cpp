/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.cpp
 *  PURPOSE:     Voice data packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVoiceDataPacket.h"
#include "CPlayer.h"

CVoiceDataPacket::CVoiceDataPacket() noexcept
{
    m_pBuffer = nullptr;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;

    AllocateBuffer(1024);
}

CVoiceDataPacket::CVoiceDataPacket(CPlayer* pPlayer, const std::uint8_t* pbSrcBuffer, std::uint16_t usLength) noexcept
{
    m_pBuffer = nullptr;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;

    SetSourceElement(pPlayer);
    SetData(pbSrcBuffer, usLength);
}

CVoiceDataPacket::~CVoiceDataPacket() noexcept
{
    DeallocateBuffer();
}

bool CVoiceDataPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    if (!m_pBuffer)
        return false;
    
    BitStream.Read(m_usActualDataLength);
    if (m_usActualDataLength)
    {
        BitStream.Read(reinterpret_cast<char*>(m_pBuffer), m_usActualDataLength <= m_usDataBufferSize ? m_usActualDataLength : m_usDataBufferSize);
    }
    return true;
}

bool CVoiceDataPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    if (!m_usActualDataLength)
        return false;

    // Write the source player
    ElementID ID = m_pSourceElement->GetID();
    BitStream.Write(ID);
    // Write the length as an std::uint16_t and then write the string
    BitStream.Write(m_usActualDataLength);
    BitStream.Write(reinterpret_cast<const char*>(m_pBuffer), m_usActualDataLength);
    return true;
}

void CVoiceDataPacket::AllocateBuffer(std::uint16_t usBufferSize) noexcept
{
    // Test to see if we already have an allocated buffer
    // that will hold the requested size.
    if (m_usDataBufferSize >= usBufferSize)
        return;

    // It's not... resize the buffer.
    if (m_pBuffer)
    {
        // Free the current buffer.
        delete[] m_pBuffer;
    }

    // Allocate new buffer.
    m_pBuffer = new std::uint8_t[usBufferSize];

    // Clear buffer.
    memset(m_pBuffer, 0, usBufferSize);

    // Save the new size.
    m_usDataBufferSize = usBufferSize;
}

void CVoiceDataPacket::DeallocateBuffer() noexcept
{
    if (!m_pBuffer)
        return;

    delete[] m_pBuffer;
    m_pBuffer = nullptr;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;
}

void CVoiceDataPacket::SetData(const std::uint8_t* pbSrcBuffer, std::uint16_t usLength) noexcept
{
    // Allocate new buffer.
    AllocateBuffer(usLength);

    // Copy in the data.
    if (m_pBuffer)
    {
        memcpy(m_pBuffer, pbSrcBuffer, usLength);
        m_usActualDataLength = usLength;
    }
}

const std::uint8_t* CVoiceDataPacket::GetData() const noexcept
{
    return m_pBuffer;
}

std::uint16_t CVoiceDataPacket::GetDataLength() const noexcept
{
    return m_usActualDataLength;
}
