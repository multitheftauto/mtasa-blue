/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectManager.h
*  PURPOSE:     Object entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __COBJECTMANAGER_H
#define __COBJECTMANAGER_H

#include "CObject.h"
#include <list>

using namespace std;

class CObjectManager
{
    friend class CObject;
public:
                                CObjectManager                  ( void );
                                ~CObjectManager                 ( void );

    CObject*                    Create                          ( CElement* pParent, CXMLNode* pNode = NULL );
    CObject*                    CreateFromXML                   ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                        DeleteAll                       ( void );

    inline unsigned int         Count                           ( void )                            { return static_cast < unsigned int > ( m_List.size () ); };
    bool                        Exists                          ( CObject* pObject );

    inline list < CObject* > ::const_iterator   IterBegin       ( void )                            { return m_List.begin (); };
    inline list < CObject* > ::const_iterator   IterEnd         ( void )                            { return m_List.end (); };

    static bool                 IsValidModel                    ( unsigned long ulObjectModel );

private:
    inline void                 AddToList                       ( CObject* pObject )                { m_List.push_back ( pObject ); };
    void                        RemoveFromList                  ( CObject* pObject );

    bool                        m_bRemoveFromList;
    list < CObject* >           m_List;
    list < CObject* >           m_Attached;
};

#endif
