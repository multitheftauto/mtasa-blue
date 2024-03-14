/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLNodeImpl.h
 *  PURPOSE:     XML node class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CXMLAttributesImpl.h"
#include <tinyxml.h>
#include <xml/CXMLNode.h>

#include <list>
#include <string>

class CXMLNodeImpl : public CXMLNode
{
public:
    CXMLNodeImpl(class CXMLFileImpl* pFile, CXMLNodeImpl* pParent, TiXmlElement& Node);
    ~CXMLNodeImpl();

    // BuildFromDocument recursively builds child CXMLNodeImpl from the underlying TiXmlElement.
    //
    // This is **only** used for xmlLoadString right now.
    // Look elsewhere if you're thinking about XML files. It does things a different way.
    void BuildFromDocument();

    CXMLNode* CreateSubNode(const char* szTagName, CXMLNode* pInsertBefore = nullptr);
    void      DeleteSubNode(CXMLNode* pNode) { delete pNode; };
    void      DeleteAllSubNodes();

    std::uint32_t GetSubNodeCount();
    CXMLNode*    GetSubNode(std::uint32_t uiIndex);
    CXMLNode*    FindSubNode(const char* szTagName, std::uint32_t uiIndex = 0);

    std::list<CXMLNode*>::iterator ChildrenBegin() { return m_Children.begin(); };
    std::list<CXMLNode*>::iterator ChildrenEnd() { return m_Children.end(); };

    CXMLAttributes& GetAttributes();
    CXMLNode*       GetParent();

    int GetLine();

    const std::string& GetTagName();
    void               SetTagName(const std::string& strString);

    const std::string GetTagContent();
    bool              GetTagContent(bool& bContent);
    bool              GetTagContent(int& iContent);
    bool              GetTagContent(std::uint32_t& uiContent);
    bool              GetTagContent(float& fContent);

    void SetTagContent(const char* szContent, bool bCDATA = false);
    void SetTagContent(bool bContent);
    void SetTagContent(int iContent);
    void SetTagContent(std::uint32_t uiContent);
    void SetTagContent(float fContent);
    void SetTagContentf(const char* szFormat, ...);

    eXMLClass     GetClassType() { return CXML_NODE; };
    unsigned long GetID()
    {
        dassert((!m_pFile) || m_pFile && m_pFile->IsUsingIDs());
        return m_ulID;
    };
    bool IsUsingIDs() { return m_bUsingIDs; };

    CXMLNode* CopyNode(CXMLNode* pParent = NULL);
    bool      CopyChildrenInto(CXMLNode* pDestination, bool bRecursive);

    TiXmlElement* GetNode();
    void          DeleteWrapper();

    void AddToList(CXMLNode* pNode);
    void RemoveFromList(CXMLNode* pNode);
    void RemoveAllFromList();

    bool IsValid() { return !m_bUsingIDs || m_ulID != INVALID_XML_ID; };

    virtual SString GetAttributeValue(const SString& strAttributeName);
    virtual SString GetCommentText();
    virtual void    SetCommentText(const char* szCommentText, bool bLeadingBlankLine = false);

    std::string ToString();

private:
    bool StringToLong(const char* szString, long& lValue);

    unsigned long m_ulID;
    const bool    m_bUsingIDs;

    class CXMLFileImpl* m_pFile;
    CXMLNodeImpl*       m_pParent;
    TiXmlElement*       m_pNode;
    TiXmlDocument*      m_pDocument;

    std::list<CXMLNode*> m_Children;
    bool                 m_bCanRemoveFromList;

    CXMLAttributesImpl m_Attributes;
};
