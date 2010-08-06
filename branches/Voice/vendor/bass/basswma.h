/*
	BASSWMA 2.4 C/C++ header file
	Copyright (c) 2002-2009 Un4seen Developments Ltd.

	See the BASSWMA.CHM file for more detailed documentation
*/

#ifndef BASSWMA_H
#define BASSWMA_H

#include "bass.h"

#if BASSVERSION!=0x204
#error conflicting BASS and BASSWMA versions
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSWMADEF
#define BASSWMADEF(f) WINAPI f
#endif

typedef DWORD HWMENCODE;	// WMA encoding handle

// Additional error codes returned by BASS_ErrorGetCode
#define BASS_ERROR_WMA_LICENSE		1000	// the file is protected
#define BASS_ERROR_WMA				1001	// Windows Media (9 or above) is not installed
#define BASS_ERROR_WMA_WM9			BASS_ERROR_WMA
#define BASS_ERROR_WMA_DENIED		1002	// access denied (user/pass is invalid)
#define BASS_ERROR_WMA_INDIVIDUAL	1004	// individualization is needed
#define BASS_ERROR_WMA_PUBINIT		1005	// publishing point initialization problem

// Additional BASS_SetConfig options
#define BASS_CONFIG_WMA_PRECHECK	0x10100
#define BASS_CONFIG_WMA_PREBUF		0x10101
#define BASS_CONFIG_WMA_BASSFILE	0x10103
#define BASS_CONFIG_WMA_NETSEEK		0x10104
#define BASS_CONFIG_WMA_VIDEO		0x10105

// additional WMA sync types
#define BASS_SYNC_WMA_CHANGE		0x10100
#define BASS_SYNC_WMA_META			0x10101

// additional BASS_StreamGetFilePosition WMA mode
#define BASS_FILEPOS_WMA_BUFFER		1000	// internet buffering progress (0-100%)

// Additional flags for use with BASS_WMA_EncodeOpen/File/Network/Publish
#define BASS_WMA_ENCODE_STANDARD	0x2000	// standard WMA
#define BASS_WMA_ENCODE_PRO			0x4000	// WMA Pro
#define BASS_WMA_ENCODE_24BIT		0x8000	// 24-bit
#define BASS_WMA_ENCODE_SCRIPT		0x20000	// set script (mid-stream tags) in the WMA encoding

// Additional flag for use with BASS_WMA_EncodeGetRates
#define BASS_WMA_ENCODE_RATES_VBR	0x10000	// get available VBR quality settings

typedef void (CALLBACK CLIENTCONNECTPROC)(HWMENCODE handle, BOOL connect, const char *ip, void *user);
/* Client connection notification callback function.
handle : The encoder
connect: TRUE=client is connecting, FALSE=disconnecting
ip     : The client's IP (xxx.xxx.xxx.xxx:port)
user   : The 'user' parameter value given when calling BASS_WMA_EncodeSetNotify */

typedef void (CALLBACK WMENCODEPROC)(HWMENCODE handle, DWORD type, const void *buffer, DWORD length, void *user);
/* Encoder callback function.
handle : The encoder handle
type   : The type of data, one of BASS_WMA_ENCODE_xxx values
buffer : The encoded data
length : Length of the data
user   : The 'user' parameter value given when calling BASS_WMA_EncodeOpen */

// WMENCODEPROC "type" values
#define BASS_WMA_ENCODE_HEAD	0
#define BASS_WMA_ENCODE_DATA	1
#define BASS_WMA_ENCODE_DONE	2

// BASS_WMA_EncodeSetTag "form" values
#define BASS_WMA_TAG_ANSI		0
#define BASS_WMA_TAG_UNICODE	1
#define BASS_WMA_TAG_UTF8		2
#define BASS_WMA_TAG_BINARY		0x100 // FLAG: binary tag (HIWORD=length)

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_WMA		0x10300
#define BASS_CTYPE_STREAM_WMA_MP3	0x10301

// Additional BASS_ChannelGetTags types
#define BASS_TAG_WMA		8	// WMA header tags : series of null-terminated UTF-8 strings
#define BASS_TAG_WMA_META	11	// WMA mid-stream tag : UTF-8 string

HSTREAM BASSWMADEF(BASS_WMA_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSWMADEF(BASS_WMA_StreamCreateFileAuth)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags, const char *user, const char *pass);
HSTREAM BASSWMADEF(BASS_WMA_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *procs, void *user);

const char *BASSWMADEF(BASS_WMA_GetTags)(const void *file, DWORD flags);

const DWORD *BASSWMADEF(BASS_WMA_EncodeGetRates)(DWORD freq, DWORD chans, DWORD flags);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpen)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, WMENCODEPROC *proc, void *user);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpenFile)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, const char *file);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpenNetwork)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, DWORD port, DWORD clients);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpenNetworkMulti)(DWORD freq, DWORD chans, DWORD flags, const DWORD *bitrates, DWORD port, DWORD clients);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpenPublish)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, const char *url, const char *user, const char *pass);
HWMENCODE BASSWMADEF(BASS_WMA_EncodeOpenPublishMulti)(DWORD freq, DWORD chans, DWORD flags, const DWORD *bitrates, const char *url, const char *user, const char *pass);
DWORD BASSWMADEF(BASS_WMA_EncodeGetPort)(HWMENCODE handle);
BOOL BASSWMADEF(BASS_WMA_EncodeSetNotify)(HWMENCODE handle, CLIENTCONNECTPROC *proc, void *user);
DWORD BASSWMADEF(BASS_WMA_EncodeGetClients)(HWMENCODE handle);
BOOL BASSWMADEF(BASS_WMA_EncodeSetTag)(HWMENCODE handle, const char *tag, const char *text, DWORD form);
BOOL BASSWMADEF(BASS_WMA_EncodeWrite)(HWMENCODE handle, const void *buffer, DWORD length);
BOOL BASSWMADEF(BASS_WMA_EncodeClose)(HWMENCODE handle);

void *BASSWMADEF(BASS_WMA_GetWMObject)(DWORD handle);

#ifdef __cplusplus
}
#endif

#endif
