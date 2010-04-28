/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMovingObjectsManager.h
*  PURPOSE:     Header for moving objects manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMOVINGOBJECTSMANAGER_H
#define __CMOVINGOBJECTSMANAGER_H

#include "CDeathmatchObject.h"
#include <list>

class CMovingObjectsManager
{
public:
    inline void                         Add                         ( CDeathmatchObject* pObject )      { m_List.push_back ( pObject ); };
    inline void                         Remove                      ( CDeathmatchObject* pObject )      { if ( !m_List.empty() ) m_List.remove ( pObject ); };

    void                                DoPulse                     ( void );

private:
    std::list < CDeathmatchObject* >    m_List;
};

#endif
