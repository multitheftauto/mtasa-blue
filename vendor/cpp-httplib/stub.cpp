/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        vendor/cpp-httplib/stub.cpp
 *  PURPOSE:     Placeholder translation unit for the header-only cpp-httplib
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// cpp-httplib is header-only, so its premake project has no real sources.
// A StaticLib with no object files makes the macOS/Linux archiver (ar) fail
// with "no archive members specified". This otherwise-empty translation unit
// gives the archive a single object so the build succeeds on every platform.
