/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLNode.h
 *  PURPOSE:     XML node interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CXMLCommon.h"
#include "CXMLAttributes.h"
#include <string>
#include "SString.h"

class CXMLNode : public CXMLCommon
{
public:
    virtual ~CXMLNode(){};

    virtual CXMLNode* CreateSubNode(const char* szTagName, CXMLNode* pInsertAfter = nullptr) = 0;
    virtual void      DeleteSubNode(CXMLNode* pNode) = 0;
    virtual void      DeleteAllSubNodes() = 0;

    virtual unsigned int GetSubNodeCount() = 0;
    virtual CXMLNode*    GetSubNode(unsigned int uiIndex) = 0;
    virtual CXMLNode*    FindSubNode(const char* szTagName, unsigned int uiIndex = 0) = 0;

    virtual std::list<CXMLNode*>::iterator ChildrenBegin() = 0;
    virtual std::list<CXMLNode*>::iterator ChildrenEnd() = 0;

    virtual CXMLAttributes& GetAttributes() = 0;
    virtual CXMLNode*       GetParent() = 0;

    virtual int                GetLine() = 0;
    virtual const std::string& GetTagName() = 0;
    virtual void               SetTagName(const std::string& strString) = 0;

    virtual const std::string GetTagContent() = 0;
    virtual bool              GetTagContent(bool& bContent) = 0;
    virtual bool              GetTagContent(int& iContent) = 0;
    virtual bool              GetTagContent(unsigned int& uiContent) = 0;
    virtual bool              GetTagContent(float& fContent) = 0;

    virtual void SetTagContent(const char* szContent, bool bCDATA = false) = 0;
    virtual void SetTagContent(bool bContent) = 0;
    virtual void SetTagContent(int iContent) = 0;
    virtual void SetTagContent(unsigned int uiContent) = 0;
    virtual void SetTagContent(float fContent) = 0;
    virtual void SetTagContentf(const char* szFormat, ...) = 0;

    virtual CXMLNode* CopyNode(CXMLNode* pParent) = 0;
    virtual bool      CopyChildrenInto(CXMLNode* pDestination, bool bRecursive) = 0;

    virtual bool IsValid() = 0;

    virtual SString GetAttributeValue(const SString& strAttributeName) = 0;
    virtual SString GetCommentText() = 0;
    virtual void    SetCommentText(const char* szCommentText, bool bLeadingBlankLine = false) = 0;
};
