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
#include "ehs/ehs.h"

#ifndef MAX_PATH
#define MAX_PATH        260
#endif

class CResourceHTMLItem : public CResourceFile
{

public:

                                        CResourceHTMLItem               ( class CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes, bool bIsDefault, bool bIsRaw, bool bRestricted, bool bOOPEnabled );
                                        ~CResourceHTMLItem              ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    ResponseCode                        Request                         ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, class CAccount * account );
    bool                                AppendToPageBuffer              ( const char * szText, size_t length=0 );

    void                                SetResponseHeader               ( const char* szHeaderName, const char* szHeaderValue );
    void                                SetResponseCode                 ( int responseCode );
    void                                SetResponseCookie               ( const char* szCookieName, const char* szCookieValue );
    void                                ClearPageBuffer                 ( void );

    inline bool                         IsDefaultPage                   ( void ) { return m_bDefault; }
    inline void                         SetDefaultPage                  ( bool bDefault ) { m_bDefault = bDefault; }
    inline bool                         IsRestricted                    ( void ) { return m_bRestricted; };

private:
    inline char                         ReadChar                        ( FILE * pFile ) { return (unsigned char)fgetc ( pFile ); }
    void                                GetMimeType                     ( const char * szFilename );

    bool                                m_bIsBeingRequested; // crude mutex
    bool                                m_bIsRaw;
    CLuaMain *                          m_pVM;
    std::string                         m_strPageBuffer;     // contains what we're sending
    bool                                m_bDefault;          // is this the default page for this resource?
    std::string                         m_strMime;
    bool                                m_bRestricted;

    bool                                m_bOOPEnabled;

    ResponseCode                        m_responseCode;
    HttpResponse *                      m_currentResponse;
};

#endif

