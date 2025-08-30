/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/hookcheck.cpp
 *  PURPOSE:     A tool to inspect the value of __LOCAL_SIZE inside
 *               __declspec(naked) hook functions, to verify that its value is
 *               equal to zero. It uses the PDB file to identify these functions
 *               and then provides a list of problems, if any were identified.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#define TOOL_NAME     "hookcheck"
#define TOOL_VERSION  "1.0.0"
#define DEFAULT_LABEL "_localSize"

#include "dll-analyzer.h"
#include "pdb-analyzer.h"
#include "utility.h"

#include <filesystem>
#include <string>
#include <algorithm>
#include <array>
#include <format>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fs = std::filesystem;

int wmain(int argc, wchar_t* argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    if (argc < 2)
    {
        PrintLn(L"Usage: " TOOL_NAME L" <dll_path> [<label:" DEFAULT_LABEL L">]");
        return 1;
    }

    const fs::path dllFilePath = fs::absolute(argv[1]);
    std::wstring   labelName = (argc >= 3) ? argv[2] : (L"" DEFAULT_LABEL);

    if (!fs::exists(dllFilePath) || !fs::is_regular_file(dllFilePath))
    {
        PrintErrorLn(L"{} is not a regular file", dllFilePath.wstring());
        return 1;
    }

    Trim(labelName);

    if (labelName.empty())
    {
        PrintWarningLn(L"'label' program argument is empty, using " DEFAULT_LABEL);
        labelName = (L"" DEFAULT_LABEL);
    }

#ifdef MTA_DEBUG
    PrintLn(L"[~] " TOOL_NAME L" " TOOL_VERSION);
    PrintLn(L"[~] Label: '{}'", labelName);
    PrintLn(L"[~] DLL:   '{}'", dllFilePath.wstring());
#endif

    DllAnalyzer dll;

    if (auto [errorCode, hr] = dll.LoadDll(dllFilePath.wstring()); errorCode != DllAnalyzer::LoadError::None)
    {
        std::wstring message = L"An unknown error has occurred in DllAnalyzer::LoadDll";

        switch (errorCode)
        {
            using enum DllAnalyzer::LoadError;

            case OpenDll:
                message = L"CreateFileW has failed for the DLL";
                break;
            case CreateMapping:
                message = L"CreateFileMappingW has failed for the DLL";
                break;
            case CreateView:
                message = L"MapViewOfFile has failed for the DLL";
                break;
            case DosSignature:
                message = L"Unexpected signature in IMAGE_DOS_HEADER";
                break;
            case NtSignature:
                message = L"Unexpected signature in IMAGE_NT_HEADERS";
                break;
            case ImageArch:
                message = L"This tool supports only 32-Bit DLLs";
                break;
            default:
                break;
        }

        PrintErrorLn(hr, L"{}", message);
        return 1;
    }

    fs::path pdbFilePath;

    if (auto [errorCode, filePath] = dll.ExtractPdbPath(); errorCode != DllAnalyzer::ExtractError::None)
    {
        std::wstring message = L"An unknown error has occurred in DllAnalyzer::ExtractPdbPath";

        switch (errorCode)
        {
            using enum DllAnalyzer::ExtractError;

            case NotLoaded:
                message = L"You must load a DLL before trying to extract a PDB path";
                break;
            case NoDebugDirectory:
                message = L"IMAGE_DIRECTORY_ENTRY_DEBUG not found in data directory";
                break;
            case NotFound:
                message = L"CodeView entry with PDB information not found in IMAGE_DIRECTORY_ENTRY_DEBUG";
                break;
            default:
                break;
        }

        PrintErrorLn(L"{}", message);
        return 1;
    }
    else
    {
        pdbFilePath = fs::absolute(filePath);
    }

#ifdef MTA_DEBUG
    PrintLn(L"[~] PDB:   '{}'", pdbFilePath.wstring());
#endif

    if (!fs::exists(pdbFilePath) || !fs::is_regular_file(pdbFilePath))
    {
        PrintErrorLn(L"{} is not a regular file", pdbFilePath.wstring());
        return 1;
    }

    if (HRESULT hr = CoInitialize(nullptr); FAILED(hr))
    {
        PrintErrorLn(hr, L"CoInitialize has failed");
        return 1;
    }

    PdbAnalyzer pdb;

    if (auto [errorCode, hr] = pdb.LoadPdb(pdbFilePath.wstring()); errorCode != PdbAnalyzer::LoadError::None)
    {
        std::wstring message = L"An unknown error has occurred in PdbAnalyzer::LoadPdb";

        switch (errorCode)
        {
            using enum PdbAnalyzer::LoadError;

            case DiaSdkNotFound:
                message = L"Failed to create DIA data source. Is DIA SDK installed?";
                break;
            case PdbNotLoadable:
                message = L"Failed to load the PDB file";
                break;
            case NoSession:
                message = L"Failed to open a DIA session";
                break;
            case NoGlobalSymbol:
                message = L"Failed to retrieve the global symbol";
                break;
            default:
                break;
        }

        PrintErrorLn(hr, L"{}", message);
        return 1;
    }

    if (auto hr = pdb.Analyze(); FAILED(hr))
    {
        PrintErrorLn(hr, L"Analysis of PDB has failed");
        return 1;
    }

    const std::vector<PdbAnalyzer::Function>& functions = pdb.GetFunctions();

    if (functions.empty())
        return 0;

    int exitCode = 0;

    const auto isOurLabel = [&labelName](const PdbAnalyzer::Label& l) { return l.Name == labelName; };

    for (const PdbAnalyzer::Function& function : functions)
    {
        const auto label = std::find_if(std::begin(function.Labels), std::end(function.Labels), isOurLabel);
        const bool hasLabel = label != std::end(function.Labels);

        std::vector<std::wstring> problems;

        if (function.Name.starts_with(L"std::") || function.Name.starts_with(L'_'))
        {
            if (!hasLabel)
                continue;

            problems.emplace_back(L"warning: rename this naked hook function, to not start with an underscore, and neither with 'std::'");
        }

        if (!hasLabel)
        {
            problems.emplace_back(L"error: a naked hook function must use MTA_VERIFY_HOOK_LOCAL_SIZE");
            exitCode = 1;
        }

        if (!function.HasInlineAssembly)
        {
            problems.emplace_back(L"warning: a naked hook function should contain inline assembly");
        }

        if (!function.IsStatic)
        {
            problems.emplace_back(L"warning: a naked hook function should be static");
        }

        if (hasLabel)
        {
            std::optional<DWORD> localSize = dll.GetLocalSize(static_cast<DWORD>(label->VirtualAddress));

            if (localSize.has_value())
            {
                if (localSize.value() > 0)
                {
                    problems.emplace_back(std::format(L"error: naked hook function is using local space (__LOCAL_SIZE is {} bytes)", localSize.value()));
                    exitCode = 1;
                }
            }
            else
            {
                problems.emplace_back(L"error: failed to extract the value of __LOCAL_SIZE");
                exitCode = 1;
            }
        }

        if (problems.empty())
            continue;

        if (function.SourceFile.empty())
        {
            PrintLn(L"");
            PrintLn(L"[>] {}", function.Name);

            for (const std::wstring& problem : problems)
                PrintLn(L"[-] {}", problem);
        }
        else
        {
            for (const std::wstring& problem : problems)
                PrintLn(L"{}({}): {}", function.SourceFile, function.SourceLine, problem);
        }
    }

    return exitCode;
}
