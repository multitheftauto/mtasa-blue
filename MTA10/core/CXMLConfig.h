/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CXMLConfig.h
*  PURPOSE:     Header file for XML configuration parser class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLConfig_H
#define __CXMLConfig_H

#include <core/CCoreInterface.h>

class CXMLConfig
{
public:
                                CXMLConfig          ( char* szFileName );
                                ~CXMLConfig         ( void );

    inline char*                GetFileName         ( void )                { return m_szFileName; }
    void                        SetFileName         ( char* szFileName );
    inline class CXMLFile*      GetFile             ( void )                { return m_pFile; }

    virtual bool                Load                ( char* szFileName = NULL ) = 0;
    virtual bool                Save                ( char* szFileName = NULL ) = 0;

protected:
    enum
    {
        DOESNT_EXIST,
        INVALID_VALUE,
        IS_SUCCESS,
    };

    int                         GetBoolean          ( class CXMLNode* pParent, const char* szKey, bool& bResult );
    int                         GetInteger          ( class CXMLNode* pParent, const char* szKey, int& iResult, int iMin = -1, int iMax = -1 );
    int                         GetString           ( class CXMLNode* pParent, const char* szKey, std::string& strValue, int iMinLength = -1, int iMaxLength = -1 );
    int                         GetRGBA             ( class CXMLNode* pParent, const char* szKey, unsigned char& R, unsigned char& G, unsigned char& B, unsigned char& A );
    void                        SetBoolean          ( class CXMLNode* pParent, const char* szKey, bool bValue );
    void                        SetInteger          ( class CXMLNode* pParent, const char* szKey, int iValue );
    void                        SetString           ( class CXMLNode* pParent, const char* szKey, std::string strValue );
    void                        SetRGBA             ( class CXMLNode* pParent, const char* szKey, unsigned char R, unsigned char G, unsigned char B, unsigned char A );

    class CXMLNode*             GetNodeForSave      ( class CXMLNode* pParent, const char* szKey );

    char*                       m_szFileName;
    class CXMLFile*             m_pFile;
};

#endif