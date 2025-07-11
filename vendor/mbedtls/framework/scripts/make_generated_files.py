#!/usr/bin/env python3

# make_generated_files.py
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""
Generate the TF-PSA-Crypto generated files
"""
import argparse
import filecmp
import shutil
import subprocess
import sys

from pathlib import Path
from typing import List, Optional

from mbedtls_framework import build_tree

class GenerationScript:
    """
    Representation of a script generating a configuration independent file.
    """
    # pylint: disable=too-few-public-methods
    def __init__(self, script: Path, files: List[Path],
                 output_dir_option: Optional[str] = None,
                 output_file_option: Optional[str] = None):
        # Path from the root of Mbed TLS or TF-PSA-Crypto of the generation script
        self.script = script

        # Executable to run the script, needed for Windows
        if script.suffix == ".py":
            self.exe = "python"
        elif script.suffix == ".pl":
            self.exe = "perl"

        # List of the default paths from the Mbed TLS or TF-PSA-Crypto root of the
        # files the script generates.
        self.files = files

        # Output directory script argument. Can be an empty string in case it is a
        # positional argument.
        self.output_dir_option = output_dir_option

        # Output file script argument. Can be an empty string in case it is a
        # positional argument.
        self.output_file_option = output_file_option

def get_generation_script_files(generation_script: str):
    """
    Get the list of the default paths of the files that a given script
    generates. It is assumed that the script supports the "--list" option.
    """
    files = []
    if generation_script.endswith(".py"):
        cmd = ["python"]
    elif generation_script.endswith(".pl"):
        cmd = ["perl"]
    cmd += [generation_script, "--list"]

    output = subprocess.check_output(cmd, universal_newlines=True)
    for line in output.splitlines():
        files.append(Path(line))

    return files

if build_tree.looks_like_tf_psa_crypto_root("."):
    TF_PSA_CRYPTO_GENERATION_SCRIPTS = [
        GenerationScript(
            Path("scripts/generate_driver_wrappers.py"),
            [Path("core/psa_crypto_driver_wrappers.h"),
             Path("core/psa_crypto_driver_wrappers_no_static.c")],
            "", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_test_keys.py"),
            [Path("tests/include/test/test_keys.h")],
            None, "--output"
        ),
        GenerationScript(
            Path("scripts/generate_psa_constants.py"),
            [Path("programs/psa/psa_constant_names_generated.c")],
            "", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_bignum_tests.py"),
            get_generation_script_files("framework/scripts/generate_bignum_tests.py"),
            "--directory", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_config_tests.py"),
            get_generation_script_files("framework/scripts/generate_config_tests.py"),
            "--directory", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_ecp_tests.py"),
            get_generation_script_files("framework/scripts/generate_ecp_tests.py"),
            "--directory", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_psa_tests.py"),
            get_generation_script_files("framework/scripts/generate_psa_tests.py"),
            "--directory", None
        ),
    ]


if build_tree.looks_like_mbedtls_root(".") and not build_tree.is_mbedtls_3_6():
    MBEDTLS_GENERATION_SCRIPTS = [
        GenerationScript(
            Path("scripts/generate_errors.pl"),
            [Path("library/error.c")],
            None, "tf-psa-crypto/drivers/builtin/include/mbedtls \
                   include/mbedtls/ \
                   scripts/data_files"
        ),
        GenerationScript(
            Path("scripts/generate_features.pl"),
            [Path("library/version_features.c")],
            None, "include/mbedtls/ scripts/data_files"
        ),
        GenerationScript(
            Path("framework/scripts/generate_ssl_debug_helpers.py"),
            [Path("library/ssl_debug_helpers_generated.c")],
            "", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_test_keys.py"),
            [Path("tests/include/test/test_keys.h")],
            None, "--output"
        ),
        GenerationScript(
            Path("framework/scripts/generate_test_cert_macros.py"),
            [Path("tests/include/test/test_certs.h")],
            None, "--output"
        ),
        GenerationScript(
            Path("scripts/generate_query_config.pl"),
            [Path("programs/test/query_config.c")],
            None, "include/mbedtls/mbedtls_config.h \
                   tf-psa-crypto/include/psa/crypto_config.h \
                   scripts/data_files/query_config.fmt"
        ),
        GenerationScript(
            Path("framework/scripts/generate_config_tests.py"),
            get_generation_script_files("framework/scripts/generate_config_tests.py"),
            "--directory", None
        ),
        GenerationScript(
            Path("framework/scripts/generate_tls13_compat_tests.py"),
            [Path("tests/opt-testcases/tls13-compat.sh")],
            None, "--output"
        ),
        GenerationScript(
            Path("framework/scripts/generate_tls_handshake_tests.py"),
            [Path("tests/opt-testcases/handshake-generated.sh")],
            None, "--output"
        ),
        GenerationScript(
            Path("scripts/generate_visualc_files.pl"),
            get_generation_script_files("scripts/generate_visualc_files.pl"),
            "--directory", None
        ),
    ]

def get_generated_files(generation_scripts: List[GenerationScript]):
    """
    List the generated files in Mbed TLS or TF-PSA-Crypto. The path from root
    is returned for each generated files.
    """
    files = []
    for generation_script in generation_scripts:
        files += generation_script.files

    return files

def make_generated_files(generation_scripts: List[GenerationScript]):
    """
    Generate the configuration independent files in their default location in
    the Mbed TLS or TF-PSA-Crypto tree.
    """
    for generation_script in generation_scripts:
        subprocess.run([generation_script.exe, str(generation_script.script)], check=True)

def check_generated_files(generation_scripts: List[GenerationScript], root: Path):
    """
    Check that the given root directory contains the generated files as expected/
    generated by this script.
    """
    for generation_script in generation_scripts:
        for file in generation_script.files:
            file = root / file
            bak_file = file.with_name(file.name + ".bak")
            if bak_file.exists():
                bak_file.unlink()
            file.rename(bak_file)

        command = [generation_script.exe, str(generation_script.script)]
        if generation_script.output_dir_option is not None:
            command += [generation_script.output_dir_option,
                        str(root / Path(generation_script.files[0].parent))]
        elif generation_script.output_file_option is not None:
            command += generation_script.output_file_option.split()
            command += [str(root / Path(generation_script.files[0]))]
        subprocess.run([item for item in command if item.strip()], check=True)

        for file in generation_script.files:
            file = root / file
            bak_file = file.with_name(file.name + ".bak")
            if not filecmp.cmp(file, bak_file):
                ref_file = file.with_name(file.name + ".ref")
                ref_file = root / ref_file
                if ref_file.exists():
                    ref_file.unlink()
                shutil.copy(file, ref_file)
                print(f"Generated file {file} not identical to the reference one {ref_file}.")
            file.unlink()
            bak_file.rename(file)

def main():
    """
    Main function of this program
    """
    parser = argparse.ArgumentParser()

    parser.add_argument('--list', action='store_true',
                        default=False, help='List generated files.')
    parser.add_argument('--root', metavar='DIR',
                        help='Root of the tree containing the generated files \
                              to check (default: Mbed TLS or TF-PSA-Cryto root.)')
    parser.add_argument('--check', action='store_true',
                        default=False, help='Check the generated files in root')

    args = parser.parse_args()

    if not build_tree.looks_like_root("."):
        raise RuntimeError("This script must be run from Mbed TLS or TF-PSA-Crypto root.")

    if build_tree.looks_like_tf_psa_crypto_root("."):
        generation_scripts = TF_PSA_CRYPTO_GENERATION_SCRIPTS
    elif not build_tree.is_mbedtls_3_6():
        generation_scripts = MBEDTLS_GENERATION_SCRIPTS
    else:
        raise Exception("No support for Mbed TLS 3.6")

    if args.list:
        files = get_generated_files(generation_scripts)
        for file in files:
            print(str(file))
    elif args.check:
        check_generated_files(generation_scripts, Path(args.root or "."))
    else:
        make_generated_files(generation_scripts)

if __name__ == "__main__":
    sys.exit(main())
