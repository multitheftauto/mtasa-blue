// XFont.h  Version 1.1
//
// Copyright (C) 2003 Hans Dietrich
//
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

// XFont.cpp  Version 1.1
//
// Author:       Philip Patrick (GetFontProperties)
//
// Version 1.0   - Initial release of GetFontProperties()
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// Version 1.1:  - Removed MFC dependency from GetFontProperties()
//               - Converted CFile file I/O to memory mapped file
//               - Added Unicode support
//               - Combined with my GetFontFile() routine
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <crtdbg.h>

#ifndef XFONT_H
#define XFONT_H

typedef struct _tagFONT_PROPERTIES
{
	TCHAR csName[1024];
	TCHAR csCopyright[1024];
	TCHAR csTrademark[1024];
	TCHAR csFamily[1024];
} FONT_PROPERTIES, *LPFONT_PROPERTIES;

///////////////////////////////////////////////////////////////////////////////
//
// structs used by GetFontProperties()
//
typedef struct _tagFONT_PROPERTIES_ANSI
{
	char csName[1024];
	char csCopyright[1024];
	char csTrademark[1024];
	char csFamily[1024];
} FONT_PROPERTIES_ANSI;

typedef struct _tagTT_OFFSET_TABLE
{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
} TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY
{
	char	szTag[4];			//table name
	ULONG	uCheckSum;			//Check sum
	ULONG	uOffset;			//Offset from beginning of file
	ULONG	uLength;			//length of the table in bytes
} TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER
{
	USHORT	uFSelector;			//format selector. Always 0
	USHORT	uNRCount;			//Name Records count
	USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
} TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD
{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
} TT_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))


BOOL GetFontProperties(LPCTSTR lpszFilePath, FONT_PROPERTIES * lpFontPropsX)
{
	FONT_PROPERTIES_ANSI fp;
	FONT_PROPERTIES_ANSI * lpFontProps = &fp;

	memset(lpFontProps, 0, sizeof(FONT_PROPERTIES_ANSI));

	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = ::CreateFile(lpszFilePath,
						 GENERIC_READ,// | GENERIC_WRITE,
						 0,
						 NULL,
						 OPEN_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						 NULL);

	if (hFile == INVALID_HANDLE_VALUE)
    {
        // Windows 8 fix
	    hFile = ::CreateFile(lpszFilePath,
						     GENERIC_READ,// | GENERIC_WRITE,
						     FILE_SHARE_READ,
						     NULL,
						     OPEN_ALWAYS,
						     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						     NULL);
    }

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//TRACE(_T("ERROR:  failed to open '%s'\n"), lpszFilePath);
		//TRACE(_T("ERROR: %s failed\n"), _T("CreateFile"));
		return FALSE;
	}

	// get the file size
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);

	if (dwFileSize == INVALID_FILE_SIZE)
	{
		//TRACE(_T("ERROR: %s failed\n"), _T("GetFileSize"));
		::CloseHandle(hFile);
		return FALSE;
	}

	//TRACE(_T("dwFileSize = %d\n"), dwFileSize);

	// Create a file mapping object that is the current size of the file
	HANDLE hMappedFile = NULL;
	hMappedFile = ::CreateFileMapping(hFile,
									  NULL,
									  PAGE_READONLY, //PAGE_READWRITE,
									  0,
									  dwFileSize,
									  NULL);

	if (hMappedFile == NULL)
	{
		//TRACE(_T("ERROR: %s failed\n"), _T("CreateFileMapping"));
		::CloseHandle(hFile);
		return FALSE;
	}

	LPBYTE lpMapAddress = (LPBYTE) ::MapViewOfFile(hMappedFile,		// handle to file-mapping object
											FILE_MAP_READ,//FILE_MAP_WRITE,			// access mode
											0,						// high-order DWORD of offset
											0,						// low-order DWORD of offset
											0);						// number of bytes to map

	if (lpMapAddress == NULL)
	{
		//TRACE(_T("ERROR: %s failed\n"), _T("MapViewOfFile"));
		::CloseHandle(hMappedFile);
		::CloseHandle(hFile);
		return FALSE;
	}

	BOOL bRetVal = FALSE;
	int index = 0;

	TT_OFFSET_TABLE ttOffsetTable;
	memcpy(&ttOffsetTable, &lpMapAddress[index], sizeof(TT_OFFSET_TABLE));
	index += sizeof(TT_OFFSET_TABLE);

	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	//check is this is a true type font and the version is 1.0
	if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		return bRetVal;

	TT_TABLE_DIRECTORY tblDir;
	memset(&tblDir, 0, sizeof(TT_TABLE_DIRECTORY));
	BOOL bFound = FALSE;
	char szTemp[4096];
	memset(szTemp, 0, sizeof(szTemp));

	for (int i = 0; i< ttOffsetTable.uNumOfTables; i++)
	{
		//f.Read(&tblDir, sizeof(TT_TABLE_DIRECTORY));
		memcpy(&tblDir, &lpMapAddress[index], sizeof(TT_TABLE_DIRECTORY));
		index += sizeof(TT_TABLE_DIRECTORY);

		strncpy(szTemp, tblDir.szTag, 4);
		if (stricmp(szTemp, "name") == 0)
		{
			bFound = TRUE;
			tblDir.uLength = SWAPLONG(tblDir.uLength);
			tblDir.uOffset = SWAPLONG(tblDir.uOffset);
			break;
		}
		else if (szTemp[0] == 0)
		{
			break;
		}
	}

	if (bFound)
	{
		index = tblDir.uOffset;

		TT_NAME_TABLE_HEADER ttNTHeader;
		memcpy(&ttNTHeader, &lpMapAddress[index], sizeof(TT_NAME_TABLE_HEADER));
		index += sizeof(TT_NAME_TABLE_HEADER);

		ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
		TT_NAME_RECORD ttRecord;
		bFound = FALSE;

		for (int i = 0;
			 i < ttNTHeader.uNRCount &&
			 (lpFontProps->csCopyright[0] == 0 ||
			  lpFontProps->csName[0] == 0      ||
			  lpFontProps->csTrademark[0] == 0 ||
			  lpFontProps->csFamily[0] == 0);
			 i++)
		{
			memcpy(&ttRecord, &lpMapAddress[index], sizeof(TT_NAME_RECORD));
			index += sizeof(TT_NAME_RECORD);

			ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
			ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
			ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

			if (ttRecord.uNameID == 1 || ttRecord.uNameID == 0 || ttRecord.uNameID == 7)
			{
				int nPos = index; //f.GetPosition();

				index = tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset;

				memset(szTemp, 0, sizeof(szTemp));

				memcpy(szTemp, &lpMapAddress[index], ttRecord.uStringLength);
				index += ttRecord.uStringLength;

                // Sometimes neeeds a unitext to sortofansitext hack
                std::string strTemp = szTemp;
                if ( strTemp.length () == 0 && ttRecord.uStringLength > 2 )
                {
                    std::wstring strTempE = (const wchar_t*)(szTemp+1);
                    strTemp = UTF16ToMbUTF8 ( strTempE );
                }
                const char* szTempAnsi = strTemp.c_str ();

				if (szTempAnsi[0] != 0)
				{
					_ASSERTE(strlen(szTempAnsi) < sizeof(lpFontProps->csName));

					switch (ttRecord.uNameID)
					{
						case 0:
							if (lpFontProps->csCopyright[0] == 0)
								strncpy(lpFontProps->csCopyright, szTempAnsi,
									sizeof(lpFontProps->csCopyright)-1);
							break;

						case 1:
							if (lpFontProps->csFamily[0] == 0)
								strncpy(lpFontProps->csFamily, szTempAnsi,
									sizeof(lpFontProps->csFamily)-1);
							bRetVal = TRUE;
							break;

						case 4:
							if (lpFontProps->csName[0] == 0)
								strncpy(lpFontProps->csName, szTempAnsi,
									sizeof(lpFontProps->csName)-1);
							break;

						case 7:
							if (lpFontProps->csTrademark[0] == 0)
								strncpy(lpFontProps->csTrademark, szTempAnsi,
									sizeof(lpFontProps->csTrademark)-1);
							break;

						default:
							break;
					}
				}
				index = nPos;
			}
		}
	}

	::UnmapViewOfFile(lpMapAddress);
	::CloseHandle(hMappedFile);
	::CloseHandle(hFile);

	if (lpFontProps->csName[0] == 0)
		strcpy(lpFontProps->csName, lpFontProps->csFamily);

	memset(lpFontPropsX, 0, sizeof(FONT_PROPERTIES));

    /*
	::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csName, -1, lpFontPropsX->csName,
		sizeof(lpFontPropsX->csName)/sizeof(TCHAR)-1);
	::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csCopyright, -1, lpFontPropsX->csCopyright,
		sizeof(lpFontPropsX->csCopyright)/sizeof(TCHAR)-1);
	::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csTrademark, -1, lpFontPropsX->csTrademark,
		sizeof(lpFontPropsX->csTrademark)/sizeof(TCHAR)-1);
	::MultiByteToWideChar(CP_ACP, 0, lpFontProps->csFamily, -1, lpFontPropsX->csFamily,
		sizeof(lpFontPropsX->csFamily)/sizeof(TCHAR)-1);
    */

    strcpy(lpFontPropsX->csName, lpFontProps->csName);
	strcpy(lpFontPropsX->csCopyright, lpFontProps->csCopyright);
	strcpy(lpFontPropsX->csTrademark, lpFontProps->csTrademark);
	strcpy(lpFontPropsX->csFamily, lpFontProps->csFamily);

	return bRetVal;
}

#endif //XFONT_H
