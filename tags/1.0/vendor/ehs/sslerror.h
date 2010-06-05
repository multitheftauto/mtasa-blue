
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


#ifndef SSL_ERROR
#define SSL_ERROR

#ifdef COMPILE_WITH_SSL

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

/// wrapper for the OpenSSL error mechanism
class SslError 
{

  public:

	/// gets info about the previous error and removes from SSL error queue returns 0 when no error available on queue
	int GetError ( std::string & irsReport,
				   int inPeek = 0 );

	/// gets info about the previous error and leaves it on SSL error queue returns 0 when no error available on queue
	int PeekError ( std::string & irsReport );


  protected:

	/// represents whether the error strings have been loaded
	static int nErrorMessagesLoaded;

};

#endif // COMPILE_WITH_SSL

#endif // SSL_ERROR

