/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEffectManager.h
*  PURPOSE:     Effect manager
*
*****************************************************************************/

#ifndef __CCLIENTEFFECTMANAGER_H
#define __CCLIENTEFFECTMANAGER_H

#include "CClientEffect.h"

#include <game/CFxSystem.h>
#include <game/CFxManager.h>

class CClientEffectManager
{
    friend class CClientManager;

public:
    CClientEffect*                  Create                  ( const SString& strEffectName, const CVector& vecPosition, ElementID ID );
    void                            SAEffectDestroyed       ( void * pFxSAInterface );
    void                            DeleteAll               ( void );

    inline unsigned int             Count                   ( void )  { return m_Effects.size (); };
    static CClientEffect*           Get                     ( ElementID ID );
    inline void                     AddToList               ( CClientEffect * pEffect ) { m_Effects.push_back(pEffect); }
    void                            RemoveFromList          ( CClientEffect * pEffect );

private:
                                    CClientEffectManager   ( class CClientManager* pManager );
                                    ~CClientEffectManager  ( void );
                                    CClientEffect*                  Get                     ( void * pFxSA );

    class CClientManager*           m_pManager;
    bool                            m_bCanRemoveFromList;
    std::list < CClientEffect* >    m_Effects;
};

#endif
