/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaUTFDefs.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaUTFDefs.h"
#include "lua/CLuaFunctionParser.h"

auto UtfLen(std::string input) -> int
{
    return MbUTF8ToUTF16(input).size();
}

auto UtfSeek(std::string input, const int position) -> std::variant<bool, int>
{
    if (std::wstring utfString = MbUTF8ToUTF16(input); position <= static_cast<int>(utfString.size()) && position >= 0)
    {
        utfString = utfString.substr(0, position);

        return static_cast<int>(UTF16ToMbUTF8(utfString).size());
    }

    return false;
}

auto UtfSub(std::string input, int start, int end) -> std::string
{
    std::wstring    utfString = MbUTF8ToUTF16(input);
    const ptrdiff_t length = utfString.size();

    // posrelat them both
    if (start < 0)
    {
        start += length + 1;
    }

    start = (start >= 0) ? start : 0;

    if (end < 0)
    {
        end += length + 1;
    }

    end = (end >= 0) ? end : 0;

    if (start < 1)
    {
        start = 1;
    }

    if (end > length)
    {
        end = length;
    }

    if (start <= end)
    {
        utfString = utfString.substr(start - 1, end - start + 1);

        return UTF16ToMbUTF8(utfString);
    }

    return "";
}

auto UtfChar(const int code) -> std::string
{
    if (code > 65534 || code < 32)
    {
        throw std::invalid_argument("characterCode out of range, expected number between 32 and 65534.");
    }

    // Generate a null-terminating string for our character
    const wchar_t string[2] = {static_cast<wchar_t>(code), '\0'};

    // Convert our UTF character into an ANSI string
    return UTF16ToMbUTF8(string);
}

auto UtfCode(std::string input) -> int
{
    return static_cast<unsigned long>(MbUTF8ToUTF16(input).c_str()[0]);
}

void CLuaUTFDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"utfLen", ArgumentParserWarn<false, UtfLen>},     {"utfSeek", ArgumentParserWarn<false, UtfSeek>}, {"utfSub", ArgumentParserWarn<false, UtfSub>},
        {"utfChar", ArgumentParserWarn<nullptr, UtfChar>}, {"utfCode", ArgumentParserWarn<false, UtfCode>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
    {
        CLuaCFunctions::AddFunction(name, func);
    }
}
