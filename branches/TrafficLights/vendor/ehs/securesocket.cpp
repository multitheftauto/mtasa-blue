
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

#include "securesocket.h"


#ifdef COMPILE_WITH_SSL
#include <stdio.h>

SslError g_oSslError;


NetworkAbstraction::InitResult 
SecureSocket::Init ( int inPort ///< port on which to listen
	) {

	// registers available cyphers and digests.  Absolutely required.
	SSL_library_init ( );

	
	// check and see if we need to initialize one-time data structures
	pthread_mutex_t oMutex;
	pthread_mutex_init ( &oMutex, NULL );
	pthread_mutex_lock ( &oMutex );

	// no need to ever clean these up

	if ( poDynamicSslLocking == NULL ) {
		poDynamicSslLocking = new DynamicSslLocking;
	}

	if ( poStaticSslLocking == NULL ) {
		poStaticSslLocking = new StaticSslLocking;
	}
		
	// not sure this is needed?
	if ( poSslError == NULL ) {
		poSslError = new SslError;
	}

	if ( poCtx == NULL ) {
		InitializeCertificates ( );

	}

	if ( poCtx == NULL ) {

		return NetworkAbstraction::INITSOCKET_CERTIFICATE;

	}

	pthread_mutex_unlock ( &oMutex );



	// set up the accept socket
	char psBuffer [ 10 ];
	sprintf ( psBuffer, "%d", inPort );
	m_poAcceptBio = BIO_new_accept ( psBuffer );

	if ( m_poAcceptBio == NULL ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "couldn't create accept BIO on port '%s'\n",
				  psBuffer );
#endif
		assert ( 0 );
	}

	int nAcceptSocketFd = BIO_get_fd ( m_poAcceptBio, NULL );
//	fprintf ( stderr, "Got accept socket fd = '%d'\n", nAcceptSocketFd );

	u_long MyTrueVariable = 1;
	ioctl ( nAcceptSocketFd, FIONBIO, &MyTrueVariable );


	if ( BIO_do_accept ( m_poAcceptBio ) <= 0 ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "Couldn't set up accept BIO on port: '%d'\n",
				  inPort );
#endif
		return NetworkAbstraction::INITSOCKET_BINDFAILED;
	}

	return NetworkAbstraction::INITSOCKET_SUCCESS;

}


int SecureSocket::GetFd ( ) 
{

	// there has got to be a better place for this, but this should work
	if ( m_nFd == -1 ) {
		m_nFd = BIO_get_fd ( m_poAcceptBio, NULL );
	}

	return m_nFd;

}


NetworkAbstraction * SecureSocket::Accept ( ) 
{
#ifdef EHS_DEBUG
	fprintf ( stderr, "Accept called with acceptbio = %x\n",
			  m_poAcceptBio );
#endif
	
	int nAcceptSocket =  BIO_get_fd ( m_poAcceptBio, NULL );
//	fprintf ( stderr, "accepting fd='%d'\n",
//			  nAcceptSocket );
	assert ( nAcceptSocket >= 0 );

	SecureSocket * poNewSecureSocket = NULL;
	
	if ( BIO_do_accept ( m_poAcceptBio ) < 0 ) {
		
#ifdef EHS_DEBUG
		fprintf ( stderr, "Error accepting new connection\n" );
#endif

	} else {


		
		BIO * poClientBio = BIO_pop ( m_poAcceptBio );
//		fprintf ( stderr, "Got new connection with new BIO='%x' fd='%d'\n",
//				  poClientBio,
//				  BIO_get_fd ( poClientBio, NULL ) );

		assert ( poClientBio != NULL );

		SSL * poClientSsl = SSL_new ( poCtx );
		std::string sErrorString;

//		g_oSslError.GetError ( sErrorString );
//		fprintf ( stderr, "Error: %s\n", sErrorString.c_str ( ) );
		if ( poClientSsl == NULL ) {
//			fprintf ( stderr, "poClientSsl == NULL\n" );
		}
		SSL_set_accept_state ( poClientSsl );
		SSL_set_bio ( poClientSsl, poClientBio, poClientBio );

		poNewSecureSocket = new SecureSocket ( poClientSsl, poClientBio );


		socklen_t oInternetSocketAddressLength = sizeof ( poNewSecureSocket->oInternetSocketAddress );

		getpeername ( poNewSecureSocket->GetFd ( ),
					  (sockaddr*) &(poNewSecureSocket->oInternetSocketAddress),
					  &oInternetSocketAddressLength );

	}

	return poNewSecureSocket;

}




// SSL-only function for initializing special random numbers
int SecureSocket::SeedRandomNumbers ( int inBytes ) {
	
	if ( !RAND_load_file ( "/dev/random", inBytes ) ) {
		return 0;
	}
	
	return 1;
	
}


int PeerCertificateVerifyCallback ( int inOk,
									X509_STORE_CTX * ipoStore ) {

	char psBuffer [ 256 ];
//	fprintf ( stderr, "inOk = %d\n", inOk );
	if ( !inOk ) {

		X509 * poCertificate = X509_STORE_CTX_get_current_cert ( ipoStore );
		int nDepth = X509_STORE_CTX_get_error_depth ( ipoStore );
		int nError = X509_STORE_CTX_get_error ( ipoStore );

#ifdef EHS_DEBUG
		fprintf ( stderr, "Error in certificate at depth: %i\n", nDepth );
		X509_NAME_oneline ( X509_get_issuer_name ( poCertificate ), psBuffer, 256 );
		fprintf ( stderr, "  issuer  = '%s'\n", psBuffer );
		X509_NAME_oneline ( X509_get_subject_name ( poCertificate ), psBuffer, 256 );
		fprintf ( stderr, "  subject = '%s'\n", psBuffer );
		fprintf ( stderr, "  error %i,%s\n", nError, X509_verify_cert_error_string ( nError ) );
#endif
	}
	
	return inOk;

}


int deleteme ( char * buf, int size, int flag, void * userdata ) 
{
#ifdef EHS_DEBUG
	fprintf ( stderr, "private key callback buffer size=%d, userdata=%x\n",
			  size, userdata );
#endif

	strcpy ( buf, "comoesta" );

	return 8;

}


SSL_CTX * 
SecureSocket::InitializeCertificates ( ) {
		
	// set up the CTX object in compatibility mode.
	//   We'll remove SSLv2 compatibility later for security reasons
	poCtx = SSL_CTX_new ( SSLv23_method ( ) );
		
	if ( poCtx == NULL ) {
		std::string sError;
		g_oSslError.GetError ( sError );

#ifdef EHS_DEBUG
		fprintf ( stderr, "Error creating CTX object: '%s'\n", sError.c_str ( ) );
#endif
		return NULL;
	}
			
#ifdef EHS_DEBUG
	if ( m_sServerCertificate == "" ) {
		fprintf ( stderr, "No filename for server certificate specified\n" );
	} else {
		fprintf ( stderr, "Using '%s' for certificate\n",
				  m_sServerCertificate.c_str ( ) );
	}
#endif


	/*
	// this sets default locations for trusted CA certificates.  This is not
	//   necessary since we're not dealing with client-side certificat
	if ( ( m_sCertificateFile != "" ||
		   m_sCertificateDirectory != "" ) ) {
		if ( SSL_CTX_load_verify_locations ( poCtx, 
											 m_sCertificateFile.c_str ( ),
											 m_sCertificateDirectory != "" ? m_sCertificateDirectory.c_str ( ) : NULL ) != 1 ) {
			std::string sError;
			g_oSslError.GetError ( sError );
			fprintf ( stderr, "Error loading custom certificate file and/or directory: '%s'\n", sError.c_str ( ) );
			delete poCtx;
			return NULL;

		}
	}
	*/
			
	/*
	// Unknown what this does
	if ( SSL_CTX_set_default_verify_paths ( poCtx ) != 1 ) {
		fprintf ( stderr, "Error loading default certificate file and/or directory\n" );
		return NULL;
	}
	*/
		

	// if no callback is specified, use the default one
	if ( m_pfOverridePassphraseCallback == NULL ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "setting passphrase callback to default\n" );
#endif
		m_pfOverridePassphraseCallback = SecureSocket::DefaultCertificatePassphraseCallback;
	} else {
#ifdef EHS_DEBUG
		fprintf ( stderr, "NOT setting passphrase callback to default\n" );
#endif
	}

	if ( m_sServerCertificatePassphrase != "" ) {

#ifdef EHS_DEBUG
		fprintf ( stderr, "default callback is at '%x'\n",SecureSocket::DefaultCertificatePassphraseCallback);
				  
		fprintf ( stderr, "setting callback to '%x' and passphrase = '%s'\n",
				  m_pfOverridePassphraseCallback,
				  m_sServerCertificatePassphrase.c_str ( ) );
#endif

		// set the callback
		SSL_CTX_set_default_passwd_cb ( poCtx, 
										m_pfOverridePassphraseCallback );

		// set the data
		SSL_CTX_set_default_passwd_cb_userdata ( poCtx, 
												 (void *) &m_sServerCertificatePassphrase );

	}


	if ( m_sServerCertificate != "" ) {
		if ( SSL_CTX_use_certificate_chain_file ( poCtx, m_sServerCertificate.c_str ( ) ) != 1 ) {
			std::string sError;
			g_oSslError.GetError ( sError );
#ifdef EHS_DEBUG
			fprintf ( stderr, "Error loading server certificate '%s': '%s'\n", m_sServerCertificate.c_str ( ), sError.c_str ( ) );
#endif
			delete poCtx;
			poCtx = NULL;
			return NULL;
		}
	}
	

	if ( m_sServerCertificate != "" ) {
		if ( SSL_CTX_use_PrivateKey_file ( poCtx, m_sServerCertificate.c_str ( ), SSL_FILETYPE_PEM ) != 1 ) {

#ifdef EHS_DEBUG
			fprintf ( stderr, "Error loading private key\n" );
#endif
			delete poCtx;
			poCtx = NULL;
			return NULL;

		}
	}
		
	SSL_CTX_set_verify ( poCtx, 0//SSL_VERIFY_PEER 
						 //| SSL_VERIFY_FAIL_IF_NO_PEER_CERT
						 ,
						 PeerCertificateVerifyCallback );
			
	//SSL_CTX_set_verify_depth ( poCtx, 4 );
			
	// set all workarounds for buggy clients and turn off SSLv2 because it's insecure
	SSL_CTX_set_options ( poCtx, SSL_OP_ALL | SSL_OP_NO_SSLv2 );
			
	if ( SSL_CTX_set_cipher_list ( poCtx, CIPHER_LIST ) != 1 ) {
#ifdef EHS_DEBUG
		fprintf ( stderr, "Error setting ciper list (no valid cipers)\n" );
#endif
		delete poCtx;
		poCtx = NULL;
		return NULL;
	}
			
	return poCtx;

}


int SecureSocket::Read ( void * ipBuffer, int ipBufferLength )
{

//	fprintf ( stderr, "SecureSocket::Read m_poAcceptBio = '%x' ipBuffer = '%x', ipBufferLenght = '%d'\n",
//			  m_poAcceptBio, ipBuffer, ipBufferLength );

	int nReadResult = SSL_read (m_poAcceptSsl, ipBuffer, ipBufferLength );
//	fprintf ( stderr, "Got readresult = %d\n", nReadResult );
	// TODO: should really handle errors here

	return nReadResult;

}

int SecureSocket::Send ( const void * ipMessage, size_t inLength, int )
{

	int nWriteResult = SSL_write ( m_poAcceptSsl, ipMessage, inLength );

	// TODO: should really handle errors here
	
	return nWriteResult;

}

void SecureSocket::Close ( )
{

	m_nClosed = 1;
	BIO_free_all ( m_poAcceptBio );

}

int 
SecureSocket::DefaultCertificatePassphraseCallback ( char * ipsBuffer,
													 int inSize,
													 int inRWFlag,
													 void * ipUserData )
{
#ifdef EHS_DEBUG
	fprintf ( stderr, "Using default certificate passphrase callback function\n" );
#endif
	strcpy ( ipsBuffer, ((std::string *)ipUserData)->c_str ( ) );

	int nLength = ((std::string *)ipUserData)->length ( );

	return nLength;

}


void 
SecureSocket::SetPassphraseCallback ( int ( * ipfOverridePassphraseCallback ) ( char *, int, int, void * ) )
{

	m_pfOverridePassphraseCallback = ipfOverridePassphraseCallback;

}


std::string SecureSocket::GetAddress ( )
{

	// buffer long enough to hold 255.255.255.255
	char psBuffer [ 25 ];

	sprintf ( psBuffer,
			  "%d.%d.%d.%d",
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 24 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 16 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 8 ) & 0xff,
			  (ntohl(oInternetSocketAddress.sin_addr.s_addr) >> 0 ) & 0xff
		);

	return psBuffer;


}

int SecureSocket::GetPort ( )
{

	return ntohs ( oInternetSocketAddress.sin_port );

}


#endif // COMPILE_WITH_SSL
