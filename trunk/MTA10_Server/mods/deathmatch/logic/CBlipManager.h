/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlipManager.h
*  PURPOSE:     Blip entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CBlipManager;

#ifndef __CBLIPMANAGER_H
#define __CBLIPMANAGER_H

#include "CBlip.h"
#include <list>

using namespace std;

class CBlipManager
{
    friend class CBlip;

public:
                            CBlipManager                ( void );
    inline                  ~CBlipManager               ( void )                        { DeleteAll (); };

    CBlip*                  Create                      ( CElement* pParent, CXMLNode* pNode = NULL );
    CBlip*                  CreateFromXML               ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                    DeleteAll                   ( void );

    inline unsigned int     Count                       ( void )                        { return static_cast < unsigned int > ( m_List.size () ); };
    bool                    Exists                      ( CBlip* pBlip );

    list < CBlip* > ::const_iterator    IterBegin       ( void )                        { return m_List.begin (); };
    list < CBlip* > ::const_iterator    IterEnd         ( void )                        { return m_List.end (); };

    static bool             IsValidIcon                 ( unsigned long ulIcon )        { return ulIcon <= 63; };

private:
    list < CBlip* >         m_List;
    bool                    m_bRemoveFromList;
};

#endif
