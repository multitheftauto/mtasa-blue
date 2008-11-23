/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDisplayGroup.cpp
*  PURPOSE:     Text display grouping class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CClientDisplayGroup_H
#define __CClientDisplayGroup_H

#include <list>
using namespace std;

class CClientDisplay;

class CClientDisplayGroup
{
public:
                                            CClientDisplayGroup               ( void );
                                            ~CClientDisplayGroup              ( void );

    void                                    RemoveAll                               ( void );


protected:
    list < CClientDisplay * >               m_List;
    bool                                    m_bVisible;
};

#endif