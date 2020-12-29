
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
or see http://www.gnu.org/licenses/gpl.html

Zac Hansen ( xaxxon@slackworks.com )

*/

// must be outside COMPILE_WITH_SSL check
#include "config.h"

#ifdef COMPILE_WITH_SSL

#include "sslerror.h"


int SslError::nErrorMessagesLoaded = 0;


int SslError::GetError ( std::string & irsReport,
						 int inPeek )
{

	// get the error code
	unsigned long nError = ERR_get_error ( );

	// if there is no error, or we don't want full text, return the error
	//   code now
	if ( nError == 0 || irsReport == "noreport" ) {
		irsReport = "";
		return nError;
	}

	// do we need to load the strings?
	if ( !nErrorMessagesLoaded ) {
		SSL_load_error_strings ( );
		nErrorMessagesLoaded = 1;
	}

	char psBuffer [ 256 ];
	ERR_error_string_n ( nError,
						 psBuffer,
						 256 );

	irsReport = psBuffer;

	return nError;

}

int SslError::PeekError ( std::string & irsReport )
{
	return GetError ( irsReport, 1 );
}


#endif // COMPILE_WITH_SSL
