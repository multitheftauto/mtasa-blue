/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/utility.cpp
 *  PURPOSE:     Implementation for utility functions, for example for formatted output.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "utility.h"
#include <filesystem>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void Trim(std::wstring& input)
{
    if (input.empty())
        return;

    if (size_t position = input.find_first_not_of(L" \r\n\t\0"); position != std::string_view::npos && position > 0)
        input = input.substr(position);

    if (size_t position = input.find_last_not_of(L" \r\n\t\0"); position != std::string_view::npos)
        input = input.substr(0, position + 1);
}

void NormalizePath(std::wstring& input)
{
    if (input.empty())
        return;

    std::error_code ec{};

    if (const auto canon = std::filesystem::canonical(input, ec); !ec)
        input = canon.wstring();

    std::replace(input.begin(), input.end(), L'/', L'\\');
}

auto ToWideString(const char* input) -> std::wstring
{
    int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
    std::wstring wide(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, input, -1, &wide[0], length);
    Trim(wide);
    return wide;
}
