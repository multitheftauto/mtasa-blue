/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPtrNodeSA.h
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CPtrNodeSA_
#define _CPtrNodeSA_

class CPtrNodeSingleSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[8];
};

class CPtrNodeDoubleSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[12];
};

#endif //_CPtrNodeSA_