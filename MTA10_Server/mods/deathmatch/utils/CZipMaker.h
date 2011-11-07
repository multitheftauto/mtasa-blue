/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CZipMaker.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CZipMaker  
{
public:
	            CZipMaker               ( const SString& strZipPathFilename );
                ~CZipMaker              ( void );

    bool        IsValid                 ( void );
    bool        Close                   ( void );
    bool        InsertFile              ( const SString& strSrc, const SString& strDest );
    bool        InsertDirectoryTree     ( const SString& strSrc, const SString& strDest );

protected:
    bool        MakeDirectoriesToFile   ( const SString& strDest );
    bool        MakeDirectory           ( const SString& strDirToMake );
    bool        AddFile                 ( const SString& strDest, const std::vector < char >& buffer );

	void*       m_uzFile;
};
