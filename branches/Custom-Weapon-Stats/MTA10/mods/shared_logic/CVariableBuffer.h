/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CVariableBuffer.h
*  PURPOSE:     Variable buffer class header
*  DEVELOPERS:  Derek Abdine <>
*
*****************************************************************************/

#ifndef __CVARIABLEBUFFER_H
#define __CVARIABLEBUFFER_H

class CVariableBuffer
{
public:
                            CVariableBuffer ( int nInitialCapacity );
                           ~CVariableBuffer ( );
    void                    add             ( const unsigned char * pbData, unsigned int len );
    void                    remove          ( unsigned int len );
    unsigned int            size            ( );
    const unsigned char*    get             ( );
    void                    clear           ( );
private:
    unsigned char*  m_pBuffer;
    unsigned int    m_nLen;
    unsigned int    m_nCapacity;
};

#endif
