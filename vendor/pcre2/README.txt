PCRE2 Vendor Source Setup
=========================

This directory contains the PCRE2 library source. The .c and .h files must be
copied from the upstream PCRE2 repository.

Upstream: https://github.com/PCRE2Project/pcre2
Recommended version: pcre2-10.47 (latest stable release as of 2026-06)

Setup steps:

1. Clone the PCRE2 repository:
   git clone https://github.com/PCRE2Project/pcre2.git
   cd pcre2
   git checkout pcre2-10.47

2. Copy these source files from pcre2/src/ into vendor/pcre2/:

   REQUIRED source files (.c):
   - pcre2_auto_possess.c
   - pcre2_compile.c
   - pcre2_compile_cgroup.c
   - pcre2_compile_class.c
   - pcre2_config.c
   - pcre2_context.c
   - pcre2_convert.c
   - pcre2_dfa_match.c
   - pcre2_error.c
   - pcre2_extuni.c
   - pcre2_find_bracket.c
   - pcre2_maketables.c
   - pcre2_match.c
   - pcre2_match_data.c
   - pcre2_match_next.c
   - pcre2_newline.c
   - pcre2_ord2utf.c
   - pcre2_pattern_info.c
   - pcre2_script_run.c
   - pcre2_serialize.c
   - pcre2_string_utils.c
   - pcre2_study.c
   - pcre2_substitute.c
   - pcre2_substring.c
   - pcre2_tables.c
   - pcre2_ucd.c
   - pcre2_valid_utf.c
   - pcre2_xclass.c

   REQUIRED header files (.h):
   - pcre2.h.generic   (rename to pcre2.h after copying)
   - pcre2_internal.h
   - pcre2_intmodedep.h
   - pcre2_ucp.h
   - pcre2_util.h
   - pcre2_compile.h
   - pcre2_chkdint.c

   REQUIRED data file:
   - pcre2_chartables.c.dist  (rename to pcre2_chartables.c after copying)

   OPTIONAL POSIX wrapper (if you need pcre2posix):
   - pcre2posix.c
   - pcre2posix.h

3. After copying, rename:
   - pcre2.h.generic        -> pcre2.h
   - pcre2_chartables.c.dist -> pcre2_chartables.c

4. The pcre2.h header from upstream already supports PCRE2_CODE_UNIT_WIDTH=8,
   which we define via the premake5.lua build configuration.

   IMPORTANT: Do NOT edit pcre2.h to add PCRE2_CODE_UNIT_WIDTH -- it is
   defined via the premake build system instead. The pcre2.h file uses:
     #ifndef PCRE2_CODE_UNIT_WIDTH
     #error PCRE2_CODE_UNIT_WIDTH must be defined before including pcre2.h.
   So it must be defined by the build system, not in the header.

5. The config.h in this directory is a custom minimal config for Windows/MSVC.
   For Linux builds, you may need to generate config.h via ./configure or
   create a separate config suitable for GCC/Clang.
