/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CXMLConfig.cpp
 *  PURPOSE:     XML-based configuration parsing routines class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CXMLConfig.h"

#include <cctype>
#include <cstdlib>

CXMLConfig::CXMLConfig(const char* szFileName)
{
    m_pFile = NULL;
    m_strFileName = szFileName ? szFileName : "";
    SetFileName(szFileName);
}

CXMLConfig::~CXMLConfig()
{
    delete m_pFile;
}

void CXMLConfig::SetFileName(const char* szFileName)
{
    m_strFileName = szFileName ? szFileName : "";
}

int CXMLConfig::GetBoolean(CXMLNode* pParent, const char* szKey, bool& bResult)
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (pNode)
    {
        if (pNode->GetTagContent(bResult))
        {
            return IS_SUCCESS;
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}

int CXMLConfig::GetInteger(CXMLNode* pParent, const char* szKey, int& iResult, int iMin, int iMax)
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (pNode)
    {
        if (pNode->GetTagContent(iResult))
        {
            if ((iMin == -1 || iResult >= iMin) && (iMax == -1 || iResult <= iMax))
            {
                return IS_SUCCESS;
            }
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}

int CXMLConfig::GetString(CXMLNode* pParent, const char* szKey, std::string& strValue, int iMinLength, int iMaxLength)
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (pNode)
    {
        // Grab the string
        strValue = pNode->GetTagContent();

        // Check the lengths
        size_t sizeString = strValue.length();
        if ((iMinLength == -1 || sizeString >= static_cast<unsigned int>(iMinLength)) &&
            (iMaxLength == -1 || sizeString <= static_cast<unsigned int>(iMaxLength)))
        {
            return IS_SUCCESS;
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}

int CXMLConfig::GetRGBA(CXMLNode* pParent, const char* szKey, unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A)
{
    int Status = INVALID_VALUE;

    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (pNode)
    {
        const std::string strValue = pNode->GetTagContent();

        const char* p = strValue.c_str();
        int         values[4] = {0, 0, 0, 0};
        for (int i = 0; i < 4; ++i)
        {
            while (*p && (std::isspace(static_cast<unsigned char>(*p)) || *p == ','))
                ++p;

            if (!*p)
                return INVALID_VALUE;

            char*      end = nullptr;
            const long v = std::strtol(p, &end, 10);
            if (end == p)
                return INVALID_VALUE;
            if (v < 0 || v > 255)
                return INVALID_VALUE;

            values[i] = static_cast<int>(v);
            p = end;
        }

        R = static_cast<unsigned char>(values[0]);
        G = static_cast<unsigned char>(values[1]);
        B = static_cast<unsigned char>(values[2]);
        A = static_cast<unsigned char>(values[3]);
        Status = IS_SUCCESS;
    }
    else
        Status = DOESNT_EXIST;

    return Status;
}

void CXMLConfig::SetBoolean(CXMLNode* pParent, const char* szKey, bool bValue)
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (!pNode)
        pNode = pParent->CreateSubNode(szKey);
    if (pNode)
    {
        pNode->SetTagContent(bValue);
    }
}

void CXMLConfig::SetInteger(CXMLNode* pParent, const char* szKey, int iValue)
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (!pNode)
        pNode = pParent->CreateSubNode(szKey);
    if (pNode)
    {
        pNode->SetTagContent(iValue);
    }
}

void CXMLConfig::SetString(CXMLNode* pParent, const char* szKey, const std::string& strValue)
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (!pNode)
        pNode = pParent->CreateSubNode(szKey);
    if (pNode)
    {
        pNode->SetTagContent(strValue.c_str());
    }
}

void CXMLConfig::SetRGBA(CXMLNode* pParent, const char* szKey, unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (!pNode)
        pNode = pParent->CreateSubNode(szKey);
    if (pNode)
    {
        char szBuffer[256];
        sprintf(szBuffer, "%d %d %d %d", R, G, B, A);
        pNode->SetTagContent(szBuffer);
    }
}

CXMLNode* CXMLConfig::GetNodeForSave(CXMLNode* pParent, const char* szKey)
{
    CXMLNode* pNode = pParent->FindSubNode(szKey);
    if (!pNode)
        pNode = pParent->CreateSubNode(szKey);

    return pNode;
}
