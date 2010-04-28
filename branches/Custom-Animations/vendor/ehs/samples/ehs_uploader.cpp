/*

  This is a sample EHS program that allows you to upload
  a file to the server 

*/

#include <iostream>
#include <fstream>

using namespace std;

#include "../ehs.h"

std::string g_sPort = "";



class FileUploader : public EHS {

public:

	FileUploader ( ) {}
		
	ResponseCode HandleRequest ( HttpRequest * ipoHttpRequest,
											 HttpResponse * ipoHttpResponse );


};



ResponseCode FileUploader::HandleRequest ( HttpRequest * ipoHttpRequest,
										   HttpResponse * ipoHttpResponse )
{

	// make a regex to get rid of the path if it exists
	const char * psErrorString;
	int nErrorOffset;
	// zero or more anything followed by a backslash or forward slash
	//   followed by one or more things that aren't a backslash or forward slash
	//   it takes four backslashes \\\\ to make an actual backslash
	PME oFilenameRegex ( ".*?[\\\\\\/]?([^\\\\\\/]+)$" );

	std::string sUri = ipoHttpRequest->sUri;
	fprintf ( stderr, "url: %s\n", sUri.c_str ( ) );

	if ( sUri == "/" ) {
		
		std::string sBody = " <html><head><title>ehs uploader</title></head> <body> <form method = POST action=";
	  
		sBody += "/upload.html enctype=multipart/form-data> upload file:<BR> <input type=file name=file><BR> <input type=submit value=submit> </form> </body> </html>";


		ipoHttpResponse->SetBody ( sBody.c_str ( ), sBody.length ( ) );
		return HTTPRESPONSECODE_200_OK;

	}
	// it doesn't have to end in .html.. just sort of traditional
	else if ( sUri == "/upload.html" ) {
		fprintf ( stderr, "body length = %d\n", 
				  ipoHttpRequest->oFormValueMap [ "file" ].sBody.length ( ) );
		if ( ipoHttpRequest->oFormValueMap [ "file" ].sBody.length ( ) != 0 ) {

			std::string sFilename = ipoHttpRequest->oFormValueMap [ "file" ].
				oContentDisposition.oContentDispositionHeaders [ "filename" ];

			fprintf ( stderr, "sFilename = '%s'\n", sFilename.c_str ( ) );

			int pnVector [ 120 ];
			int nMatchResult;

			int nMatches = oFilenameRegex.match ( sFilename );
			fprintf ( stderr, "Found %d matches\n", nMatches );
			
			fprintf ( stderr, "'%s' %d -- %d:%d\n",
					  sFilename.c_str ( ),
					  nMatchResult, pnVector [ 2 ], pnVector [ 3 ] );

			sFilename = oFilenameRegex [ 1 ];

			if ( sFilename.length ( ) != 0 ) {
				fprintf ( stderr, "Writing '%d' bytes to filename: '%s'\n",
						  ipoHttpRequest->oFormValueMap [ "file" ].sBody.length ( ),
						  sFilename.c_str ( ) );
				ofstream outfile ( sFilename.c_str ( ) );
				outfile.write ( ipoHttpRequest->oFormValueMap [ "file" ].sBody.c_str ( ),
								ipoHttpRequest->oFormValueMap [ "file" ].sBody.length ( ) );

				outfile.close ( );

			} else {
				fprintf ( stderr, "NO FILENAME FOUND\n" );
				assert ( 0 );

			}


		} 
		// there was nothing sent in as a file
		else {
			
			ipoHttpResponse->SetBody ( "Must upload file", strlen ( "Must upload file" ) );
			return HTTPRESPONSECODE_200_OK;



		}

	} 

		return HTTPRESPONSECODE_404_NOTFOUND;

}



int main ( int argc, char ** argv )
{

	if ( argc < 2 ) {
		printf ( "usage: %s <port>\n", argv[0] );
		exit ( 0 );
	}

	FileUploader srv;

	EHSServerParameters oSP;
	oSP["port"] = argv [ 1 ];
	oSP [ "mode" ] = "threadpool";
	// unnecessary as 1 is the default
	oSP["threadcount"] = 1;

	srv.StartServer ( oSP );

	while ( 1 ) {
		sleep ( 1 );
	}

	srv.StopServer ( );

}









