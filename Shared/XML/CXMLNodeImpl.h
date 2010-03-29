/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLNodeImpl.h
*  PURPOSE:     XML node class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLNODEIMPL_H
#define __CXMLNODEIMPL_H

#include "CXMLAttributesImpl.h"
#include <tinyxml.h>
#include <xml/CXMLNode.h>

#include <list>
#include <string>

class CXMLNodeImpl : public CXMLNode
{
public:
                            CXMLNodeImpl        ( class CXMLFileImpl* pFile, CXMLNodeImpl* pParent, TiXmlElement& Node );
                            ~CXMLNodeImpl       ( void );

    CXMLNode*               CreateSubNode       ( const char* szTagName );
    void                    DeleteSubNode       ( CXMLNode* pNode ) { delete pNode; };
    void                    DeleteAllSubNodes   ( void );

    unsigned int            GetSubNodeCount     ( void );
    CXMLNode*               GetSubNode          ( unsigned int uiIndex );
    CXMLNode*               FindSubNode         ( const char* szTagName, unsigned int uiIndex = 0 );

    std::list < CXMLNode* > ::iterator
                            ChildrenBegin       ( void ) { return m_Children.begin (); };
    std::list < CXMLNode* > ::iterator
                            ChildrenEnd         ( void ) { return m_Children.end (); };

    CXMLAttributes&         GetAttributes       ( void );
    CXMLNode*               GetParent           ( void );

    int                     GetLine             ( void );

    const std::string&      GetTagName          ( void );
    void                    SetTagName          ( const std::string& strString );

    const std::string       GetTagContent       ( void );
    bool                    GetTagContent       ( bool& bContent );
    bool                    GetTagContent       ( int& iContent );
    bool                    GetTagContent       ( unsigned int& uiContent );
    bool                    GetTagContent       ( float& fContent );

    void                    SetTagContent       ( const char* szContent );
    void                    SetTagContent       ( bool bContent );
    void                    SetTagContent       ( int iContent );
    void                    SetTagContent       ( unsigned int uiContent );
    void                    SetTagContent       ( float fContent );
    void                    SetTagContentf      ( const char* szFormat, ... );

    eXMLClass               GetClassType        ( void )    { return CXML_NODE; };
    unsigned long           GetID               ( void )    { return m_ulID; };

    CXMLNode*               CopyNode            ( CXMLNode* pParent = NULL );
    bool                    CopyChildrenInto    ( CXMLNode* pDestination, bool bRecursive );

    TiXmlElement*           GetNode             ( void );
    void                    DeleteWrapper       ( void );

    void                    AddToList           ( CXMLNode* pNode );
    void                    RemoveFromList      ( CXMLNode* pNode );
    void                    RemoveAllFromList   ( void );

    bool                    IsValid             ( void ) { return m_ulID != INVALID_XML_ID; };

private:
    bool                    StringToLong        ( const char* szString, long& lValue );

    class CXMLFileImpl*         m_pFile;
    CXMLNodeImpl*               m_pParent;
    TiXmlElement*               m_pNode;
    TiXmlDocument*              m_pDocument;

    std::list < CXMLNode* >     m_Children;
    bool                        m_bCanRemoveFromList;

    CXMLAttributesImpl          m_Attributes;

    unsigned long               m_ulID;
};

#endif
