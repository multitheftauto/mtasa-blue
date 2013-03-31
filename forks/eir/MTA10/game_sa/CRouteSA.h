/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRouteSA.h
*  PURPOSE:     Route for navigation
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CRouteSA_
#define _CRouteSA_

class CPointRouteSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[100];
};

class CPatrolRouteSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[420];
};

class CNodeRouteSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[36];
};

#endif //_CRouteSA_