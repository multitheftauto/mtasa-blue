/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLNode.h
*  PURPOSE:     XML node interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLNODE_H
#define __CXMLNODE_H

#include "CXMLCommon.h"
#include "CXMLAttributes.h"
#include <string>

class CXMLNode: public CXMLCommon
{
public:
    virtual                     ~CXMLNode           ( void ) {};

    virtual CXMLNode*           CreateSubNode       ( const char* szTagName ) = 0;
    virtual void                DeleteSubNode       ( CXMLNode* pNode ) = 0;
    virtual void                DeleteAllSubNodes   ( void ) = 0;

    virtual unsigned int        GetSubNodeCount     ( void ) = 0;
    virtual CXMLNode*           GetSubNode          ( unsigned int uiIndex ) = 0;
    virtual CXMLNode*           FindSubNode         ( const char* szTagName, unsigned int uiIndex = 0 ) = 0;

    virtual list < CXMLNode* > ::iterator
                                ChildrenBegin       ( void ) = 0;
    virtual list < CXMLNode* > ::iterator
                                ChildrenEnd         ( void ) = 0;

    virtual CXMLAttributes&     GetAttributes       ( void ) = 0;
    virtual CXMLNode*           GetParent           ( void ) = 0;

    virtual int                 GetLine             ( void ) = 0;
    virtual const std::string&  GetTagName          ( void ) = 0;
    virtual void                SetTagName          ( const std::string& strString ) = 0;

    virtual const std::string   GetTagContent       ( void ) = 0;
    virtual bool                GetTagContent       ( bool& bContent ) = 0;
    virtual bool                GetTagContent       ( int& iContent ) = 0;
    virtual bool                GetTagContent       ( unsigned int& uiContent ) = 0;
    virtual bool                GetTagContent       ( float& fContent ) = 0;

    virtual void                SetTagContent       ( const char* szContent ) = 0;
    virtual void                SetTagContent       ( bool bContent ) = 0;
    virtual void                SetTagContent       ( int iContent ) = 0;
    virtual void                SetTagContent       ( unsigned int uiContent ) = 0;
    virtual void                SetTagContent       ( float fContent ) = 0;
    virtual void                SetTagContentf      ( const char* szFormat, ... ) = 0;

	virtual CXMLNode*			CopyNode			( CXMLNode* pParent ) = 0;
    virtual bool                CopyChildrenInto    ( CXMLNode* pDestination, bool bRecursive ) = 0;
};

#endif
