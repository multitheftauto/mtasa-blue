/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLNode.h
 *  PURPOSE:     XML node interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CXMLAttribute.h"
#include "CXMLCommon.h"
#include "../SString.h"
#include <string>
#include <list>
#include <memory>

class CXMLNode : public CXMLCommon
{
public:
    virtual ~CXMLNode() = default;

    virtual CXMLNode* GetParent() const = 0;
    virtual CXMLNode* CreateChild(const std::string& tagName, CXMLNode* insertAfter = nullptr) = 0;
    virtual void      RemoveChild(CXMLNode* node) = 0;

    virtual void                                        RemoveAllChildren() = 0;
    virtual unsigned int                                GetChildCount() const = 0;
    virtual CXMLNode*                                   GetChild(unsigned int index) const = 0;
    virtual CXMLNode*                                   GetChild(const std::string& tagName, unsigned int index = 0) const = 0;
    virtual const std::list<std::unique_ptr<CXMLNode>>& GetChildren() const = 0;

    virtual const std::string GetTagName() const = 0;
    virtual void              SetTagName(const std::string& string) = 0;

    virtual const std::string GetTagContent() const = 0;
    virtual bool              GetTagContent(bool& content) const = 0;
    virtual bool              GetTagContent(int& content) = 0;
    virtual bool              GetTagContent(unsigned int& content) = 0;
    virtual bool              GetTagContent(float& content) = 0;

    virtual void SetTagContent(const std::string& content, bool cdata = false) = 0;
    virtual void SetTagContent(const char* content, bool cdata = false) = 0;
    virtual void SetTagContent(bool content) = 0;
    virtual void SetTagContent(int content) = 0;
    virtual void SetTagContent(unsigned int content) = 0;
    virtual void SetTagContent(float content) = 0;
    virtual void SetTagContentf(const char* format, ...) = 0;

    virtual int  GetLine() const = 0;
    virtual bool IsValid() const = 0;

    virtual CXMLAttribute*                                   AddAttribute(const std::string& name) = 0;
    virtual bool                                             RemoveAttribute(const std::string& name) = 0;
    virtual CXMLAttribute*                                   GetAttribute(const std::string& name) const = 0;
    virtual const std::list<std::unique_ptr<CXMLAttribute>>& GetAttributes() const = 0;

    virtual SString GetCommentText() const = 0;
    virtual void    SetCommentText(const char* commentText, bool leadingBlankLine = false) = 0;
};
