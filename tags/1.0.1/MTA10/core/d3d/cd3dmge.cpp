/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/d3d/cd3dmge.cpp
*  PURPOSE:		Interface for rendering Direct3D primitives/sprites
*  DEVELOPERS:	Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <tchar.h>
#include "include/cd3dmge.h"

inline D3DSPRITEVERTEX2D InitFont2DVertex( FLOAT x, FLOAT y, FLOAT z, FLOAT rhw, DWORD dwColor, FLOAT tu, FLOAT tv )
{
    D3DSPRITEVERTEX2D v = { x, y, z, rhw, dwColor, tu, tv };
    return v;
}

inline D3DSPRITEVERTEX3D InitBox3DVertex( FLOAT x, FLOAT y, FLOAT z, DWORD dwColor )
{
    D3DSPRITEVERTEX3D v = { x, y, z, dwColor };
    return v;
}

CD3DMGEng::CD3DMGEng ( LPDIRECT3DDEVICE8 pD3DDevice )
{
	m_pDevice = pD3DDevice;
		
	m_pFontTexture = NULL;
	m_fFontTexWidth = 0.0f;
	m_fFontTexHeight = 0.0f;
	m_pVB = NULL;
	m_bFontInfoLoaded = FALSE;
	m_dwSavedStateBlock = 0L;
	m_dwRenderStateBlock = 0L;

	memset(m_aFontCoords,0,sizeof(m_aFontCoords));

}

VOID CD3DMGEng::OnRestoreDevice ( void )
{

	m_pDevice->CreateVertexBuffer( D3DSPRITE_NUMVERTS*sizeof(D3DSPRITEVERTEX2D),  //sizeof(2D) > sizeof(3D)
								   D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								   0,
								   D3DPOOL_DEFAULT,
								   &m_pVB );

	{
		/////////////////////////////////////////////////////////////////////
		// Create the state blocks.
		m_pDevice->CreateStateBlock( D3DSBT_ALL, &m_dwSavedStateBlock );


		/////////////////////////////////////////////////////////////////////
		// Setup the rendering state block.		
		m_pDevice->BeginStateBlock();

		m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		m_pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
        m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pDevice->EndStateBlock( &m_dwRenderStateBlock );
	}
}

VOID CD3DMGEng::OnDeleteDevice ( void )
{

}

VOID CD3DMGEng::OnInvalidateDevice ( void )
{
	SAFE_RELEASE( m_pVB )
	
	if ( m_dwSavedStateBlock ) 
	{
		m_pDevice->DeleteStateBlock( m_dwSavedStateBlock );
	}

	if ( m_dwRenderStateBlock )
	{
		m_pDevice->DeleteStateBlock( m_dwRenderStateBlock );
	}

	m_dwSavedStateBlock = 0L;
	m_dwRenderStateBlock = 0L;
}

BOOL CD3DMGEng::Render2DSprite ( LPDIRECT3DTEXTURE8 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColor )
{
	return Render2DSpriteEx( pTexture, pScaling, pTranslation, dwColor, dwColor, dwColor, dwColor );
}

BOOL CD3DMGEng::Render2DSpriteEx ( LPDIRECT3DTEXTURE8 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR )
{
	D3DSURFACE_DESC myDesc;
	D3DSPRITEVERTEX2D *pVertices;

	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	////////////////////////////////////////////////////
	// If we have a NULL translation, use point 0,0.
	float  x = 0;
	float  y = 0;
	if ( pTranslation != NULL )
	{
		x = pTranslation->x;
		y = pTranslation->y;
	}

	////////////////////////////////////////////////////
	// Get the width and height of the image to render.
	pTexture->GetLevelDesc( 0, &myDesc );
	float tx = static_cast<float>(myDesc.Width);
	float ty = static_cast<float>(myDesc.Height);

	
	///////////////////////////////////////////////////
	// Scale the image if necessary.
	if ( pScaling != NULL )
	{
		tx *= pScaling->x;
		ty *= pScaling->y;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
    m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX2DTEX );
    m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX2D) );
	m_pDevice->SetTexture( 0, pTexture );


	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
	{
		pVertices[0].fX =		x;
		pVertices[0].fY =		y;
		pVertices[0].fZ =		0.0f;
		pVertices[0].fRHW =		1.0f;
		pVertices[0].tu =		0.0f;
		pVertices[0].tv =		0.0f;
		pVertices[0].dwColor =	dwColorTL;  //TOP LEFT

		pVertices[1].fX =		x+tx;
		pVertices[1].fY =		y;
		pVertices[1].fZ =		0.0f;
		pVertices[1].fRHW =		1.0f;
		pVertices[1].tu =		1.0f;
		pVertices[1].tv =		0.0f;
		pVertices[1].dwColor =	dwColorTR;  //TOP RIGHT

		pVertices[2].fX =		x+tx;
		pVertices[2].fY =		y+ty;
		pVertices[2].fZ =		0.0f;
		pVertices[2].fRHW =		1.0f;
		pVertices[2].tu =		1.0f;
		pVertices[2].tv =		1.0f;
		pVertices[2].dwColor =	dwColorBR;  //BOTTOM RIGHT

		pVertices[3].fX =		x;
		pVertices[3].fY =		y+ty;
		pVertices[3].fZ =		0.0f;
		pVertices[3].fRHW =		1.0f;
		pVertices[3].tu =		0.0f;
		pVertices[3].tv =		1.0f;
		pVertices[3].dwColor =	dwColorBL;  //BOTTOM LEFT
	}
	m_pVB->Unlock();


	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	m_pDevice->SetTexture( 0, NULL );


	m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );

	return TRUE;
}

BOOL CD3DMGEng::Render3DSprite ( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColor, BOOL bIgnoreSetup )
{
	return Render3DSpriteEx( pTexture, fScale, pTranslation, pMatView, dwColor, dwColor, dwColor, dwColor, bIgnoreSetup );
}

BOOL CD3DMGEng::BeginDrawing ()
{
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
	m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX3DTEX );
	m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX3D) );
	return TRUE;
}

BOOL CD3DMGEng::EndDrawing ()
{
	m_pDevice->SetTexture( 0, NULL );

	////////////////////////////////////////////////////
	// Restore old states and return.
	m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );
	return TRUE;
}
BOOL CD3DMGEng::Render3DSpriteEx ( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR, BOOL bIgnoreSetup ) 
{
	D3DSPRITEVERTEX3D *pVertices;

	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	////////////////////////////////////////////////////
	// If we have a NULL translation, return an error.
	if ( pTranslation == NULL )
	{
		return FALSE;
	}
	float  x = pTranslation->x;
	float  y = pTranslation->y;
	float  z = pTranslation->z;

	////////////////////////////////////////////////////
	// Setup the adjustment vectors.
	D3DXVECTOR3 upVect = D3DXVECTOR3(pMatView->_11,pMatView->_21,pMatView->_31);
	D3DXVECTOR3 rightVect = D3DXVECTOR3(pMatView->_12,pMatView->_22,pMatView->_32);
	D3DXVec3Normalize( &rightVect, &rightVect );
	D3DXVec3Normalize( &upVect, &upVect );
	rightVect *= fScale;
	upVect *= fScale;
	
	if(!bIgnoreSetup)
	{
		///////////////////////////////////////////////////
		// Setup the rendering.
		m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
		m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
		m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX3DTEX );
		m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX3D) );
	}
	m_pDevice->SetTexture( 0, pTexture );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
	{
		pVertices[0].fX =		x-rightVect.x+upVect.x;
		pVertices[0].fY =		y-rightVect.y+upVect.y;
		pVertices[0].fZ =		z-rightVect.z+upVect.z;
		pVertices[0].tu =		1.0f;
		pVertices[0].tv =		1.0f;
		pVertices[0].dwColor =	dwColorBR;  //BOTTOM RIGHT

		pVertices[1].fX =		x+upVect.x+rightVect.x;
		pVertices[1].fY =		y+upVect.y+rightVect.y;
		pVertices[1].fZ =		z+upVect.z+rightVect.z;
		pVertices[1].tu =		1.0f;
		pVertices[1].tv =		0.0f;
		pVertices[1].dwColor =	dwColorTR;  //TOP RIGHT

		pVertices[2].fX =		x-upVect.x-rightVect.x;
		pVertices[2].fY =		y-upVect.y-rightVect.y;
		pVertices[2].fZ =		z-upVect.z-rightVect.z;
		pVertices[2].tu =		0.0f;
		pVertices[2].tv =		1.0f;
		pVertices[2].dwColor =	dwColorBL;  //BOTTOM LEFT

		pVertices[3].fX =		x+rightVect.x-upVect.x;
		pVertices[3].fY =		y+rightVect.y-upVect.y;
		pVertices[3].fZ =		z+rightVect.z-upVect.z;
		pVertices[3].tu =		0.0f;
		pVertices[3].tv =		0.0f;
		pVertices[3].dwColor =	dwColorTL;  //TOP LEFT
	}
	m_pVB->Unlock();

	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	if(!bIgnoreSetup)
	{
		m_pDevice->SetTexture( 0, NULL );

		////////////////////////////////////////////////////
		// Restore old states and return.
		m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );
	}
	return TRUE;
}

BOOL CD3DMGEng::Render2DBox ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColor )
{
	return Render2DBoxEx( x, y, fWidth, fHeight, dwColor, dwColor, dwColor, dwColor );
}

BOOL CD3DMGEng::Render2DBoxEx ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR )
{
	D3DSPRITEVERTEX2D *pVertices;

	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
    m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX2D );
    m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX2D) );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
	{
		pVertices[0].fX =		x;
		pVertices[0].fY =		y;
		pVertices[0].fZ =		0.0f;
		pVertices[0].fRHW =		1.0f;
		pVertices[0].tu =		0.0f;
		pVertices[0].tv =		0.0f;
		pVertices[0].dwColor =	dwColorTL;  //TOP LEFT

		pVertices[1].fX =		x+fWidth;
		pVertices[1].fY =		y;
		pVertices[1].fZ =		0.0f;
		pVertices[1].fRHW =		1.0f;
		pVertices[1].tu =		1.0f;
		pVertices[1].tv =		0.0f;
		pVertices[1].dwColor =	dwColorTR;  //TOP RIGHT

		pVertices[2].fX =		x+fWidth;
		pVertices[2].fY =		y+fHeight;
		pVertices[2].fZ =		0.0f;
		pVertices[2].fRHW =		1.0f;
		pVertices[2].tu =		1.0f;
		pVertices[2].tv =		1.0f;
		pVertices[2].dwColor =	dwColorBR;  //BOTTOM RIGHT

		pVertices[3].fX =		x;
		pVertices[3].fY =		y+fHeight;
		pVertices[3].fZ =		0.0f;
		pVertices[3].fRHW =		1.0f;
		pVertices[3].tu =		0.0f;
		pVertices[3].tv =		1.0f;
		pVertices[3].dwColor =	dwColorBL;  //BOTTOM LEFT
	}
	m_pVB->Unlock();


	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );

	return TRUE;
}

BOOL CD3DMGEng::Render3DBox ( FLOAT x, FLOAT y, FLOAT z, FLOAT fLWH, DWORD dwColor )
{
	D3DSPRITEVERTEX3D *pVertices;

	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
    m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX3D );
    m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX3D) );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	FLOAT w = fLWH/2;
	FLOAT h = fLWH/2;
	FLOAT d = fLWH/2;
	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
	{
		////////////////////////////////////////////////
		// Do floating point arithmetic once only.
		FLOAT px1 = x-w;
		FLOAT px2 = x+w;
		FLOAT py1 = y-h;
		FLOAT py2 = y+h;
		FLOAT pz1 = z-d;
		FLOAT pz2 = z+d;

		///////////////////////////////////////////////
		// Render faces of cube.
		*pVertices++ = InitBox3DVertex( px1,  py2, pz2,  dwColor ); //FRONT FACE
		*pVertices++ = InitBox3DVertex( px2,  py2, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz2,  dwColor );

		*pVertices++ = InitBox3DVertex( px1,  py2, pz1,  dwColor ); //BACK FACE
		*pVertices++ = InitBox3DVertex( px2,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz1,  dwColor );

		*pVertices++ = InitBox3DVertex( px1,  py2, pz2,  dwColor ); // TOP FACE
		*pVertices++ = InitBox3DVertex( px1,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py2, pz2,  dwColor );

		*pVertices++ = InitBox3DVertex( px1,  py1, pz2,  dwColor ); // BOTTOM FACE
		*pVertices++ = InitBox3DVertex( px1,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz2,  dwColor );

		*pVertices++ = InitBox3DVertex( px1,  py2, pz2,  dwColor ); // LEFT FACE
		*pVertices++ = InitBox3DVertex( px1,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py2, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px1,  py1, pz2,  dwColor );

		*pVertices++ = InitBox3DVertex( px2,  py2, pz2,  dwColor ); // RIGHT FACE
		*pVertices++ = InitBox3DVertex( px2,  py2, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz1,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py2, pz2,  dwColor );
		*pVertices++ = InitBox3DVertex( px2,  py1, pz2,  dwColor );
	}
	m_pVB->Unlock();

	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 12 );

	////////////////////////////////////////////////////
	// Restore old states and return.
	m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );

	return TRUE;
}

BOOL CD3DMGEng::LoadFontTextureFromMem ( PVOID pvMemory, DWORD dwSizeOfFile )
{
	D3DSURFACE_DESC desc;
	
	//////////////////////////////////////////////////
	// Release any existing texture.
	SAFE_RELEASE( m_pFontTexture );

	//////////////////////////////////////////////////
	// Load the requested texture from memory.
	if ( (D3D_OK != D3DXCreateTextureFromFileInMemory( m_pDevice, pvMemory, dwSizeOfFile, &m_pFontTexture )) )
	{
		return FALSE;
	}

	m_pFontTexture->GetLevelDesc( 0, &desc );
	m_fFontTexWidth = static_cast<FLOAT>(desc.Width);
	m_fFontTexHeight = static_cast<FLOAT>(desc.Height);
	LoadDefaultFontInfo();

	return TRUE;
}

BOOL CD3DMGEng::LoadFontTextureFromFile ( TCHAR *wszFileName )
{
	D3DSURFACE_DESC desc;

	//////////////////////////////////////////////////
	// Release any existing texture.
	SAFE_RELEASE( m_pFontTexture );

	//////////////////////////////////////////////////
	// Load the requested texture.from file.
	if ( (D3D_OK != D3DXCreateTextureFromFile( m_pDevice, wszFileName, &m_pFontTexture )) )
	{
		return FALSE;
	}

	m_pFontTexture->GetLevelDesc( 0, &desc );
	m_fFontTexWidth = static_cast<FLOAT>(desc.Width);
	m_fFontTexHeight = static_cast<FLOAT>(desc.Height);
	LoadDefaultFontInfo();

	return TRUE;
}

BOOL CD3DMGEng::LoadFontInfoFromMem ( PVOID pvMemory )
{
	//////////////////////////////////////////////////////
	// We must have our texture loaded first!
	if ( m_pFontTexture == NULL )
	{
		return FALSE;
	}

	BYTE *pTypeWidths = static_cast<PBYTE>(pvMemory);
	FLOAT fDistX = 0.0f;
	FLOAT fDistY = 0.0f;
	FLOAT fCharHeight = m_fFontTexHeight / D3DFE_CHARACTERSPERROW;

	for ( int r=0; r < D3DFE_CHARACTERSPERROW; r++ )
	{
		/////////////////////////////////////////////////////////
		// Need to calculate widths separately for truetype.
		for ( int c=0; c < D3DFE_CHARACTERSPERCOLUMN; c++ )
		{
			UCHAR i = (r * D3DFE_CHARACTERSPERROW) + c;
			FLOAT fCharWidth = static_cast<FLOAT>(pTypeWidths[i]);
			
			m_aFontCoords[i].tx1 = fDistX / m_fFontTexWidth;
			m_aFontCoords[i].tx2 = (fDistX + fCharWidth) / m_fFontTexWidth;
			m_aFontCoords[i].ty1 = fDistY / m_fFontTexHeight;
			m_aFontCoords[i].ty2 = (fDistY + fCharHeight) / m_fFontTexHeight;
			m_aFontCoords[i].fWidth = fCharWidth;
			m_aFontCoords[i].fHeight = fCharHeight;

			fDistX += m_fFontTexWidth / D3DFE_CHARACTERSPERCOLUMN;
		}

		fDistX  = 0.0f;
		fDistY += fCharHeight;
	}

	return (m_bFontInfoLoaded = TRUE);
}

BOOL CD3DMGEng::LoadFontInfoFromFile ( TCHAR *wszFileName )
{
	HANDLE hFile;
	DWORD dwSizeRead;
	BYTE *pTypeWidths;

	//////////////////////////////////////////////////////
	// We must have our texture loaded first!
	if ( m_pFontTexture == NULL )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////
	// Load the widths file from disk.
	if ( (hFile = CreateFile( wszFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL )) == NULL )
	{
		return FALSE;
	}

	if ( (pTypeWidths = static_cast<PBYTE>(VirtualAlloc( NULL,D3DFE_MAXFONTWIDTHS,MEM_COMMIT,PAGE_READWRITE ))) == NULL ) 
	{
		return FALSE;
	}

    ReadFile(hFile, pTypeWidths, D3DFE_MAXFONTWIDTHS, &dwSizeRead, NULL);
	CloseHandle(hFile);

	//////////////////////////////////////////////////////
	// Start filling up our coordinate array.
	FLOAT fDistX = 0.0f;
	FLOAT fDistY = 0.0f;
	FLOAT fCharHeight = m_fFontTexHeight / D3DFE_CHARACTERSPERROW;

	for ( int r=0; r < D3DFE_CHARACTERSPERROW; r++ )
	{
		/////////////////////////////////////////////////////////
		// Need to calculate widths separately for truetype.
		for ( int c=0; c < D3DFE_CHARACTERSPERCOLUMN; c++ )
		{
			UCHAR i = (r * D3DFE_CHARACTERSPERROW) + c;
			FLOAT fCharWidth = static_cast<FLOAT>(pTypeWidths[i]);
			
			m_aFontCoords[i].tx1 = fDistX / m_fFontTexWidth;
			m_aFontCoords[i].tx2 = (fDistX + fCharWidth) / m_fFontTexWidth;
			m_aFontCoords[i].ty1 = fDistY / m_fFontTexHeight;
			m_aFontCoords[i].ty2 = (fDistY + fCharHeight) / m_fFontTexHeight;
			m_aFontCoords[i].fWidth = fCharWidth;
			m_aFontCoords[i].fHeight = fCharHeight;

			fDistX += m_fFontTexWidth / D3DFE_CHARACTERSPERCOLUMN;
		}

		fDistX  = 0.0f;
		fDistY += fCharHeight;
	}

	//////////////////////////////////////////////////////
	// Free up the memory we used to get the file data.
	VirtualFree( static_cast<PVOID>(pTypeWidths), D3DFE_MAXFONTWIDTHS, MEM_RELEASE );

	return (m_bFontInfoLoaded = TRUE);
}

BOOL CD3DMGEng::LoadDefaultFontInfo ( VOID )
{
	//////////////////////////////////////////////////////
	// We must have our texture loaded first!
	if ( m_pFontTexture == NULL )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////
	// Start filling up our coordinate array.
	FLOAT fDistX = 0.0f;
	FLOAT fDistY = 0.0f;
	FLOAT fCharHeight = m_fFontTexHeight / D3DFE_CHARACTERSPERROW;
	FLOAT fCharWidth = m_fFontTexWidth / D3DFE_CHARACTERSPERCOLUMN;

	for ( int r=0; r < D3DFE_CHARACTERSPERROW; r++ )
	{
		for ( int c=0; c < D3DFE_CHARACTERSPERCOLUMN; c++ )
		{
			UCHAR i = (r * D3DFE_CHARACTERSPERROW) + c;
			
			m_aFontCoords[i].tx1 = fDistX / m_fFontTexWidth;
			m_aFontCoords[i].tx2 = (fDistX + fCharWidth) / m_fFontTexWidth;
			m_aFontCoords[i].ty1 = fDistY / m_fFontTexHeight;
			m_aFontCoords[i].ty2 = (fDistY + fCharHeight) / m_fFontTexHeight;
			m_aFontCoords[i].fWidth = fCharWidth;
			m_aFontCoords[i].fHeight = fCharHeight;

			fDistX += fCharWidth;
		}

		fDistX  = 0.0f;
		fDistY += fCharHeight;
	}

	return (m_bFontInfoLoaded = TRUE);
}

BOOL CD3DMGEng::UnloadFont ( VOID )
{
	SAFE_RELEASE( m_pFontTexture );
	m_bFontInfoLoaded = FALSE;

	return TRUE;
}

BOOL CD3DMGEng::DrawText2D ( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR *wszText,  FLOAT fTextSize, FONTFLAGS ffFlags, FONTSET fsSet )
{
	D3DSPRITEVERTEX2D *pVertices;

	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	////////////////////////////////////////////////////
	// Make sure our texture and coords are loaded.
	if ( m_pFontTexture == NULL || m_bFontInfoLoaded == FALSE )
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pDevice->ApplyStateBlock( m_dwRenderStateBlock );
    m_pDevice->SetVertexShader( D3DFVF_SPRITEVERTEX2DTEX );
    m_pDevice->SetStreamSource( 0, m_pVB, sizeof(D3DSPRITEVERTEX2D) );
	m_pDevice->SetTexture( 0, m_pFontTexture );


	///////////////////////////////////////////////////
	// Do flag processing before we go any further.
	if ( ffFlags )
	{
		SIZE sz;

		// First, get extent of drawn text.
		UTIL_GetTextExtent(wszText, &sz, fTextSize);

		// Do math on x coordinate accordingly.
		switch ( ffFlags )
		{
		case D3DFF_CENTERED:
			x -= sz.cx/2;
			break;
		case D3DFF_RIGHT:
			x -= sz.cx;
			break;
		case D3DFF_LEFT:
		default:
			break;
		}
	} 
		
	
	DWORD dwTriangleCount = 0;
	const FLOAT fStartX = x;
	m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
	while ( *wszText )
	{
		TCHAR c = *wszText++;
		FLOAT fScl = fTextSize > 1.0f ? fTextSize / m_aFontCoords[fsSet].fHeight : 1.0f;

		if ( c == _T('\n') )
		{
			x = fStartX;
			y += m_aFontCoords[fsSet].fHeight * fScl;
			continue;
		}

		if ( c < _T(' ') || c > _T(128) )
		{
			continue;
		}

		FLOAT tx1 = m_aFontCoords[fsSet+c-32].tx1;
		FLOAT tx2 = m_aFontCoords[fsSet+c-32].tx2;
		FLOAT ty1 = m_aFontCoords[fsSet+c-32].ty1;
		FLOAT ty2 = m_aFontCoords[fsSet+c-32].ty2;
		FLOAT   w = m_aFontCoords[fsSet+c-32].fWidth;
		FLOAT   h = m_aFontCoords[fsSet+c-32].fHeight;
		
		w *= fScl;
		h *= fScl;

		if ( c == _T(' ') )
		{
			x += w;
			continue;
		}

		*pVertices++ = InitFont2DVertex( x,   y,   0.0f, 1.0f, dwColor, tx1, ty1 );
		*pVertices++ = InitFont2DVertex( x+w, y,   0.0f, 1.0f, dwColor, tx2, ty1 );
		*pVertices++ = InitFont2DVertex( x,   y+h, 0.0f, 1.0f, dwColor, tx1, ty2 );

		*pVertices++ = InitFont2DVertex( x,   y+h, 0.0f, 1.0f, dwColor, tx1, ty2 );
		*pVertices++ = InitFont2DVertex( x+w, y,   0.0f, 1.0f, dwColor, tx2, ty1 );
		*pVertices++ = InitFont2DVertex( x+w, y+h, 0.0f, 1.0f, dwColor, tx2, ty2 );

		x += w;
		dwTriangleCount += 2;

		////////////////////////////////////////////////////
		// If we've gone over our limit, draw what we have.
		if ( dwTriangleCount*3 >= D3DSPRITE_NUMVERTS-6 )
		{
			m_pVB->Unlock();
			m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwTriangleCount );
			m_pVB->Lock( 0, 0, (BYTE**)&(pVertices=NULL), D3DLOCK_DISCARD );
			dwTriangleCount = 0L;
		}

	}
	m_pVB->Unlock();


	////////////////////////////////////////////////////
	// Draw if we have anything.
	if ( dwTriangleCount > 0 )
	{
		m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwTriangleCount );
		m_pDevice->SetTexture( 0, NULL );
	}

	m_pDevice->ApplyStateBlock( m_dwSavedStateBlock );

	return TRUE;
}

BOOL CD3DMGEng::UTIL_FormatText ( const TCHAR* wszInputText, TCHAR* wszOutputText, float fMaxWidth, FLOAT fTextSize, FONTSET fsSet )
{
	FLOAT fCurrentWidth = 0.0f;
	DWORD dwOutputIndex = 0;

	// Make sure we have valid parameters.
	if ( wszInputText == NULL || wszOutputText == NULL )
	{
		return FALSE;
	}

	// Make sure we've been loaded correctly!
	if ( m_bFontInfoLoaded == FALSE )
	{
		return FALSE;
	}

	while ( *wszInputText )
	{
		TCHAR c = *wszInputText++;
		FLOAT fScl = fTextSize > 1.0f ? fTextSize / m_aFontCoords[fsSet].fHeight : 1.0f;

		// Check for newline case.
		// If we find a newline, reset current width and continue.
		if ( c == _T('\n') )
		{
			fCurrentWidth = 0.0f;
			wszOutputText[dwOutputIndex++] = c;
			continue;
		}

		// Add the character to the width if it isn't a space.
		if ( c != _T(' ') )
		{
			fCurrentWidth += m_aFontCoords[fsSet+c-32].fWidth * fScl;
		}

		// If we've gone over our limit, emit a newline,
		// increment the output index, reset, and continue.
		if ( fCurrentWidth >= fMaxWidth )
		{
			wszOutputText[dwOutputIndex++] = '\n';
			fCurrentWidth = 0.0f;
			continue;
		}

		wszOutputText[dwOutputIndex++] = c;

		// If we reach a space, we have to look ahead to see
		// if the word will fit in our string.  If not, we either
		// have to put the word on the next line, or chop the word
		// at some point.
		if ( c == _T(' ') )
		{
			FLOAT fWordWidth = 0.0f;
			const TCHAR *theWord = wszInputText;

			while ( *theWord )
			{
				TCHAR c = *theWord++;
				

				fWordWidth += m_aFontCoords[fsSet+c-32].fWidth * fScl;
				
				// Check if word length is over limit.  If it is,
				// we emit a newline, reset, and break out of the
				// word loop.
				if ( (fWordWidth+fCurrentWidth) >= fMaxWidth )
				{
					wszOutputText[dwOutputIndex++] = '\n';
					fCurrentWidth = 0.0f;
					break;
				}

				// We've found the end of the word, and it
				// fits in, so lets copy it into our output
				// buffer.
				if ( c == _T(' ') || c == _T('\n') )
				{
					DWORD dwWordLen = theWord-wszInputText-1;

					wszOutputText[dwOutputIndex] = NULL;
					_tcsncat( wszOutputText, wszInputText, dwWordLen );
					dwOutputIndex += dwWordLen;
					wszInputText += dwWordLen;
					
					if ( c == _T(' ') )
					{
						fCurrentWidth += fWordWidth;
					}
					else
					{
						fCurrentWidth = 0.0f;
					}
					
					break;
				}
			}
		}
	}
	
	// Put a NULL terminator on our output string.
	wszOutputText[dwOutputIndex] = NULL;
	return TRUE;
}

BOOL CD3DMGEng::UTIL_GetTextExtent ( const TCHAR* strText, SIZE* pSize, FLOAT fTextSize, FONTSET fsSet )
{
    if( NULL == strText || NULL == pSize )
	{
		return FALSE;
	}

	// Make sure we've been loaded correctly!
	if ( m_bFontInfoLoaded == FALSE )
	{
		return FALSE;
	}

	FLOAT fRowWidth  = 0.0f;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = m_aFontCoords[fsSet].fHeight;
	FLOAT fScl = fTextSize > 1.0f ? fTextSize / fHeight : 1.0f;
	
	fHeight *= fScl;
    while( *strText )
    {
        TCHAR c = *strText++;  

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += m_aFontCoords[fsSet].fHeight * fScl;
        }

        if( c < _T(' ') )
		{
            continue;
		}
		

        fRowWidth += m_aFontCoords[fsSet+c-32].fWidth * fScl;


        if( fRowWidth > fWidth )
		{
            fWidth = fRowWidth;
		}
	}

    pSize->cx = (int)fWidth;
	pSize->cy = (int)fHeight;

	return TRUE;
}