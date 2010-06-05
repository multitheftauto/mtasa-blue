/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVariableBuffer.cpp
*  PURPOSE:     Variable buffer class
*  DEVELOPERS:  Derek Abdine <>
*
*****************************************************************************/

#include <StdInc.h>

/**
 * Constructs a new object with the specified initial capacity.
 * 
 * @param nInitialCapacity The capacity of the starting buffer.
 */
CVariableBuffer::CVariableBuffer ( int nInitialCapacity )
{
    m_pBuffer = new unsigned char [ nInitialCapacity ];
    m_nLen = 0;
    m_nCapacity = nInitialCapacity;
}

/**
 * Standard dtor to cleanup on exit.
 */
CVariableBuffer::~CVariableBuffer ( )
{
    delete [] m_pBuffer;
}

/**
 * Adds the specified buffer bytes to the internal buffer.
 *
 * @param pbData The data stream to add.
 *
 * @param len The length of the data stream.
 */
void CVariableBuffer::add ( const unsigned char * pbData, unsigned int len )
{
    if ( ( m_nLen + len ) > m_nCapacity )
    {
        /* 
         * We don't want to keep having to renew buffers, so 
         * allocate more than enough if this happens.
         */
        unsigned int newCapacity = m_nLen + (len * 2);
        unsigned char* newBuff = new unsigned char [ newCapacity ];
        memcpy( newBuff, m_pBuffer, m_nLen );
        delete [] m_pBuffer;
        m_pBuffer = newBuff;
        m_nCapacity = newCapacity;
    }
    memcpy( (m_pBuffer+m_nLen), pbData, len );
    m_nLen += len;
}

/**
 * Removes the specified length of bytes from the internal buffer.
 * 
 * This method will not operate on the internal buffer's capacity.
 * 
 * @param len The length of bytes to remove.
 */
void CVariableBuffer::remove ( unsigned int len )
{
    if (len < m_nLen)
        m_nLen -= len;
    else
        m_nLen = 0;
}

/**
 * Retrieves this buffer.
 * 
 * @return The buffer.
 */
const unsigned char * CVariableBuffer::get ( )
{
    return m_pBuffer;
}

/**
 * Retrieves the buffer size.
 *
 * @return The buffer size.
 */
unsigned int CVariableBuffer::size ( )
{
    return m_nLen;
}

/**
 * Clears the buffer.
 * This function will not alter the buffer, but will
 * set the buffer size to zero.
 */
void CVariableBuffer::clear ( )
{
    m_nLen = 0;
}
