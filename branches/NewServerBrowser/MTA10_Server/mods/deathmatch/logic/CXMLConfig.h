/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CXMLConfig.h
*  PURPOSE:     XML-based configuration parsing routines class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLConfig_H
#define __CXMLConfig_H

#include "CCommon.h"
#include <string>

class CXMLConfig
{
public:
                                CXMLConfig          ( const char* szFileName );
                                ~CXMLConfig         ( void );

    inline std::string&         GetFileName         ( void )                    { return m_strFileName; }
    void                        SetFileName         ( const char* szFileName );
    inline CXMLFile*            GetFile             ( void )                    { return m_pFile; }

    virtual bool                Load                ( const char* szFileName = NULL ) = 0;
    virtual bool                Save                ( const char* szFileName = NULL ) = 0;

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
    void                        SetString           ( class CXMLNode* pParent, const char* szKey, const std::string& strValue );
    void                        SetRGBA             ( class CXMLNode* pParent, const char* szKey, unsigned char R, unsigned char G, unsigned char B, unsigned char A );

    CXMLNode*                   GetNodeForSave      ( CXMLNode* pParent, const char* szKey );

    std::string                 m_strFileName;
    CXMLFile*                   m_pFile;
};

#endif
