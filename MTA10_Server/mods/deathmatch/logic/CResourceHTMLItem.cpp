/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceHTMLItem.cpp
*  PURPOSE:     Resource server-side HTML item class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single HTML resource item.
// This parses it and converts into into a script

#include "StdInc.h"

extern CServerInterface* g_pServerInterface;
extern CGame* g_pGame;

CResourceHTMLItem::CResourceHTMLItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes, bool bIsDefault, bool bIsRaw, bool bRestricted ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_bIsRaw = bIsRaw;
    m_szBuffer = NULL;
    m_type = RESOURCE_FILE_TYPE_HTML;
    m_bDefault = bIsDefault;
    m_pPageBuffer = NULL;
    m_pVM = NULL;
    m_bIsBeingRequested = false;
	m_bRestricted = bRestricted;
}

CResourceHTMLItem::~CResourceHTMLItem ( void )
{
    Stop();
}

ResponseCode CResourceHTMLItem::Request ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, CAccount * account )
{
    if ( m_bIsBeingRequested )
    {
        ipoHttpResponse->SetBody("Busy!", strlen("Busy!"));
        return HTTPRESPONSECODE_500_INTERNALSERVERERROR;
    }

    m_bIsBeingRequested = true;

	m_responseCode = HTTPRESPONSECODE_200_OK;

    if ( !m_bIsRaw )
    {
        if ( m_pPageBuffer )
            delete m_pPageBuffer;

        m_pPageBuffer = new expanding_char ( 500 );
		
		ipoHttpResponse->oResponseHeaders [ "content-type" ] = m_szMime;

		CLuaArguments * formData = new CLuaArguments();
		for ( FormValueMap::iterator iter = ipoHttpRequest->oFormValueMap.begin(); iter != ipoHttpRequest->oFormValueMap.end(); iter++ )
		{
			formData->PushString ( (*iter).first.c_str() );
			formData->PushString ( ((FormValue)(*iter).second).sBody.c_str() );
		}

		CLuaArguments * cookies = new CLuaArguments();
		for ( CookieMap::iterator iter = ipoHttpRequest->oCookieMap.begin(); iter != ipoHttpRequest->oCookieMap.end(); iter++ )
		{
			cookies->PushString ( (*iter).first.c_str() );
			cookies->PushString ( (*iter).second.c_str() );
		}

		CLuaArguments * headers = new CLuaArguments();
		for ( StringMap::iterator iter = ipoHttpRequest->oRequestHeaders.begin(); iter != ipoHttpRequest->oRequestHeaders.end(); iter++ )
		{
			headers->PushString ( (*iter).first.c_str() );
			headers->PushString ( (*iter).second.c_str() );
		}

		m_currentResponse = ipoHttpResponse;
        CLuaArguments * querystring = new CLuaArguments(*formData);
        CLuaArguments args;
		args.PushTable ( headers ); // requestHeaders
        args.PushTable ( formData ); // form
        args.PushTable ( cookies ); // cookies
        args.PushString ( ipoHttpRequest->GetAddress().c_str() ); // hostname
        args.PushString ( ipoHttpRequest->sOriginalUri.c_str() ); // url
        args.PushTable ( querystring ); // querystring
        args.PushAccount ( account );

       // g_pGame->Lock(); // get the mutex (blocking)
        args.CallGlobal ( m_pVM, "renderPage" );
       // g_pGame->Unlock(); // release the mutex

        ipoHttpResponse->SetBody ( m_pPageBuffer->GetValue(), m_pPageBuffer->GetLength() );
        
    }
    else
    {
        // its a raw page
        FILE * file = fopen ( m_szResourceFileName, "rb" );
        if ( file )
        {
            fseek ( file, 0, SEEK_END );
            m_lBufferLength = ftell ( file );
            m_szBuffer = new char [ m_lBufferLength + 1 ];
            rewind ( file );
            fread ( m_szBuffer, 1, m_lBufferLength, file );
            fclose ( file );
            ipoHttpResponse->oResponseHeaders [ "content-type" ] = m_szMime;
            ipoHttpResponse->SetBody ( m_szBuffer, m_lBufferLength );
            delete[] m_szBuffer;
            m_szBuffer = NULL;
        }
        else
            ipoHttpResponse->SetBody ( "Can't read file!", strlen("Can't read file!") );
    }
    m_bIsBeingRequested = false;
    return m_responseCode;
}

void CResourceHTMLItem::ClearPageBuffer ( )
{
	if ( m_pPageBuffer )
        delete m_pPageBuffer;

    m_pPageBuffer = new expanding_char ( 500 );
}

void CResourceHTMLItem::SetResponseHeader ( char * szHeaderName, char * szHeaderValue )
{
	m_currentResponse->oResponseHeaders [ szHeaderName ] = szHeaderValue;
}

void CResourceHTMLItem::SetResponseCode	( int responseCode )
{
	m_responseCode = (ResponseCode)responseCode;
}

void CResourceHTMLItem::SetResponseCookie ( char * szCookieName, char * szCookieValue )
{
	CookieParameters params;
	Datum data;
	data = szCookieValue;
	params [ szCookieName ] = data;
	m_currentResponse->SetCookie ( params );
}

CLuaArguments * CResourceHTMLItem::GetQueryString ( char * szUrl )
{
    CLuaArguments * arguments = new CLuaArguments();
    if ( strlen ( szUrl ) != 0 )
    {
        size_t length = strlen ( szUrl );
        size_t i = 0;
        for ( ; i < length && szUrl[i] != '?'; i++ )
        {
        }

        
        if ( length != i )
        {
            char * lpstr = (char *)&szUrl[i + 1];
            if ( lpstr[0] != '\0' )
            {
                int	cequal = 0;	// Location of a '='.
                int	cand = -1;	// Location of a '&' (first at lpstr[-1]).
                int	keysize = 0;	// Size of Key part.
                int	valuesize = 0;	// Size of Value part.
                int i = 0;
                bool bSkip = false;
                do
                {
                    if((lpstr[i] == '&') || (lpstr[i] == '\0'))
                    {
                        if(cequal < cand)
                        {
                            valuesize = 0;	// no '=' in last key=value.
                            keysize = i - cand - 1;
                        }
                        else
                        {
                            valuesize = i - cequal - 1;
                            keysize = cequal - cand - 1;	
                        }

                        // allocate space for the key
                        // TODO: Memory leak?
                        char * keyName = new char[keysize + 1];

                        // copy the key and value.
                        strncpy(keyName, lpstr + cand + 1, keysize);
                        keyName[keysize] = '\0';

                        char * szUnescapedKey = new char [ keysize + 2 ];
                        Unescape ( keyName, szUnescapedKey, keysize + 1 );
                        strncpy(keyName, szUnescapedKey, keysize );
                        delete[] szUnescapedKey;

                        char * szValue = new char[valuesize + 1];
                        strncpy(szValue, lpstr + cequal + 1, valuesize);
                        szValue[valuesize] = '\0';

                        char * szUnescapedValue = new char [ valuesize + 2 ];
                        Unescape ( szValue, szUnescapedValue, valuesize + 1 );
                        strncpy(szValue, szUnescapedValue, valuesize );
                        delete[] szUnescapedValue;

                        arguments->PushString(keyName);
                        arguments->PushString(szValue);

                        cand = i;			
                    }
                    else if(lpstr[i] == '=')
                    {
                        cequal = i;
                    }
                }
                while(lpstr[i++]);
            }
        }
    }
    return arguments;
}

char * CResourceHTMLItem::Unescape ( char * szIn, char * szOut, long bufferSize )
{
    size_t length = strlen ( szIn );
    expanding_char unescaped ( 10 );
    size_t i = 0;
    for ( ; i < length; i++)
    {
        if ( szIn[i] == '%' )
        {
            i++;
            expanding_char szNumberBuffer(5);
            for ( ; i < length; i++ )
            {
                char szLetter[2];
                szLetter[0] = szIn[i];
                szLetter[1] = '\0';
                if ( szIn[i] == '0' || atoi(szLetter) != 0 || (szIn[i] >= 'A' && szIn[i] <= 'F') )
                    szNumberBuffer += szIn[i];
                else
                {
                    break;
                }
            }

            if ( szNumberBuffer.GetLength() == 0 ) // a % sign with nothing after?
            {
                unescaped += '%';
                i--;
            }
            else
            {
                char szHexString[6];
                sprintf ( szHexString, "0X%s", szNumberBuffer.GetValue() );
                unescaped += (char)strtol(szHexString, NULL, 16);
                i--;
            }
        }
        else
            unescaped += (char)szIn[i];
    }
    strncpy ( szOut, unescaped.GetValue(), bufferSize );
    return szOut;
}

bool CResourceHTMLItem::AppendToPageBuffer ( char * szText, size_t length )
{
    if ( szText )
        (*m_pPageBuffer).append(szText, length);
    return true;
}

bool CResourceHTMLItem::Start ( void )
{
    if ( !m_bIsRaw )
    {
        // go through and search for <? or ?>
        FILE * file = fopen ( m_szResourceFileName, "r" );
        if ( file )
        {
            bool bInCode = false;
            bool bJustStartedCodeBlock = false;
            bool bIsShorthandCodeBlock = false;
            expanding_char buffer;
            buffer.append("function renderPage ( requestHeaders, form, cookies, hostname, url, querystring, user )\n");
            buffer.append("\nhttpWrite ( \""); // bit hacky, possibly can be terminated straight away
            unsigned char c;
            unsigned char lastc;
            int i = 0;
            while ( !feof ( file ) )
            {
                c = ReadChar ( file );
                if ( feof ( file ) )
                    break;
                if ( bInCode == false ) // we're in a plain HTML section
                {
                    if ( c == '<' && !feof ( file ) )
                    {
                        lastc = c;
                        c = ReadChar ( file );
                        if ( c == '*' ) // we've found <*
                        {
                            bInCode = true;
                            bJustStartedCodeBlock = true;
                            buffer.append("\" )\n"); // add ") to the end to terminate our last non-code section
                        }
                        else
                        { // we found < but not a *, so just output both characters we read
                            buffer += lastc;
                            buffer += c;
                        }
                    }
                    else
                    {
                        if ( c == '\n' || c == '\r' )
                            buffer.append("\\n");
                        else if ( c == '\\' )
                        {
                            buffer += '\\';
                            buffer += '\\';
                        }
                        else if ( c == '\"' )
                        {
                            buffer += '\\';
                            buffer += '\"';
                        }
                        else
                            buffer += c;
                    }
                } 
                else 
                { // we're in a code block
                    if ( c == '*' && !feof ( file ) )
                    {
                        lastc = c;
                        c = ReadChar ( file );
                        if ( c == '>' ) // we've found *>
                        {
                            bInCode = false;
                            if ( bIsShorthandCodeBlock )
                            {
                                bIsShorthandCodeBlock = false;
                                buffer += ')'; // terminate the 'httpWrite' function
                            }
                            buffer.append ( "\nhttpWrite ( \"" ); // add httpWrite ( " to start a new non-code section
                        }
                        else
                        { // we found * but not a >, so just output both characters we read
                            buffer += lastc;
                            buffer += c;
                        }
                    }
                    else if ( c == '=' && bJustStartedCodeBlock )
                    {
                        buffer.append("httpWrite ( \"\" .. ");
                        bIsShorthandCodeBlock = true;
                    }
                    else
                    {
                        if ( c != '\t' && c != ' ' ) // we allow whitespace before the shorthand '=' sign
                            bJustStartedCodeBlock = false;
                        buffer += c;
                    }
                }
                i++;
            }

            if ( !bInCode )
                buffer.append("\" )\n");
            buffer.append("\nend");
            buffer += '\0';
            m_szBuffer = buffer.GetValueCopy ();

       /*     FILE * debug = fopen ("debug.lua", "w" );
            fwrite ( m_szBuffer, 1, strlen(m_szBuffer), debug );
            fclose ( debug );*/

            m_pVM = g_pGame->GetLuaManager()->CreateVirtualMachine ( m_resource );
            m_pVM->LoadScript ( m_szBuffer );
            m_pVM->SetResourceFile ( this );
            m_pVM->RegisterHTMLDFunctions();

            fclose ( file );

            GetMimeType ( m_szResourceFileName );

            return true;
        }
        return false;
    }
    else
    {
        // its a raw page
        FILE * file = fopen ( m_szResourceFileName, "rb" );
        if ( file )
        {
            GetMimeType ( m_szResourceFileName );

            // don't actually read it here, it could be way too large
            fclose ( file );
            return true;
        }
        return false;
    }
}

void CResourceHTMLItem::GetMimeType ( char * szFilename )
{
    char * extn = NULL;
    size_t filenamelength = strlen(szFilename);
    for ( int i = filenamelength-1; i > 0; i-- )
    {
        if ( szFilename[i] == '.' )
        {
            extn = &szFilename[i+1];
            break;
        }
    }
    if ( extn )
    {
        if ( strcmp ( extn, "css" ) == 0 )
            strcpy ( m_szMime, "text/css" );
        else if ( strcmp ( extn, "png" ) == 0 )
            strcpy ( m_szMime, "image/png" );
        else if ( strcmp ( extn, "gif" ) == 0 )
            strcpy ( m_szMime, "image/gif" );
        else if ( strcmp ( extn, "jpg" ) == 0 || strcmp ( extn, "jpeg" ) == 0 )
            strcpy ( m_szMime, "image/jpg" );
        else if ( strcmp ( extn, "js" ) == 0 )
            strcpy ( m_szMime, "text/javascript" );
        else
            strcpy ( m_szMime, "text/html" );
    }
    else
        strcpy ( m_szMime, "text/html" );
}

bool CResourceHTMLItem::Stop ( void )
{
    if ( m_szBuffer )
        delete[] m_szBuffer;
    m_szBuffer = NULL;
    m_lBufferLength = NULL;

    if ( m_pVM )
        g_pGame->GetLuaManager()->RemoveVirtualMachine ( m_pVM );
    m_pVM = NULL;
    return true;
}
