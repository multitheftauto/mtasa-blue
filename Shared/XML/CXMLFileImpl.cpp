/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLFileImpl.cpp
 *  PURPOSE:     XML file class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <ios>
#include <iostream>
#include <fstream>

CXMLFileImpl::CXMLFileImpl(const std::string& filename, bool usingIDs, bool readOnly)
    : m_ID(INVALID_XML_ID), m_usingIDs(usingIDs), m_readOnly(readOnly), m_filename(filename)
{
    if (usingIDs)
        m_ID = CXMLArray::PopUniqueID(this);
}

CXMLFileImpl::CXMLFileImpl(const std::string& filename, CXMLNode* node, bool readOnly)
    : m_ID(INVALID_XML_ID), m_readOnly(readOnly), m_filename(filename)
{
    bool usingIDs = node->GetID() != INVALID_XML_ID;
    if (usingIDs)
        m_ID = CXMLArray::PopUniqueID(this);

    // Create document
    m_document = std::make_unique<pugi::xml_document>();

    // Copy root node info
    auto& root = reinterpret_cast<CXMLNodeImpl*>(node)->GetNode();
    m_document->append_copy(root);

    // Construct Wrapper tree
    BuildWrapperTree(usingIDs);
}

CXMLFileImpl::~CXMLFileImpl()
{
    if (m_ID != INVALID_XML_ID)
        CXMLArray::PushUniqueID(this);
}

bool CXMLFileImpl::Parse(std::vector<char>* outFileContents)
{
    if (m_filename.empty())
        return false;

    // Reset previous file
    Reset();

    // Open file
    std::ifstream file(m_filename, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file.is_open())
        return false;

    // Disable whitespace skipping
    file.unsetf(std::ios::skipws);

    // Read file contents into vector
    std::vector<char> fileContents{};
    std::streampos    fileSize = file.tellg();
    fileContents.reserve(fileSize);
    file.seekg(0, std::ios::beg);
    fileContents.insert(fileContents.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());
    file.close();

    // Load the xml
    m_document = std::make_unique<pugi::xml_document>();
    m_parserResult = m_document->load_buffer(fileContents.data(), fileContents.size());
    if (!m_parserResult)
        return false;

    // Also copy to buffer if requested
    if (outFileContents)
        outFileContents->insert(outFileContents->begin(), fileContents.begin(), fileContents.end());

    BuildWrapperTree(m_ID != INVALID_XML_ID);
    return true;
}

void CXMLFileImpl::BuildWrapperTree(bool usingIDs)
{
    m_root = WrapperTreeWalker(m_document.get(), usingIDs);
}

std::unique_ptr<CXMLNodeImpl> CXMLFileImpl::WrapperTreeWalker(pugi::xml_node* node, bool usingIDs)
{
    // Construct wrapper for this node
    auto wrapperNode = std::make_unique<CXMLNodeImpl>(*node, usingIDs, nullptr);

    // Construct Attributes
    for (auto& attribute : node->attributes())
        wrapperNode->AddAttribute(std::make_unique<CXMLAttributeImpl>(attribute, usingIDs));

    // Recursively call on our children
    for (auto& child : node->children())
        // Only for actual child nodes
        if (child.type() == pugi::node_element)
            wrapperNode->AddChild(WrapperTreeWalker(&child, usingIDs));

    return wrapperNode;
}

void CXMLFileImpl::Reset()
{
    m_root.reset(nullptr);
}

bool CXMLFileImpl::Write()
{
    // We have a filename?
    if (!m_filename.empty())
        return m_document->save_file(m_filename.c_str());

    return false;
}

CXMLNode* CXMLFileImpl::CreateRootNode(const std::string& tagName)
{
    if (m_root)
    {
        m_root->GetNode().set_name(tagName.c_str());
        return GetRootNode();
    }

    m_document = std::make_unique<pugi::xml_document>();
    auto innerRoot = m_document->append_child(tagName.c_str());
    auto rootWrapper = std::make_unique<CXMLNodeImpl>(*m_document.get(), m_ID != INVALID_XML_ID);
    rootWrapper->AddChild(std::make_unique<CXMLNodeImpl>(innerRoot, m_ID != INVALID_XML_ID));
    m_root = std::move(rootWrapper);
    return GetRootNode();
}

CXMLNode* CXMLFileImpl::GetRootNode() const
{
    // The root node for pugixml is the first child of the document node
    if (m_root)
        return m_root->GetChildren().front().get();
    return nullptr;
}

CXMLErrorCodes::Code CXMLFileImpl::GetLastError(std::string& out)
{
    auto parserStatus = m_parserResult.status;
    if (parserStatus == pugi::status_ok)
        return CXMLErrorCodes::NoError;

    out = m_parserResult.description();
    return CXMLErrorCodes::OtherError;
}
