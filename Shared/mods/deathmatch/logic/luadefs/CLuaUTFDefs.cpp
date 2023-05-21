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

auto UtfLen(std::string input) -> int
{
    return MbUTF8ToUTF16(input).size();
}

auto UtfSeek(std::string input, int position) -> std::variant<bool, int>
{
    std::wstring utfString = MbUTF8ToUTF16(input);
    if (position <= static_cast<int>(utfString.size()) && position >= 0)
    {
        utfString = utfString.substr(0, position);
        return (int)UTF16ToMbUTF8(utfString).size();
    }
    return false;
}

auto UtfSub(std::string input, int start, int end) -> std::string
{
    std::wstring utfString = MbUTF8ToUTF16(input);
    size_t       length = utfString.size();

    // posrelat them both
    if (start < 0)
        start += (ptrdiff_t)length + 1;
    start = (start >= 0) ? start : 0;

    if (end < 0)
        end += (ptrdiff_t)length + 1;
    end = (end >= 0) ? end : 0;

    if (start < 1)
        start = 1;
    if (end > (ptrdiff_t)length)
        end = (ptrdiff_t)length;
    if (start <= end)
    {
        utfString = utfString.substr(start - 1, end - start + 1);
        return UTF16ToMbUTF8(utfString).c_str();
    }
    return "";
}

auto UtfChar(int code) -> std::string
{
    if (code > 65534 || code < 32)
    {
        throw std::invalid_argument("characterCode out of range, expected number between 32 and 65534.");
    }

    // Generate a null-terminating string for our character
    wchar_t string[2] = {static_cast<wchar_t>(code), '\0'};

    // Convert our UTF character into an ANSI string
    return UTF16ToMbUTF8(string);
}

auto UtfCode(std::string input) -> int
{
    std::wstring  utfString = MbUTF8ToUTF16(input);
    unsigned long code = utfString.c_str()[0];
    return code;
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
