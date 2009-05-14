/// \file
/// \brief Rudimentary class to send email from code.  Don't expect anything fancy.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __EMAIL_SENDER_H
#define __EMAIL_SENDER_H

class FileList;
class TCPInterface;
#include "RakNetTypes.h"
#include "RakMemoryOverride.h"

/// \brief Rudimentary class to send email from code.
class EmailSender
{
public:
	/// Sends an email
	/// \param[in] hostAddress The address of the email server.
	/// \param[in] hostPort The port of the email server (usually 25)
	/// \param[in] sender The email address you are sending from.
	/// \param[in] recipient The email address you are sending to.
	/// \param[in] senderName The email address you claim to be sending from
	/// \param[in] recipientName The email address you claim to be sending to
	/// \param[in] subject Email subject
	/// \param[in] body Email body
	/// \param[in] attachedFiles List of files to attach to the email. (Can be 0 to send none).
	/// \param[in] doPrintf true to output SMTP info to console(for debugging?)
	/// \param[in] password Used if the server uses AUTHENTICATE PLAIN over TLS (such as gmail)
	/// \return 0 on success, otherwise a string indicating the error message
	const char *Send(const char *hostAddress, unsigned short hostPort, const char *sender, const char *recipient, const char *senderName, const char *recipientName, const char *subject, const char *body, FileList *attachedFiles, bool doPrintf, const char *password);

	// Returns how many bytes were written
	int Base64Encoding(const char *inputData, int dataLength, char *outputData, const char *base64Map);
protected:
	const char *GetResponse(TCPInterface *tcpInterface, const SystemAddress &emailServer, bool doPrintf);
};

#endif
