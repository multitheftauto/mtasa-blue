/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectManager.h
*  PURPOSE:     Object entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __COBJECTMANAGER_H
#define __COBJECTMANAGER_H

#include "CObject.h"
#include <list>

using std::list;

class CObject;
typedef CFastList < CObject* >   CObjectListType;

class CObjectManager
{
    friend class CObject;
public:
                                CObjectManager                  ( void );
                                ~CObjectManager                 ( void );

    CObject*                    Create                          ( CElement* pParent, CXMLNode* pNode, bool bIsLowLod );
    CObject*                    CreateFromXML                   ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents, bool bIsLowLod );
    void                        DeleteAll                       ( void );

    inline unsigned int         Count                           ( void )                            { return static_cast < unsigned int > ( m_List.size () ); };
    bool                        Exists                          ( CObject* pObject );

    CObjectListType::const_iterator     IterBegin               ( void ) const                      { return m_List.begin (); };
    CObjectListType::const_iterator     IterEnd                 ( void ) const                      { return m_List.end (); };

    static bool                 IsValidModel                    ( unsigned long ulObjectModel );
    static bool                 IsBreakableModel                ( unsigned long ulObjectModel );

private:
    inline void                 AddToList                       ( CObject* pObject )                { m_List.push_back ( pObject ); };
    void                        RemoveFromList                  ( CObject* pObject );

    CObjectListType             m_List;
};

#endif
