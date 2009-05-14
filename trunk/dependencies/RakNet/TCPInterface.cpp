/// \file
/// \brief A simple TCP based server allowing sends and receives.  Can be connected to by a telnet client.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "TCPInterface.h"
#ifdef _WIN32
typedef int socklen_t;
#else
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <string.h>
#include "RakAssert.h"
#include <stdio.h>
#include "RakAssert.h"
#include "RakSleep.h"
#include "StringCompressor.h"
#include "StringTable.h"

#ifdef _DO_PRINTF
#endif

#ifdef _WIN32
#include "WSAStartupSingleton.h"
#endif

RAK_THREAD_DECLARATION(UpdateTCPInterfaceLoop);
RAK_THREAD_DECLARATION(ConnectionAttemptLoop);

#ifdef _MSC_VER
#pragma warning( push )
#endif

TCPInterface::TCPInterface()
{
	isStarted=false;
	threadRunning=false;
	listenSocket=(SOCKET) -1;

	StringCompressor::AddReference();
	RakNet::StringTable::AddReference();

#if defined(OPEN_SSL_CLIENT_SUPPORT)
	ctx=0;
	meth=0;
#endif

#ifdef _WIN32
	WSAStartupSingleton::AddRef();
#endif
}
TCPInterface::~TCPInterface()
{
	Stop();
#ifdef _WIN32
	WSAStartupSingleton::Deref();
#endif

	StringCompressor::RemoveReference();
	RakNet::StringTable::RemoveReference();
}
bool TCPInterface::Start(unsigned short port, unsigned short maxIncomingConnections)
{
	if (isStarted)
		return false;

	isStarted=true;

	if (maxIncomingConnections>0)
	{
		listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if ((int)listenSocket ==-1)
			return false;

		struct sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);

		if (bind(listenSocket,(struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0)
			return false;

		listen(listenSocket, maxIncomingConnections);
	}

	// Start the update thread
	int errorCode = RakNet::RakThread::Create(UpdateTCPInterfaceLoop, this);
	if (errorCode!=0)
		return false;

	return true;
}
void TCPInterface::Stop(void)
{
	if (isStarted==false)
		return;

	unsigned i;
#if defined(OPEN_SSL_CLIENT_SUPPORT)
	for (i=0; i < remoteClients.Size(); i++)
		remoteClients[i]->DisconnectSSL();
#endif

	isStarted=false;

	if (listenSocket!=(SOCKET) -1)
	{
#ifdef _WIN32
		shutdown(listenSocket, SD_BOTH);
#else		
		shutdown(listenSocket, SHUT_RDWR);
#endif
		closesocket(listenSocket);
		listenSocket=(SOCKET) -1;
	}

	// Abort waiting connect calls
	blockingSocketListMutex.Lock();
	for (i=0; i < blockingSocketList.Size(); i++)
	{
		closesocket(blockingSocketList[i]);
	}
	blockingSocketListMutex.Unlock();

	// Wait for the thread to stop
	while ( threadRunning )
		RakSleep(15);

	RakSleep(100);

	// Stuff from here on to the end of the function is not threadsafe
	for (i=0; i < remoteClients.Size(); i++)
	{
		closesocket(remoteClients[i]->socket);
#if defined(OPEN_SSL_CLIENT_SUPPORT)
		remoteClients[i]->FreeSSL();
#endif
		RakNet::OP_DELETE(remoteClients[i], __FILE__, __LINE__);
	}
	remoteClients.Clear();

	for (i=0; i < remoteClientsInsertionQueue.Size(); i++)
	{
		closesocket(remoteClientsInsertionQueue[i]->socket);
		RakNet::OP_DELETE(remoteClientsInsertionQueue[i], __FILE__, __LINE__);
	}
	remoteClientsInsertionQueue.Clear();

	outgoingMessages.Clear();
	incomingMessages.Clear();
	newIncomingConnections.Clear();
	newRemoteClients.Clear();
	lostConnections.Clear();
	requestedCloseConnections.Clear();
	failedConnectionAttempts.Clear();
	completedConnectionAttempts.Clear();
	failedConnectionAttempts.Clear();
	for (i=0; i < headPush.Size(); i++)
		DeallocatePacket(headPush[i]);
	headPush.Clear();
	for (i=0; i < tailPush.Size(); i++)
		DeallocatePacket(tailPush[i]);
	tailPush.Clear();

#if defined(OPEN_SSL_CLIENT_SUPPORT)
	SSL_CTX_free (ctx);
	startSSL.Clear();
	activeSSLConnections.Clear();
#endif
}
SystemAddress TCPInterface::Connect(const char* host, unsigned short remotePort, bool block)
{
	if (block)
	{
		SystemAddress systemAddress;
		systemAddress.binaryAddress=inet_addr(host);
		systemAddress.port=remotePort;

		SOCKET sockfd = SocketConnect(host, remotePort);
		if (sockfd==(SOCKET)-1)
		{	
			failedConnectionAttemptMutex.Lock();
			failedConnectionAttempts.Push(systemAddress);
			failedConnectionAttemptMutex.Unlock();

			return UNASSIGNED_SYSTEM_ADDRESS;
		}

		RemoteClient *remoteClient;
		remoteClient = AllocRemoteClient( __FILE__, __LINE__ );
		remoteClient->socket=sockfd;
		remoteClient->systemAddress=systemAddress;
		InsertRemoteClient(remoteClient);

		completedConnectionAttemptMutex.Lock();
		completedConnectionAttempts.Push(remoteClient->systemAddress);
		completedConnectionAttemptMutex.Unlock();

		return remoteClient->systemAddress;
	}
	else
	{
		ThisPtrPlusSysAddr *s = RakNet::OP_NEW<ThisPtrPlusSysAddr>( __FILE__, __LINE__ );
		s->systemAddress.SetBinaryAddress(host);
		s->systemAddress.port=remotePort;
		s->tcpInterface=this;

		// Start the connection thread
		int errorCode = RakNet::RakThread::Create(ConnectionAttemptLoop, s);
		if (errorCode!=0)
		{
			RakNet::OP_DELETE(s, __FILE__, __LINE__);
			failedConnectionAttempts.Push(s->systemAddress);
		}
		return UNASSIGNED_SYSTEM_ADDRESS;
	}	
}
#if defined(OPEN_SSL_CLIENT_SUPPORT)
void TCPInterface::StartSSLClient(SystemAddress systemAddress)
{
	if (ctx==0)
	{
		SSLeay_add_ssl_algorithms();
		meth = SSLv2_client_method();
		SSL_load_error_strings();
		ctx = SSL_CTX_new (meth);
		RakAssert(ctx!=0);
	}

	SystemAddress *id = startSSL.WriteLock();
	*id=systemAddress;
	startSSL.WriteUnlock();
	unsigned index = activeSSLConnections.GetIndexOf(systemAddress);
	if (index==(unsigned)-1)
		activeSSLConnections.Insert(systemAddress);
}
bool TCPInterface::IsSSLActive(SystemAddress systemAddress)
{
	return activeSSLConnections.GetIndexOf(systemAddress)!=-1;
}
#endif
void TCPInterface::Send( const char *data, unsigned length, SystemAddress systemAddress, bool broadcast )
{
	if (isStarted==false)
		return;
	if (remoteClients.Size()==0)
		return;
	if (data==0)
		return;
	if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS && broadcast==false)
		return;
	OutgoingMessage *p;
	if (broadcast==false)
	{
		p=outgoingMessages.WriteLock();
		p->length=length;
		p->data = (unsigned char*) rakMalloc_Ex( p->length, __FILE__, __LINE__ );
		memcpy(p->data, data, p->length);
		p->systemAddress=systemAddress;
		p->broadcast=broadcast;
		outgoingMessages.WriteUnlock();
	}
}
bool TCPInterface::SendList( char **data, const int *lengths, const int numParameters, SystemAddress systemAddress, bool broadcast )
{
	if (isStarted==false)
		return false;
	if (remoteClients.Size()==0)
		return false;
	if (data==0)
		return false;
	if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS && broadcast==false)
		return false;
	unsigned int totalLength=0;
	unsigned int lengthOffset;
	int i;
	for (i=0; i < numParameters; i++)
	{
		if (lengths[i]>0)
			totalLength+=lengths[i];
	}
	if (totalLength==0)
		return false;
	OutgoingMessage *p;
	p=outgoingMessages.WriteLock();
	p->length=totalLength;
	p->data = (unsigned char*) rakMalloc_Ex( totalLength, __FILE__, __LINE__ );
	for (i=0, lengthOffset=0; i < numParameters; i++)
	{
		if (lengths[i]>0)
		{
			memcpy(p->data+lengthOffset, data[i], lengths[i]);
			lengthOffset+=lengths[i];
		}
	}
	p->systemAddress=systemAddress;
	p->broadcast=broadcast;
	outgoingMessages.WriteUnlock();

	return true;
}
Packet* TCPInterface::Receive( void )
{
	if (isStarted==false)
		return 0;
	if (headPush.IsEmpty()==false)
		return headPush.Pop();
	Packet *p = incomingMessages.ReadLock();
	if (p)
		return p;
	if (tailPush.IsEmpty()==false)
		return tailPush.Pop();
	return 0;
}
void TCPInterface::CloseConnection( SystemAddress systemAddress )
{
	if (isStarted==false)
		return;
	if (systemAddress==UNASSIGNED_SYSTEM_ADDRESS)
		return;
	SystemAddress *id = requestedCloseConnections.WriteLock();
	*id=systemAddress;
	requestedCloseConnections.WriteUnlock();

#if defined(OPEN_SSL_CLIENT_SUPPORT)
	unsigned index = activeSSLConnections.GetIndexOf(systemAddress);
	if (index!=(unsigned)-1)
		activeSSLConnections.RemoveAtIndex(index);
#endif
}
void TCPInterface::DeallocatePacket( Packet *packet )
{
	if (packet==0)
		return;
	if (packet->deleteData)
	{
		RakAssert(incomingMessages.CheckReadUnlockOrder(packet));
		rakFree_Ex(packet->data, __FILE__, __LINE__ );
		incomingMessages.ReadUnlock();
	}
	else
	{
		// Came from userspace AllocatePacket
		rakFree_Ex(packet->data, __FILE__, __LINE__ );
		RakNet::OP_DELETE(packet, __FILE__, __LINE__);
	}
}
Packet* TCPInterface::AllocatePacket(unsigned dataSize)
{
	Packet*p = RakNet::OP_NEW<Packet>(__FILE__,__LINE__);
	p->data=(unsigned char*) rakMalloc_Ex(dataSize,__FILE__,__LINE__);
	p->length=dataSize;
	p->bitSize=BYTES_TO_BITS(dataSize);
	p->deleteData=false;
	p->guid=UNASSIGNED_RAKNET_GUID;
	p->systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	p->systemIndex=(SystemIndex)-1;
	return p;
}
void TCPInterface::PushBackPacket( Packet *packet, bool pushAtHead )
{
	if (pushAtHead)
		headPush.Push(packet);
	else
		tailPush.Push(packet);
}
SystemAddress TCPInterface::HasCompletedConnectionAttempt(void)
{
	SystemAddress sysAddr=UNASSIGNED_SYSTEM_ADDRESS;
	completedConnectionAttemptMutex.Lock();
	if (completedConnectionAttempts.IsEmpty()==false)
		sysAddr=completedConnectionAttempts.Pop();
	completedConnectionAttemptMutex.Unlock();
	return sysAddr;
}
SystemAddress TCPInterface::HasFailedConnectionAttempt(void)
{
	SystemAddress sysAddr=UNASSIGNED_SYSTEM_ADDRESS;
	failedConnectionAttemptMutex.Lock();
	if (failedConnectionAttempts.IsEmpty()==false)
		sysAddr=failedConnectionAttempts.Pop();
	failedConnectionAttemptMutex.Unlock();
	return sysAddr;
}
SystemAddress TCPInterface::HasNewIncomingConnection(void)
{
	SystemAddress *out;
	out = newIncomingConnections.ReadLock();
	if (out)
	{
		newIncomingConnections.ReadUnlock();
		return *out;
	}
	else
	{
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
}
SystemAddress TCPInterface::HasLostConnection(void)
{
	SystemAddress *out;
	out = lostConnections.ReadLock();
	if (out)
	{
		lostConnections.ReadUnlock();
		return *out;
	}
	else
	{
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
}
void TCPInterface::DeleteRemoteClient(RemoteClient *remoteClient, fd_set *exceptionFD)
{
	(void) exceptionFD;
#if defined(OPEN_SSL_CLIENT_SUPPORT)
	remoteClient->DisconnectSSL();
	remoteClient->FreeSSL();
#endif
//	FD_CLR(remoteClient->socket, exceptionFD);
	closesocket(remoteClient->socket);
	//shutdown(remoteClient->socket, SD_SEND);
	RakNet::OP_DELETE(remoteClient, __FILE__, __LINE__);
}

void TCPInterface::InsertRemoteClient(RemoteClient* remoteClient)
{
	remoteClientsInsertionQueueMutex.Lock();
	remoteClientsInsertionQueue.Push(remoteClient);
	remoteClientsInsertionQueueMutex.Unlock();
}
RemoteClient* TCPInterface::AllocRemoteClient(const char *file, unsigned int line) const
{
	return RakNet::OP_NEW<RemoteClient>(file, line);
}
SOCKET TCPInterface::SocketConnect(const char* host, unsigned short remotePort)
{
	sockaddr_in serverAddress;

#if !defined(_XBOX) && !defined(_X360)
	struct hostent * server;
	server = gethostbyname(host);
	if (server == NULL)
		return (SOCKET) -1;
#endif

	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		return (SOCKET) -1;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons( remotePort );


#if !defined(_XBOX) && !defined(_X360)
	memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
#else
	serverAddress.sin_addr.s_addr = inet_addr( host );
#endif

	blockingSocketListMutex.Lock();
	blockingSocketList.Insert(sockfd, __FILE__, __LINE__);
	blockingSocketListMutex.Unlock();

	// This is blocking
	int connectResult = connect( sockfd, ( struct sockaddr * ) &serverAddress, sizeof( struct sockaddr ) );

	unsigned sockfdIndex;
	blockingSocketListMutex.Lock();
	sockfdIndex=blockingSocketList.GetIndexOf(sockfd);
	if (sockfdIndex!=(unsigned)-1)
		blockingSocketList.RemoveAtIndexFast(sockfdIndex);
	blockingSocketListMutex.Unlock();

	if (connectResult==-1)
	{
		closesocket(sockfd);
		return (SOCKET) -1;
	}

	return sockfd;
}
RAK_THREAD_DECLARATION(ConnectionAttemptLoop)
{
	TCPInterface::ThisPtrPlusSysAddr *s = (TCPInterface::ThisPtrPlusSysAddr *) arguments;
	SystemAddress systemAddress = s->systemAddress;
	TCPInterface *tcpInterface = s->tcpInterface;
	RakNet::OP_DELETE(s, __FILE__, __LINE__);

	char str1[64];
	systemAddress.ToString(false, str1);
	SOCKET sockfd = tcpInterface->SocketConnect(str1, systemAddress.port);
	if (sockfd==(SOCKET)-1)
	{
		tcpInterface->failedConnectionAttemptMutex.Lock();
		tcpInterface->failedConnectionAttempts.Push(systemAddress);
		tcpInterface->failedConnectionAttemptMutex.Unlock();
		return 0;
	}

	RemoteClient *remoteClient;
	remoteClient = tcpInterface->AllocRemoteClient( __FILE__, __LINE__ );
	remoteClient->socket=sockfd;
	remoteClient->systemAddress=systemAddress;
	tcpInterface->InsertRemoteClient(remoteClient);

	// Wait for the other thread to pick up the remote client
	bool isEmpty;
	do 
	{
		RakSleep(30);
		tcpInterface->remoteClientsInsertionQueueMutex.Lock();
		isEmpty=tcpInterface->remoteClientsInsertionQueue.IsEmpty();
		tcpInterface->remoteClientsInsertionQueueMutex.Unlock();
	} while(isEmpty==false && tcpInterface->threadRunning);	

	// Notify user that the connection attempt has completed.
	if (tcpInterface->threadRunning)
	{
		tcpInterface->completedConnectionAttemptMutex.Lock();
		tcpInterface->completedConnectionAttempts.Push(systemAddress);
		tcpInterface->completedConnectionAttemptMutex.Unlock();
	}	

	return 0;
}

RAK_THREAD_DECLARATION(UpdateTCPInterfaceLoop)
{
	TCPInterface * sts = ( TCPInterface * ) arguments;
	RemoteClient *remoteClient;
//	const int BUFF_SIZE=8096;
	const int BUFF_SIZE=1048576;
	//char data[ BUFF_SIZE ];
	char * data = (char*) rakMalloc_Ex(BUFF_SIZE,__FILE__,__LINE__);
	TCPInterface::OutgoingMessage *outgoingMessage;
	Packet *incomingMessage;
	SystemAddress *systemAddress;
	fd_set      readFD, exceptionFD;
	sts->threadRunning=true;

	sockaddr_in sockAddr;
	int sockAddrSize = sizeof(sockAddr);

	unsigned i;
	int len;
	SOCKET newSock;
	timeval tv;
	int selectResult;
	tv.tv_sec=0;
	tv.tv_usec=25000;

	while (sts->isStarted)
	{
#if defined(OPEN_SSL_CLIENT_SUPPORT)
		systemAddress = sts->startSSL.ReadLock();
		if (systemAddress)
		{
			for (i=0; i < sts->remoteClients.Size(); i++)
			{
				if (sts->remoteClients[i]->systemAddress==*systemAddress)
				{
					if (sts->remoteClients[i]->ssl==0)
						sts->remoteClients[i]->InitSSL(sts->ctx,sts->meth);
				}
			}
			sts->startSSL.ReadUnlock();
		}
#endif

		outgoingMessage=sts->outgoingMessages.ReadLock();
		while (outgoingMessage)
		{
			if (outgoingMessage->broadcast)
			{
				// Send to all, possible exception system
				for (i=0; i < sts->remoteClients.Size(); i++)
				{
					if (sts->remoteClients[i]->systemAddress!=outgoingMessage->systemAddress)
						sts->remoteClients[i]->Send((const char*)outgoingMessage->data, outgoingMessage->length);
				}
			}
			else
			{
				// Send to this player
				for (i=0; i < sts->remoteClients.Size(); i++)
					if (sts->remoteClients[i]->systemAddress==outgoingMessage->systemAddress )
						sts->remoteClients[i]->Send((const char*)outgoingMessage->data, outgoingMessage->length);
			}

			rakFree_Ex(outgoingMessage->data, __FILE__, __LINE__ );
			sts->outgoingMessages.ReadUnlock();
			outgoingMessage=sts->outgoingMessages.ReadLock();
		}

		if (sts->remoteClientsInsertionQueue.IsEmpty()==false)
		{
			sts->remoteClientsInsertionQueueMutex.Lock();
			if (sts->remoteClientsInsertionQueue.IsEmpty()==false)
				sts->remoteClients.Insert(sts->remoteClientsInsertionQueue.Pop(), __FILE__, __LINE__);
			sts->remoteClientsInsertionQueueMutex.Unlock();
		}

		systemAddress=sts->requestedCloseConnections.ReadLock();
		if (systemAddress)
		{
			for (i=0; i < sts->remoteClients.Size(); i++)
			{
				if (sts->remoteClients[i]->systemAddress==*systemAddress )
				{
					systemAddress=sts->lostConnections.WriteLock();
					*systemAddress=sts->remoteClients[i]->systemAddress;
					sts->lostConnections.WriteUnlock();

					sts->DeleteRemoteClient(sts->remoteClients[i], &exceptionFD);
					sts->remoteClients.RemoveAtIndex(i);


					/*
					systemAddress=sts->lostConnections.WriteLock();
					*systemAddress=sts->remoteClients[i]->systemAddress;
					sts->lostConnections.WriteUnlock();
					sts->remoteClients.Del(i);
					*/
					break;
				}
			}

			sts->requestedCloseConnections.ReadUnlock();
		}

		SOCKET largestDescriptor=0; // see select()'s first parameter's documentation under linux

		// Reset readFD and exceptionFD since select seems to clear it
		FD_ZERO(&readFD);
		FD_ZERO(&exceptionFD);
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
		if (sts->listenSocket!=(SOCKET) -1)
		{
			FD_SET(sts->listenSocket, &readFD);
			FD_SET(sts->listenSocket, &exceptionFD);
			largestDescriptor = sts->listenSocket; // @see largestDescriptor def
		}
		
		for (i=0; i < sts->remoteClients.Size(); i++)
		{
			FD_SET(sts->remoteClients[i]->socket, &readFD);
			FD_SET(sts->remoteClients[i]->socket, &exceptionFD);
			if(sts->remoteClients[i]->socket > largestDescriptor) // @see largestDescriptorDef
				largestDescriptor = sts->remoteClients[i]->socket;
		}


		// Linux' select() implementation changes the timeout
		tv.tv_sec=0;
		tv.tv_usec=25000;

#ifdef _MSC_VER
#pragma warning( disable : 4244 ) // warning C4127: conditional expression is constant
#endif
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
		selectResult=socketselect(largestDescriptor+1, &readFD, 0, &exceptionFD, &tv);
#else
		selectResult=(int) select(largestDescriptor+1, &readFD, 0, &exceptionFD, &tv);		
#endif
		//selectResult=select(largestDescriptor+1, &readFD, 0, &exceptionFD, &tv);

		//selectResult=select(0, &readFD, 0, &exceptionFD, &tv);

		if (selectResult > 0)
		{
			if (sts->listenSocket!=(SOCKET) -1 && FD_ISSET(sts->listenSocket, &readFD))
			{
				newSock = accept(sts->listenSocket, (sockaddr*)&sockAddr, (socklen_t*)&sockAddrSize);

				if (newSock != (SOCKET) -1)
				{
					remoteClient = sts->AllocRemoteClient( __FILE__, __LINE__ );
					remoteClient->socket=newSock;
					remoteClient->systemAddress.binaryAddress=sockAddr.sin_addr.s_addr;
					remoteClient->systemAddress.port=ntohs( sockAddr.sin_port);
					sts->remoteClients.Insert(remoteClient, __FILE__, __LINE__);
					systemAddress=sts->newIncomingConnections.WriteLock();
					*systemAddress=remoteClient->systemAddress;
					sts->newIncomingConnections.WriteUnlock();

					FD_SET(newSock, &readFD);
					FD_SET(newSock, &exceptionFD);
				}
				else
				{
#ifdef _DO_PRINTF
					RAKNET_DEBUG_PRINTF("Error: connection failed\n");
#endif
				}
			}
			else if (sts->listenSocket!=(SOCKET) -1 && FD_ISSET(sts->listenSocket, &exceptionFD))
			{
#ifdef _DO_PRINTF
				int err;
				int errlen = sizeof(err);
				getsockopt(sts->listenSocket, SOL_SOCKET, SO_ERROR,(char*)&err, &errlen);
				RAKNET_DEBUG_PRINTF("Socket error %s on listening socket\n", err);
#endif
			}
			else
			{
				i=0;
				while (i < sts->remoteClients.Size())
				{
					if (FD_ISSET(sts->remoteClients[i]->socket, &exceptionFD))
					{
#ifdef _DO_PRINTF
						if (sts->listenSocket!=-1)
						{
							int err;
							int errlen = sizeof(err);
							getsockopt(sts->listenSocket, SOL_SOCKET, SO_ERROR,(char*)&err, &errlen);
							in_addr in;
							in.s_addr = sts->remoteClients[i]->systemAddress.binaryAddress;
							RAKNET_DEBUG_PRINTF("Socket error %i on %s:%i\n", err,inet_ntoa( in ), sts->remoteClients[i]->systemAddress.port );
						}
						
#endif
						// Connection lost abruptly
						systemAddress=sts->lostConnections.WriteLock();
						*systemAddress=sts->remoteClients[i]->systemAddress;
						sts->lostConnections.WriteUnlock();
						sts->DeleteRemoteClient(sts->remoteClients[i], &exceptionFD);
						sts->remoteClients.RemoveAtIndex(i);
					}
					else
					{
						if (FD_ISSET(sts->remoteClients[i]->socket, &readFD))
						{
							// if recv returns 0 this was a graceful close
							len = sts->remoteClients[i]->Recv(data,BUFF_SIZE);
							if (len>0)
							{
								incomingMessage=sts->incomingMessages.WriteLock();
								incomingMessage->data = (unsigned char*) rakMalloc_Ex( len+1, __FILE__, __LINE__ );
								memcpy(incomingMessage->data, data, len);
								incomingMessage->data[len]=0; // Null terminate this so we can print it out as regular strings.  This is different from RakNet which does not do this.
								incomingMessage->length=len;
								incomingMessage->deleteData=true; // actually means came from SPSC, rather than AllocatePacket
								incomingMessage->systemAddress=sts->remoteClients[i]->systemAddress;
								sts->incomingMessages.WriteUnlock();
								i++; // Nothing deleted so increment the index
							}
							else
							{
								// Connection lost gracefully
								systemAddress=sts->lostConnections.WriteLock();
								*systemAddress=sts->remoteClients[i]->systemAddress;
								sts->lostConnections.WriteUnlock();
								sts->DeleteRemoteClient(sts->remoteClients[i], &exceptionFD);
								sts->remoteClients.RemoveAtIndex(i);
							}
						}
						else
							i++; // Nothing deleted so increment the index
					}
				}
			}
		}
		else if (selectResult==0)
		{
			// No input - sleep for a while
			RakSleep(50);
		}
		else
		{
			// FD_CLOSE? closesocket(remoteClient->socket);

#if defined(_DO_PRINTF) && defined(_WIN32)
			DWORD dwIOError = WSAGetLastError();
			RAKNET_DEBUG_PRINTF("Socket error %i\n", dwIOError);
#endif
		}



	}
	sts->threadRunning=false;

	rakFree_Ex(data,__FILE__,__LINE__);

	return 0;
}

#if defined(OPEN_SSL_CLIENT_SUPPORT)
void RemoteClient::InitSSL(SSL_CTX* ctx, SSL_METHOD *meth)
{
	(void) meth;

	ssl = SSL_new (ctx);                         
	RakAssert(ssl);    
	SSL_set_fd (ssl, socket);
	SSL_connect (ssl);
}
void RemoteClient::DisconnectSSL(void)
{
	if (ssl)
		SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
}
void RemoteClient::FreeSSL(void)
{
	if (ssl)
		SSL_free (ssl);
}
void RemoteClient::Send(const char *data, unsigned int length)
{
	int err;
	if (ssl)
	{
		err = SSL_write (ssl, data, length);
		RakAssert(err>0);
	}
	else
		send(socket, data, length, 0);
}
int RemoteClient::Recv(char *data, const int dataSize)
{
	if (ssl)
	{
		return SSL_read (ssl, data, dataSize);
	}
	else
		return recv(socket, data, dataSize, 0);
}
#else
void RemoteClient::Send(const char *data, unsigned int length)
{
	send(socket, data, length, 0);
}
int RemoteClient::Recv(char *data, const int dataSize)
{
	return recv(socket, data, dataSize, 0);
}
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
