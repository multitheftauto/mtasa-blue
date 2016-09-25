/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

CXMLFileImpl::CXMLFileImpl(const std::string &strFilename, bool bUsingIDs) : 
    m_ulID(INVALID_XML_ID), 
    m_strFilename(strFilename)
{
    if (bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);
}

CXMLFileImpl::CXMLFileImpl(const std::string &strFilename, CXMLNode *pNode) : 
    m_ulID(INVALID_XML_ID), 
    m_strFilename(strFilename)
{
    bool bUsingIDs = pNode->GetID() != INVALID_XML_ID;
    if (bUsingIDs)
        m_ulID = CXMLArray::PopUniqueID(this);

    // Create document
    m_pDocument = std::make_unique<pugi::xml_document>();

    // Copy root node info
    auto &root = reinterpret_cast<CXMLNodeImpl*>(pNode)->GetNode();
    m_pDocument->set_name(root.name());
    m_pDocument->set_value(root.value());

    for (auto &child : root.children())
    {
        m_pDocument->append_copy(child);
    }
    for (auto &attr : root.attributes())
    {
        m_pDocument->append_copy(attr);
    }

    // Construct Wrapper tree
    BuildWrapperTree(bUsingIDs);
}

CXMLFileImpl::~CXMLFileImpl(void)
{
    if (m_ulID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

bool CXMLFileImpl::Parse(std::vector<char> *pOutFileContents)
{
    if (!m_strFilename.empty())
    {
        // Reset previous file
        Reset();

        // Load file
        m_pDocument = std::make_unique<pugi::xml_document>();
        m_parserResult = m_pDocument->load_file(m_strFilename.c_str());
        if (!m_parserResult)
        {
            return false;
        }

        BuildWrapperTree(m_ulID != INVALID_XML_ID);
        return true;
    }
    return false;
}

void CXMLFileImpl::BuildWrapperTree(bool bUsingIDs)
{
    m_pRoot = WrapperTreeWalker(m_pDocument.get(), bUsingIDs);
}

std::unique_ptr<CXMLNodeImpl> CXMLFileImpl::WrapperTreeWalker(pugi::xml_node * node, bool bUsingIDs)
{
    // Construct wrapper for this node
    auto wrapperNode = std::make_unique<CXMLNodeImpl>(*node, bUsingIDs);

    // Construct Attributes
    for (auto &attribute : node->attributes())
    {
        wrapperNode->AddAttribute(std::make_unique<CXMLAttributeImpl>(attribute, bUsingIDs));
    }

    // Recursively call on our children
    for (auto &child : node->children())
    {
        wrapperNode->AddChild(WrapperTreeWalker(&child, bUsingIDs));
    }

    return wrapperNode;
}

void CXMLFileImpl::Reset() 
{ 
    m_pRoot.reset(nullptr); 
}

bool CXMLFileImpl::Write(void)
{
    // We have a filename?
    if (!m_strFilename.empty())
    {
        return m_pDocument->save_file(m_strFilename.c_str());
    }

    return false;
}

CXMLNode *CXMLFileImpl::CreateRootNode(const std::string &strTagName)
{
    if (m_pRoot)
    {
        m_pRoot->GetNode().set_name(strTagName.c_str());
        return GetRootNode();
    }
    else
    {
        m_pDocument = std::make_unique<pugi::xml_document>();
        auto innerRoot = m_pDocument->append_child(strTagName.c_str());
        auto rootWrapper = std::make_unique<CXMLNodeImpl>(*m_pDocument.get(),
                                                          m_ulID != INVALID_XML_ID);
        rootWrapper->AddChild(
            std::make_unique<CXMLNodeImpl>(innerRoot, m_ulID != INVALID_XML_ID));
        m_pRoot = std::move(rootWrapper);
        return GetRootNode();
    }
}

CXMLNode *CXMLFileImpl::GetRootNode()
{
    // The root node for pugixml is the first child of the document node
    if (m_pRoot)
        return m_pRoot->GetChildren().front().get();
    return nullptr;
}

CXMLErrorCodes::Code CXMLFileImpl::GetLastError(std::string &strOut)
{
    auto parserStatus = m_parserResult.status;

    if (parserStatus == pugi::status_ok)
    {
        return CXMLErrorCodes::NoError;
    }
    else
    {
        strOut = m_parserResult.description();
        return CXMLErrorCodes::OtherError;
    }
}
