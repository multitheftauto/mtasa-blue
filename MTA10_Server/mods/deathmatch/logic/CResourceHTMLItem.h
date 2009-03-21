/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceHTMLItem.h
*  PURPOSE:     Resource server-side HTML item class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCEHTMLITEM_H
#define CRESOURCEHTMLITEM_H

#include "CResourceFile.h"
#include "../utils/expanding_string.h"
#include <list>
#include "ehs/ehs.h"

#ifndef MAX_PATH
#define MAX_PATH        260
#endif

class CResourceHTMLItem : public CResourceFile
{
    
public:

                                        CResourceHTMLItem               ( class CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes, bool bIsDefault, bool bIsRaw, bool bRestricted );
                                        ~CResourceHTMLItem              ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    ResponseCode                        Request                         ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, class CAccount * account );
    bool                                AppendToPageBuffer              ( char * szText, size_t length=0 );

	void								SetResponseHeader				( char * szHeaderName, char * szHeaderValue );
	void								SetResponseCode					( int responseCode );
	void								SetResponseCookie				( char * szCookieName, char * szCookieValue );
	void								ClearPageBuffer					( void );

    inline bool                         IsDefaultPage                   ( void ) { return m_bDefault; }
    inline void                         SetDefaultPage                  ( bool bDefault ) { m_bDefault = bDefault; }
	inline bool							IsRestricted					( void ) { return m_bRestricted; };
    
private:
    inline char                         ReadChar                        ( FILE * file ) { char character; fread ( &character, 1, 1, file ); return character; }
    void                                GetMimeType                     ( char * szFilename );
    class CLuaArguments *               GetQueryString                  ( char * szUrl );
    char *                              Unescape                        ( char * szIn, char * szOut, long bufferSize );

    bool                                m_bIsBeingRequested; // crude mutex
    bool                                m_bIsRaw;
    long                                m_lBufferLength; // for raw data
    char *                              m_szBuffer; // contains our file as a script
    CLuaMain *                          m_pVM;
    expanding_char *                    m_pPageBuffer; // contains what we're sending
    bool                                m_bDefault; // is this the default page for this resource?
    char                                m_szMime [ 20 ]; // the mime type
	bool								m_bRestricted;

    ResponseCode						m_responseCode;
    HttpResponse *						m_currentResponse;
};

#endif

