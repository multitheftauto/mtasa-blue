/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDisplayManager.cpp
*  PURPOSE:     Text display manager class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

class CClientDisplayManager;

#ifndef __CClientDisplayManager_H
#define __CClientDisplayManager_H

#include "CClientManager.h"
#include <list>
using namespace std;

class CClientDisplay;
class CClientDisplayGroup;

class CClientDisplayManager
{
    friend CClientManager;
    friend CClientDisplay;

public:
                                    CClientDisplayManager           ( void );
                                    ~CClientDisplayManager          ( void );

    void                            DoPulse                         ( void );

    inline unsigned int             Count                           ( void )        { return static_cast < unsigned int > ( m_List.size () ); };
    CClientDisplay*                 Get                             ( unsigned long ulID );

    void                            DrawText2D                      ( const char* szCaption, const CVector& vecPosition, float fScale = 1.0f, RGBA rgbaColor = 0xFFFFFFFF );
    void                            DrawText3D                      ( const char* szCaption, const CVector& vecPosition, float fScale = 1.0f, RGBA rgbaColor = 0xFFFFFFFF );

    void                            RemoveAll                       ( void );


    void                            AddToList                       ( CClientDisplay* pDisplay );
    void                            RemoveFromList                  ( CClientDisplay* pDisplay );
    void                            RemoveFromList                  ( CClientDisplayGroup* pGroup );

    list < CClientDisplay* >        m_List;
    list < CClientDisplayGroup* >   m_Groups;
    bool                            m_bCanRemoveFromList;
};

#endif
