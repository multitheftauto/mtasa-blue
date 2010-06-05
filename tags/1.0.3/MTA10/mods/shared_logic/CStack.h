/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CStack.h
*  PURPOSE:     Stack class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CSTACK_H
#define __CSTACK_H

#define INVALID_STACK_ID INVALID_ELEMENT_ID
#define MAX_STACK_SIZE MAX_CLIENT_ELEMENTS

template < typename T >
class CStack
{
public:
    inline CStack ( void )
    {
        m_ulBlipStackPosition = MAX_STACK_SIZE;
        m_usBlipIDStack [ 0 ] = INVALID_STACK_ID;

        for ( T i = 1; i < MAX_STACK_SIZE; ++i )
        {
            m_usBlipIDStack [ i ] = MAX_STACK_SIZE - i;
        }
    }

    inline T Pop ( void )
    {
        // Got any items? Pop off and return the first item
        if ( m_ulBlipStackPosition > 0 )
        {
            --m_ulBlipStackPosition;
            return m_usBlipIDStack [m_ulBlipStackPosition];
        }

        // No IDs left
        return INVALID_STACK_ID;
    }

    inline void Push ( T ID )
    {
        if ( m_ulBlipStackPosition < MAX_STACK_SIZE )
        {
            m_usBlipIDStack [m_ulBlipStackPosition] = ID;
            ++m_ulBlipStackPosition;
        }
    }

private:
    T               m_usBlipIDStack [MAX_STACK_SIZE];
    unsigned long   m_ulBlipStackPosition;
};

#endif
