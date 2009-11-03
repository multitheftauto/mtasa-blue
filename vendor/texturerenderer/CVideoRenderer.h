class CVideoRenderer;

#ifndef __CVIDEORENDERER_H
#define __CVIDEORENDERER_H

#include "ITextureRenderer.h"
#include "IVideoRenderer.h"

class CVideoRenderer : public IVideoRenderer
{
	public:
		CVideoRenderer( IDirect3DDevice9* pDevice );
		virtual ~CVideoRenderer( void );

		IDirect3DTexture9 *		GetVideoTexture( void );
		void					SetVideoTexture( IDirect3DTexture9 * pTexture );

		void					Lock( void ) { EnterCriticalSection ( &m_Lock ); }
		void					Unlock( void ) { LeaveCriticalSection ( &m_Lock ); }

		LPCRITICAL_SECTION		GetMutex( void );

		void					Play( void );
		void					Stop( void );

		void					Mute( void );
		void					Unmute( void );

		void					SetPosition( LONGLONG pCurrent );
		inline void				GetPositions( LONGLONG& pCurrent, LONGLONG& pDuration )	 { m_pMediaSeeking->GetPositions ( &pCurrent, &pDuration ); }

		void					OnLostDevice( void );
		void					OnResetDevice( IDirect3DDevice9* pDevice );

		bool					Initialize( const WCHAR * fileName );

	private:
		IDirect3DDevice9 *		m_pDevice;
		IDirect3DTexture9 *		m_pVideoTexture;

		CRITICAL_SECTION		m_Lock;

		IGraphBuilder *			m_pGraph;
		DWORD					m_dwGraphRegister;

		IBaseFilter *			m_pFilter;
		IMediaControl *			m_pMediaControl;
		IMediaSeeking *			m_pMediaSeeking;
		IBasicAudio *			m_pBasicAudio;

		long					lDefaultVolume;
};

#endif
