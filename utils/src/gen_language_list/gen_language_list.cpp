/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/src/gen_language_list/gen_language_list.cpp
 *  PURPOSE:     Tool to generate an include file with the list of all available
 *               languages in the locale directory. The Client Core project is
 *               going to use that include file to lookup the native name of a
 *               language without loading the translation files first. This
 *               safes a nice amount of time on startup.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <tinygettext.hpp>
#include <log.hpp>
#include <filesystem>
#include <iostream>
#include <format>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#if defined(_WIN32)
    #include <Windows.h>
#endif

#define MTA_LOCALE_TEXTDOMAIN "client"

using namespace tinygettext;

namespace fs = std::filesystem;

struct NativeLanguageName
{
    std::string locale;
    std::string inNative;
    std::string inEnglish;
};

auto quoted(const std::string& input) -> std::string
{
    std::string            result = input;
    std::string::size_type pos = 0;
    while ((pos = result.find('\"', pos)) != std::string::npos)
    {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    result = '"' + result + '"';
    return result;
}

auto bytes(const std::string& input) -> std::string
{
    std::stringstream ss;
    for (const unsigned char c : input)
    {
        ss << std::format("\\x{:02X}", c);
    }
    return ss.str();
}

int main(int argc, char* argv[])
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc != 3)
    {
        std::cerr << "Usage: gen_language_list <locale_dir> <output_file>" << std::endl;
        return 1;
    }

    const fs::path localeDirPath = argv[1];
    const fs::path outputFilePath = argv[2];

    if (!fs::exists(localeDirPath) || !fs::is_directory(localeDirPath))
    {
        std::cerr << "Error: " << localeDirPath << " is not a valid directory." << std::endl;
        return 1;
    }

    std::ofstream outputFile(outputFilePath);

    if (!outputFile.is_open())
    {
        std::cerr << "Error: Failed to open " << outputFilePath << " for writing." << std::endl;
        return 1;
    }

    Log::set_log_info_callback(nullptr);
    Log::set_log_warning_callback(nullptr);
    Log::set_log_error_callback(nullptr);

    DictionaryManager dictManager;
    {
        std::u8string raw = localeDirPath.u8string();
        dictManager.add_directory(std::string{raw.begin(), raw.end()});
    }

    std::vector<NativeLanguageName> nativeLanguageNames;

    for (const auto& lang : dictManager.get_languages(MTA_LOCALE_TEXTDOMAIN))
    {
        Dictionary dict = dictManager.get_dictionary(lang, MTA_LOCALE_TEXTDOMAIN);
        std::string name = dict.translate("English");

        if (name == "English" && lang.get_language() != "en")
        {
            name = lang.get_name();
        }

#if defined(MTA_DEBUG)
        std::cout << lang.str() << " => " << name << std::endl;
#endif

        nativeLanguageNames.push_back({lang.str(), name, lang.get_name()});
    }

    // Sort our list by locale for faster binary search later.
    std::sort(nativeLanguageNames.begin(), nativeLanguageNames.end(),
              [](const NativeLanguageName& a, const NativeLanguageName& b) { return a.locale < b.locale; });

    for (const NativeLanguageName& lang : nativeLanguageNames)
    {
        outputFile << "{ " << quoted(lang.locale) << ", " << quoted(bytes(lang.inNative)) << " },  // " << lang.inEnglish << std::endl;
    }

    return 0;
}
