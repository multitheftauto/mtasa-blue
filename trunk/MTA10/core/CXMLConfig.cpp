/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CXMLConfig.cpp
*  PURPOSE:     XML configuration parser
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>

CXMLConfig::CXMLConfig ( char* szFileName )
{
    m_pFile = NULL;
    m_szFileName = NULL;
    SetFileName ( szFileName );
}


CXMLConfig::~CXMLConfig ( void )
{
    delete m_pFile;
    delete [] m_szFileName;
}


void CXMLConfig::SetFileName ( char* szFileName )
{
    delete [] m_szFileName;
    m_szFileName = NULL;
    if ( szFileName )
    {
        m_szFileName = new char [ strlen ( szFileName ) + 1 ];
        strcpy ( m_szFileName, szFileName );
    }
}


int CXMLConfig::GetBoolean ( CXMLNode* pParent, const char* szKey, bool& bResult )
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( pNode )
    {
        if ( pNode->GetTagContent ( bResult ) )
        {
            return IS_SUCCESS;
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}


int CXMLConfig::GetInteger ( CXMLNode* pParent, const char* szKey, int& iResult, int iMin, int iMax )
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( pNode )
    {
        if ( pNode->GetTagContent ( iResult ) )
        {
            if ( ( iMin == -1 || iResult >= iMin ) &&
                 ( iMax == -1 || iResult <= iMax ) )
            {
                return IS_SUCCESS;
            }
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}


int CXMLConfig::GetString ( CXMLNode* pParent, const char *szKey, std::string& strValue, int iMinLength, int iMaxLength )
{
    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( pNode )
    {
        // Grab the string
        strValue = pNode->GetTagContent ();

        // Check the lengths
        size_t sizeString = strValue.length ();
        if ( ( iMinLength == -1 || sizeString >= static_cast < unsigned int > ( iMinLength ) ) &&
             ( iMaxLength == -1 || sizeString <= static_cast < unsigned int > ( iMaxLength ) ) )
        {
            return IS_SUCCESS;
        }

        return INVALID_VALUE;
    }

    return DOESNT_EXIST;
}


int CXMLConfig::GetRGBA ( CXMLNode* pParent, const char* szKey, unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A )
{
    int Status = INVALID_VALUE;

    // Grab the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( pNode )
    {
        istringstream iss;
        int iR, iG, iB, iA;

        std::string strValue;
        strValue = pNode->GetTagContent ();
        iss.str ( strValue );

        try {
            iss >> iR >> iG >> iB >> iA;
            Status = IS_SUCCESS;
            R = (char)iR;
            G = (char)iG;
            B = (char)iB;
            A = (char)iA;
        } catch (std::ios::failure e) {}
    } else
        Status = DOESNT_EXIST;
    return Status;
}


void CXMLConfig::SetBoolean ( CXMLNode* pParent, const char* szKey, bool bValue )
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( !pNode ) pNode = pParent->CreateSubNode ( szKey );
    if ( pNode )
    {
        pNode->SetTagContent ( bValue );
    }
}


void CXMLConfig::SetInteger ( CXMLNode* pParent, const char* szKey, int iValue )
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( !pNode ) pNode = pParent->CreateSubNode ( szKey );
    if ( pNode )
    {
        pNode->SetTagContent ( iValue );
    }
}


void CXMLConfig::SetString ( CXMLNode* pParent, const char* szKey, std::string strValue )
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( !pNode ) pNode = pParent->CreateSubNode ( szKey );
    if ( pNode )
    {
        pNode->SetTagContent ( strValue.c_str () );
    }
}


void CXMLConfig::SetRGBA ( CXMLNode* pParent, const char* szKey, unsigned char R, unsigned char G, unsigned char B, unsigned char A )
{
    // Create the XML node
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( !pNode ) pNode = pParent->CreateSubNode ( szKey );
    if ( pNode )
    {
        char szBuffer [ 256 ];
        sprintf ( szBuffer, "%u %u %u %u", R, G, B, A );
        pNode->SetTagContent ( szBuffer );
    }
}


CXMLNode* CXMLConfig::GetNodeForSave ( CXMLNode* pParent, const char* szKey )
{
    CXMLNode* pNode = pParent->FindSubNode ( szKey );
    if ( !pNode ) pNode = pParent->CreateSubNode ( szKey );    

    return pNode;
}
