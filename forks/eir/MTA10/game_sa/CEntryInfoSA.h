/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntryInfoSA.h
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CEntryInfoSA_
#define _CEntryInfoSA_

class CEntryInfoSA
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE            m_pad[20];
};

#endif //_CEntryInfoSA_