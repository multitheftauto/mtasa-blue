/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/dll-analyzer.cpp
 *  PURPOSE:     Implementation for a class that can extract both the PDB file
 *               path and the value of the __LOCAL_SIZE assignment from our
 *               special macro.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "dll-analyzer.h"

#include <string>
#include <cstddef>
#include <filesystem>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace fs = std::filesystem;

static const std::string EmptyString;

struct CV_INFO_PDB70
{
    DWORD CvSignature;
    GUID  Guid;
    DWORD Age;
    char  PdbFileName[1];
};

DllAnalyzer::DllAnalyzer() = default;

DllAnalyzer::~DllAnalyzer()
{
    Unload();
}

auto DllAnalyzer::LoadDll(const std::wstring& dll) -> std::pair<LoadError, HRESULT>
{
    using enum LoadError;

    HANDLE fileHandle = CreateFileW(dll.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        const auto hr = HRESULT_FROM_WIN32(GetLastError());
        return std::make_pair(OpenDll, hr);
    }

    HANDLE mapping = CreateFileMappingW(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!mapping)
    {
        const auto hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(fileHandle);
        return std::make_pair(CreateMapping, hr);
    }

    LPVOID fileView = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if (!fileView)
    {
        const auto hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(mapping);
        CloseHandle(fileHandle);
        return std::make_pair(CreateView, hr);
    }

    const auto base = reinterpret_cast<const std::byte*>(fileView);
    const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(fileView);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
    {
        UnmapViewOfFile(fileView);
        CloseHandle(mapping);
        CloseHandle(fileHandle);
        return std::make_pair(DosSignature, E_UNEXPECTED);
    }

    const auto nt = reinterpret_cast<const _IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE)
    {
        UnmapViewOfFile(fileView);
        CloseHandle(mapping);
        CloseHandle(fileHandle);
        return std::make_pair(NtSignature, E_UNEXPECTED);
    }

    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        UnmapViewOfFile(fileView);
        CloseHandle(mapping);
        CloseHandle(fileHandle);
        return std::make_pair(ImageArch, E_UNEXPECTED);
    }

    m_file = fileHandle;
    m_mapping = mapping;
    m_view = fileView;
    m_nt = nt;
    return std::make_pair(None, S_OK);
}

void DllAnalyzer::Unload()
{
    if (m_view)
    {
        UnmapViewOfFile(m_view);
        m_view = {};
        m_nt = {};
    }

    if (m_mapping)
    {
        CloseHandle(m_mapping);
        m_mapping = {};
    }

    if (m_file)
    {
        CloseHandle(m_file);
        m_file = {};
    }
}

auto DllAnalyzer::ExtractPdbPath() const -> std::pair<ExtractError, std::string>
{
    using enum ExtractError;

    if (!m_nt)
        return std::make_pair(NotLoaded, EmptyString);

    const auto base = reinterpret_cast<const std::byte*>(m_view);
    const auto opt32 = reinterpret_cast<const IMAGE_OPTIONAL_HEADER32*>(&m_nt->OptionalHeader);
    const auto ddir = &opt32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

    if (!ddir->Size)
        return std::make_pair(NoDebugDirectory, EmptyString);

    const auto rva = ddir->VirtualAddress;
    auto       section = IMAGE_FIRST_SECTION(m_nt);

    for (WORD i = 0; i < m_nt->FileHeader.NumberOfSections; ++i, ++section)
    {
        const auto rvaBegin = section->VirtualAddress;
        const auto rvaEnd = rvaBegin + section->SizeOfRawData;

        if (rva < rvaBegin || rva >= rvaEnd)
            continue;

        auto        entry = reinterpret_cast<const IMAGE_DEBUG_DIRECTORY*>(base + section->PointerToRawData + (rva - rvaBegin));
        const DWORD numEntries = ddir->Size / sizeof(IMAGE_DEBUG_DIRECTORY);

        for (DWORD j = 0; j < numEntries; ++j, ++entry)
        {
            if (entry->Type == IMAGE_DEBUG_TYPE_CODEVIEW)
            {
                const auto info = reinterpret_cast<const CV_INFO_PDB70*>(base + entry->PointerToRawData);

                if (info->CvSignature == 'SDSR')
                {
                    return std::make_pair(None, std::string{ info->PdbFileName });
                }
            }
        }

        break;
    }

    return std::make_pair(NotFound, EmptyString);
}

auto DllAnalyzer::GetLocalSize(DWORD virtualAddress) -> std::optional<DWORD>
{
    if (!m_nt)
        return std::nullopt;

    const auto base = reinterpret_cast<const std::byte*>(m_view);
    auto       section = IMAGE_FIRST_SECTION(m_nt);

    for (WORD i = 0; i < m_nt->FileHeader.NumberOfSections; ++i, ++section)
    {
        const auto rvaBegin = section->VirtualAddress;
        const auto rvaEnd = rvaBegin + section->SizeOfRawData;

        if (virtualAddress < rvaBegin || virtualAddress >= rvaEnd)
            continue;

        // B8 00 00 00 00 | mov eax, __LOCAL_SIZE
        // We must skip a singular byte (0xB8 for mov) to retrieve the value of __LOCAL_SIZE.
        auto value = reinterpret_cast<const DWORD*>(base + section->PointerToRawData + (virtualAddress - rvaBegin) + 1);
        return *value;
    }

    return std::nullopt;
}
