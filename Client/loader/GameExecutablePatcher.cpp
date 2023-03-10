/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/GameExecutablePatcher.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "GameExecutablePatcher.h"
#include "FileSystem.h"
#include "Utils.h"
#include "Main.h"

namespace fs = std::filesystem;

static constexpr FileHash HASH_GTA_EXE{0xf6, 0x3f, 0xa6, 0x23, 0xd1, 0x4e, 0x17, 0x0d, 0x0a, 0xe9, 0xe7, 0x03, 0x21, 0x86, 0x66, 0x9c,
                                       0xf3, 0xa1, 0x77, 0x79, 0x3b, 0xe2, 0x1c, 0x48, 0x24, 0x04, 0x3f, 0x02, 0x79, 0xbd, 0x50, 0x6a};

static constexpr size_t SIZE_GTA_EXE{0x00db7a00};

/**
 * @brief Checks if the provided buffer resembles a runnable 32-bit portable executable format.
*/
static bool Is32BitExecutable(const std::vector<unsigned char>& buffer)
{
    if (buffer.size() < sizeof(IMAGE_DOS_HEADER))
        return false;

    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(buffer.data());

    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    if (buffer.size() < (sizeof(IMAGE_NT_HEADERS32) + dos->e_lfanew))
        return false;

    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(buffer.data() + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return false;

    return true;
}

/**
 * @brief Wraps a buffer which represents an executable and makes modifications easier.
*/
class PatchableExecutable
{
public:
    explicit PatchableExecutable(std::vector<unsigned char>& buffer_) : buffer(buffer_)
    {
        auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(buffer.data());
        auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(buffer.data() + dos->e_lfanew);
        fileHeader = &nt->FileHeader;
        optHeader = &nt->OptionalHeader;
        firstSection = (fileHeader->NumberOfSections > 0) ? IMAGE_FIRST_SECTION(nt) : nullptr;
        numSections = fileHeader->NumberOfSections;
    }

    PatchableExecutable(const PatchableExecutable&) = delete;
    PatchableExecutable(PatchableExecutable&&) = delete;

    /**
     * @brief Converts a virtual address to a pointer in the buffer.
    */
    auto ResolveAddress(DWORD virtualAddress) -> unsigned char*
    {
        if (!numSections)
            return nullptr;

        IMAGE_SECTION_HEADER* section = firstSection;

        for (WORD i = 0; i < numSections; ++i, ++section)
        {
            if (!section->SizeOfRawData || !section->Misc.VirtualSize || !section->PointerToRawData)
                continue;

            const DWORD first = section->VirtualAddress;
            const DWORD last = first + section->Misc.VirtualSize;

            if (virtualAddress >= first && virtualAddress < last)
            {
                const DWORD offset = virtualAddress - first;
                return &buffer[section->PointerToRawData + offset];
            }
        }

        return nullptr;
    }

    /**
     * @brief Converts a data directory index to a pointer in the buffer where the data directory points to.
    */
    auto GetDataDirectory(DWORD index) -> unsigned char*
    {
        if (optHeader->NumberOfRvaAndSizes < index)
            return nullptr;

        IMAGE_DATA_DIRECTORY* directory = &optHeader->DataDirectory[index];

        if (!directory->VirtualAddress || !directory->Size)
            return nullptr;

        return ResolveAddress(directory->VirtualAddress);
    }

public:
    std::vector<unsigned char>& buffer;
    IMAGE_FILE_HEADER*          fileHeader;
    IMAGE_OPTIONAL_HEADER32*    optHeader;
    IMAGE_SECTION_HEADER*       firstSection;
    WORD                        numSections;
};

/**
 * @brief Enables the large address awareness flag for the executable.
*/
class LargeAddressAwarePatch final
{
public:
    static void Apply(PatchableExecutable& pe) { pe.fileHeader->Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE; }

    static void Restore(PatchableExecutable& pe) { pe.fileHeader->Characteristics &= ~IMAGE_FILE_LARGE_ADDRESS_AWARE; }
};

/**
 * @brief Enables data execution prevention (DEP) for the executable.
*/
class DataExecutionPreventionPatch final
{
public:
    static void Apply(PatchableExecutable& pe) { pe.optHeader->DllCharacteristics |= IMAGE_DLLCHARACTERISTICS_NX_COMPAT; }

    static void Restore(PatchableExecutable& pe) { pe.optHeader->DllCharacteristics &= ~IMAGE_DLLCHARACTERISTICS_NX_COMPAT; }
};

/**
 * @brief Changes the link timestamp to trick Windows 7 into using Aero.
*/
class WindowsAeroPatch final
{
public:
    static void Apply(PatchableExecutable& pe) { pe.fileHeader->TimeDateStamp = 0x437101ca; }

    static void Restore(PatchableExecutable& pe) { pe.fileHeader->TimeDateStamp = 0x427101ca; }
};

/**
 * @brief Replaces winmm.dll with mtasa.dll to enable injection of core.dll on game launch.
*/
class LibraryRedirectionPatch final
{
    static constexpr auto LIBRARY_NAME_BEFORE = "WINMM.dll";            // Do not correct this value. It must be equal to the one in the original binary.
    static constexpr auto LIBRARY_NAME_AFTER = LOADER_PROXY_DLL_NAME;   // This must be equal to the library produced by 'Loader Proxy' project.

    static auto GetImportDescriptor(PatchableExecutable& pe) -> IMAGE_IMPORT_DESCRIPTOR*
    {
        if (auto directory = pe.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT))
        {
            return reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(directory);
        }

        return nullptr;
    }

public:
    static void Apply(PatchableExecutable& pe)
    {
        if (auto descriptor = GetImportDescriptor(pe))
        {
            for (; descriptor->Name; ++descriptor)
            {
                auto name = reinterpret_cast<char*>(pe.ResolveAddress(descriptor->Name));

                if (stricmp(LIBRARY_NAME_BEFORE, name))
                    continue;

                strcpy(name, LIBRARY_NAME_AFTER);
                return;
            }
        }
    }

    static void Restore(PatchableExecutable& pe)
    {
        if (auto descriptor = GetImportDescriptor(pe))
        {
            for (; descriptor->Name; ++descriptor)
            {
                auto name = reinterpret_cast<char*>(pe.ResolveAddress(descriptor->Name));

                if (stricmp(LIBRARY_NAME_AFTER, name))
                    continue;

                strcpy(name, LIBRARY_NAME_BEFORE);
                return;
            }
        }
    }
};

/**
 * @brief Enables high performance graphics rendering on hybrid GPU systems.
*/
class HighPerformanceGraphicsPatch final
{
    // Export directory in .rdata section.
    static constexpr DWORD EXPORT_DIRECTORY_OFFSET{0x4b2b0};
    static constexpr DWORD EXPORT_DIRECTORY_RA{0x456800 + EXPORT_DIRECTORY_OFFSET};
    static constexpr DWORD EXPORT_DIRECTORY_VA{0x458000 + EXPORT_DIRECTORY_OFFSET};
    static constexpr DWORD EXPORT_DIRECTORY_SIZE{144};

    // Export values in .data section.
    static constexpr DWORD EXPORT_VALUES_OFFSET{0x3FF00};
    static constexpr DWORD EXPORT_VALUES_RA{0x4A1C00 + EXPORT_VALUES_OFFSET};
    static constexpr DWORD EXPORT_VALUES_VA{0x4A4000 + EXPORT_VALUES_OFFSET};
    static constexpr DWORD EXPORT_VALUES_SIZE{256};

    struct ExportItem
    {
        std::string name;
        uint32_t    value;
        uint16_t    ordinal;
        uint32_t    rvaName;
        uint32_t    rvaValue;
    };

    using ExportItems = std::array<ExportItem, 2>;

    /**
     * @brief Writes the export item values to the .data section.
              Values must be written to a different section to prevent our exports to be classified as forwarded exports.
    */
    static void WriteExportValues(PatchableExecutable& pe, ExportItems& exports)
    {
        unsigned char* base = &pe.buffer[EXPORT_VALUES_RA];
        size_t         offset = 0;

        for (ExportItem& item : exports)
        {
            item.rvaValue = EXPORT_VALUES_VA + offset;

            auto value = reinterpret_cast<uint32_t*>(base + offset);
            *value = item.value;
            offset += sizeof(uint32_t);
        }
    }

    /**
     * @brief Writes the export directory to the .rdata section.
    */
    static auto WriteExportDirectory(PatchableExecutable& pe, ExportItems& exports) -> size_t
    {
        unsigned char* base = &pe.buffer[EXPORT_DIRECTORY_RA];

        auto header = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base);
        header->TimeDateStamp = 0xffffffff;
        header->Base = 1;
        header->NumberOfFunctions = static_cast<DWORD>(exports.size());
        header->NumberOfNames = static_cast<DWORD>(exports.size());

        size_t offset = sizeof(IMAGE_EXPORT_DIRECTORY);

        // Write executable name.
        header->Name = EXPORT_DIRECTORY_VA + offset;
        {
            auto value = reinterpret_cast<char*>(base + offset);
            strcpy(value, GTA_EXE_NAME);
            offset += sizeof(GTA_EXE_NAME);
        }

        // Write export names.
        for (ExportItem& item : exports)
        {
            item.rvaName = EXPORT_DIRECTORY_VA + offset;

            auto value = reinterpret_cast<char*>(base + offset);
            std::copy(item.name.data(), item.name.data() + item.name.size() + 1, value);            // Plus one for null byte.
            offset += item.name.size() + 1;                                                         // Length plus null byte.
        }

        // Write export name RVAs.
        header->AddressOfNames = EXPORT_DIRECTORY_VA + offset;

        for (const ExportItem& item : exports)
        {
            auto value = reinterpret_cast<uint32_t*>(base + offset);
            *value = item.rvaName;
            offset += sizeof(uint32_t);
        }

        // Write export ordinals.
        header->AddressOfNameOrdinals = EXPORT_DIRECTORY_VA + offset;

        for (const ExportItem& item : exports)
        {
            auto value = reinterpret_cast<uint16_t*>(base + offset);
            *value = item.ordinal;
            offset += sizeof(uint16_t);
        }

        // Write function RVAs.
        header->AddressOfFunctions = EXPORT_DIRECTORY_VA + offset;

        for (const ExportItem& item : exports)
        {
            auto value = reinterpret_cast<uint32_t*>(base + offset);
            *value = item.rvaValue;
            offset += sizeof(uint32_t);
        }

        return offset;
    }

public:
    static void Apply(PatchableExecutable& pe)
    {
        if (pe.optHeader->NumberOfRvaAndSizes < IMAGE_DIRECTORY_ENTRY_EXPORT)
            return;

        std::array<ExportItem, 2> exports{{
            {"AmdPowerXpressRequestHighPerformance", 0x00000001},
            {"NvOptimusEnablement", 0x00000001},
        }};

        for (size_t i = 0; i < exports.size(); ++i)
        {
            exports[i].ordinal = static_cast<uint16_t>(i);
        }

        // We must write the values for their RVAs first.
        WriteExportValues(pe, exports);
        size_t directorySize = WriteExportDirectory(pe, exports);

        IMAGE_DATA_DIRECTORY* directory = &pe.optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        directory->VirtualAddress = EXPORT_DIRECTORY_VA;
        directory->Size = directorySize;
    }

    static void Restore(PatchableExecutable& pe)
    {
        if (pe.optHeader->NumberOfRvaAndSizes < IMAGE_DIRECTORY_ENTRY_EXPORT)
            return;

        IMAGE_DATA_DIRECTORY* directory = &pe.optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        directory->VirtualAddress = 0;
        directory->Size = 0;

        // Reset our bytes block in .rdata section.
        std::fill_n(pe.buffer.data() + EXPORT_DIRECTORY_RA, EXPORT_DIRECTORY_SIZE, 0x0);

        // Reset our bytes block in .data section.
        std::fill_n(pe.buffer.data() + EXPORT_VALUES_RA, EXPORT_VALUES_SIZE, 0x0);
    }
};

/**
 * @brief Computes the image checksum and applies it, because an invalid checksum might indicate malware activity.
*/
class ImageChecksumPatch final
{
public:
    static void Apply(PatchableExecutable& pe)
    {
        // Reset checksum to make the calculation a little bit easier.
        pe.optHeader->CheckSum = 0;

        // Compute the checksum.
        DWORD checksum = 0;
        {
            const size_t numWords = pe.buffer.size() / sizeof(WORD);
            auto         words = reinterpret_cast<const WORD*>(pe.buffer.data());

            for (size_t i = 0; i < numWords; ++i)
            {
                checksum += words[i];
                checksum = HIWORD(checksum) + LOWORD(checksum);
            }

            checksum += HIWORD(checksum);
        }

        // Update the checksum.
        pe.optHeader->CheckSum = checksum + pe.buffer.size();
    }

    static void Restore(PatchableExecutable& pe) { pe.optHeader->CheckSum = 0x00dbd689; }
};

bool GameExecutablePatcher::Load(std::error_code& ec)
{
    ec.clear();

    std::vector<unsigned char> buffer{};

    if (!GetFileContent(m_path, buffer, ec))
        return false;

    if (buffer.size() != SIZE_GTA_EXE)
    {
        ec.assign(ERROR_FILE_NOT_SUPPORTED, std::system_category());
        return false;
    }

    // Check if the buffer resembles a 32-bit portable executable format before proceeding.
    if (!Is32BitExecutable(buffer))
    {
        ec.assign(ERROR_INVALID_EXE_SIGNATURE, std::system_category());
        return false;
    }

    // Check if we have to unroll patches we've made to the binary in a previous run.
    FileHash hash = GetFileBufferHash(buffer);

    if (hash != HASH_GTA_EXE)
    {
        // Revert all of our patches in-memory. Order doesn't matter here.
        PatchableExecutable pe(buffer);
        LargeAddressAwarePatch::Restore(pe);
        DataExecutionPreventionPatch::Restore(pe);
        LibraryRedirectionPatch::Restore(pe);
        WindowsAeroPatch::Restore(pe);
        HighPerformanceGraphicsPatch::Restore(pe);
        ImageChecksumPatch::Restore(pe);

        // Check if the executable matches the original without patches.
        if (GetFileBufferHash(buffer) != HASH_GTA_EXE)
        {
            ec.assign(ERROR_DATA_CHECKSUM_ERROR, std::system_category());
            return false;
        }
    }

    m_onDiskHash = hash;
    m_executable = std::move(buffer);
    m_isAlreadyPatched = false;
    return true;
}

bool GameExecutablePatcher::ApplyPatches(std::error_code& ec)
{
    ec.clear();

    if (m_executable.size() != SIZE_GTA_EXE)
    {
        ec.assign(ERROR_BAD_LENGTH, std::system_category());
        return false;
    }

    // Abort if we already patched our buffer. This can only be a developer mistake.
    if (m_isAlreadyPatched)
    {
        ec.assign(ERROR_ALREADY_INITIALIZED, std::system_category());
        return false;
    }

    m_isAlreadyPatched = true;

    // Apply our patches in-memory.
    PatchableExecutable pe(m_executable);
    LargeAddressAwarePatch::Apply(pe);
    DataExecutionPreventionPatch::Apply(pe);
    LibraryRedirectionPatch::Apply(pe);

    if (GetApplicationSettingInt("aero-enabled"))
        WindowsAeroPatch::Apply(pe);

    if (GetApplicationSettingInt("nvhacks", "optimus-export-enablement"))
        HighPerformanceGraphicsPatch::Apply(pe);

    // This patch must always come last because it expects all modifications to be done.
    ImageChecksumPatch::Apply(pe);

    // Avoid writing to the file on disk if there are no differences after applying our patches.
    if (m_onDiskHash == GetFileBufferHash(m_executable))
        return true;

    return SetFileContent(m_path, m_executable, ec);
}

auto GameExecutablePatcher::GenerateMD5() const -> std::string
{
    return GenerateHashHexString(EHashFunctionType::MD5, m_executable.data(), m_executable.size());
}
