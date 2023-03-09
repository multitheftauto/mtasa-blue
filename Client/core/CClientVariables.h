/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CClientVariables.h
 *  PURPOSE:     Header file for client variable class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include "CSingleton.h"
#include <core/CCVarsInterface.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <CVector.h>
#include <CVector2D.h>
#include "CChat.h"

// Macros
#define CVARS_GET       CClientVariables::GetSingleton().Get
#define CVARS_SET       CClientVariables::GetSingleton().Set
#define CVARS_GET_VALUE CClientVariables::GetSingleton().GetValue

class CClientVariables : public CCVarsInterface, public CSingleton<CClientVariables>
{
    // Sanity macros   << Who ever did this is idiot
    #define SAN     if(!m_pStorage) return
    #define SANGET  if(!Node(strVariable)) return false

public:
    CClientVariables();
    ~CClientVariables();

    // Get queries
    bool Get(const std::string& strVariable, bool& val)
    {
        SANGET;
        Node(strVariable)->GetTagContent(val);
        return true;
    };
    bool Get(const std::string& strVariable, std::string& val)
    {
        SANGET;
        val = Node(strVariable)->GetTagContent();
        return !val.empty();
    };
    bool Get(const std::string& strVariable, int& val)
    {
        SANGET;
        Node(strVariable)->GetTagContent(val);
        return true;
    };
    bool Get(const std::string& strVariable, unsigned int& val)
    {
        SANGET;
        Node(strVariable)->GetTagContent(val);
        return true;
    };
    bool Get(const std::string& strVariable, float& val)
    {
        SANGET;
        Node(strVariable)->GetTagContent(val);
        return true;
    };
    bool Get(const std::string& strVariable, CVector& val);
    bool Get(const std::string& strVariable, CVector2D& val);
    bool Get(const std::string& strVariable, CColor& val);

    // Set queries
    void Set(const std::string& strVariable, bool val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val);
    };
    void Set(const std::string& strVariable, const char* val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val);
    };
    void Set(const std::string& strVariable, const std::string& val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val.c_str());
    };
    void Set(const std::string& strVariable, int val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val);
    };
    void Set(const std::string& strVariable, unsigned int val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val);
    };
    void Set(const std::string& strVariable, float val)
    {
        SAN;
        m_iRevision++;
        Node(strVariable)->SetTagContent(val);
    };
    void Set(const std::string& strVariable, CVector val);
    void Set(const std::string& strVariable, CVector2D val);
    void Set(const std::string& strVariable, CColor val);

    void ClampValue(const std::string& strVariable, int iMinValue, int iMaxValue);
    void ClampValue(const std::string& strVariable, float fMinValue, float fMaxValue);
    void ClampValue(const std::string& strVariable, CColor minValue, CColor maxValue);
    void ClampValue(const std::string& strVariable, CVector2D minValue, CVector2D maxValue);

    bool Exists(const std::string& strVariable);

    bool Load();
    bool IsLoaded() { return m_bLoaded; }
    int  GetRevision() { return m_iRevision; }
    void ValidateValues();

private:
    CXMLNode* Node(const std::string& strVariable);
    void      LoadDefaults();

    bool      m_bLoaded;
    CXMLNode* m_pStorage;
    int       m_iRevision;
};
