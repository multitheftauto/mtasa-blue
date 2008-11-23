/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColManager.h
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLMANAGER_H
#define __CCOLMANAGER_H

#include "CColShape.h"
#include <list>

using namespace std;

class CColManager
{
    friend class CColShape;

public:
                                            CColManager         ( void );
                                            ~CColManager        ( void );

    void                                    DoHitDetection      ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius, CElement* pElement, CColShape * pJustThis = NULL );

    bool                                    Exists              ( CColShape* pShape );
    void                                    DeleteAll           ( void );

    list < CColShape* > ::const_iterator    IterBegin           ( void )                    { return m_List.begin (); }
    list < CColShape* > ::const_iterator    IterEnd             ( void )                    { return m_List.end (); }

private:
    inline void                             AddToList           ( CColShape* pShape )       { m_List.push_back ( pShape ); };
    void                                    RemoveFromList      ( CColShape* pShape );
    void                                    TakeOutTheTrash     ( void );

    list < CColShape* >                     m_List;
    bool                                    m_bCanRemoveFromList;
    bool                                    m_bIteratingList;
    list < CColShape* >                     m_TrashCan;
};

#endif
