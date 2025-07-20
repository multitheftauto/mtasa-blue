/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSteamClient.cpp
 *  PURPOSE:     Source file for Steam client integration
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CSteamClient.h"
#include <optional>
#include <WinTrust.h>
#include <SoftPub.h>
#include <Psapi.h>

#define STEAM_GTASA_APP_ID "12120"

struct CertStoreDeleter
{
    void operator()(HCERTSTORE p) const noexcept { CertCloseStore(p, 0); }
};

using CertStoreScope = std::unique_ptr<std::remove_pointer_t<HCERTSTORE>, CertStoreDeleter>;

struct CryptMsgDeleter
{
    void operator()(HCRYPTMSG p) const noexcept { CryptMsgClose(p); }
};

using CryptMsgScope = std::unique_ptr<std::remove_pointer_t<HCRYPTMSG>, CryptMsgDeleter>;

struct CertContextDeleter
{
    void operator()(PCCERT_CONTEXT p) const noexcept { CertFreeCertificateContext(p); }
};

using CertContextScope = std::unique_ptr<std::remove_pointer_t<PCCERT_CONTEXT>, CertContextDeleter>;

struct HandleDeleter
{
    void operator()(HANDLE p) const noexcept { CloseHandle(p); }
};

using HandleScope = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter>;

struct SignerInfo
{
    SignerInfo(HCRYPTMSG Msg)
    {
        DWORD bufferSize{};
        BOOL  success = CryptMsgGetParam(
            /* CryptMsg  */ Msg,
            /* ParamType */ CMSG_SIGNER_INFO_PARAM,
            /* Index     */ 0,
            /* Data      */ nullptr,
            /* DataSize  */ &bufferSize);

        if (!success || !bufferSize)
            return;

        m_buffer = std::make_unique<char[]>(bufferSize);

        success = CryptMsgGetParam(
            /* CryptMsg  */ Msg,
            /* ParamType */ CMSG_SIGNER_INFO_PARAM,
            /* Index     */ 0,
            /* Data      */ m_buffer.get(),
            /* DataSize  */ &bufferSize);

        if (!success)
            m_buffer.reset();
    }

    operator bool() const noexcept { return !!m_buffer; }

    PCMSG_SIGNER_INFO operator->() { return m_buffer ? reinterpret_cast<PCMSG_SIGNER_INFO>(m_buffer.get()) : nullptr; }

    std::unique_ptr<char[]> m_buffer{};
};

class ISteamClient
{
public:
    virtual HSteamPipe CreateSteamPipe() = 0;
    virtual bool       BReleaseSteamPipe(HSteamPipe hSteamPipe) = 0;
    virtual HSteamUser ConnectToGlobalUser(HSteamPipe hSteamPipe) = 0;
    virtual HSteamUser CreateLocalUser(HSteamPipe* phSteamPipe, int eAccountType) = 0;
    virtual void       ReleaseUser(HSteamPipe hSteamPipe, HSteamUser hUser) = 0;
};

/**
 * @brief Removes leading and trailing whitespace characters from a wide string.
 * @param input A reference to the wide string to be trimmed in place.
 */
static void Trim(std::wstring& input)
{
    if (input.empty())
        return;

    if (size_t position = input.find_first_not_of(L" \r\n\t\0"); position != std::string_view::npos && position > 0)
        input = input.substr(position);

    if (size_t position = input.find_last_not_of(L" \r\n\t\0"); position != std::string_view::npos)
        input = input.substr(0, position);
}

/**
 * @brief Retrieves the name of a certificate as a wide string based on the specified name type.
 * @param Context A certificate to query.
 * @param Type An identifier of the certificate value to query (e.g., CERT_NAME_SIMPLE_DISPLAY_TYPE).
 * @return A trimmed wide string with the queried value on success, an empty wide string otherwise.
 */
static std::wstring GetCertName(PCCERT_CONTEXT Context, DWORD Type)
{
    DWORD length = CertGetNameStringW(Context, Type, 0, nullptr, nullptr, 0);

    // The string must contain at least one character plus null terminator.
    if (length < 2)
        return {};

    std::wstring value(length, L'\0');
    length = CertGetNameStringW(Context, Type, 0, nullptr, value.data(), length);

    if (length < 2)
        return {};

    Trim(value);
    return value;
};

/**
 * @brief Determines whether the digital signature of a binary file is trusted according to Windows trust policies.
 * @param filePath Path to the binary to verify.
 * @return true if the digital signature is trustworthy, false otherwise.
 */
static bool IsBinarySignatureTrusted(const wchar_t* filePath)
{
    WINTRUST_FILE_INFO fileInfo{};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = filePath;

    WINTRUST_DATA trustData{};
    trustData.cbStruct = sizeof(WINTRUST_DATA);
    trustData.dwUIChoice = WTD_UI_NONE;
    trustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    trustData.dwUnionChoice = WTD_CHOICE_FILE;
    trustData.pFile = &fileInfo;
    trustData.dwStateAction = WTD_STATEACTION_VERIFY;
    trustData.dwProvFlags = WTD_SAFER_FLAG;

    GUID policyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG trustError = WinVerifyTrust(NULL, &policyGUID, &trustData);

    trustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &policyGUID, &trustData);

    return trustError == ERROR_SUCCESS || trustError == CERT_E_UNTRUSTEDROOT || trustError == CERT_E_CHAINING || trustError == TRUST_E_TIME_STAMP;
}

/**
 * @brief Retrieves the display name of the certificate signer from a signed file.
 * @param filePath Path to the binary with a digital signature.
 * @return An wide string containing the certificate display name if found, an empty optional otherwise.
 */
static std::optional<std::wstring> GetCertificateDisplayName(const wchar_t* filePath)
{
    HCERTSTORE certStore{};
    HCRYPTMSG  cryptMsg{};
    BOOL       success = CryptQueryObject(
        /* ObjectType               */ CERT_QUERY_OBJECT_FILE,
        /* Object                   */ filePath,
        /* ExpectedContentTypeFlags */ CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
        /* ExpectedFormatTypeFlags  */ CERT_QUERY_FORMAT_FLAG_BINARY,
        /* Flags                    */ 0,
        /* MsgAndCertEncodingType   */ nullptr,
        /* ContentType              */ nullptr,
        /* FormatType               */ nullptr,
        /* CertStore                */ &certStore,
        /* Msg                      */ &cryptMsg,
        /* Context                  */ nullptr);

    if (!success)
        return std::nullopt;

    CertStoreScope closeCertStore{certStore};
    CryptMsgScope  closeCryptMsg{cryptMsg};
    SignerInfo     signerInfo{cryptMsg};

    if (!signerInfo)
        return std::nullopt;

    CERT_INFO ci{};
    ci.Issuer = signerInfo->Issuer;
    ci.SerialNumber = signerInfo->SerialNumber;

    PCCERT_CONTEXT certContext = CertFindCertificateInStore(
        /* CertStore        */ certStore,
        /* CertEncodingType */ X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
        /* FindFlags        */ 0,
        /* FindType         */ CERT_FIND_SUBJECT_CERT,
        /* FindPara         */ &ci,
        /* PrevCertContext  */ nullptr);

    if (!certContext)
        return std::nullopt;

    CertContextScope closeCertContext{certContext};
    return GetCertName(certContext, CERT_NAME_SIMPLE_DISPLAY_TYPE);
}

/**
 * @brief Queries an integer value from the registry.
 * @param rootKey The handle to a root registry key (e.g., HKEY_CURRENT_USER).
 * @param keyName The name of the registry key to open.
 * @param valueName The name of the registry value to query.
 * @param value Reference to a DWORD variable that receives the queried integer value.
 * @return true if the integer value was successfully queried, false otherwise.
 */
static bool RegQueryInteger(HKEY rootKey, LPCWSTR keyName, LPCWSTR valueName, DWORD& value)
{
    value = {};

    HKEY key{};
    if (RegOpenKeyExW(rootKey, keyName, 0, KEY_READ, &key) != ERROR_SUCCESS)
        return false;

    DWORD   valueSize = sizeof(value);
    LSTATUS status = RegQueryValueExW(key, valueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &valueSize);
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

/**
 * @brief Queries a REG_SZ value from the registry.
 * @param rootKey The handle to a root registry key (e.g., HKEY_CURRENT_USER).
 * @param keyName The name of the registry key to open.
 * @param valueName The name of the registry value to query.
 * @param value A reference to a std::wstring that receives the queried string value.
 * @return true if the string value was successfully queried, false otherwise.
 */
static bool RegQueryString(HKEY rootKey, LPCWSTR keyName, LPCWSTR valueName, std::wstring& value)
{
    value = {};

    HKEY key{};
    if (RegOpenKeyExW(rootKey, keyName, 0, KEY_READ, &key) != ERROR_SUCCESS)
        return false;

    DWORD   valueSize = {};
    DWORD   valueType = {};
    LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &valueType, nullptr, &valueSize);

    if (status != ERROR_SUCCESS || valueType != REG_SZ)
        return false;
    if (!valueSize)
        return true;

    value.resize(valueSize);
    status = RegQueryValueExW(key, valueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(value.data()), &valueSize);
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

/**
 * @brief Returns the active Steam.exe process id from the registry, if found.
 * @return A Steam.exe process id on success, an empty optional otherwise.
 */
static std::optional<DWORD> GetSteamProcessFromRegistry()
{
    if (DWORD pid; RegQueryInteger(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"pid", pid))
        return pid;

    return std::nullopt;
}

/**
 * @brief Checks if the game is installed on Steam by querying the registry.
 * @return true if it's installed according to the registry, false otherwise.
 */
static bool IsGameInstalledOnSteam()
{
    if (DWORD installed; RegQueryInteger(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\Apps\\" STEAM_GTASA_APP_ID, L"Installed", installed))
        return installed == 1;

    return false;
}

/**
 * @brief Retrieves the path to Valve's steamclient.dll from the registry.
 * @return The path to the steamclient.dll on success, an empty optional otherwise.
 */
static std::optional<std::wstring> GetSteamClientPathFromRegistry()
{
    std::wstring steamClientDll;

    if (!RegQueryString(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"SteamClientDll", steamClientDll))
        return std::nullopt;

    Trim(steamClientDll);

    std::replace(steamClientDll.begin(), steamClientDll.end(), L'/', L'\\');

    if (steamClientDll.empty())
        return std::nullopt;

    return steamClientDll;
}

/**
 * @brief Determines whether the specified process id corresponds to an active Steam.exe process.
 * @param pid The process id to check.
 * @return true if it's a Steam.exe process, false otherwise.
 */
static bool IsSteamProcess(DWORD pid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (!process)
        return false;

    HandleScope closeProcess{process};

    wchar_t processName[MAX_PATH];

    if (!GetModuleBaseNameW(process, nullptr, processName, sizeof(processName) / sizeof(wchar_t)))
        return false;

    if (wcsicmp(processName, L"steam.exe") != 0)
        return false;

    DWORD exitCode = 0;
    return GetExitCodeProcess(process, &exitCode) && exitCode == STILL_ACTIVE;
}

bool CSteamClient::Load()
{
    if (Native::Handle)
        return true;

    if (!IsGameInstalledOnSteam())
    {
        WriteErrorEvent("GTA:SA is not installed on Steam");
        return false;
    }

    std::optional<DWORD> steamProcessPid = GetSteamProcessFromRegistry();

    if (!steamProcessPid.has_value())
        return false;

    if (!IsSteamProcess(steamProcessPid.value()))
    {
        WriteErrorEvent(SString("Active steam process is unverified (pid: %lu)", steamProcessPid.value()));
        return false;
    }

    std::optional<std::wstring> steamClientPath = GetSteamClientPathFromRegistry();

    if (!steamClientPath.has_value())
    {
        WriteErrorEvent("Path to steamclient.dll not resolved");
        return false;
    }

    std::wstring steamDirPath;

    if (size_t position = steamClientPath->find_last_of(L'\\'); position != std::wstring::npos)
    {
        steamDirPath = steamClientPath->substr(0, position);
    }
    else
    {
        WriteErrorEvent("Path to Steam installation directory not resolved");
        return false;
    }

    WriteDebugEvent(SString("Using steamclient.dll: %s", *ToUTF8(steamClientPath.value())));
    SetEnvironmentVariableW(L"SteamAppId", L"" STEAM_GTASA_APP_ID);
    AddDllDirectory(steamDirPath.c_str());
    {
        static wchar_t pathBuffer[65536];
        GetEnvironmentVariableW(L"PATH", pathBuffer, sizeof(pathBuffer) / sizeof(wchar_t));

        wcscat(pathBuffer, L";");
        wcscat(pathBuffer, steamDirPath.c_str());

        SetEnvironmentVariableW(L"PATH", pathBuffer);
    }

    HANDLE libraryLock = CreateFileW(steamClientPath->c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (libraryLock == INVALID_HANDLE_VALUE)
    {
        WriteErrorEvent("Failed to lock steamclient.dll");
        return false;
    }

    HandleScope releaseLibraryLock{libraryLock};

    if (!IsBinarySignatureTrusted(steamClientPath->c_str()))
    {
        WriteErrorEvent("Digital signature of steamclient.dll is untrusted");
        return false;
    }

    std::optional<std::wstring> certificateName = GetCertificateDisplayName(steamClientPath->c_str());

    if (!certificateName.has_value() || certificateName != L"Valve Corp.")
    {
        WriteErrorEvent("Digital signature of steamclient.dll is untrusted");
        return false;
    }

    const HMODULE dll = LoadLibraryW(steamClientPath->c_str());

    if (!dll)
    {
        const SString systemError = GetSystemErrorMessage(GetLastError());
        WriteErrorEvent(SString("Failed to load steamclient.dll (%s)", *systemError));
        return false;
    }

    releaseLibraryLock.reset();
    
    Native::CreateInterface = reinterpret_cast<decltype(Native::CreateInterface)>(GetProcAddress(dll, "CreateInterface"));

    if (!Native::CreateInterface)
    {
        FreeLibrary(dll);
        return false;
    }

    Native::Handle = dll;
    return true;
}

bool CSteamClient::Connect()
{
    if (m_client)
        return true;

    if (!Load() || !Native::CreateInterface)
        return false;

    ISteamClient* steamClient = Native::CreateInterface("SteamClient017", nullptr);

    if (!steamClient)
    {
        WriteErrorEvent("Failed to create SteamClient017 interface using steamclient.dll");
        return false;
    }

    m_pipe = steamClient->CreateSteamPipe();
    m_user = steamClient->ConnectToGlobalUser(m_pipe);
    m_client = steamClient;
    return true;
}
