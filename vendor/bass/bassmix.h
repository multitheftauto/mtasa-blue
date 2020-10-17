/*
	BASSmix 2.4 C/C++ header file
	Copyright (c) 2005-2020 Un4seen Developments Ltd.

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

// Additional BASS_SetConfig options
#define BASS_CONFIG_MIXER_BUFFER	0x10601
#define BASS_CONFIG_MIXER_POSEX		0x10602
#define BASS_CONFIG_SPLIT_BUFFER	0x10610

// BASS_Mixer_StreamCreate flags
#define BASS_MIXER_END			0x10000	// end the stream when there are no sources
#define BASS_MIXER_NONSTOP		0x20000	// don't stall when there are no sources
#define BASS_MIXER_RESUME		0x1000	// resume stalled immediately upon new/unpaused source
#define BASS_MIXER_POSEX		0x2000	// enable BASS_Mixer_ChannelGetPositionEx support

// BASS_Mixer_StreamAddChannel/Ex flags
#define BASS_MIXER_CHAN_ABSOLUTE		0x1000	// start is an absolute position
#define BASS_MIXER_CHAN_BUFFER		0x2000	// buffer data for BASS_Mixer_ChannelGetData/Level
#define BASS_MIXER_CHAN_LIMIT		0x4000	// limit mixer processing to the amount available from this source
#define BASS_MIXER_CHAN_MATRIX		0x10000	// matrix mixing
#define BASS_MIXER_CHAN_PAUSE		0x20000	// don't process the source
#define BASS_MIXER_CHAN_DOWNMIX		0x400000 // downmix to stereo/mono
#define BASS_MIXER_CHAN_NORAMPIN		0x800000 // don't ramp-in the start
#define BASS_MIXER_BUFFER		BASS_MIXER_CHAN_BUFFER
#define BASS_MIXER_LIMIT		BASS_MIXER_CHAN_LIMIT
#define BASS_MIXER_MATRIX		BASS_MIXER_CHAN_MATRIX
#define BASS_MIXER_PAUSE		BASS_MIXER_CHAN_PAUSE
#define BASS_MIXER_DOWNMIX		BASS_MIXER_CHAN_DOWNMIX
#define BASS_MIXER_NORAMPIN		BASS_MIXER_CHAN_NORAMPIN

// Mixer attributes
#define BASS_ATTRIB_MIXER_LATENCY	0x15000

// BASS_Split_StreamCreate flags
#define BASS_SPLIT_SLAVE		0x1000	// only read buffered data
#define BASS_SPLIT_POS			0x2000

// Splitter attributes
#define BASS_ATTRIB_SPLIT_ASYNCBUFFER		0x15010
#define BASS_ATTRIB_SPLIT_ASYNCPERIOD		0x15011

// Envelope node
typedef struct {
	QWORD pos;
	float value;
} BASS_MIXER_NODE;

// Envelope types
#define BASS_MIXER_ENV_FREQ		1
#define BASS_MIXER_ENV_VOL		2
#define BASS_MIXER_ENV_PAN		3
#define BASS_MIXER_ENV_LOOP		0x10000 // flag: loop
#define BASS_MIXER_ENV_REMOVE	0x20000 // flag: remove at end

// Additional sync types
#define BASS_SYNC_MIXER_ENVELOPE		0x10200
#define BASS_SYNC_MIXER_ENVELOPE_NODE	0x10201

// Additional BASS_Mixer_ChannelSetPosition flag
#define BASS_POS_MIXER_RESET	0x10000 // flag: clear mixer's playback buffer

// BASS_CHANNELINFO types
#define BASS_CTYPE_STREAM_MIXER	0x10800
#define BASS_CTYPE_STREAM_SPLIT	0x10801

DWORD BASSMIXDEF(BASS_Mixer_GetVersion)();

HSTREAM BASSMIXDEF(BASS_Mixer_StreamCreate)(DWORD freq, DWORD chans, DWORD flags);
BOOL BASSMIXDEF(BASS_Mixer_StreamAddChannel)(HSTREAM handle, DWORD channel, DWORD flags);
BOOL BASSMIXDEF(BASS_Mixer_StreamAddChannelEx)(HSTREAM handle, DWORD channel, DWORD flags, QWORD start, QWORD length);
DWORD BASSMIXDEF(BASS_Mixer_StreamGetChannels)(HSTREAM handle, DWORD *channels, DWORD count);

HSTREAM BASSMIXDEF(BASS_Mixer_ChannelGetMixer)(DWORD handle);
DWORD BASSMIXDEF(BASS_Mixer_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask);
BOOL BASSMIXDEF(BASS_Mixer_ChannelRemove)(DWORD handle);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode);
QWORD BASSMIXDEF(BASS_Mixer_ChannelGetPosition)(DWORD handle, DWORD mode);
QWORD BASSMIXDEF(BASS_Mixer_ChannelGetPositionEx)(DWORD channel, DWORD mode, DWORD delay);
DWORD BASSMIXDEF(BASS_Mixer_ChannelGetLevel)(DWORD handle);
BOOL BASSMIXDEF(BASS_Mixer_ChannelGetLevelEx)(DWORD handle, float *levels, float length, DWORD flags);
DWORD BASSMIXDEF(BASS_Mixer_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
HSYNC BASSMIXDEF(BASS_Mixer_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, void *user);
BOOL BASSMIXDEF(BASS_Mixer_ChannelRemoveSync)(DWORD channel, HSYNC sync);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetMatrix)(DWORD handle, const void *matrix);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetMatrixEx)(DWORD handle, const void *matrix, float time);
BOOL BASSMIXDEF(BASS_Mixer_ChannelGetMatrix)(DWORD handle, void *matrix);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetEnvelope)(DWORD handle, DWORD type, const BASS_MIXER_NODE *nodes, DWORD count);
BOOL BASSMIXDEF(BASS_Mixer_ChannelSetEnvelopePos)(DWORD handle, DWORD type, QWORD pos);
QWORD BASSMIXDEF(BASS_Mixer_ChannelGetEnvelopePos)(DWORD handle, DWORD type, float *value);

HSTREAM BASSMIXDEF(BASS_Split_StreamCreate)(DWORD channel, DWORD flags, const int *chanmap);
DWORD BASSMIXDEF(BASS_Split_StreamGetSource)(HSTREAM handle);
DWORD BASSMIXDEF(BASS_Split_StreamGetSplits)(DWORD handle, HSTREAM *splits, DWORD count);
BOOL BASSMIXDEF(BASS_Split_StreamReset)(DWORD handle);
BOOL BASSMIXDEF(BASS_Split_StreamResetEx)(DWORD handle, DWORD offset);
DWORD BASSMIXDEF(BASS_Split_StreamGetAvailable)(DWORD handle);

#ifdef __cplusplus
}
#endif

#endif
