
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


#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#ifdef COMPILE_WITH_SSL

#include <openssl/ssl.h>
#include <openssl/rand.h>


#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <string>

#include "networkabstraction.h"
#include "dynamicssllocking.h"
#include "staticssllocking.h"
#include "sslerror.h"


/** use all cipers except adh=anonymous ciphers, low=64 or 54-bit cipers,
 * exp=export crippled ciphers, or md5-based ciphers that have known weaknesses
 * @STRENGTH means order by number of bits
 */
#define CIPHER_LIST "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"


/// Secure socket implementation used for HTTPS
class SecureSocket : public NetworkAbstraction 
{

  public:
	
	/// has close been called on this object?
	int m_nClosed;

	/// initialize OpenSSL and this socket object
	virtual InitResult Init ( int inPort );

	/// constructor for server socket
	SecureSocket ( std::string isServerCertificate = "",
				   std::string isServerCertificatePassphrase = "" ) : 
		m_poAcceptBio ( NULL ),
		m_sServerCertificate ( isServerCertificate ),
		m_sServerCertificatePassphrase ( isServerCertificatePassphrase ),
		m_pfOverridePassphraseCallback ( NULL ),
		m_nClosed ( 0 ),
		m_nFd ( -1 )
		{ 
#ifdef EHS_DEBUG
			fprintf ( stderr, "calling SecureSocket constructor A\n" );
#endif
		}

	/// constructor for client socket
	SecureSocket ( SSL * ipoAcceptSsl, 
				   BIO * ipoAcceptBio, 
				   std::string isServerCertificate = "",
				   std::string isServerCertificatePassphrase = "") : 
		m_poAcceptSsl ( ipoAcceptSsl ),
		m_poAcceptBio ( ipoAcceptBio ),
		m_sServerCertificate ( isServerCertificate ),
		m_sServerCertificatePassphrase ( isServerCertificatePassphrase ),
		m_pfOverridePassphraseCallback ( NULL ),
		m_nClosed ( 0 ),
		m_nFd ( -1 )
		{
#ifdef EHS_DEBUG
			fprintf ( stderr, "calling SecureSocket constructor B\n" );
#endif
		}

	/// destructor
	virtual ~SecureSocket ( ) {}

	/// accepts on secure socket
	virtual NetworkAbstraction * Accept ( );

	/// returns true because this socket is considered secure
	virtual int IsSecure ( ) { return 1; }

	/// does random number stuff using OpenSSL 
	int SeedRandomNumbers ( int inBytes );

	/// returns the FD associated with this secure socket
	virtual int GetFd ( );

	/// deals with certificates for doing secure communication
	SSL_CTX * InitializeCertificates ( );

	/// does OpenSSL read
	virtual int Read ( void * ipBuffer, int ipBufferLength );

	/// does OpenSSL send
	virtual int Send ( const void * ipMessage, size_t inLength, int inFlags = 0 );
	
	/// closes OpenSSL connectio
	virtual void Close ( );

	/// default callback that just uses m_sServerCertificatePassphrase member
	static int DefaultCertificatePassphraseCallback ( char * ipsBuffer,
													  int inSize,
													  int inRWFlag,
													  void * ipUserData );
	
	/// sets a callback for loading a certificate
	void SetPassphraseCallback ( int ( * m_ipfOverridePassphraseCallback ) ( char *, int, int, void * ) );
	
	/// stores the address of the current connection
	sockaddr_in oInternetSocketAddress;

  protected:

	/// stores the file descriptor.  Needed for dealing with the object after the FD is closed because it's indexed by FD elsewhere
	int m_nFd; 

	/// the BIO associated with this connection
	BIO * m_poAcceptBio;

	/// the SSL object associated with this SSL connectio
	SSL * m_poAcceptSsl;

	/// filename for certificate file
	std::string m_sServerCertificate;

	/// passphrase for certificate
	std::string m_sServerCertificatePassphrase; 

	/// pointer to callback function
	int (*m_pfOverridePassphraseCallback)(char*, int, int, void*);


	// STATIC VARIABLES

	/// dynamic portion of SSL locking mechanism
	static DynamicSslLocking * poDynamicSslLocking;

	/// static portion of SSL locking mechanism
	static StaticSslLocking * poStaticSslLocking;

	/// error object for getting openssl error messages
	static SslError * poSslError;

	/// certificate information
	static SSL_CTX * poCtx;

	/// gets the address associated with this connection
	std::string GetAddress ( );

	/// gets the port assocaited with this connection
	int GetPort ( );

};

/// global error object
extern SslError g_oSslError;

#endif

#endif // SECURE_SOCKET_H
