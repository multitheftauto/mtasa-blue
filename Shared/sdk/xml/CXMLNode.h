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
#pragma once 

#include "CXMLCommon.h"
#include <string>
#include <memory>

class CXMLAttribute;
class CXMLNode : public CXMLCommon
{
public:
    virtual                     ~CXMLNode() = default;

    virtual CXMLNode*           GetParent() = 0;
    virtual CXMLNode*           CreateChild(const std::string& strTagName) = 0;
    virtual void                RemoveChild(CXMLNode* pNode) = 0;
    
    virtual void                RemoveAllChildren() = 0;
    virtual unsigned int        GetChildCount() = 0;
    virtual CXMLNode*           GetChild(unsigned int uiIndex) = 0;
    virtual CXMLNode*           GetChild(const std::string& strTagName, unsigned int uiIndex = 0) = 0;
    virtual const std::list < std::unique_ptr<CXMLNode> >& GetChildren() = 0;
    
    virtual const std::string   GetTagName() = 0;
    virtual void                SetTagName(const std::string& strString) = 0;

    
    virtual const std::string   GetTagContent() = 0;
    virtual bool                GetTagContent(bool& bContent) = 0;
    virtual bool                GetTagContent(int& iContent) = 0;
    virtual bool                GetTagContent(unsigned int& uiContent) = 0;
    virtual bool                GetTagContent(float& fContent) = 0;

    virtual void                SetTagContent(const std::string& strContent, bool bCDATA = false) = 0;
    virtual void                SetTagContent(const char* szContent, bool bCDATA = false) = 0;
    virtual void                SetTagContent(bool bContent) = 0;
    virtual void                SetTagContent(int iContent) = 0;
    virtual void                SetTagContent(unsigned int uiContent) = 0;
    virtual void                SetTagContent(float fContent) = 0;
    virtual void                SetTagContentf(const char* szFormat, ...) = 0;

    virtual CXMLAttribute*      AddAttribute(const std::string& strName) = 0;
    virtual bool                RemoveAttribute(const std::string& strName) = 0;
    virtual CXMLAttribute*      GetAttribute(const std::string& strName) = 0;
    virtual const std::list<std::unique_ptr<CXMLAttribute>>& GetAttributes() = 0;
};
