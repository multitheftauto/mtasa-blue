/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SString.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <cstdarg>
#include "SharedUtil.IntTypes.h"

#ifdef WIN32
#ifndef va_copy
    #define va_copy(dest, orig) (dest) = (orig)
#endif
#endif

class SString : public std::string
{
public:
    // Constructors
    SString() : std::string() {}

    SString(const char* szText) : std::string(szText ? szText : "") {}

    explicit SString(const char* szFormat, ...) : std::string()
    {
        if (szFormat)
        {
            va_list vl;

            va_start(vl, szFormat);
            vFormat(szFormat, vl);
            va_end(vl);
        }
    }

    SString(const std::string& strText) : std::string(strText) {}

    SString& Format(const char* szFormat, ...)
    {
        va_list vl;

        va_start(vl, szFormat);
        SString& str = vFormat(szFormat, vl);
        va_end(vl);

        return str;
    }

    SString& vFormat(const char* szFormat, va_list vl);
    void     OnFormatException(const char* szFormat);
    void     OnInvalidParameter(const char* szFormat);

    // Access
    char& operator[](int iOffset) { return std::string::operator[](iOffset); }

    // Operators
    SString operator+(const char* other) const { return std::string(*this) + other; }
    SString operator+(const std::string& other) const { return std::string(*this) + other; }
    SString operator+(const SString& other) const { return std::string(*this) + other; }

    // Assignment
                operator const char*() const { return c_str(); }            // Auto assign to const char* without using c_str()
    const char* operator*() const { return c_str(); }

    // Functions
    void           Split(const SString& strDelim, std::vector<SString>& outResult, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0) const;
    bool           Split(const SString& strDelim, SString* pstrLeft, SString* pstrRight, int iIndex = 1) const;
    SString        SplitLeft(const SString& strDelim, SString* pstrRight = NULL, int iIndex = 1) const;
    SString        SplitRight(const SString& strDelim, SString* pstrLeft = NULL, int iIndex = 1) const;
    SString        Replace(const char* szOld, const char* szNew, bool bSearchJustReplaced = false) const;
    SString        ReplaceI(const char* szOld, const char* szNew, bool bSearchJustReplaced = false) const;
    SString        TrimStart(const char* szOld) const;
    SString        TrimEnd(const char* szOld) const;
    SString        ToLower() const;
    SString        ToUpper() const;
    SString        ConformLineEndings() const;
    bool           Contains(const SString& strOther) const;
    bool           ContainsI(const SString& strOther) const;
    bool           CompareI(const SString& strOther) const;
    bool           EqualsI(const SString& strOther) const { return CompareI(strOther); }
    SString        SubStr(int iPos, int iCount = 0x3fffffff) const;
    SString        Left(int iCount) const;
    SString        Right(int iCount) const;
    bool           EndsWith(const SString& strOther) const;
    bool           EndsWithI(const SString& strOther) const;
    bool           BeginsWith(const SString& strOther) const;
    bool           BeginsWithI(const SString& strOther) const;
    static SString Join(const SString& strDelim, const std::vector<SString>& parts, int iFirst = 0, int iCount = 0x3fffffff);
    void           AssignLeft(const char* szOther, uint uiMaxLength);
};

class SStringX : public SString
{
public:
    SStringX(const char* szText) : SString(std::string(szText ? szText : "")) {}
    SStringX(const char* szText, uint uiLength) : SString(std::string(szText ? szText : "", uiLength)) {}
};

//
// SCharStringRef
//
// String reference - Used for direct access to Lua strings
//
struct SCharStringRef
{
    SCharStringRef() : pData(NULL), uiSize(0) {}
    char*  pData;
    size_t uiSize;
};

//
// Faster type of SString::Split
// Uses pointers to a big buffer rather than an array of strings
//
template <class STRING_TYPE, class CHAR_TYPE>
class TSplitString : public std::vector<const CHAR_TYPE*>
{
    using std::vector<const CHAR_TYPE*>::push_back;

public:
    TSplitString() {}
    TSplitString(const STRING_TYPE& strInput, const STRING_TYPE& strDelim, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0)
    {
        Split(strInput, strDelim, uiMaxAmount, uiMinAmount);
    }

    void Split(const STRING_TYPE& strInput, const STRING_TYPE& strDelim, unsigned int uiMaxAmount = 0, unsigned int uiMinAmount = 0)
    {
        // Copy string to buffer
        uint iInputLength = strInput.length();
        buffer.resize(iInputLength + 1);
        memcpy(&buffer[0], &strInput[0], (iInputLength + 1) * sizeof(CHAR_TYPE));

        // Prime result list
        this->clear();
        this->reserve(16U < uiMaxAmount ? 16U : uiMaxAmount);

        // Split into pointers
        size_t ulCurrentPoint = 0;
        while (true)
        {
            size_t ulPos = strInput.find(strDelim, ulCurrentPoint);
            if (ulPos == STRING_TYPE::npos || (uiMaxAmount > 0 && uiMaxAmount <= this->size() + 1))
            {
                if (ulCurrentPoint <= strInput.length())
                    push_back(&buffer[ulCurrentPoint]);
                break;
            }
            push_back(&buffer[ulCurrentPoint]);
            buffer[ulPos] = 0;
            ulCurrentPoint = ulPos + strDelim.length();
        }
        while (this->size() < uiMinAmount)
            push_back(&buffer[iInputLength]);
    }

protected:
    std::vector<CHAR_TYPE> buffer;
};

typedef TSplitString<std::string, char>     CSplitString;
typedef TSplitString<std::wstring, wchar_t> CSplitStringW;
