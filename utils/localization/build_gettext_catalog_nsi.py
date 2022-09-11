##############################################################################
#
#  PROJECT:     Multi Theft Auto
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/localization/build_gettext_catalog_nsi.py
#  PURPOSE:     Create a template .pot file from a .NSI script
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from https://www.multitheftauto.com/
#
##############################################################################
import argparse
from bisect import bisect_left
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
import re
import typing as t

import polib

# NOTE: regex does not support single quoted LangString, or escaped $\"
LANG_STRING_PTN = re.compile(r'\nLangString\s+(\w+)\s+\${LANG_ENGLISH}\s+"([\s\S]*?)"', re.IGNORECASE)

MSGID_CLEAN_PTN = re.compile(r'(\\\n[\s]*)')

@dataclass
class LangString:
    nsi_label: str
    line_number: int
    msgid: str


def get_metadata(project: str, version: str) -> t.Dict[str, str]:
    return {
        "Project-Id-Version" : f"{project} {version}".strip(),
        "Report-Msgid-Bugs-To" : "",
        "POT-Creation-Date" : datetime.now().strftime('%Y-%m-%d %H:%M%z'),
        "PO-Revision-Date" : "YEAR-MO-DA HO:MI+ZONE",
        "Last-Translator" : "FULL NAME <EMAIL@ADDRESS>",
        "Language-Team" : "LANGUAGE <LL@li.org>",
        "Language"  : "",
        "MIME-Version" : "1.0",
        "Content-Type" : "text/plain; charset=UTF-8",
        "Content-Transfer-Encoding" : "8bit"
    }


def get_newline_positions(content: str) -> t.List[int]:
    return [i for i, c in enumerate(content) if c == "\n"]


def get_line_number_from_position(positions: t.List[int], pos: int) -> int:
    return bisect_left(positions, pos) + 1


def clean_msgid(msgid: str) -> str:
    """
    Get rid of newlines in the source code file itself (that arent in the string)
    which may impact a msgid
    """
    return MSGID_CLEAN_PTN.sub("", msgid)


def parse_nsi(content: str) -> t.List[LangString]:
    positions = get_newline_positions(content)
    lang_strings = []
    for m in LANG_STRING_PTN.finditer(content):
        lang_strings.append(
            LangString(
                nsi_label=m.group(1),
                line_number=get_line_number_from_position(positions, m.start(1)),
                msgid=clean_msgid(m.group(2))
            )
        )
    return lang_strings


def write_pofile(
    lang_strings: t.List[LangString],
    po_file: polib.POFile,
    filename: str
) -> None:
    for lang_string in lang_strings:
        entry = polib.POEntry(
            msgid=polib.unescape(lang_string.msgid),
            msgstr='',
            occurrences=[(filename, lang_string.line_number)],
            comment=lang_string.nsi_label
        )
        po_file.append(entry)


def main(input_nsi: Path, output: Path, project: str, version: str) -> None:
    content = input_nsi.read_text()
    lang_strings = parse_nsi(content)

    po_file = polib.POFile()
    po_file.metadata = get_metadata(project=project, version=version)
    write_pofile(lang_strings, po_file, str(input_nsi))
    po_file.save(output)

    print ( "NSI POT Export Operation complete" )


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
        default="Shared/installer/locale/en_US.pot"
    )
    parser.add_argument(
        "-p",
        "--project",
        help="Project name to write to the POT file",
        default="MTA San Andreas Installer"
    )
    parser.add_argument(
        "-v",
        "--version",
        help="MTA:SA Version to write to the POT file",
        default="1.x"
    )
    args = parser.parse_args()
    main(
        input_nsi=Path(args.input),
        output=Path(args.output),
        project=args.project,
        version=args.version
    )
