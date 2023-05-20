/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaUTFDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaUTFDefs.h"
#include "lua/CLuaFunctionParser.h"

auto UtfLen(std::string strInput) -> int
{
    return MbUTF8ToUTF16(strInput).size();
}

auto UtfSeek(std::string strInput, int iPos) -> std::variant<bool, int>
{
    std::wstring strUTF = MbUTF8ToUTF16(strInput);
    if (iPos <= static_cast<int>(strUTF.size()) && iPos >= 0)
    {
        strUTF = strUTF.substr(0, iPos);
        return (int)UTF16ToMbUTF8(strUTF).size();
    }
    return false;
}

auto UtfSub(std::string strInput, int iStart, int iEnd) -> std::string
{
    std::wstring strUTF = MbUTF8ToUTF16(strInput);
    size_t       l = strUTF.size();

    // posrelat them both
    if (iStart < 0)
        iStart += (ptrdiff_t)l + 1;
    iStart = (iStart >= 0) ? iStart : 0;

    if (iEnd < 0)
        iEnd += (ptrdiff_t)l + 1;
    iEnd = (iEnd >= 0) ? iEnd : 0;

    if (iStart < 1)
        iStart = 1;
    if (iEnd > (ptrdiff_t)l)
        iEnd = (ptrdiff_t)l;
    if (iStart <= iEnd)
    {
        strUTF = strUTF.substr(iStart - 1, iEnd - iStart + 1);
        return UTF16ToMbUTF8(strUTF).c_str();
    }
    return "";
}

auto UtfChar(int iCode) -> std::string
{
    if (iCode > 65534 || iCode < 32)
    {
        throw std::invalid_argument("characterCode out of range, expected number between 32 and 65534.");
    }

    // Generate a null-terminating string for our character
    wchar_t wUNICODE[2] = {static_cast<wchar_t>(iCode), '\0'};

    // Convert our UTF character into an ANSI string
    SString strANSI = UTF16ToMbUTF8(wUNICODE);

    return strANSI;
}

auto UtfCode(std::string strInput) -> int
{
    std::wstring  strUTF = MbUTF8ToUTF16(strInput);
    unsigned long ulCode = strUTF.c_str()[0];
    return ulCode;
}

void CLuaUTFDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"utfLen", ArgumentParserWarn<false, UtfLen>},     {"utfSeek", ArgumentParserWarn<false, UtfSeek>}, {"utfSub", ArgumentParserWarn<false, UtfSub>},
        {"utfChar", ArgumentParserWarn<nullptr, UtfChar>}, {"utfCode", ArgumentParserWarn<false, UtfCode>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}
