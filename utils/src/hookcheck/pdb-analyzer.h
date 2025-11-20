/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/pdb-analyzer.h
 *  PURPOSE:     Header for a class for extracting information from a PDB file
 *               using the DIA SDK provided by Microsoft. Its main focus is to
 *               identify __declspec(naked) functions, and provide information
 *               surrounding each one.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <atlbase.h>

struct IDiaDataSource;
struct IDiaSession;
struct IDiaSymbol;

using HRESULT = long;
using ULONGLONG = unsigned long long;

class PdbAnalyzer final
{
public:
    /**
     * @brief Enumerates possible errors that can occur during a load operation.
     */
    enum class LoadError
    {
        None,
        DiaSdkNotFound,
        PdbNotLoadable,
        NoSession,
        NoGlobalSymbol,
    };

    /**
     * @brief Represents a label with a name and a virtual address.
     */
    struct Label
    {
        std::wstring Name;
        ULONGLONG    VirtualAddress{};
    };

    /**
     * @brief Represents information about a function, including its name, source location, attributes, and associated labels.
     */
    struct Function
    {
        std::wstring Name;
        std::wstring SourceFile;
        DWORD        SourceLine{};
        ULONGLONG    VirtualAddress{};
        BOOL         IsStatic{};
        BOOL         HasInlineAssembly{};

        std::vector<Label> Labels;
    };

public:
    PdbAnalyzer();
    ~PdbAnalyzer();

    PdbAnalyzer(const PdbAnalyzer&) = delete;
    PdbAnalyzer& operator=(const PdbAnalyzer&) = delete;

    PdbAnalyzer(PdbAnalyzer&&) noexcept = default;
    PdbAnalyzer& operator=(PdbAnalyzer&&) noexcept = default;

public:
    /**
     * @brief Loads a PDB (Program Database) file.
     * @param pdb The path to the PDB file to load.
     * @return A pair consisting of a LoadError value indicating the generic error of the load operation,
     *         and an HRESULT code providing additional status information.
     */
    [[nodiscard]]
    auto LoadPdb(const std::wstring& pdb) -> std::pair<LoadError, HRESULT>;

    /**
     * @brief Performs an analysis on the PDB.
     * @return An HRESULT value indicating the success or failure of the analysis operation.
     */
    [[nodiscard]]
    auto Analyze() -> HRESULT;

    /**
     * @brief Returns all __declspec(naked) functions found in the PDB.
     * @return A reference to the std::vector<Function> containing the functions. The result should not be discarded.
     */
    [[nodiscard]]
    auto GetFunctions() -> std::vector<Function>&
    {
        return m_functions;
    }

    /**
     * @brief Returns all __declspec(naked) functions found in the PDB.
     * @return A reference to the std::vector<Function> containing the functions. The result should not be discarded.
     */
    [[nodiscard]]
    auto GetFunctions() const -> const std::vector<Function>&
    {
        return m_functions;
    }

private:
    void AnalyzeFunction(IDiaSymbol* functionSymbol);

    void AttachLocation(IDiaSymbol* functionSymbol, Function& attachTo);

    void AttachLabels(IDiaSymbol* functionSymbol, Function& attachTo);

private:
    CComPtr<IDiaDataSource> m_dataSource;
    CComPtr<IDiaSession>    m_session;
    CComPtr<IDiaSymbol>     m_globalSymbol;
    std::vector<Function>   m_functions;
};
