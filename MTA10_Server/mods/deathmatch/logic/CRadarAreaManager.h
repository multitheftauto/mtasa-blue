/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarAreaManager.h
*  PURPOSE:     Radar area entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRADARAREAMANAGER_H
#define __CRADARAREAMANAGER_H

#include "CRadarArea.h"
#include <list>

using namespace std;

class CRadarAreaManager
{
    friend class CRadarArea;
public:
                                    CRadarAreaManager                       ( void );
                                    ~CRadarAreaManager                      ( void );

    CRadarArea*                     Create                                  ( CElement* pParent, CXMLNode* pNode );
    CRadarArea*                     CreateFromXML                           ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                            DeleteAll                               ( void );

    unsigned int                    Count                                   ( void )                        { return static_cast < unsigned int > ( m_List.size () ); };
    bool                            Exists                                  ( CRadarArea* pArea );

    inline list < CRadarArea* > ::const_iterator    IterBegin               ( void )                        { return m_List.begin (); };
    inline list < CRadarArea* > ::const_iterator    IterEnd                 ( void )                        { return m_List.end (); };

private:
    inline void                     AddToList                               ( CRadarArea* pArea )     { m_List.push_back ( pArea ); };
    void                            RemoveFromList                          ( CRadarArea* pArea );

    list < CRadarArea * >           m_List;
    bool                            m_bDontRemoveFromList;
};

#endif
