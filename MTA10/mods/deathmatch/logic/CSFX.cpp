/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSFX.cpp
*  PURPOSE:     SFX class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <libv2_default_bank.h>
#include <libv2.h>

CSFX::CSFX ( void )
{
	Initialize ();
}

CSFX::~CSFX ( void )
{
	dsLock ();
	for ( unsigned int i = 0; i < m_Synths.size(); i++)
		delete m_Synths[i];
	dsUnlock ();

	Destroy ();
}

CSFXSynth* CSFX::CreateSynth ( void )
{
	dsLock ();
	CSFXSynth *pSynth = new CSFXSynth ();
	m_Synths.push_back ( pSynth );
	pSynth->Initialize ();
	dsUnlock ();
	return pSynth;
}

void CSFX::DeleteSynth ( CSFXSynth* pSynth )
{
	dsLock ();
	for ( unsigned int i = 0; i < m_Synths.size(); i++) {
		if ( m_Synths[i] == pSynth ) {
			m_Synths.erase ( m_Synths.begin () + i );
		}
	}
	delete pSynth;
	dsUnlock ();
}

void CSFX::Initialize ( void )
{
	// Initialize DirectSound output
	dsInit (CSFX::RenderCallback, this, GetForegroundWindow ());
	SetVolume ( 1.0f );

	g_pCore->GetConsole ()->Printf ( "Created DirectSound thread" );
}

void CSFX::Destroy ( void )
{
	// Close DirectSound output
	dsClose ();
}

void CSFX::SetVolume ( float fVolume )
{
	m_fVolume = fVolume;
	dsSetVolume ( fVolume );
}

void CSFX::Render ( float *pBuffer, unsigned long nLen )
{
	if ( !pBuffer || !nLen ) return;

	// This runs in a different thread!
	for ( unsigned int i = 0; i < m_Synths.size(); i++) {
		m_Synths[i]->Render ( pBuffer, nLen );
	}
	if ( m_Synths.size() == 0 ) {
		  __asm {
			  mov edi, [pBuffer]
			  mov ecx, [nLen]
			  shl ecx, 1
			  xor eax, eax
			  rep stosd
		  }
	}
}

CSFXSynth::CSFXSynth ( void )
{
	m_pGlobals = m_pPatchMap = m_pLyrics = NULL;
	m_nInstruments = 0;
}

CSFXSynth::~CSFXSynth ( void )
{
	if ( m_pGlobals )	delete m_pGlobals;
	if ( m_pPatchMap )	delete m_pPatchMap;
	if ( m_pLyrics )	delete m_pLyrics;
}

void CSFXSynth::Initialize ( void )
{
	dsLock ();

	// Default settings
	unsigned char globals[] = { 0x2a, 0x70, 0x20, 0x56, 0x5f, 0x50, 0x3e, 0x42, 0x00, 0x00, 0x40, 0x00,
								0x79, 0x00, 0x00, 0x01, 0x02, 0x5a, 0x20, 0x14, 0x40, 0x40, 0x00 };

	LoadV2B ( libv2_default_bank );
	synthInit ( m_Synth, m_pPatchMap, SFX_SYNTH_SAMPLE_RATE );
	SetGlobals ( globals, sizeof(globals) );

	g_pCore->GetConsole ()->Printf ( "Initialized libv2, (C) Tammo 'kb' Hinrichs 2000-2008" );

	dsUnlock ();
}

#define COPY_STUFF(__x)			if ( __x ) free ( __x );	\
								__x = malloc ( nSize );		\
								memcpy ( __x, pData, nSize )


bool CSFXSynth::LoadV2B ( unsigned char *szBuffer )
{
	// This function loads a libv2 VST .v2b bank file

	#define HEADER_DATA "v2p0"
	#define HEADER_SIZE	4
	#define DESC_START	4			// Start address for patch descriptors (names)
	#define PATCH_START	0x1008		// Start address for patch data
	#define PATCH_SIZE	0x357		// Patch size, seems to be fixed
	#define INSTRUMENTS	127

	unsigned char *data = new unsigned char [(sizeof(unsigned long) + PATCH_SIZE) * INSTRUMENTS];

	// Verify header
	char szHeader[HEADER_SIZE + 1] = { 0 };
	memcpy ( szHeader, szBuffer, HEADER_SIZE );
	if ( strcmp ( szHeader, HEADER_DATA ) != 0 ) return NULL;

	// Load patch data
	szBuffer = szBuffer + PATCH_START;
	unsigned long ptr_data = sizeof(unsigned long) * INSTRUMENTS;
	for ( unsigned int i = 0; i < INSTRUMENTS; i++ ) {
		// Read patch data into buffer
		memcpy ( &data[ptr_data], szBuffer, PATCH_SIZE );
		szBuffer += PATCH_SIZE;

		// Update patch pointer
		unsigned long *ptr = (unsigned long*)&data[i * sizeof(unsigned long)];
		*ptr = ptr_data;

		ptr_data += PATCH_SIZE;
	}

	SetPatchMap ( data, (sizeof(unsigned long) + PATCH_SIZE) * INSTRUMENTS );
	delete [] data;

	return true;
}

void CSFXSynth::SetPatchMap ( void *pData, unsigned long nSize )
{
	COPY_STUFF ( m_pPatchMap );

	// Process the patch map to see how many instruments we have

	// Patch map layout:
	// [4-byte patch pointers][patch data]
	unsigned long ptr_smallest = 0xFFFFFFFF;
	unsigned long index = 0, instr = 0;
	while(index < ptr_smallest) {
		unsigned long ptr = *((unsigned long*)&((unsigned char *)m_pPatchMap)[index]);
		if(ptr < ptr_smallest) ptr_smallest = ptr;
		index += 4;
		instr++;
	}
	m_nInstruments = instr;

	g_pCore->GetConsole ()->Printf ( "Loaded libv2 patch map (%u instruments)", m_nInstruments );
}

void CSFXSynth::SetGlobals ( void *pData, unsigned long nSize )
{
	COPY_STUFF ( m_pGlobals );
	synthSetGlobals ( m_Synth, m_pGlobals );
}

void CSFXSynth::SetLyrics ( void *pData, unsigned long nSize )
{
	COPY_STUFF ( m_pLyrics );
}

bool CSFXSynth::CheckMIDI ( unsigned char *pMIDI, unsigned long nSize )
{
	// Checks the sanity of a MIDI command (since libv2 tends to crash)
	// e.g. MIDI commands: 0xCX (change patch)
	return true;
}

bool CSFXSynth::ProcessMIDI ( unsigned char *pMIDI, unsigned long nSize )
{
	if ( !CheckMIDI ( pMIDI, nSize ) ) return false;

	#define nReserved 4

	nSize = (nSize > SFX_SYNTH_BUFFER_SIZE) ? SFX_SYNTH_BUFFER_SIZE : nSize;

	unsigned char pBuf[SFX_SYNTH_BUFFER_SIZE + nReserved];
	memcpy ( pBuf, pMIDI, nSize );

	// Make sure we have a little space at the end of the buffer,
	// in case we're sending an incomplete MIDI command and libv2
	// reads beyond the boundary. 0xFD is libv2's terminator.
	for(unsigned int i = 0; i < nReserved; i++) {
		pBuf[nSize + i] = 0xFD;
	}

	synthProcessMIDI ( m_Synth, pBuf );
	return true;
}

void CSFXSynth::Render ( float *pBuffer, unsigned long nLen )
{
	if ( !pBuffer ) return;

	synthRender ( m_Synth, pBuffer, nLen, 0 );
}

void CSFXSynth::GetVUMaster ( float &fLeft, float &fRight )
{
	//synthGetMainVU ( m_Synth, &fLeft, &fRight );	// unresolved?
}

void CSFXSynth::GetVUChannel ( int iChannel, float &fLeft, float &fRight )
{
	synthGetChannelVU ( m_Synth, iChannel, &fLeft, &fRight );
}

void CSFXSynth::SetVUMode ( VUMode Mode )
{
	synthSetVUMode ( m_Synth, (int) Mode );
}
