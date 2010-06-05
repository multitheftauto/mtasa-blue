/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementDeleter.h
*  PURPOSE:     Element lazy deletion manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
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
    inline                          ~CElementDeleter        ( void )        { DoDeleteAll (); };

    void                            Delete                  ( class CElement* pElement, bool bUnlink = true );
    void                            DoDeleteAll             ( void );

    bool                            IsBeingDeleted          ( class CElement* pElement );

private:
    std::list < class CElement* >   m_List;
};

#endif
