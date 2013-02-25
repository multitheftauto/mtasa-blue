/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectRespawner.h
*  PURPOSE:     Header for object respawner class
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __COBJECTRESPAWNER_H
#define __COBJECTRESPAWNER_H

class CObjectRespawner
{
public:
                                CObjectRespawner        ( void );
    inline                      ~CObjectRespawner       ( void )        { DoRespawnAll (); };

    void                        Respawn                 ( CClientObject* pObject );
    void                        DoRespawnAll            ( void );

    bool                        IsBeingRespawned        ( CClientObject* pObject );

private:
    std::vector < CClientObject* >                      m_List;
};

#endif