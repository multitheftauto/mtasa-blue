##############################################################################
#
#  PROJECT:     Multi Theft Auto
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/localization/build_locale_nsi.py
#  PURPOSE:     Build a multi-language .NSI script based upon input PO files
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from https://www.multitheftauto.com/
#
##############################################################################
import argparse
from dataclasses import dataclass
from pathlib import Path
import polib
import typing as t


locale_to_name: t.Mapping[str, str] = {
    "af" : "Afrikaans",
    "sq" : "Albanian",
    "ar" : "Arabic",
    "hy" : "Armenian",
    "ast": "Asturian",
    "eu" : "Basque",
    "be" : "Belarusian",
    "bs" : "Bosnian",
    "br" : "Breton",
    "bg" : "Bulgarian",
    "ca" : "Catalan",
    "bem" : "Cibemba",
    "co" : "Corsican",
    "hr" : "Croatian",
    "cs" : "Czech",
    "da" : "Danish",
    "nl" : "Dutch",
    "efi" : "Efik",
    "en" : "English",
    "eo" : "Esperanto",
    "et" : "Estonian",
    "fa" : "Farsi",
    "fi" : "Finnish",
    "fr" : "French",
    "gl" : "Galician",
    "ka" : "Georgian",
    "de" : "German",
    "el" : "Greek",
    "he" : "Hebrew",
    "hi" : "Hindi",
    "hu" : "Hungarian",
    "is" : "Icelandic",
    "ig" : "Igbo",
    "id" : "Indonesian",
    "ga" : "Irish",
    "it" : "Italian",
    "ja" : "Japanese",
    "km" : "Khmer",
    "ko" : "Korean",
    "ku" : "Kurdish",
    "lv" : "Latvian",
    "lt" : "Lithuanian",
    "lb" : "Luxembourgish",
    "mk" : "Macedonian",
    "mg" : "Malagasy",
    "ms" : "Malay",
    "mn" : "Mongolian",
    "nb" : "Norwegian",
    "nn" : "NorwegianNynorsk",
    "ps" : "Pashto",
    "pl" : "Polish",
    "pt" : "Portuguese",
    "pt_BR" : "PortugueseBR",
    "ro" : "Romanian",
    "ru" : "Russian",
    "gd" : "ScotsGaelic",
    "sr": "Serbian",
    "sr_Latn_BA": "SerbianLatin",
    "zh_CN" : "SimpChinese",
    "sk" : "Slovak",
    "sl" : "Slovenian",
    "es" : "Spanish",
    "es_AR" : "SpanishInternational",
    "sw" : "Swahili",
    "sv" : "Swedish",
    "tt": "Tatar",
    "th" : "Thai",
    "zh_HK" : "TradChinese",
    "zh_TW" : "TradChinese",
    "tr" : "Turkish",
    "uk" : "Ukrainian",
    "uz" : "Uzbek",
    "ca@valencia" : "Valencian",
    "vi" : "Vietnamese",
    "cy" : "Welsh",
    "yo" : "Yoruba",
}

rtl_locales = [ "ar", "he" ]


def escape_nsis(msgstr: str) -> str:
    return (msgstr.replace('\\', r'$\\')
             .replace('\t', r'$\t')
             .replace('\r', r'\r')
             .replace('\n', r'\n')
             .replace('\"', r'$\"')
             .replace('$$\\', '$\\'))

@dataclass
class TranslatedLangString:
    msgstr: str
    nsi_label: str


def get_default_lang_strings(
    locale: str, rtl_locales=rtl_locales
) -> t.List[TranslatedLangString]:
    defaults = [
        TranslatedLangString(nsi_label="LANGUAGE_CODE", msgstr=locale)
    ]

    if locale in rtl_locales:
        defaults.append(
            TranslatedLangString(nsi_label="LANGUAGE_RTL", msgstr="1")
        )

    return defaults


def get_translations_from_pofile(po: polib.pofile) -> t.List[TranslatedLangString]:
    translations = [
        TranslatedLangString(
            nsi_label=entry.comment,
            msgstr=escape_nsis(entry.msgstr)
        )
        for entry in po.translated_entries()
    ]
    translations += [
        TranslatedLangString(
            nsi_label=entry.comment,
            msgstr=escape_nsis(entry.msgid)
        )
        for entry in po.untranslated_entries()
    ]
    return translations


def get_translations(podir: Path) -> t.Dict[str, t.List[TranslatedLangString]]:
    translations: t.Dict[str, t.List[TranslatedLangString]] = {}
    for po_path in podir.glob("*.po"):
        locale = po_path.stem
        if not locale in locale_to_name:
            print(f"Warning: Locale {locale} not supported by NSIS")
            continue

        translations[locale] = get_default_lang_strings(locale)

        po = polib.pofile(str(po_path))
        translations[locale] += get_translations_from_pofile(po)

    return translations


def create_locale_lang_strings(
    translations: t.List[TranslatedLangString],
    lang_name: str
) -> t.List[str]:
    strings = [ f'!insertmacro MUI_LANGUAGE "{lang_name}"\n' ]
    strings += [
        f'LangString {entry.nsi_label} ${{LANG_{lang_name}}} "{entry.msgstr}"\n'
        for entry in translations
    ]
    return strings


def create_nsi_lang_strings(
    translations: t.Dict[str, t.List[TranslatedLangString]]
) -> t.List[str]:
    return [
        lang_string
        for locale, locale_translations in translations.items()
        for lang_string in create_locale_lang_strings(
            locale_translations, locale_to_name[locale]
        )
    ]


def split_nsi(nsi_lines: t.List[str]) -> t.Tuple[t.List[str], t.List[str]]:
    slice = nsi_lines.index(';@INSERT_TRANSLATIONS@\n')
    return nsi_lines[:slice], nsi_lines[slice+1:]


def main(input_nsi: Path, output: Path, podir: Path) -> None:
    nsi_lines = input_nsi.read_text(encoding="utf-8").splitlines(keepends=True)
    top, bottom = split_nsi(nsi_lines)

    translations = get_translations(podir)
    translation_lines = create_nsi_lang_strings(translations)

    new_nsi_lines = top + translation_lines + bottom

    output.write_text("".join(new_nsi_lines), encoding="utf-8")

    print ( "NSI Localization Operation Complete" )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--input",
        help="nightly.nsi location",
        default="Shared/installer/nightly.nsi"
    )
    parser.add_argument(
        "-o",
        "--output",
        help="output file destination",
        default="Shared/installer/nightly_localized.nsi"
    )
    parser.add_argument(
        "-p",
        "--podir",
        help="Directory containing PO files",
        default="Shared/installer/locale/"
    )
    args = parser.parse_args()
    main(
        input_nsi=Path(args.input),
        output=Path(args.output),
        podir=Path(args.podir),
    )
