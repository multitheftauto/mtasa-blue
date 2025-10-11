/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/dll-analyzer.h
 *  PURPOSE:     Header for a class that can extract both the PDB file path
 *               and the value of the __LOCAL_SIZE assignment from our special
 *               macro.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <utility>
#include <string>
#include <optional>

using HRESULT = long;
using HANDLE = void*;
using LPVOID = void*;
using DWORD = unsigned long;
using ULONGLONG = unsigned long long;

struct _IMAGE_NT_HEADERS;

class DllAnalyzer final
{
public:
    enum class LoadError
    {
        None,
        OpenDll,
        CreateMapping,
        CreateView,
        DosSignature,
        NtSignature,
        ImageArch,
    };

    enum class ExtractError
    {
        None,
        NotLoaded,
        NoDebugDirectory,
        NotFound,
    };

public:
    DllAnalyzer();
    ~DllAnalyzer();

    DllAnalyzer(const DllAnalyzer&) = delete;
    DllAnalyzer& operator=(const DllAnalyzer&) = delete;

    DllAnalyzer(DllAnalyzer&&) noexcept = default;
    DllAnalyzer& operator=(DllAnalyzer&&) noexcept = default;

public:
    /**
     * @brief Loads a DLL (Dynamic-Link Library) file.
     * @param dll The path to the DLL file to load.
     * @return A pair consisting of a LoadError value indicating the generic error of the load operation,
     *         and an HRESULT code providing additional status information.
     */
    [[nodiscard]]
    auto LoadDll(const std::wstring& dll) -> std::pair<LoadError, HRESULT>;

    /**
     * @brief Releases or unloads resources.
     */
    void Unload();

    /**
     * @brief Extracts the path to the PDB (Program Database) file.
     * @return A pair consisting of a ExtractError value indicating an error and an empty string on failure,
     *         or a pair consisting of ExtractError::None and the path to the PDB file on success.
     */
    [[nodiscard]]
    auto ExtractPdbPath() const -> std::pair<ExtractError, std::string>;

    /**
     * @brief Retrieves the value of __LOCAL_SIZE using the virtual address of the label before it.
     * @param virtualAddress The virtual address of the label before the assignment.
     * @return The value of __LOCAL_SIZE on success, otherwise an empty value on failure.
     */
    [[nodiscard]]
    auto GetLocalSize(DWORD virtualAddress) -> std::optional<DWORD>;

private:
    HANDLE m_file{};
    HANDLE m_mapping{};
    LPVOID m_view{};

    const _IMAGE_NT_HEADERS* m_nt{};
};
