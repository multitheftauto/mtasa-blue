##############################################################################
#
#  PROJECT:     Multi Theft Auto
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/localization/build_gettext_catalog.py
#  PURPOSE:     Create a template .pot file from .cpp and .h project files for client and server
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from https://www.multitheftauto.com/
#
##############################################################################
import argparse
from pathlib import Path
import os
import subprocess
import tempfile
import typing as t


def get_source_files_from_dir(dir: Path) -> t.List[Path]:
    file_types = [".c", ".cpp", ".h", ".hpp"]

    return [
        file_path
        for file_type in file_types
        for file_path in dir.glob(f"**/*{file_type}")
        if not str(file_path).endswith(".lua.h")
    ]


def get_source_files_from_dirs(dirs: t.List[Path]) -> t.List[Path]:
    return [
        file_path
        for dir_path in dirs
        for file_path in get_source_files_from_dir(dir_path)
    ]


def write_directory_list_file(files: t.List[Path], fp: t.TextIO):
    fp.writelines(f"{path}\n" for path in files)


def main(output: Path, version: str, xgettext: str, paths: t.Optional[t.List[Path]] = None) -> None:
    paths = paths or [Path("Client"), Path("Shared")]

    # If we have .pot in the destination, strip it (xgettext seems to append an extension regardless)
    output = output.with_suffix("")

    files = get_source_files_from_dirs(paths)
    print ( f"Files found: {len(files)}" )

    # xgettext requires a list of filepaths in a newline delimited file
    # so we create a tmpfile containing all our paths and give it to xgettext
    fd, fp_path = tempfile.mkstemp()
    fp_path = Path(fp_path)
    with fp_path.open("w", encoding="utf-8") as fp:
        write_directory_list_file(files, fp)

    try:
        subprocess.run([
            xgettext,
            "-f",
            str(fp_path),
            "-d",
            str(output.with_suffix("")),
            "--c++",
            "--from-code=UTF-8",
            "--add-comments",
            "--keyword=_",
            "--keyword=_td",
            "--keyword=_tn:1,2",
            "--keyword=_tc:1c,2",
            "--keyword=_tcn:1c,2,3",
            "--package-name=MTA San Andreas",
            f"--package-version={version}"
        ], check=True)

        # Rename our template to .pot (xgettext always outputs .po)
        output.with_suffix(".po").replace(output.with_suffix(".pot"))

        print ( "POT Generation Operation Complete" )
    except BaseException as err:
        print(err)

    # Delete our temporary file
    os.close(fd)
    fp_path.unlink()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-e",
        "--exe",
        help="xgettext executable location",
        default=("utils/xgettext.exe" if os.name == "nt" else "xgettext")
    )
    parser.add_argument(
        "-o",
        "--output",
        help="output file destination",
        default="Shared/data/MTA San Andreas/MTA/locale/en_US/client.pot"
    )
    parser.add_argument(
        "-v",
        "--version",
        help="MTA:SA Version to write to the POT file",
        default="1.x"
    )
    args = parser.parse_args()
    main(xgettext=args.exe, output=Path(args.output), version=args.version)
