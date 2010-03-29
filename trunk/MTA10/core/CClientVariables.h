/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CClientVariables.h
*  PURPOSE:     Header file for client variable class
*  DEVELOPERS:  Derek Abdine <>
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
    bool            Get                     ( const std::string& strVariable, bool &val )                    { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string& strVariable, std::string &val )             { SANGET; val = Node(strVariable)->GetTagContent (); return val.empty (); };
    bool            Get                     ( const std::string& strVariable, int &val )                     { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string& strVariable, unsigned int &val )            { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string& strVariable, float &val )                   { SANGET; Node(strVariable)->GetTagContent ( val ); return true; };
    bool            Get                     ( const std::string& strVariable, CVector &val );
    bool            Get                     ( const std::string& strVariable, CVector2D &val );
    bool            Get                     ( const std::string& strVariable, CColor &val );

    // Set queries
    void            Set                     ( const std::string& strVariable, bool val )                     { SAN; m_iRevision++; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string& strVariable, const std::string& val )       { SAN; m_iRevision++; Node(strVariable)->SetTagContent ( val.c_str () ); };
    void            Set                     ( const std::string& strVariable, int val )                      { SAN; m_iRevision++; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string& strVariable, unsigned int val )             { SAN; m_iRevision++; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string& strVariable, float val )                    { SAN; m_iRevision++; Node(strVariable)->SetTagContent ( val ); };
    void            Set                     ( const std::string& strVariable, CVector val );
    void            Set                     ( const std::string& strVariable, CVector2D val );
    void            Set                     ( const std::string& strVariable, CColor val );

    bool            Exists                  ( const std::string& strVariable );

    bool            Load                    ( void );
    bool            IsLoaded                ( void ) { return m_bLoaded; }
    int             GetRevision             ( void ) { return m_iRevision; }

private:
    CXMLNode*       Node                    ( const std::string& strVariable );
    void            LoadDefaults            ( void );

    bool            m_bLoaded;
    CXMLNode        *m_pStorage;
    int             m_iRevision;
};

#endif
