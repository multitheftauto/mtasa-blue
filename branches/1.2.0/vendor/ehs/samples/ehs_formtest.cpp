
/*

  EHS is a library for adding web server support to a C++ application
  Copyright (C) 2001, 2002 Zac Hansen
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; version 2
  of the License only.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  
  Zac Hansen ( xaxxon@slackworks.com )

*/


#include <string>
#include <list>

#include "../ehs.h"

typedef std::list < std::string > StringList;

class FormTester : public EHS {

public:

	FormTester ( ) {}

	ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
								 HttpResponse * ipoHttpResponse );
	
	StringList oNameList;
	

};


// creates a page based on user input -- either displays data from
//   form or presents a form for users to submit data.
ResponseCode FormTester::HandleRequest ( HttpRequest * ipoHttpRequest,
										 HttpResponse * ipoHttpResponse )
{
	char psHtml[ 5000 ];
	

	// if we got data from the user, show it
	if ( ipoHttpRequest->oFormValueMap [ "user" ].sBody.length ( ) ||
		 ipoHttpRequest->oFormValueMap [ "existinguser" ].sBody.length ( ) ) {
			
		std::string sName;
			
		sName = ipoHttpRequest->oFormValueMap [ "existinguser" ].sBody;
		if ( ipoHttpRequest->oFormValueMap [ "user" ].sBody.length() ) {
			sName = ipoHttpRequest->oFormValueMap [ "user" ].sBody;
		}

		fprintf ( stderr, "Got name of %s\n", sName.c_str ( ) );
			
		char * psHtml = new char [ 5000 ];
		sprintf ( psHtml, "<html><head><title>StringList</title></head>\n<body>Hi %s</body></html>", sName.c_str ( ) );
		oNameList.push_back ( sName );
			
		ipoHttpResponse->SetBody( psHtml, strlen( psHtml ) );
		return HTTPRESPONSECODE_200_OK;

	} else {

		// otherwise, present the form to the user to fill in
		fprintf ( stderr, "Got no form data\n" );

		// create the options for the dropdown box
		char * psOptions;
        if ( oNameList.size ( ) > 0 )
            psOptions = new char [ oNameList.size ( ) * 200 ];
        else
            psOptions = new char [1];

		psOptions [ 0 ] = '\0';

		for ( StringList::iterator oCurrentName = oNameList.begin();
			  oCurrentName != oNameList.end ( );
			  oCurrentName++ ) {

			char psOption [ 200 ];
			sprintf ( psOption, "<option>%s\n",
					  oCurrentName->substr ( 0, 150 ).c_str ( ) );
			strcat ( psOptions, psOption );
			
		}

		sprintf ( psHtml, "<html><head><title>StringList</title></head> <body>Please log in<P> <form action = \"/\" method=GET> User name: <input type = text  name = user><BR> <select name = existinguser width = 20> %s </select> <input type = submit> </form>\n", 
				  psOptions );
		
        delete[] psOptions;

		ipoHttpResponse->SetBody( psHtml, strlen( psHtml ) );
		return HTTPRESPONSECODE_200_OK;

	}

}

void PrintUsage ( int argc, char ** argv ) {
	printf ( "usage: %s <port> [<certificate_file> <certificate_passphrase>]\n", argv[0] );
	printf ( "\tIf you specify the last 2 parameters, it will run in https mode\n" );
	exit ( 0 );

}

// create a multithreaded EHS object with HTTPS support based on command line
//   options.
int main ( int argc, char ** argv )
{

	if ( argc != 2 && argc != 4 ) {
		PrintUsage ( argc, argv );
	}

	FormTester srv;

	int nUseHttps = 0;
	if ( argc > 2 ) {
		nUseHttps = atoi ( argv [ 2 ] );
	}
	EHSServerParameters oSP;


	oSP["port"] = argv [ 1 ];

	oSP [ "mode" ] = "threadpool";

	// unnecessary because 1 is the default
	oSP["threadcount"] = 1;


	if ( argc == 4 ) {
		printf ( "in https mode\n" );
		oSP["https"] = 1;
		printf ( "https = %d\n", oSP["https"].GetInt ( ) );
		oSP["certificate"] = argv [ 2 ];
		oSP["passphrase"] = argv [ 3 ];

	}	

	srv.StartServer ( oSP );

	while ( 1 ) {
		sleep ( 1 );
	}

	srv.StopServer ( );

}

