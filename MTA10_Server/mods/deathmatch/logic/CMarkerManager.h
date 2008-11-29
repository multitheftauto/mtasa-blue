/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMarkerManager.h
*  PURPOSE:     Marker entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CMarkerManager;

#ifndef __CMarkerManager_H
#define __CMarkerManager_H

#include "CColManager.h"
#include "CMarker.h"
#include <list>

using namespace std;

class CMarkerManager
{
    friend class CMarker;

public:
                                                        CMarkerManager                    ( CColManager* pColManager );
    inline                                              ~CMarkerManager                   ( void )        { DeleteAll (); };

    CMarker*                                            Create                      ( CElement* pParent, CXMLNode* pNode = NULL );
    CMarker*                                            CreateFromXML               ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                                                DeleteAll                   ( void );

    inline unsigned int                                 Count                       ( void )            { return static_cast < unsigned int > ( m_Markers.size () ); };
    bool                                                Exists                      ( CMarker* pMarker );

    inline list < CMarker* > ::const_iterator           IterBegin                   ( void )            { return m_Markers.begin (); };
    inline list < CMarker* > ::const_iterator           IterEnd                     ( void )            { return m_Markers.end (); };

    static int                                          StringToType                ( const char* szString );               
    static bool                                         TypeToString                ( unsigned int uiType, char* szString );
    static unsigned char                                StringToIcon                ( const char* szString );
    static bool                                         IconToString                ( unsigned char ucIcon, char* szString );

private:
    void                                                AddToList                   ( CMarker* pMarker ){ m_Markers.push_back ( pMarker ); };
    void                                                RemoveFromList              ( CMarker* pMarker );

    CColManager*                                        m_pColManager;
    bool                                                m_bDontRemove;
    list < CMarker* >                                   m_Markers;
};

#endif
