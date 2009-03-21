/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSFX.h
*  PURPOSE:     Header forSFX class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Sound effects management
class CSFX;

// Synthesizer (libv2)
class CSFXSynth;

#ifndef __CSFX_H
#define __CSFX_H

#define SFX_SYNTH_SAMPLE_RATE	44100
#define SFX_SYNTH_BUFFER_SIZE	2048

#include <vector>

class CSFX
{
public:
							CSFX			( void );
							~CSFX			( void );

	void					SetVolume		( float fVolume );
	float					GetVolume		( void ) { return m_fVolume; }

	// DirectSound render callback
	static void __stdcall	RenderCallback	( void *pThis, float *pBuffer, unsigned long nLen )
											{ reinterpret_cast < CSFX* >(pThis)->Render ( pBuffer, nLen ); }

	CSFXSynth*				CreateSynth		( void );
	void					DeleteSynth		( CSFXSynth* pSynth );

	void					Render			( float *pBuffer, unsigned long nLen );

private:
	void					    Initialize		( void );
	void					    Destroy			( void );

	float					    m_fVolume;

	std::vector < CSFXSynth* >	m_Synths;
};

class CSFXSynth
{
	friend class CSFX;

	enum VUMode {
		VU_METER_PEAK = 0,
		VU_METER_RMS = 1
	};

protected:
							CSFXSynth		( void );
							~CSFXSynth		( void );

	void					Render			( float *pBuffer, unsigned long nLen );
	bool					LoadV2B			( unsigned char *szBuffer );

public:
	// libv2 wrapper functions
	void					Initialize		( void );
	void					SetPatchMap		( void *pData, unsigned long nSize );
	void					SetGlobals		( void *pData, unsigned long nSize );
	void					SetLyrics		( void *pData, unsigned long nSize );
	bool					ProcessMIDI		( unsigned char *pMIDI, unsigned long nSize );

	// VU-meter functions
	void					GetVUMaster		( float &fLeft, float &fRight );
	void					GetVUChannel	( int iChannel, float &fLeft, float &fRight );
	void					SetVUMode		( VUMode Mode );
	
private:
	bool					CheckMIDI		( unsigned char *pMIDI, unsigned long nSize );

	void*					m_pGlobals;
	void*					m_pPatchMap;
	void*					m_pLyrics;

	unsigned char			m_Synth[3*1024*1024];
	unsigned char			m_nInstruments;
};

#endif
