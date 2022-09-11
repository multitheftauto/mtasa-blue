import build_locale_nsi
import polib


def test_get_default_lang_strings_rtl():
    defaults = build_locale_nsi.get_default_lang_strings("aa", ["aa"])

    assert defaults == [
        build_locale_nsi.TranslatedLangString(nsi_label="LANGUAGE_CODE", msgstr="aa"),
        build_locale_nsi.TranslatedLangString(nsi_label="LANGUAGE_RTL", msgstr="1")
    ]


def test_get_default_lang_strings_ltr():
    defaults = build_locale_nsi.get_default_lang_strings("aa", [])

    assert defaults == [
        build_locale_nsi.TranslatedLangString(nsi_label="LANGUAGE_CODE", msgstr="aa")
    ]


def test_split_nsi():
    # Given
    nsi_lines = [
        "1\n",
        "2\n",
        ";@INSERT_TRANSLATIONS@\n",
        "3\n",
        "4\n",
    ]
    # When
    top, bottom = build_locale_nsi.split_nsi(nsi_lines)
    # Then
    assert top == ["1\n", "2\n"]
    assert bottom == ["3\n", "4\n"]


def test_get_translations_from_pofile():
    # Given
    po = polib.POFile()
    po.append(polib.POEntry(
        msgid="untranslated",
        msgstr="",
        comment="test_UNTRANSLATED"
    ))
    po.append(polib.POEntry(
        msgid="text",
        msgstr="text_translated",
        comment="test_TRANSLATED"
    ))
    po.append(polib.POEntry(
        msgid="text_needs_escape",
        msgstr="\\ \t \r \n \"",
        comment="test_TRANSLATED2"
    ))
    # When
    translations = build_locale_nsi.get_translations_from_pofile(po)
    # Then
    assert translations == [
        build_locale_nsi.TranslatedLangString(
            nsi_label="test_TRANSLATED", msgstr="text_translated"
        ),
        build_locale_nsi.TranslatedLangString(
            nsi_label="test_TRANSLATED2", msgstr=r'$\\ $\t \r \n $\"'
        ),
        build_locale_nsi.TranslatedLangString(
            nsi_label="test_UNTRANSLATED", msgstr="untranslated"
        ),
    ]


def test_create_nsi_lang_strings():
    # Given
    translations = {
        'fr': [
            build_locale_nsi.TranslatedLangString(
                nsi_label="test_fr1", msgstr="frenchtext1"
            ),
            build_locale_nsi.TranslatedLangString(
                nsi_label="test_fr2", msgstr="frenchtext2"
            ),
        ],
        'de': [
            build_locale_nsi.TranslatedLangString(
                nsi_label="test_de1", msgstr="germantext1"
            ),
            build_locale_nsi.TranslatedLangString(
                nsi_label="test_de2", msgstr="germantext2"
            ),
        ]
    }
    # When
    lang_strings = build_locale_nsi.create_nsi_lang_strings(translations)
    # Then
    assert lang_strings == [
        '!insertmacro MUI_LANGUAGE "French"\n',
        'LangString test_fr1 ${LANG_French} "frenchtext1"\n',
        'LangString test_fr2 ${LANG_French} "frenchtext2"\n',
        '!insertmacro MUI_LANGUAGE "German"\n',
        'LangString test_de1 ${LANG_German} "germantext1"\n',
        'LangString test_de2 ${LANG_German} "germantext2"\n',
    ]
