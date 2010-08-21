/*
	BASSmix 2.4 C/C++ header file
	Copyright (c) 2005-2008 Un4seen Developments Ltd.

	See the BASSMIX.CHM file for more detailed documentation
*/

#ifndef BASSMIX_H
#define BASSMIX_H

#include "bass.h"

#if BASSVERSION!=0x204
#error conflicting BASS and BASSmix versions
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSMIXDEF
#define BASSMIXDEF(f) WINAPI f
#endif

// additional BASS_SetConfig option
#define BASS_CONFIG_MIXER_FILTER	0x10600
#define BASS_CONFIG_MIXER_BUFFER	0x10601
#define BASS_CONFIG_SPLIT_BUFFER	0x10610

// BASS_Mixer_StreamCreate flags
#define BASS_MIXER_END			0x10000	// end the stream when there are no sources
#define BASS_MIXER_NONSTOP		0x20000	// don't stall when there are no sources
#define BASS_MIXER_RESUME		0x1000	// resume stalled immediately upon new/unpaused source

// source flags
#define BASS_MIXER_FILTER		0x1000	// resampling filter
#define BASS_MIXER_BUFFER		0x2000	// buffer data for BASS_Mixer_ChannelGetData/Level
#define BASS_MIXER_LIMIT		0x4000	// limit mixer processing to the amount available from this source
#define BASS_MIXER_MATRIX		0x10000	// matrix mixing
#define BASS_MIXER_PAUSE		0x20000	// don't process the source
#define BASS_MIXER_DOWNMIX		0x400000 // downmix to stereo/mono
#define BASS_MIXER_NORAMPIN		0x800000 // don't ramp-in the start

// envelope node
typedef struct {
	QWORD pos;
	float value;
} BASS_MIXER_NODE;

// envelope types
#define BASS_MIXER_ENV_FREQ		1
#define BASS_MIXER_ENV_VOL		2
#define BASS_MIXER_ENV_PAN		3
#define BASS_MIXER_ENV_LOOP		0x10000 // FLAG: loop

// additional sync type
#define BASS_SYNC_MIXER_ENVELOPE	0x10200

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_MIXER	0x10800
#define BASS_CTYPE_STREAM_SPLIT	0x10801

DWORD BASSMIXDEF(BASS_Mixer_GetVersion)();

HSTREAM BASSMIXDEF(BASS_Mixer_StreamCreate)(DWORD freq, DWORD chans, DWORD flags);
BOOL BASSMIXDEF(BASS_Mixer_StreamAddChannel)(HSTREAM handle, DWORD channel, DWORD flags);
BOOL BASSMIXDEF(BASS_Mixer_StreamAddChannelEx)(HSTREAM handle, DWORD channel, DWORD flags, QWORD start, QWORD length);

HSTREAM BASSMIXDEF(BASS_Mixer_ChannelGetMixer)(DWORD handle);
DWORD BASSMIXDEF(BASS_Mixer_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask);
BOOL BASSMIXDEF(BASS_Mixer_ChannelRemove)(DWORD handle);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode);
QWORD BASSMIXDEF(BASS_Mixer_ChannelGetPosition)(DWORD handle, DWORD mode);
DWORD BASSMIXDEF(BASS_Mixer_ChannelGetLevel)(DWORD handle);
DWORD BASSMIXDEF(BASS_Mixer_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
HSYNC BASSMIXDEF(BASS_Mixer_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, void *user);
BOOL BASSMIXDEF(BASS_Mixer_ChannelRemoveSync)(DWORD channel, HSYNC sync);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetMatrix)(DWORD handle, const float *matrix);
BOOL BASSMIXDEF(BASS_Mixer_ChannelGetMatrix)(DWORD handle, float *matrix);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetEnvelope)(DWORD handle, DWORD type, const BASS_MIXER_NODE *nodes, DWORD count);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetEnvelopePos)(DWORD handle, DWORD type, QWORD pos);
QWORD BASSMIXDEF(BASS_Mixer_ChannelGetEnvelopePos)(DWORD handle, DWORD type, float *value);

HSTREAM BASSMIXDEF(BASS_Split_StreamCreate)(DWORD channel, DWORD flags, int *chanmap);
DWORD BASSMIXDEF(BASS_Split_StreamGetSource)(HSTREAM handle);
BOOL BASSMIXDEF(BASS_Split_StreamReset)(DWORD handle);

#ifdef __cplusplus
}
#endif

#endif
