/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPickupManager.h
*  PURPOSE:     Pickup entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPickupManager;

#ifndef __CPICKUPMANAGER_H
#define __CPICKUPMANAGER_H

#include "CColManager.h"
#include "CPickup.h"
#include <list>

class CPickupManager
{
    friend class CPickup;

public:
                                    CPickupManager                          ( CColManager* pColManager );
    inline                          ~CPickupManager                         ( void )                    { DeleteAll (); };

    CPickup*                        Create                                  ( CElement* pParent );
    CPickup*                        CreateFromXML                           ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                            DeleteAll                               ( void );

    inline unsigned int             Count                                   ( void )                    { return static_cast < unsigned int > ( m_List.size () ); };
    bool                            Exists                                  ( CPickup* pPickup );

    inline list < CPickup* > ::const_iterator   IterBegin                   ( void )                    { return m_List.begin (); };
    inline list < CPickup* > ::const_iterator   IterEnd                     ( void )                    { return m_List.end (); };

    static bool                     IsValidWeaponID                         ( unsigned int uiWeaponID );
    static unsigned short           GetWeaponModel                          ( unsigned int uiWeaponID );
    inline static unsigned short    GetHealthModel                          ( void )                    { return 1240; };
    inline static unsigned short    GetArmorModel                           ( void )                    { return 1242; };

private:
    inline void                     AddToList                               ( CPickup* pPickup )        { m_List.push_back ( pPickup ); };
    inline void                     RemoveFromList                          ( CPickup* pPickup )        { if ( !m_bDontRemoveFromList && !m_List.empty() ) m_List.remove ( pPickup); };

    CColManager*                    m_pColManager;
    list < CPickup* >               m_List;
    bool                            m_bDontRemoveFromList;
};

#endif
