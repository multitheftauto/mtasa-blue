/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHTTPD.cpp
*  PURPOSE:     Built-in HTTP webserver class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

CHTTPD::CHTTPD ( void )
{
    m_resource = NULL;
    m_server = NULL;
    m_bStartedServer = false;

    m_pGuestAccount = new CAccount ( g_pGame->GetAccountManager (), false, "http_guest" );
}


CHTTPD::~CHTTPD ()
{
    // Stop the server if we started it
    if ( m_bStartedServer )
    {
        // Stop the server
        StopServer ();

        // Wait for the server threads to stop. To prevent a crash a while after ending.
        // StopServer should really have waited for this itself or provide a method to
        // tell whether it's done or not, but it doesn't so this seems like the only way.
        // :(
        Sleep ( 1000 );
    }
}

bool CHTTPD::StartHTTPD ( const char* szIP, unsigned int port )
{
    bool bResult = false;

    // Server not already started?
    if ( !m_bStartedServer )
    {
	    EHSServerParameters parameters;

	    char szPort[10];
        itoa ( port, szPort, 10 );
	    parameters[ "port" ] = szPort;

        if ( szIP )
        {
            // Convert the dotted ip to a long
            long lIP = inet_addr ( szIP );
            parameters[ "bindto" ] = lIP;
        }
        else
        {
            // Bind to default;
            parameters[ "bindto" ] = (long) INADDR_ANY;
        }

	    parameters[ "mode" ] = "threadpool";		// or "singlethreaded"/"threadpool"
	    parameters[ "threadcount" ] = 5;				// unnecessary because 1 is the default

        bResult = ( StartServer ( parameters ) == STARTSERVER_SUCCESS );
        m_bStartedServer = true;
    }

    return bResult;
}


// creates a page based on user input -- either displays data from
//   form or presents a form for users to submit data.
ResponseCode CHTTPD::HandleRequest ( HttpRequest * ipoHttpRequest,
										 HttpResponse * ipoHttpResponse )
{
    CAccount * account = CheckAuthentication ( ipoHttpRequest );

    if ( account )
    {
        if ( !m_strDefaultResourceName.empty () )
        {
            SString strWelcome ( "<a href='/%s/'>This is the page you want</a>", m_strDefaultResourceName.c_str () );
            ipoHttpResponse->SetBody ( strWelcome.c_str (), strWelcome.size () );
            SString strNewURL ( "http://%s/%s/", ipoHttpRequest->oRequestHeaders["host"].c_str(), m_strDefaultResourceName.c_str () );
            ipoHttpResponse->oResponseHeaders["location"] = strNewURL.c_str ();
            return HTTPRESPONSECODE_302_FOUND;

            /*CAccessControlListManager * pACLManager = g_pGame->GetACLManager();
            char * szAccountName = account->GetName();

            if ( pACLManager->CanObjectUseRight ( szAccountName,
												  CAccessControlListGroupObject::OBJECT_TYPE_USER,
												  m_szDefaultResourceName,
												  CAccessControlListRight::RIGHT_TYPE_RESOURCE,
												  true ) &&
                pACLManager->CanObjectUseRight ( szAccountName,
												 CAccessControlListGroupObject::OBJECT_TYPE_USER,
												 "http",
												 CAccessControlListRight::RIGHT_TYPE_GENERAL,
												 true ) )
            {
                CResource * resource = g_pGame->GetResourceManager()->GetResource ( m_szDefaultResourceName );
                if ( resource )
                {
					ResponseCode ret = resource->HandleRequest ( ipoHttpRequest, ipoHttpResponse );

					// Log if this request was not a 200 OK response
					if ( ret != HTTPRESPONSECODE_200_OK )
						CLogger::LogPrintf ( "HTTPD: Request from %s (%d: %s)\n", ipoHttpRequest->GetAddress ().c_str (), ret, ipoHttpRequest->sUri.c_str () );

                    return ret;
                }
            }
            else
            {
                return RequestLogin ( ipoHttpResponse );
            }*/
        }
    }

    SString strWelcome ( "You haven't set a default resource in your configuration file. You can either do this or visit http://%s/<i>resourcename</i>/ to see a specific resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.", ipoHttpRequest->oRequestHeaders["host"].c_str() );
    ipoHttpResponse->SetBody ( strWelcome.c_str (), strWelcome.size () );
    return HTTPRESPONSECODE_200_OK;
}

ResponseCode CHTTPD::RequestLogin ( HttpResponse * ipoHttpResponse )
{
    const char* szAuthenticateMessage = "Access denied, please login";
    ipoHttpResponse->SetBody ( szAuthenticateMessage, strlen(szAuthenticateMessage) );
    SString strName ( "Basic realm=\"%s\"", g_pGame->GetConfig ()->GetServerName ().c_str () );
    ipoHttpResponse->oResponseHeaders [ "WWW-Authenticate" ] = strName.c_str ();
    return HTTPRESPONSECODE_401_UNAUTHORIZED;
}

CAccount * CHTTPD::CheckAuthentication ( HttpRequest * ipoHttpRequest )
{
    string authorization = ipoHttpRequest->oRequestHeaders["authorization"];
    if ( authorization.length() > 6 )
    {
        if ( authorization.substr(0,6) == "Basic " )
        {
            // Basic auth
            std::string strAuthDecoded;
            Base64::decode ( authorization.substr(6), strAuthDecoded );

            string authName;
            string authPassword;
            for ( size_t i = 0; i < strAuthDecoded.length(); i++ )
            {
                if ( strAuthDecoded.substr(i, 1) == ":" )
                {
                    authName = strAuthDecoded.substr(0,i);
                    authPassword = strAuthDecoded.substr(i+1);
                    break;
                }
            }

            CAccount * account = g_pGame->GetAccountManager()->Get ( (char *)authName.c_str());
            if ( account )
            {
                // Check that the password is right
				if ( account->IsPassword ( authPassword.c_str () ) )
                {
                    // Check that it isn't the Console account
                    std::string strAccountName = account->GetName ();
                    if ( strAccountName.compare ( "Console" ) != 0 )
                    {
                        // @@@@@ Check they can access HTTP
                        return account;
                    }
                }
            }
        }
    }
    return m_pGuestAccount;
}
