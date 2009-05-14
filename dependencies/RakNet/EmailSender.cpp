// Useful sites
// http://www.faqs.org\rfcs\rfc2821.html
// http://en.wikipedia.org/wiki/Base64
// http://www2.rad.com\networks/1995/mime/examples.htm

#include "EmailSender.h"
#include "TCPInterface.h"
#include "GetTime.h"
#include "Rand.h"
#include "FileList.h"
#include "BitStream.h"
#include <stdio.h>

#if defined(_XBOX) || defined(X360)
#include "XBOX360Includes.h"
#endif

#include "RakSleep.h"

static const char base64Map[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char *EmailSender::Send(const char *hostAddress, unsigned short hostPort, const char *sender, const char *recipient, const char *senderName, const char *recipientName, const char *subject, const char *body, FileList *attachedFiles, bool doPrintf, const char *password)
{
	Packet *packet;
	char query[1024];
	TCPInterface tcpInterface;
	SystemAddress emailServer;
	if (tcpInterface.Start(0, 0)==false)
		return "Unknown error starting TCP";
	emailServer=tcpInterface.Connect(hostAddress, hostPort,true);
	if (emailServer==UNASSIGNED_SYSTEM_ADDRESS)
		return "Failed to connect to host";
	RakNetTime timeoutTime = RakNet::GetTime()+3000;
	packet=0;
	while (RakNet::GetTime() < timeoutTime)
	{
		packet = tcpInterface.Receive();
		if (packet)
		{
			if (doPrintf)
				RAKNET_DEBUG_PRINTF("%s", packet->data);
			break;
		}
		RakSleep(250);
	}

	if (packet==0)
		return "Timeout while waiting for initial data from server.";
	
	tcpInterface.Send("EHLO\r\n", 6, emailServer,false);
	const char *response;
	bool authenticate=false;
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
	while (1)
	{
		response=GetResponse(&tcpInterface, emailServer, doPrintf);

		if (response!=0 && strcmp(response, "AUTHENTICATE")==0)
		{
			authenticate=true;
			break;
		}

		// Something other than continue?
		if (response!=0 && strcmp(response, "CONTINUE")!=0)
			return response;

		// Success?
		if (response==0)
			break;
	}

	if (authenticate)
	{
		sprintf(query, "EHLO %s\r\n", sender);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
		response=GetResponse(&tcpInterface, emailServer, doPrintf);
		if (response!=0)
			return response;

		if (password==0)
			return "Password needed";
		char *outputData = RakNet::OP_NEW_ARRAY<char >((const int) (strlen(sender)+strlen(password)+2)*3, __FILE__, __LINE__ );
		RakNet::BitStream bs;
		char zero=0;
		bs.Write(&zero,1);
		bs.Write(sender,(const unsigned int)strlen(sender));
		//bs.Write("jms1@jms1.net",(const unsigned int)strlen("jms1@jms1.net"));
		bs.Write(&zero,1);
		bs.Write(password,(const unsigned int)strlen(password));
		bs.Write(&zero,1);
		//bs.Write("not.my.real.password",(const unsigned int)strlen("not.my.real.password"));
		Base64Encoding((const char*)bs.GetData(), bs.GetNumberOfBytesUsed(), outputData, base64Map);
		sprintf(query, "AUTH PLAIN %s", outputData);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
		response=GetResponse(&tcpInterface, emailServer, doPrintf);
		if (response!=0)
			return response;
	}


	if (sender)
		sprintf(query, "MAIL From: <%s>\r\n", sender);
	else
		sprintf(query, "MAIL From: <>\r\n");
	tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	response=GetResponse(&tcpInterface, emailServer, doPrintf);
	if (response!=0)
		return response;

	if (recipient)
		sprintf(query, "RCPT TO: <%s>\r\n", recipient);
	else
		sprintf(query, "RCPT TO: <>\r\n");
	tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	response=GetResponse(&tcpInterface, emailServer, doPrintf);
	if (response!=0)
		return response;

	tcpInterface.Send("DATA\r\n", (unsigned int)strlen("DATA\r\n"), emailServer,false);

	// Wait for 354...

	response=GetResponse(&tcpInterface, emailServer, doPrintf);
	if (response!=0)
		return response;

	if (subject)
	{
		sprintf(query, "Subject: %s\r\n", subject);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	}
	if (senderName)
	{
		sprintf(query, "From: %s\r\n", senderName);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	}
	if (recipientName)
	{
		sprintf(query, "To: %s\r\n", recipientName);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	}

	const int boundarySize=60;
	char boundary[boundarySize+1];
	int i,j;
	if (attachedFiles && attachedFiles->fileList.Size())
	{
		seedMT((unsigned int) RakNet::GetTime());
		// Random multipart message boundary
		for (i=0; i < boundarySize; i++)
			boundary[i]=base64Map[randomMT()%64];
		boundary[boundarySize]=0;
	}

	sprintf(query, "MIME-version: 1.0\r\n");
	tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);

	if (attachedFiles && attachedFiles->fileList.Size())
	{
		sprintf(query, "Content-type: multipart/mixed; BOUNDARY=\"%s\"\r\n\r\n", boundary);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);

		sprintf(query, "This is a multi-part message in MIME format.\r\n\r\n--%s\r\n", boundary);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	}
	
	sprintf(query, "Content-Type: text/plain; charset=\"US-ASCII\"\r\n\r\n");
	tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);

	// Write the body of the email, doing some lame shitty shit where I have to make periods at the start of a newline have a second period.
	char *newBody;
	int bodyLength;
	bodyLength=(int)strlen(body);
	newBody = (char*) rakMalloc_Ex( bodyLength*3, __FILE__, __LINE__ );
	if (bodyLength>0)
		newBody[0]=body[0];
	for (i=1, j=1; i < bodyLength; i++)
	{
		// Transform \n . \r \n into \n . . \r \n
		if (i < bodyLength-2 &&
			body[i-1]=='\n' &&
			body[i+0]=='.' &&
			body[i+1]=='\r' &&
			body[i+2]=='\n')
		{
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='\r';
			newBody[j++]='\n';
			i+=2;
		}
		// Transform \n . . \r \n into \n . . . \r \n
		// Having to process .. is a bug in the mail server - the spec says ONLY \r\n.\r\n should be transformed
		else if (i <= bodyLength-3 &&
			body[i-1]=='\n' &&
			body[i+0]=='.' &&
			body[i+1]=='.' &&
			body[i+2]=='\r' &&
			body[i+3]=='\n')
		{
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='\r';
			newBody[j++]='\n';
			i+=3;
		}
		// Transform \n . \n into \n . . \r \n (this is a bug in the mail server - the spec says do not count \n alone but it does)
		else if (i < bodyLength-1 &&
			body[i-1]=='\n' &&
			body[i+0]=='.' &&
			body[i+1]=='\n')
		{
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='\r';
			newBody[j++]='\n';
			i+=1;
		}
		// Transform \n . . \n into \n . . . \r \n (this is a bug in the mail server - the spec says do not count \n alone but it does)
		// In fact having to process .. is a bug too - because the spec says ONLY \r\n.\r\n should be transformed
		else if (i <= bodyLength-2 &&
			body[i-1]=='\n' &&
			body[i+0]=='.' &&
			body[i+1]=='.' &&
			body[i+2]=='\n')
		{
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='.';
			newBody[j++]='\r';
			newBody[j++]='\n';
			i+=2;
		}
		else
			newBody[j++]=body[i];
	}
	
	newBody[j++]='\r';
	newBody[j++]='\n';
	tcpInterface.Send(newBody, j, emailServer,false);

	rakFree_Ex(newBody, __FILE__, __LINE__ );
	int outputOffset;

	// What a pain in the rear.  I have to map the binary to printable characters using 6 bits per character.
	if (attachedFiles && attachedFiles->fileList.Size())
	{
		for (i=0; i < (int) attachedFiles->fileList.Size(); i++)
		{
			// Write boundary
			sprintf(query, "\r\n--%s\r\n", boundary);
			tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);

			sprintf(query, "Content-Type: APPLICATION/Octet-Stream; SizeOnDisk=%i; name=\"%s\"\r\nContent-Transfer-Encoding: BASE64\r\nContent-Description: %s\r\n\r\n", attachedFiles->fileList[i].dataLengthBytes, attachedFiles->fileList[i].filename, attachedFiles->fileList[i].filename);
			tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);

			newBody = (char*) rakMalloc_Ex( (size_t) (attachedFiles->fileList[i].dataLengthBytes*3)/2, __FILE__, __LINE__ );

			outputOffset=Base64Encoding(attachedFiles->fileList[i].data, (int) attachedFiles->fileList[i].dataLengthBytes, newBody, base64Map);

			// Send the base64 mapped file.
			tcpInterface.Send(newBody, outputOffset, emailServer,false);
			rakFree_Ex(newBody, __FILE__, __LINE__ );

		}

		// Write last boundary
		sprintf(query, "\r\n--%s--\r\n", boundary);
		tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	}


	sprintf(query, "\r\n.\r\n");
	tcpInterface.Send(query, (unsigned int)strlen(query), emailServer,false);
	response=GetResponse(&tcpInterface, emailServer, doPrintf);
	if (response!=0)
		return response;

	tcpInterface.Send("QUIT\r\n", (unsigned int)strlen("QUIT\r\n"), emailServer,false);

	RakSleep(30);
	if (doPrintf)
	{
		packet = tcpInterface.Receive();
		while (packet)
		{
			RAKNET_DEBUG_PRINTF("%s", packet->data);
			packet = tcpInterface.Receive();
		}
	}
	tcpInterface.Stop();
	return 0; // Success
}

const char *EmailSender::GetResponse(TCPInterface *tcpInterface, const SystemAddress &emailServer, bool doPrintf)
{
	Packet *packet;
	RakNetTime timeout;
	timeout=RakNet::GetTime()+5000;
#ifdef _MSC_VER
	#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
	while (1)
	{
		if (tcpInterface->HasLostConnection()==emailServer)
			return "Connection to server lost.";
		packet = tcpInterface->Receive();
		if (packet)
		{
			if (doPrintf)
			{
				RAKNET_DEBUG_PRINTF("%s", packet->data);
			}
#if defined(OPEN_SSL_CLIENT_SUPPORT)
			if (strstr((const char*)packet->data, "220"))
			{
				tcpInterface->StartSSLClient(packet->systemAddress);
				return "AUTHENTICATE"; // OK
			}
#endif
			if (strstr((const char*)packet->data, "235"))
				return 0; // Authentication accepted
			if (strstr((const char*)packet->data, "354"))
				return 0; // Go ahead
#if defined(OPEN_SSL_CLIENT_SUPPORT)
			if (strstr((const char*)packet->data, "250-STARTTLS"))
			{
				tcpInterface->Send("STARTTLS\r\n", (unsigned int) strlen("STARTTLS\r\n"), packet->systemAddress);
				return "CONTINUE";
			}
#endif
			if (strstr((const char*)packet->data, "250"))
				return 0; // OK
			if (strstr((const char*)packet->data, "550"))
				return "Failed on error code 550";
			if (strstr((const char*)packet->data, "553"))
				return "Failed on error code 553";
		}
		if (RakNet::GetTime() > timeout)
			return "Timed out";
		RakSleep(100);
	}
}

int EmailSender::Base64Encoding(const char *inputData, int dataLength, char *outputData, const char *base64Map)
{
	int outputOffset, charCount;
	int write3Count;
	outputOffset=0;
	charCount=0;
	int j;

	write3Count=dataLength/3;
	for (j=0; j < write3Count; j++)
	{
		// 6 leftmost bits from first byte, shifted to bits 7,8 are 0
		outputData[outputOffset++]=base64Map[inputData[j*3+0] >> 2];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Remaining 2 bits from first byte, placed in position, and 4 high bits from the second byte, masked to ignore bits 7,8
		outputData[outputOffset++]=base64Map[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// 4 low bits from the second byte and the two high bits from the third byte, masked to ignore bits 7,8
		outputData[outputOffset++]=base64Map[((inputData[j*3+1] << 2) | (inputData[j*3+2] >> 6)) & 63]; // Third 6 bits
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Last 6 bits from the third byte, masked to ignore bits 7,8
		outputData[outputOffset++]=base64Map[inputData[j*3+2] & 63];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}
	}

	if (dataLength % 3==1)
	{
		// One input byte remaining
		outputData[outputOffset++]=base64Map[inputData[j*3+0] >> 2];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Remaining 2 bits from first byte, placed in position, and 4 high bits from the second byte, masked to ignore bits 7,8
		outputData[outputOffset++]=base64Map[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Pad with two equals
		outputData[outputOffset++]='=';
		outputData[outputOffset++]='=';
	}
	else if (dataLength % 3==2)
	{
		// Two input bytes remaining

		// 6 leftmost bits from first byte, shifted to bits 7,8 are 0
		outputData[outputOffset++]=base64Map[inputData[j*3+0] >> 2];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Remaining 2 bits from first byte, placed in position, and 4 high bits from the second byte, masked to ignore bits 7,8
		outputData[outputOffset++]=base64Map[((inputData[j*3+0] << 4) | (inputData[j*3+1] >> 4)) & 63];
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// 4 low bits from the second byte, followed by 00
		outputData[outputOffset++]=base64Map[(inputData[j*3+1] << 2) & 63]; // Third 6 bits
		if ((++charCount % 76)==0) {outputData[outputOffset++]='\r'; outputData[outputOffset++]='\n'; charCount=0;}

		// Pad with one equal
		outputData[outputOffset++]='=';
		//outputData[outputOffset++]='=';
	}

	// Append \r\n
	outputData[outputOffset++]='\r';
	outputData[outputOffset++]='\n';
	outputData[outputOffset]=0;

	return outputOffset;
}
