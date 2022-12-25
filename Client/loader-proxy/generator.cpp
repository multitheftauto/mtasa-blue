/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        loader-proxy/generator.cpp
 *  PURPOSE:     Generates setup.cpp, winmm.asm and exports.def for the loader proxy
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <type_traits>
#include <cstdio>
#include <windows.h>
#include <shlobj.h>
#include <ImageHlp.h>

#pragma comment(lib, "dbghelp.lib")

struct HandleScopeDeleter
{
    void operator()(HANDLE object) const noexcept { CloseHandle(object); }
};

using HandleScope = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleScopeDeleter>;

struct ViewScopeDeleter
{
    void operator()(const void* baseAddress) const noexcept { UnmapViewOfFile(baseAddress); }
};

using ViewScope = std::unique_ptr<const void, ViewScopeDeleter>;

auto GetKnownFolderPath(REFKNOWNFOLDERID id, DWORD flags = 0) -> std::filesystem::path
{
    wchar_t* path{};

    if (HRESULT hr = SHGetKnownFolderPath(id, flags, nullptr, &path); SUCCEEDED(hr))
    {
        std::filesystem::path result(path);
        CoTaskMemFree(path);
        return result;
    }

    return {};
}

struct ImageExport
{
    uint32_t    ordinal{};
    uint32_t    hint{};
    uintptr_t   address{};
    std::string name{};
    std::string forwarded{};
};

auto GetImageExports(const std::wstring& filePath) -> std::vector<ImageExport>
{
    HANDLE fileHandle = CreateFileW(
        /* FileName            */ filePath.c_str(),
        /* DesiredAccess       */ GENERIC_READ,
        /* ShareMode           */ FILE_SHARE_READ,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ FILE_ATTRIBUTE_NORMAL,
        /* TemplateFile        */ nullptr);

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        std::fprintf(stderr, "Error: GetImageExports is unable to open '%ls'\n", filePath.c_str());
        return {};
    }

    // Automatically close the handle to the file.
    HandleScope fileHandleScope(fileHandle);

    HANDLE mappingHandle = CreateFileMappingW(
        /* File                  */ fileHandle,
        /* FileMappingAttributes */ nullptr,
        /* Protect               */ PAGE_READONLY,
        /* MaximumSizeHigh       */ 0,
        /* MaximumSizeLow        */ 0,
        /* Name                  */ nullptr);

    if (!mappingHandle)
    {
        std::fprintf(stderr, "Error: GetImageExports is unable to create file mapping for '%ls'", filePath.c_str());
        return {};
    }

    // Automatically close the handle to the file mapping.
    HandleScope mappingHandleScope(mappingHandle);

    void* baseAddress = MapViewOfFile(mappingHandle, FILE_MAP_READ, 0, 0, 0);

    if (!baseAddress)
    {
        std::fprintf(stderr, "Error: GetImageExports is unable to map view of '%ls'", filePath.c_str());
        return {};
    }

    // Automatically close the map view of the file.
    ViewScope baseAddressScope(baseAddress);

    ULONG                 directorySize{};
    IMAGE_SECTION_HEADER* sectionHeader{};

    auto exportsDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(ImageDirectoryEntryToDataEx(
        /* Base           */ baseAddress,
        /* MappedAsImage  */ FALSE,
        /* DirectoryEntry */ IMAGE_DIRECTORY_ENTRY_EXPORT,
        /* Size           */ &directorySize,
        /* FoundHeader    */ &sectionHeader));

    if (!directorySize || !exportsDirectory)
    {
        std::fprintf(stderr, "Error: IMAGE_DIRECTORY_ENTRY_EXPORT not found or empty in '%ls'", filePath.c_str());
        return {};
    }

    if (!exportsDirectory->NumberOfFunctions)
    {
        std::fprintf(stderr, "Error: IMAGE_EXPORT_DIRECTORY is empty in '%ls'", filePath.c_str());
        return {};
    }

    std::vector<ImageExport> exports;
    exports.reserve(exportsDirectory->NumberOfFunctions);

    IMAGE_NT_HEADERS* ntHeaders = ImageNtHeader(baseAddress);

    const auto Translate = [&](ULONG rva) -> void* { return ImageRvaToVa(ntHeaders, baseAddress, rva, nullptr); };

    auto names = reinterpret_cast<const uint32_t*>(Translate(exportsDirectory->AddressOfNames));
    auto ordinals = reinterpret_cast<const uint16_t*>(Translate(exportsDirectory->AddressOfNameOrdinals));
    auto addresses = reinterpret_cast<const uint32_t*>(Translate(exportsDirectory->AddressOfFunctions));

    auto      sectionFirst = reinterpret_cast<uintptr_t>(exportsDirectory);
    uintptr_t sectionLast = sectionFirst + directorySize;

    for (uint32_t i = 0; i < exportsDirectory->NumberOfFunctions; ++i)
    {
        ImageExport item{};
        item.hint = i;

        if (i < exportsDirectory->NumberOfNames)
        {
            uint32_t ordinal = ordinals[i];
            item.ordinal = ordinal + exportsDirectory->Base;
            item.name = reinterpret_cast<const char*>(Translate(names[i]));
            item.address = addresses[ordinal];
        }
        else
        {
            uint32_t ordinal = i - exportsDirectory->NumberOfNames;
            item.ordinal = ordinal + exportsDirectory->Base;
            item.address = addresses[ordinal];
        }

        auto forwarded = reinterpret_cast<uintptr_t>(Translate(item.address));

        if (forwarded >= sectionFirst && forwarded < sectionLast)
            item.forwarded = reinterpret_cast<const char*>(forwarded);

        exports.emplace_back(item);
    }

    return exports;
}

int main(int argc, char* argv[])
{
    std::error_code       ec{};
    std::filesystem::path systemPath = GetKnownFolderPath(FOLDERID_SystemX86);

    if (systemPath.empty() || !std::filesystem::is_directory(systemPath, ec))
    {
        std::fprintf(stderr, "Error: System directory not found");
        return 1;
    }

    std::filesystem::path winmmPath = systemPath / "winmm.dll";

    if (!std::filesystem::is_regular_file(winmmPath, ec))
    {
        std::fprintf(stderr, "Error: winmm.dll not found at '%ls'", winmmPath.wstring().c_str());
        return 1;
    }

    std::wstring filePath = winmmPath.wstring();
    std::printf("File: %ls\n\n", filePath.c_str());

    std::vector<ImageExport> exports = GetImageExports(filePath);

    if (exports.empty())
        return 1;

    for (const ImageExport& item : exports)
    {
        const char* name = (item.name.empty()) ? "[NONAME]" : item.name.c_str();

        if (item.forwarded.empty())
            std::printf("%7u %4X %08X %s\n", item.ordinal, item.hint, static_cast<uint32_t>(item.address), name);
        else
            std::printf("%7u %4X %08X %s -> %s\n", item.ordinal, item.hint, static_cast<uint32_t>(item.address), name, item.forwarded.c_str());
    }

    {
        std::ofstream output("exports.def");
        output << "EXPORTS\n";

        for (const ImageExport& item : exports)
        {
            output << "    ";

            if (item.name.empty())
                output << "__NONAME__" << item.hint << "__";
            else
                output << item.name;

            output << "=";

            if (item.forwarded.empty())
                output << "__EXPORT__" << item.hint << "__";
            else
                output << item.forwarded;

            output << " @" << item.ordinal;

            if (item.name.empty())
                output << " NONAME";

            output << "\n";
        }
    }

    {
        std::ofstream output("winmm.asm");
        output << ".386\n"
               << ".model flat, stdcall\n"
               << "\n"
               << ".code\n"
               << "\n"
               << "  extern functions : dword\n"
               << "\n";

        for (const ImageExport& item : exports)
        {
            if (!item.forwarded.empty())
                continue;

            output << "  __EXPORT__" << item.hint << "__ proc  ; ";

            if (item.name.empty())
                output << "[NONAME]";
            else
                output << item.name;

            output << " @" << item.ordinal << "\n";

            output << "    jmp functions[" << item.hint << "*4]\n";
            output << "  __EXPORT__" << item.hint << "__ endp\n\n";
        }

        output << "end\n";
    }

    {
        std::ofstream output("setup.cpp");
        output << "#define WIN32_LEAN_AND_MEAN\n"
               << "#include <windows.h>\n"
               << "#include <array>\n"
               << "\n"
               << "extern \"C\" std::array<FARPROC, " << exports.size() << "> functions{};\n"
               << "\n"
               << "void SetupLibraryExports(HMODULE handle) noexcept\n"
               << "{\n";

        for (const ImageExport& item : exports)
        {
            if (!item.forwarded.empty())
                continue;

            output << "    functions[" << item.hint << "] = GetProcAddress(handle, LPCSTR(" << item.ordinal << "));\n";
        }

        output << "}\n";
    }
    return 0;
}
