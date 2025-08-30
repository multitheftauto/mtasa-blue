/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/hookcheck/pdb-analyzer.cpp
 *  PURPOSE:     Implementation for a class for extracting information from a PDB
 *               file using the DIA SDK provided by Microsoft. Its main focus is
 *               to identify __declspec(naked) functions, and provide information
 *               surrounding each one.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "pdb-analyzer.h"
#include "utility.h"

#include <iostream>
#include <format>
#include <array>

#include <dia2.h>
#include <diacreate.h>

#pragma comment(lib, "diaguids.lib")

#if 0
static constexpr auto kTags = std::to_array({
    SymTagExe,
    SymTagCompiland,
    SymTagCompilandDetails,
    SymTagCompilandEnv,
    SymTagFunction,
    SymTagBlock,
    SymTagData,
    SymTagAnnotation,
    SymTagLabel,
    SymTagPublicSymbol,
    SymTagUDT,
    SymTagEnum,
    SymTagFunctionType,
    SymTagPointerType,
    SymTagArrayType,
    SymTagBaseType,
    SymTagTypedef,
    SymTagBaseClass,
    SymTagFriend,
    SymTagFunctionArgType,
    SymTagFuncDebugStart,
    SymTagFuncDebugEnd,
    SymTagUsingNamespace,
    SymTagVTableShape,
    SymTagVTable,
    SymTagCustom,
    SymTagThunk,
    SymTagCustomType,
    SymTagManagedType,
    SymTagDimension,
    SymTagCallSite,
    SymTagInlineSite,
    SymTagBaseInterface,
    SymTagVectorType,
    SymTagMatrixType,
    SymTagHLSLType,
    SymTagCaller,
    SymTagCallee,
    SymTagExport,
    SymTagHeapAllocationSite,
    SymTagCoffGroup,
    SymTagInlinee,
    SymTagTaggedUnionCase,
});

static constexpr auto kTagNames = std::to_array({
    L"SymTagExe",
    L"SymTagCompiland",
    L"SymTagCompilandDetails",
    L"SymTagCompilandEnv",
    L"SymTagFunction",
    L"SymTagBlock",
    L"SymTagData",
    L"SymTagAnnotation",
    L"SymTagLabel",
    L"SymTagPublicSymbol",
    L"SymTagUDT",
    L"SymTagEnum",
    L"SymTagFunctionType",
    L"SymTagPointerType",
    L"SymTagArrayType",
    L"SymTagBaseType",
    L"SymTagTypedef",
    L"SymTagBaseClass",
    L"SymTagFriend",
    L"SymTagFunctionArgType",
    L"SymTagFuncDebugStart",
    L"SymTagFuncDebugEnd",
    L"SymTagUsingNamespace",
    L"SymTagVTableShape",
    L"SymTagVTable",
    L"SymTagCustom",
    L"SymTagThunk",
    L"SymTagCustomType",
    L"SymTagManagedType",
    L"SymTagDimension",
    L"SymTagCallSite",
    L"SymTagInlineSite",
    L"SymTagBaseInterface",
    L"SymTagVectorType",
    L"SymTagMatrixType",
    L"SymTagHLSLType",
    L"SymTagCaller",
    L"SymTagCallee",
    L"SymTagExport",
    L"SymTagHeapAllocationSite",
    L"SymTagCoffGroup",
    L"SymTagInlinee",
    L"SymTagTaggedUnionCase",
});

static_assert(kTags.size() == kTagNames.size());

[[maybe_unused]]
static void Dump(IDiaSymbol* symbol)
{
    for (size_t i = {}; i < kTags.size(); ++i)
    {
        CComPtr<IDiaEnumSymbols> iterator{};

        if (HRESULT hr = symbol->findChildren(kTags[i], nullptr, nsNone, &iterator); FAILED(hr))
        {
            PrintErrorLn(hr, L"Could not find children for {}", kTagNames[i]);
            continue;
        }

        LONG numSymbols{};
        if (HRESULT hr = iterator->get_Count(&numSymbols); FAILED(hr))
        {
            PrintErrorLn(hr, L"Could not get children count for {}", kTagNames[i]);
            continue;
        }

        PrintLn(L"Found {} symbols for {}", numSymbols, kTagNames[i]);
    }
}
#endif

PdbAnalyzer::PdbAnalyzer() = default;

PdbAnalyzer::~PdbAnalyzer() = default;

auto PdbAnalyzer::LoadPdb(const std::wstring& pdb) -> std::pair<LoadError, HRESULT>
{
    using enum LoadError;

    HRESULT hr = NoRegCoCreate(L"" MSDIA_DLL, CLSID_DiaSource, IID_IDiaDataSource, (void**)&m_dataSource);

    if (FAILED(hr))
        return std::make_pair(DiaSdkNotFound, hr);

    if (hr = m_dataSource->loadDataFromPdb(pdb.c_str());  FAILED(hr))
        return std::make_pair(PdbNotLoadable, hr);

    if (hr = m_dataSource->openSession(&m_session); FAILED(hr))
        return std::make_pair(NoSession, hr);

    if (hr = m_session->get_globalScope(&m_globalSymbol); FAILED(hr))
        return std::make_pair(NoGlobalSymbol, hr);

    return std::make_pair(None, S_OK);
}

auto PdbAnalyzer::Analyze() -> HRESULT
{
    CComPtr<IDiaEnumSymbols> iterator{};

    if (HRESULT hr = m_globalSymbol->findChildren(SymTagFunction, nullptr, nsNone, &iterator); FAILED(hr))
    {
        PrintErrorLn(hr, L"IDiaEnumSymbols::findChildren has failed for SymTagFunction");
        return hr;
    }

    CComPtr<IDiaSymbol> function{};
    ULONG               counter{};

    while (SUCCEEDED(iterator->Next(1, &function, &counter)) && counter == 1)
    {
        AnalyzeFunction(function);
        function.Release();
    }

    return S_OK;
}

void PdbAnalyzer::AnalyzeFunction(IDiaSymbol* functionSymbol)
{
    BSTR name{};

    // There is nothing we can really do without a function name, skip this one.
    if (FAILED(functionSymbol->get_name(&name)) || !name)
        return;

    // We are going to immediately discard any non-naked functions from the selection.
    BOOL isNaked{};

    if (HRESULT hr = functionSymbol->get_isNaked(&isNaked); FAILED(hr) || !isNaked)
    {
        if (FAILED(hr))
            PrintWarningLn(hr, L"IDiaSymbol::get_isNaked has failed for function {}", name);

        SysFreeString(name);
        return;
    }

    ULONGLONG virtualAddress{};

    if (HRESULT hr = functionSymbol->get_virtualAddress(&virtualAddress); FAILED(hr) || !virtualAddress)
    {
        PrintWarningLn(hr, L"IDiaSymbol::get_virtualAddress has failed for function {}", name);
        SysFreeString(name);
        return;
    }

    BOOL hasInlineAssembly{};

    if (HRESULT hr = functionSymbol->get_hasInlAsm(&hasInlineAssembly); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaSymbol::get_hasInlAsm has failed for function {}", name);
        SysFreeString(name);
        return;
    }

    BOOL isStatic{};

    if (HRESULT hr = functionSymbol->get_isStatic(&isStatic); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaSymbol::get_isStatic has failed for function {}", name);
        SysFreeString(name);
        return;
    }

    Function& newFunction = m_functions.emplace_back();
    newFunction.Name = name;
    newFunction.VirtualAddress = virtualAddress;
    newFunction.IsStatic = isStatic;
    newFunction.HasInlineAssembly = hasInlineAssembly;

    AttachLocation(functionSymbol, newFunction);
    NormalizePath(newFunction.SourceFile);

    if (newFunction.SourceFile.empty())
    {
        m_functions.pop_back();
        return;
    }

    AttachLabels(functionSymbol, newFunction);
}

void PdbAnalyzer::AttachLocation(IDiaSymbol* functionSymbol, Function& attachTo)
{
    ULONGLONG length{};

    if (HRESULT hr = functionSymbol->get_length(&length); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaSymbol::get_length has failed for function {}", attachTo.Name);
        return;
    }

    CComPtr<IDiaEnumLineNumbers> iterator;

    if (HRESULT hr = m_session->findLinesByVA(attachTo.VirtualAddress, static_cast<DWORD>(length), &iterator); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaSymbol::findChildren has failed for SymTagLabel for function {}", attachTo.Name);
        return;
    }

    CComPtr<IDiaLineNumber> line{};
    ULONG                   counter{};

    // In practice there can be multiple lines associated with a virtual address, but we are only going to pick the first one.
    if (HRESULT hr = iterator->Next(1, &line, &counter); FAILED(hr) || counter != 1)
    {
        PrintWarningLn(hr, L"IDiaEnumLineNumbers::Next has failed for function {}", attachTo.Name);
        return;
    }

    CComPtr<IDiaSourceFile> sourceFile;

    if (HRESULT hr = line->get_sourceFile(&sourceFile); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaLineNumber::get_sourceFile has failed for function {}", attachTo.Name);
        return;
    }

    BSTR fileName{};

    if (HRESULT hr = sourceFile->get_fileName(&fileName); FAILED(hr))
    {
        PrintWarningLn(hr, L"IDiaSourceFile::get_fileName has failed for function {}", attachTo.Name);
        return;
    }

    attachTo.SourceFile = fileName;

    // Not a problem if this fails, only the name of the source file is better than nothing.
    (void)line->get_lineNumber(&attachTo.SourceLine);

    SysFreeString(fileName);
}

void PdbAnalyzer::AttachLabels(IDiaSymbol* functionSymbol, Function& attachTo)
{
    CComPtr<IDiaEnumSymbols> iterator;

    if (HRESULT hr = functionSymbol->findChildren(SymTagLabel, nullptr, nsNone, &iterator); FAILED(hr))
    {
        PrintErrorLn(hr, L"IDiaSymbol::findChildren has failed for SymTagLabel for function {}", attachTo.Name);
        return;
    }

    CComPtr<IDiaSymbol> item{};
    ULONG               counter{};

    while (SUCCEEDED(iterator->Next(1, &item, &counter)) && counter == 1)
    {
        CComPtr<IDiaSymbol> label = item.Detach();
        BSTR                name{};

        // There is nothing we can really do without a function name, skip this one.
        if (FAILED(label->get_name(&name)) || !name)
            continue;

        ULONGLONG virtualAddress{};

        if (HRESULT hr = label->get_virtualAddress(&virtualAddress); FAILED(hr) || !virtualAddress)
        {
            PrintErrorLn(hr, L"IDiaSymbol::get_virtualAddress has failed for label {} in function {}", name, attachTo.Name);
            SysFreeString(name);
            continue;
        }

        Label& newLabel = attachTo.Labels.emplace_back();
        newLabel.Name = name;
        newLabel.VirtualAddress = virtualAddress;
    }
}
