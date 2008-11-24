/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CClientVariables.h
*  PURPOSE:		Header file for client variable class
*  DEVELOPERS:	Derek Abdine <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTVARIABLES_H
#define __CCLIENTVARIABLES_H

#include <string>
#include "CSingleton.h"
#include <core/CCVarsInterface.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <CVector.h>
#include <CVector2D.h>
#include "CChat.h"

// This call uses XML codes as internal storage for client variables
#define CVARS_ROOT              "mainconfig"
#define CVARS_NODE_CVARS        "settings"                  // cvars node
#define CVARS_NODE_KEYBINDS     "binds"                     // keybinds node
#define CVARS_NODE_SERVER_FAV   "favourite_servers"         // favourite servers list node
#define CVARS_NODE_SERVER_REC   "recently_played_servers"   // recently played servers list node

#define CVARS_FAVOURITE_LIST_TAG   "favourite_server"
#define CVARS_RECENT_LIST_TAG      "recently_played_server"

// Macros
#define CVARS_GET   CClientVariables::GetSingleton().Get
#define CVARS_SET   CClientVariables::GetSingleton().Set

class CClientVariables : public CCVarsInterface, public CSingleton < CClientVariables >
{
    // Sanity macros
    #define SAN     if(!m_pStorage) return
    #define SANGET  if(!Node(strVariable)) return false

public:
                    CClientVariables        ( void );
                    ~CClientVariables       ( void );

    // Get queries
    bool            Get                     ( const std::string strVariable, bool &val )                    { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string strVariable, std::string &val )             { SANGET; val = Node(strVariable)->GetTagContent (); return val.empty (); };
    bool            Get                     ( const std::string strVariable, int &val )                     { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string strVariable, unsigned int &val )            { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string strVariable, float &val )                   { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string strVariable, CVector &val );
    bool            Get                     ( const std::string strVariable, CVector2D &val );
    bool            Get                     ( const std::string strVariable, CColor &val );

    // Set queries
    void            Set                     ( const std::string strVariable, bool val )                     { SAN; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string strVariable, const std::string val )        { SAN; Node(strVariable)->SetTagContent ( val.c_str () ); };
    void            Set                     ( const std::string strVariable, int val )                      { SAN; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string strVariable, unsigned int val )             { SAN; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string strVariable, float val )                    { SAN; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string strVariable, CVector val );
    void            Set                     ( const std::string strVariable, CVector2D val );
    void            Set                     ( const std::string strVariable, CColor val );

    bool            Exists                  ( const std::string strVariable );

    bool            Load                    ( const std::string strConfigFile );
    bool            Save                    ( void );
    bool            IsLoaded                ( void ) { return m_bLoaded; }

private:
    CXMLNode*       Node                    ( const std::string strVariable );
    void            LoadDefaults            ( void );

    bool            m_bLoaded;
    CXMLFile        *m_pFile;
    CXMLNode        *m_pStorage;
};

#endif