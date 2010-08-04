/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFire.h
*  PURPOSE:     Fire Manipulation Class
*  DEVELOPERS:  Fedor Sinev
*
*****************************************************************************/

#ifndef __CCLIENTFIRE_H
#define __CCLIENTFIRE_H

class CClientFire : public CClientEntity
{
    public:
                                    CClientFire                 ( CClientManager* pManager, ElementID ID, CVector& vecPosition, float fSize = 1.8, bool bSilent = false );
                                    ~CClientFire                ( void );
    eClientEntityType               GetType                     ( ) const { return CCLIENTFIRE; }
    void                            GetPosition                 ( CVector& vecPosition ) const;
    void                            Unlink                      ( );
    void                            SetPosition                 ( const CVector& vecPosition );
    
    private:
    CFire*                          m_pFire;
    CClientFireManager*             m_pManager;

    friend CClientFireManager;
};

#endif