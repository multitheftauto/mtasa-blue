from pathlib import Path

import pytest

import build_gettext_catalog

SAMPLE_FILES = [
    "dir1/CCore/core.cpp",
    "dir2/CCore/core.h",
    "dir1/CChat.cpp",
    "dir1/CChat.h",
    "dir2/lua.c",
    "dir2/shared.info.hpp",
]

IGNORE_FILES = [
    "dir2/sample.lua.h",
    "dir2/asdf.lua",
    "dir1/sample.png",
    "dir3/source.cpp",
]


@pytest.fixture
def source_files(tmp_path):
    for file_name in (SAMPLE_FILES+IGNORE_FILES):
        p = tmp_path / file_name
        p.parent.mkdir(parents=True, exist_ok=True)
        p.touch()

    return tmp_path


def test_get_source_files_from_dirs(source_files):
    # Given
    expected_found_files = {source_files / p for p in SAMPLE_FILES}
    # When
    paths = build_gettext_catalog.get_source_files_from_dirs([
        source_files / "dir1",
        source_files / "dir2"
    ])
    # Then
    assert set(paths) == expected_found_files


def test_write_directory_list_file(tmp_path):
    # Given
    path_list = [Path(p) for p in SAMPLE_FILES]
    path_list_str = {str(p) for p in path_list}
    with (tmp_path / "out.txt").open("w+") as fp:
        # When
        build_gettext_catalog.write_directory_list_file(path_list, fp)
        fp.seek(0)
        output = [p.strip() for p in fp.readlines()]

    # Then
    assert set(output) == path_list_str
