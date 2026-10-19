/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CZipMaker.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CZipMaker.h"
#include <zip.h>
#include <filesystem>

#ifdef WIN32
    #include <zip/iowin32.h>
#endif

///////////////////////////////////////////////////////////////
//
// CZipMaker::CZipMaker
//
//
//
///////////////////////////////////////////////////////////////
CZipMaker::CZipMaker(const SString& strZipPathFilename)
{
#ifdef WIN32
    // This will use CreateFile instead of fopen
    zlib_filefunc_def ffunc;
    fill_win32_filefunc(&ffunc);
    m_uzFile = zipOpen2(strZipPathFilename, APPEND_STATUS_CREATE, nullptr, &ffunc);
#else
    m_uzFile = zipOpen(strZipPathFilename, APPEND_STATUS_CREATE);  // Use APPEND_STATUS_ADDINZIP to open existing
#endif
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::~CZipMaker
//
//
//
///////////////////////////////////////////////////////////////
CZipMaker::~CZipMaker()
{
    Close();
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::IsValid
//
// Returns false if not valid
//
///////////////////////////////////////////////////////////////
bool CZipMaker::IsValid()
{
    return m_uzFile != NULL;
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::Close
//
// Returns false if failed
//
///////////////////////////////////////////////////////////////
bool CZipMaker::Close()
{
    if (!m_uzFile)
        return false;

    int iResult = zipClose(m_uzFile, NULL);
    m_uzFile = NULL;
    return iResult == ZIP_OK;
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::InsertFile
//
// Returns false if failed
//
///////////////////////////////////////////////////////////////
bool CZipMaker::InsertFile(const SString& strInSrc, const SString& strInDest)
{
    SString strSrc = PathConform(strInSrc);
    SString strDest = PathConform(strInDest);

    std::vector<char> buffer;
    if (!FileLoad(strSrc, buffer))
        return false;

    return AddFile(strDest, buffer);
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::InsertDirectoryTree
//
// strSrc is the full path for the source directory
// strDest is the destination directory, relative to the zip file root
//
///////////////////////////////////////////////////////////////
bool CZipMaker::InsertDirectoryTree(const SString& strInSrc, const SString& strInDest)
{
    SString strSrc = PathConform(strInSrc);
    SString strDest = PathConform(strInDest);

    std::error_code                     error;
    std::filesystem::directory_iterator iter(std::filesystem::u8path(strSrc.begin(), strSrc.end()), error);
    if (error)
        return false;

    for (; iter != std::filesystem::directory_iterator(); iter.increment(error))
    {
        if (error)
            return false;

        auto    utf8Name = iter->path().filename().u8string();
        SString strName = std::string(utf8Name.begin(), utf8Name.end());
        SString strSrcNext = PathJoin(strSrc, strName);
        SString strDestNext = PathJoin(strDest, strName);
        bool    bDirectory = iter->is_directory(error);
        if (error || !(bDirectory ? InsertDirectoryTree(strSrcNext, strDestNext) : InsertFile(strSrcNext, strDestNext)))
            return false;
    }
    return !error;
}

///////////////////////////////////////////////////////////////
//
// CZipMaker::AddFile
//
//
//
///////////////////////////////////////////////////////////////
bool CZipMaker::AddFile(const SString& strDest, const std::vector<char>& buffer)
{
    // save file attributes
    zip_fileinfo zfi;

    zfi.internal_fa = 0;
    zfi.external_fa = 0;

    // save file time
    time_t secondsNow = time(NULL);
    tm*    tmp = gmtime(&secondsNow);

    zfi.dosDate = 0;
    zfi.tmz_date.tm_year = tmp->tm_year + 1900;
    zfi.tmz_date.tm_mon = tmp->tm_mon;
    zfi.tmz_date.tm_mday = tmp->tm_mday;
    zfi.tmz_date.tm_hour = tmp->tm_hour;
    zfi.tmz_date.tm_min = tmp->tm_min;
    zfi.tmz_date.tm_sec = tmp->tm_sec;

    int iResult = zipOpenNewFileInZip(m_uzFile, strDest, &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);

    if (iResult == ZIP_OK)
    {
        if (!buffer.empty())
            iResult = zipWriteInFileInZip(m_uzFile, &buffer[0], buffer.size());
    }

    int iCloseResult = zipCloseFileInZip(m_uzFile);
    return iResult == ZIP_OK && iCloseResult == ZIP_OK;
}
