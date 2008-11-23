/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/d3d/CD3DMGE_D3D9.cpp
*  PURPOSE:		Interface for rendering Direct3D 9 primitives/sprites
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "include/cd3dmge_d3d9.h"

inline D3DSPRITEVERTEX2D InitFont2DVertex( FLOAT x, FLOAT y, FLOAT z, FLOAT rhw, DWORD dwColor, FLOAT tu, FLOAT tv )
{
    D3DSPRITEVERTEX2D v = { x, y, z, rhw, dwColor, tu, tv };
    return v;
}

inline D3DSPRITEVERTEX3D InitFont3DVertex( FLOAT x, FLOAT y, FLOAT z, DWORD dwColor, FLOAT tu, FLOAT tv)
{
    D3DSPRITEVERTEX3D v = { x, y, z, dwColor, tu, tv };
    return v;
}

inline D3DSPRITEVERTEX3D InitBox3DVertex( FLOAT x, FLOAT y, FLOAT z, DWORD dwColor )
{
    D3DSPRITEVERTEX3D v = { x, y, z, dwColor };
    return v;
}

CD3DMGEng::CD3DMGEng ( LPDIRECT3DDEVICE9 pD3DDevice )
{
	m_pDevice = pD3DDevice;
		
	m_pFontTexture = NULL;
	m_fFontTexWidth = 0.0f;
	m_fFontTexHeight = 0.0f;
	m_pVB = NULL;
	m_bFontInfoLoaded = FALSE;
    m_pDeviceState = NULL;
    m_pLastFont = NULL;
    memset( &m_dLastFontDesc, 0, sizeof ( D3DXFONT_DESC ) );

	memset(m_aFontCoords,0,sizeof(m_aFontCoords));
}

VOID CD3DMGEng::OnRestoreDevice ( void )
{

	m_pDevice->CreateVertexBuffer( D3DSPRITE_NUMVERTS*sizeof(D3DSPRITEVERTEX2D),  //sizeof(2D) > sizeof(3D)
								   D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								   0,
								   D3DPOOL_DEFAULT,
								   &m_pVB,
								   0 );

    assert ( m_pLastFont == NULL );

    D3DXCreateFontIndirect ( m_pDevice,&m_dLastFontDesc,&m_pLastFont ); 
}

VOID CD3DMGEng::OnDeleteDevice ( void )
{
    if ( m_pLastFont != NULL )
    {
        m_pLastFont->Release(); //release font
        m_pLastFont = NULL;
    }
}

VOID CD3DMGEng::OnInvalidateDevice ( void )
{
	SAFE_RELEASE( m_pVB )

    if ( m_pLastFont != NULL )
    {
        m_pLastFont->Release(); //release font
        m_pLastFont = NULL;
    }
}

BOOL CD3DMGEng::Render2DSprite ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColor )
{
	return Render2DSpriteEx( pTexture, pScaling, pTranslation, dwColor, dwColor, dwColor, dwColor );
}

BOOL CD3DMGEng::Render2DSpriteEx ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR )
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
	float tx, ty;
	if ( pTexture ) {
		pTexture->GetLevelDesc( 0, &myDesc );
		tx = static_cast<float>(myDesc.Width);
		ty = static_cast<float>(myDesc.Height);
	} else {
		tx = 1.0f;
		ty = 1.0f;
	}

	
	///////////////////////////////////////////////////
	// Scale the image if necessary.
	if ( pScaling != NULL )
	{
		tx *= pScaling->x;
		ty *= pScaling->y;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
    m_pDevice->SetFVF( D3DFVF_SPRITEVERTEX2DTEX );
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX2D) );
	if ( pTexture )
		m_pDevice->SetTexture( 0, pTexture );


	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
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
	if ( pTexture )
		m_pDevice->SetTexture( 0, NULL );

	return TRUE;
}

BOOL CD3DMGEng::Render3DSprite ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColor )
{
	return Render3DSpriteEx( pTexture, fScale, pTranslation, pMatView, dwColor, dwColor, dwColor, dwColor );
}

BOOL CD3DMGEng::DrawLine ( FLOAT fX1, FLOAT fY1, FLOAT fZ1, FLOAT fX2, FLOAT fY2, FLOAT fZ2, DWORD dwColor )
{
    m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    //m_pDevice->SetRenderState ( D3DRS_ZENABLE, FALSE );
    //m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	D3DLVERTEX lineList[2];
    
	////////////////////////////////////////////////////
	// Make sure we have a valid vertex buffer.
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&lineList, 0 );
	{
        lineList[0].x=fX1;
        lineList[0].y=fY1;
        lineList[0].z=fZ1;
        lineList[0].color=dwColor;
        lineList[0].specular=dwColor;

        lineList[1].x=fX2;
        lineList[1].y=fY2;
        lineList[1].z=fZ2;
        lineList[1].color=dwColor;
        lineList[1].specular=dwColor;
	}
	m_pVB->Unlock();

    m_pDevice->SetFVF(D3DFVF_LVERTEX);
    
	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitiveUP ( D3DPT_LINESTRIP, 2, lineList, sizeof(lineList)/2 );
    return TRUE;
}

BOOL CD3DMGEng::BeginDrawing ()
{
	if ( m_pVB == NULL )
	{
		return FALSE;
	}

	// Set our own states...
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

	return TRUE;
}

BOOL CD3DMGEng::EndDrawing ()
{
	m_pDevice->SetTexture( 0, NULL );

	return TRUE;
}

BOOL CD3DMGEng::DrawText3D ( FLOAT x, FLOAT y, FLOAT z, D3DMATRIX * pMatView, DWORD dwColor, const TCHAR *wszText,  FLOAT fTextSize, FONTFLAGS ffFlags, FONTSET fsSet )
{
	D3DSPRITEVERTEX3D * pVertices = NULL;
    D3DXMATRIX          MatWorld, MatView, MatTranslation, MatTransposed;
	DWORD               dwTriangleCount = 0;
	FLOAT               fStartX = 0;

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
    // Convert our view matrix from D3DMATRIX to D3DXMATRIX
    MatView = *pMatView;
    D3DXMatrixTranspose ( &MatTransposed, &MatView );
    
    // Create an Inverse matrix out of the view matrix (go from view to world space).
    MatTransposed._41 = MatTransposed._42 = 
        MatTransposed._43 = MatTransposed._14 = 
        MatTransposed._24 = MatTransposed._34 = 0.0f;

    // Create an empty identity matrix.
    D3DXMatrixIdentity ( &MatWorld );

    // Add in our scaling here.
    FLOAT fSize = fTextSize / 100.0f;
    D3DXMatrixScaling ( &MatWorld, fSize, fSize, fSize );

    // Set position of text in world.
    D3DXMatrixTranslation ( &MatTranslation, x, y, z );

    // Apply settings to new world matrix.
    D3DXMatrixMultiply ( &MatWorld, &MatWorld, &MatTransposed );
    D3DXMatrixMultiply ( &MatWorld, &MatWorld, &MatTranslation );

    m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3DTEX );
    m_pDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
	m_pDevice->SetTexture ( 0, m_pFontTexture );
    m_pDevice->SetTransform ( D3DTS_WORLD, &MatWorld );

    x = 0;
    y = 0;

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
			x += sz.cx;
			break;
		case D3DFF_LEFT:
			//x += sz.cx/2;
			break;
		default:
			break;
		}
	} 
		
	
    fStartX = x;
	m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
	while ( *wszText )
	{
		TCHAR c = *wszText++;

		if ( c == _T('\n') )
		{
			x = fStartX;
			y += m_aFontCoords[fsSet].fHeight;
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

        if ( c == _T(' ') )
		{
			x += w;
			continue;
		}

        // First triangle  (bottom right)
        *pVertices++ = InitFont3DVertex ( x+w, y+h, 0.0f, dwColor, tx2, ty1 ); //bottom right.
        *pVertices++ = InitFont3DVertex ( x+w,   y, 0.0f, dwColor, tx2, ty2 ); //top right.
        *pVertices++ = InitFont3DVertex ( x,   y+h, 0.0f, dwColor, tx1, ty1 ); //bottom left.

        // Second triangle (upper left)
        *pVertices++ = InitFont3DVertex ( x,     y, 0.0f, dwColor, tx1, ty2 ); //top left.
        *pVertices++ = InitFont3DVertex ( x+w,   y, 0.0f, dwColor, tx2, ty2 ); //top right.
        *pVertices++ = InitFont3DVertex ( x,   y+h, 0.0f, dwColor, tx1, ty1 ); //bottom right. 
                                            
		x += w;
		dwTriangleCount += 2;

		////////////////////////////////////////////////////
		// If we've gone over our limit, draw what we have.
		if ( dwTriangleCount*3 >= D3DSPRITE_NUMVERTS-6 )
		{
			m_pVB->Unlock();
			m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwTriangleCount );
			m_pVB->Lock( 0, 0, (void**)&(pVertices=NULL), D3DLOCK_DISCARD );
			dwTriangleCount = 0L;
		}
	}
	m_pVB->Unlock();


	////////////////////////////////////////////////////
	// Draw if we have anything.
	if ( dwTriangleCount > 0 )
	{
		m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwTriangleCount );
	}
	return TRUE;
}

BOOL CD3DMGEng::Render3DSpriteEx ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR ) 
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
	

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3DTEX );
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
	m_pDevice->SetTexture( 0, pTexture );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
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

    m_pDevice->SetTexture( 0, NULL );

	return TRUE;
}

BOOL CD3DMGEng::Render3DTriangle ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorB ) 
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
	

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3DTEX );
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
	m_pDevice->SetTexture( 0, pTexture );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	{
		pVertices[0].fX =		x-rightVect.x+upVect.x;
		pVertices[0].fY =		y-rightVect.y+upVect.y;
		pVertices[0].fZ =		z-rightVect.z-upVect.z;
		pVertices[0].tu =		1.0f;
		pVertices[0].tv =		1.0f;
		pVertices[0].dwColor =	dwColorB;  //BOTTOM MIDDLE

		pVertices[1].fX =		x+upVect.x+rightVect.x;
		pVertices[1].fY =		y+upVect.y+rightVect.y;
		pVertices[1].fZ =		z+upVect.z+rightVect.z;
		pVertices[1].tu =		1.0f;
		pVertices[1].tv =		0.0f;
		pVertices[1].dwColor =	dwColorTR;  //TOP RIGHT

		pVertices[2].fX =		x+rightVect.x-upVect.x;
		pVertices[2].fY =		y+rightVect.y-upVect.y;
		pVertices[2].fZ =		z+rightVect.z-upVect.z;
		pVertices[2].tu =		0.0f;
		pVertices[2].tv =		0.0f;
		pVertices[2].dwColor =	dwColorTL;  //TOP LEFT
	}
	m_pVB->Unlock();

	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

    m_pDevice->SetTexture( 0, NULL );

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
//	m_pDevice->CaptureStateBlock( m_SavedStateBlock );
//	m_pDevice->ApplyStateBlock( m_RenderStateBlock );
    m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX2D );
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX2D) );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
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

	return TRUE;
}

BOOL CD3DMGEng::DrawPlane3D ( D3DXVECTOR3 a, D3DXVECTOR3 b, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor )
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
	m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3DTEX );
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
	m_pDevice->SetTexture( 0, pTexture );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	{
		pVertices[0].fX =		a[0];
		pVertices[0].fY =		a[1];
		pVertices[0].fZ =		a[2];
		pVertices[0].tu =		1.0f;
		pVertices[0].tv =		1.0f;
		pVertices[0].dwColor =	dwColor;

		pVertices[1].fX =		a[0];
		pVertices[1].fY =		b[1];
		pVertices[1].fZ =		a[2];
		pVertices[1].tu =		1.0f;
		pVertices[1].tv =		0.0f;
		pVertices[1].dwColor =	dwColor;

		pVertices[2].fX =		b[0];
		pVertices[2].fY =		a[1];
		pVertices[2].fZ =		b[2];
		pVertices[2].tu =		0.0f;
		pVertices[2].tv =		1.0f;
		pVertices[2].dwColor =	dwColor;

		pVertices[3].fX =		b[0];
		pVertices[3].fY =		b[1];
		pVertices[3].fZ =		b[2];
		pVertices[3].tu =		0.0f;
		pVertices[3].tv =		0.0f;
		pVertices[3].dwColor =	dwColor;
	}
	m_pVB->Unlock();

	m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	/*
	m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_CLIPPLANEENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	*/


	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	m_pDevice->SetTexture( 0, NULL );

	return TRUE;
}

BOOL CD3DMGEng::DrawLine3D ( FLOAT ax, FLOAT ay, FLOAT az, FLOAT bx, FLOAT by, FLOAT bz, FLOAT w, FLOAT h, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor )
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
	m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3DTEX );
	m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
	m_pDevice->SetTexture( 0, pTexture );

	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	{
		pVertices[0].fX =		ax;
		pVertices[0].fY =		ay;
		pVertices[0].fZ =		az;
		pVertices[0].tu =		0.0f;
		pVertices[0].tv =		0.0f;
		pVertices[0].dwColor =	dwColor;

		pVertices[1].fX =		ax + w;
		pVertices[1].fY =		ay + w;
		pVertices[1].fZ =		az + h;
		pVertices[1].tu =		0.0f;
		pVertices[1].tv =		1.0f;
		pVertices[1].dwColor =	dwColor;

		pVertices[2].fX =		bx;
		pVertices[2].fY =		by;
		pVertices[2].fZ =		bz;
		pVertices[2].tu =		1.0f;
		pVertices[2].tv =		0.0f;
		pVertices[2].dwColor =	dwColor;

		pVertices[3].fX =		bx + w;
		pVertices[3].fY =		by + w;
		pVertices[3].fZ =		bz + h;
		pVertices[3].tu =		1.0f;
		pVertices[3].tv =		1.0f;
		pVertices[3].dwColor =	dwColor;
	}
	m_pVB->Unlock();

	m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
	/*
	m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_CLIPPLANEENABLE, FALSE );
	m_pDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	*/


	////////////////////////////////////////////////////
	// Draw!
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	m_pDevice->SetTexture( 0, NULL );

	return TRUE;
}

BOOL CD3DMGEng::Render3DBox ( FLOAT x, FLOAT y, FLOAT z, FLOAT fL, FLOAT fW, FLOAT fH, DWORD dwColor, BOOL bWireframe )
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
    m_pDevice->SetFVF ( D3DFVF_SPRITEVERTEX3D );

	if ( bWireframe )
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX3D) );
    m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	FLOAT w = fW/2;
	FLOAT h = fH/2;
	FLOAT d = fL/2;
	//////////////////////////////////////////////////
	// Lock the vertex buffer and copy in the verts.
	m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
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

	//if ( (pTypeWidths = static_cast<PBYTE>(VirtualAlloc( NULL,D3DFE_MAXFONTWIDTHS,MEM_COMMIT,PAGE_READWRITE ))) == NULL ) 
	if ( (pTypeWidths = static_cast<PBYTE>(VirtualAlloc( NULL,256 + 17,MEM_COMMIT,PAGE_READWRITE ))) == NULL ) 
	{
		return FALSE;
	}

    //ReadFile(hFile, pTypeWidths, D3DFE_MAXFONTWIDTHS, &dwSizeRead, NULL);
	ReadFile(hFile, pTypeWidths, 256 + 17, &dwSizeRead, NULL);
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
			FLOAT fCharWidth = static_cast<FLOAT>(pTypeWidths[17 + 32 + i]) - 2;
			
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
	VirtualFree( static_cast<PVOID>(pTypeWidths), 0, MEM_RELEASE );

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

VOID CD3DMGEng::DrawTextTTF ( int left, int top, int right, int bottom,  DWORD dwColor, const TCHAR *wszText, INT iTextHeight, INT iTextWeight, DWORD dwFormat, CHAR * szFaceName )
{
    if ( m_dLastFontDesc.Height != iTextHeight || m_dLastFontDesc.Weight != iTextWeight || strcmp(m_dLastFontDesc.FaceName, szFaceName) != 0 || m_pLastFont == NULL )
    {
        m_dLastFontDesc.Height = iTextHeight;
        m_dLastFontDesc.Width = 0;
        m_dLastFontDesc.Weight = iTextWeight;
        m_dLastFontDesc.MipLevels = 0;
        m_dLastFontDesc.Italic = false;
        m_dLastFontDesc.CharSet = DEFAULT_CHARSET;
        m_dLastFontDesc.OutputPrecision = OUT_TT_PRECIS;
        m_dLastFontDesc.Quality = PROOF_QUALITY;
        m_dLastFontDesc.PitchAndFamily = DEFAULT_PITCH;
        
        strcpy(m_dLastFontDesc.FaceName, szFaceName);

        if ( m_pLastFont != NULL )
        {
            m_pLastFont->Release(); //release font
            m_pLastFont = NULL;
        }
        D3DXCreateFontIndirect(m_pDevice,&m_dLastFontDesc,&m_pLastFont);
    }

    RECT FontPosition;

    if ( right == left )
        DWORD right = CDirect3DData::GetSingleton().GetViewportWidth();

    if ( bottom == top )
        DWORD bottom = CDirect3DData::GetSingleton().GetViewportHeight();

    FontPosition.left = left;
    FontPosition.top = top;
    FontPosition.right = right;
    FontPosition.bottom = bottom;
    
    m_pLastFont->DrawText(NULL,
                    wszText,
                    -1,
                    &FontPosition,
                    dwFormat | DT_WORDBREAK,
                    dwColor); //draw text
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
    m_pDevice->SetFVF( D3DFVF_SPRITEVERTEX2DTEX );
    m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(D3DSPRITEVERTEX2D) );
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
			x += sz.cx;
			break;
		case D3DFF_LEFT:
		default:
			break;
		}
	} 
		
	
	DWORD dwTriangleCount = 0;
	const FLOAT fStartX = x;
	m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
	while ( *wszText )
	{
		TCHAR c = *wszText++;
		FLOAT fScl = fTextSize;

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
			m_pVB->Lock( 0, 0, (void**)&(pVertices=NULL), D3DLOCK_DISCARD );
			dwTriangleCount = 0L;
		}

	}
	m_pVB->Unlock();


	////////////////////////////////////////////////////
	// Draw if we have anything.
	if ( dwTriangleCount > 0 )
	{
		m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwTriangleCount );
	}

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
		FLOAT fScl = fTextSize;

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

	FLOAT fScl       = fTextSize;
    FLOAT fRowWidth  = 0.0f;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = m_aFontCoords[fsSet].fHeight*fScl;

    while( *strText )
    {
        TCHAR c = *strText++;  

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += m_aFontCoords[fsSet].fHeight*fScl;
			continue;
        }

        if( c < _T(' ') )
		{
            continue;
		}
		

        fRowWidth += m_aFontCoords[fsSet+c-32].fWidth*fScl;


        if( fRowWidth > fWidth )
		{
            fWidth = fRowWidth;
		}
	}

    pSize->cx = (int)fWidth;
	pSize->cy = (int)fHeight;

	return TRUE;
}

VOID CD3DMGEng::UTIL_CaptureDeviceState ( )
{
    if ( m_pDeviceState == NULL )
    {
        m_pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pDeviceState );
    }
}

VOID CD3DMGEng::UTIL_RestoreDeviceState ( )
{
    if ( m_pDeviceState )
    {
        m_pDeviceState->Apply ( );
        m_pDeviceState->Release ( );
        m_pDeviceState = NULL;
    }
}