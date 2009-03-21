/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoManager.h
*  PURPOSE:     Header file for DirectShow video renderer manager class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVideoManager;

#ifndef __CVIDEOMANAGER_H
#define __CVIDEOMANAGER_H

#include <d3d9.h>
#include <dshow.h>
#include <uuids.h>
#include <vmr9.h>

#include "CSingleton.h"
#include "CVideoRenderer.h"

#include <list>

// Codec GUIDs
#include <initguid.h>
DEFINE_GUID ( CLSID_XVID,		mmioFOURCC('x','v','i','d'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID ( CLSID_VORBIS,		0x0835DC4B, 0xAA01, 0x48C3, 0xA4, 0x2D, 0xFD, 0x62, 0xC5, 0x30, 0xA3, 0xE1 );		// {0835DC4B-AA01-48C3-A42D-FD62C530A3E1} - CoreVorbis
DEFINE_GUID ( CLSID_SOURCE,		0x0A68C3B5, 0x9164, 0x4A54, 0xAF, 0xAF, 0x99, 0x5B, 0x2F, 0xF0, 0xE0, 0xD4 );		// {0A68C3B5-9164-4A54-AFAF-995B2FF0E0D4} - Gabest Matroska Source

class CVideoManager : public CSingleton < CVideoManager >
{
	friend class CVideoRenderer;

public:
										CVideoManager			( void );
										~CVideoManager			( void );

	bool								CreateRenderer			( CVideoRenderer **ppRenderer, const char * szFile );
	void								DestroyRenderer			( CVideoRenderer *pRenderer );

	void								OnLostDevice			( void );
	void								OnResetDevice			( IDirect3DDevice9* pDevice );
protected:
	inline HRESULT						CreateCodecSource		( IBaseFilter ** pFilter ) { return CreateCodec ( CLSID_SOURCE, m_pFactorySource, pFilter ); };
	inline HRESULT						CreateCodecVorbis		( IBaseFilter ** pFilter ) { return CreateCodec ( CLSID_VORBIS, m_pFactoryVorbis, pFilter ); };
	inline HRESULT						CreateCodecXVID			( IBaseFilter ** pFilter ) { return CreateCodec ( CLSID_XVID, m_pFactoryXVID, pFilter ); };

private:
	void								DestroyRenderers		( void );

	HRESULT                             LoadCodec               ( REFCLSID ClassID, const char * szPath, IClassFactory ** ppFactory );
	HRESULT                             CreateCodec             ( REFCLSID ClassID, IClassFactory * pFactory, IBaseFilter ** ppFilter );

    std::list < CVideoRenderer* >            m_List;
	IClassFactory*                      m_pFactorySource;
	IClassFactory*                      m_pFactoryXVID;
	IClassFactory*                      m_pFactoryVorbis;
};

#endif
