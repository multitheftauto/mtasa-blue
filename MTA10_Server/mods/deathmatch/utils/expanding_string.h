/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/utils/CTCPSocket.h
*  PURPOSE:     Expanding char class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef EXPANDING_CHAR_H
#define EXPANDING_CHAR_H

#define MAX_BUFFER_EXPAND_SIZE  5000
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

class expanding_char
{
private:
    char *      m_szBuffer;
    size_t      m_siBufferSize;
    size_t      m_siBufferExpandSize;
    size_t      m_siLength; // the actual number of bytes written
public:
    expanding_char(size_t siBufferExpandSize = 100)
    {
        if ( siBufferExpandSize > 0 && siBufferExpandSize < MAX_BUFFER_EXPAND_SIZE )
        {
            m_siBufferExpandSize = siBufferExpandSize;
            m_szBuffer = (char *)malloc(m_siBufferExpandSize);
           // printf ( "malloc: 0x%X\n", m_szBuffer );
            memset(m_szBuffer, 0, m_siBufferExpandSize);
	        m_siBufferSize = m_siBufferExpandSize;
            m_siLength = 0;
        }
    }

    ~expanding_char()
    {
        free ( m_szBuffer );
    }

    inline const char * GetValue()
    {
        return m_szBuffer;
    }

    inline char * GetValueCopy(bool terminate=true)
    {
        char * szCopy = new char[m_siLength + (terminate?1:0)];
        memcpy(szCopy, m_szBuffer, m_siLength);
        return szCopy;
    }

    inline size_t GetLength()
    {
        //return strlen(m_szBuffer);
        return m_siLength;
    }

    inline void SetCharacter(size_t position, char c)
    {
        if ( position < m_siLength )
            m_szBuffer[position] = c;
    }

    inline expanding_char & operator +=(char c)
    {
	    if ( m_siLength == m_siBufferSize - 1 )
	    {
		    m_siBufferSize += m_siBufferExpandSize;
		    m_szBuffer = (char *)realloc(m_szBuffer, m_siBufferSize);
           // printf ( "realloc: 0x%X\n", m_szBuffer );
		    //memset((char *)(int)m_szBuffer + ((int)m_siBufferSize - m_siBufferExpandSize), 0, m_siBufferExpandSize);
	    }
	    m_szBuffer[m_siLength] = c; 
        m_siLength++;
        return *this;
    }

    inline expanding_char & append (char * c, size_t length=NULL)
    {
        if ( length == 0 )
            length = strlen(c);
        size_t original_length = m_siLength;
        size_t iNewLength = length + m_siLength;
        if ( iNewLength > m_siBufferSize -1 )
        {
            int numberOfExpansions = static_cast < int > ( (floor((double)(iNewLength / m_siBufferExpandSize)) + 1)-(floor((double)(m_siLength/m_siBufferExpandSize))+1) );
            m_siBufferSize += numberOfExpansions * m_siBufferExpandSize;
            m_szBuffer = (char *)realloc(m_szBuffer, m_siBufferSize);
        }
        m_siLength += length;
        memcpy ( (void *)(m_szBuffer + original_length), c, length );
        return *this;
    }
};

#endif
