/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementDeleter.h
*  PURPOSE:     Header for element deleter class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CELEMENTDELETER_H
#define __CELEMENTDELETER_H

#include <list>

class CElementDeleter
{
public:
                                CElementDeleter         ( void );
    inline                      ~CElementDeleter        ( void )        { DoDeleteAll (); };

    void                        Delete                  ( class CClientEntity* pElement );
	void                        DeleteRecursive         ( class CClientEntity* pElement );

    void                        DoDeleteAll             ( void );

    bool                        IsBeingDeleted          ( class CClientEntity* pElement );

    void                        Unreference             ( class CClientEntity* pElement );

    // Call this to see if this class can be destroyed. That means no objects this class
    // has in the delete queue is undestroyable.
    bool                        CanBeDestroyed          ( void );

private:
    std::list < class CClientEntity* >      m_List;
    bool                                    m_bAllowUnreference;
};

#endif
