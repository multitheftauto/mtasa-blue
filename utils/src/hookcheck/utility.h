/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/utility.h
 *  PURPOSE:     Header for utility functions, for example for formatted output.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <iostream>
#include <format>

using HRESULT = long;

void Trim(std::wstring& input);

void NormalizePath(std::wstring& input);

template <typename... Args>
void PrintLn(const std::wformat_string<Args...> fmt, Args&&... args)
{
    std::wcout << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
void PrintWarningLn(const std::wformat_string<Args...> fmt, Args&&... args)
{
    std::wcerr << L"Warning: " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
void PrintErrorLn(const std::wformat_string<Args...> fmt, Args&&... args)
{
    std::wcerr << L"Error: " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
void PrintWarningLn(const HRESULT hr, const std::wformat_string<Args...> fmt, Args&&... args)
{
    std::wcerr << std::format(L"Warning (0x{:08x}): ", static_cast<unsigned long>(hr))
               << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
void PrintErrorLn(const HRESULT hr, const std::wformat_string<Args...> fmt, Args&&... args)
{
    std::wcerr << std::format(L"Error (0x{:08x}): ", static_cast<unsigned long>(hr))
               << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}
