/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CDummySA.h
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CDummySA_
#define _CDummySA_

class CDummySAInterface : public CEntitySAInterface
{
public:
    CDummySAInterface( void );

    void*   operator new( size_t );
    void    operator delete( void *ptr );
};

#endif //_CDummySA_