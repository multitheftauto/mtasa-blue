/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainMenuScene.cpp
*  PURPOSE:     Direct3D main menu and credits scene rendering
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#include <libv2/libv2.h>
#include <libv2/song.h>

extern CCore* g_pCore;

template<> CMainMenuScene * CSingleton< CMainMenuScene >::m_pSingleton = NULL;

#define CHECK_REF(x,y) if(x){x->AddRef();y=x->Release();assert(y>=1);}

// Frame limiter
#define CORE_MTA_SCENE_FRAME_CAP			45		// In milliseconds
#define CORE_MTA_SCENE_FRAME_TICK			45

// Data file definitions
#define CORE_MTA_VIDEO						"data\\menu.mkv"
#define CORE_MTA_MASK						"data\\mask.png"
#define CORE_MTA_ROLL						"data\\roll.png"
#define CORE_MTA_FALLBACK					"data\\fallback.png"
#define CORE_MTA_TEXTURE_PATH				"data\\textures\\%s"
#define CORE_MTA_VSH						"data\\vertex.vsh"
#define CORE_MTA_PSH						"data\\pixel.psh"
#define CORE_MTA_SCENE						"data\\scene.x"
#define CORE_MTA_CREDITS					"data\\%u.png"

#define CORE_MTA_ROLL_WIDTH_POW2			1024	// Real (power-of-two compatible) width
#define CORE_MTA_ROLL_HEIGHT_POW2			1024
#define CORE_MTA_ROLL_BOTTOM				800		// Height padded to the bottom to delay fading
#define CORE_MTA_ROLL_WIDTH					800		// Roll native resolution
#define CORE_MTA_ROLL_HEIGHT				600

// Time definitions (in REFERENCE_TIME format, 100ns)
#define CORE_MTA_VIDEO_LOOP_START			0//50000000
#define CORE_MTA_VIDEO_LOOP_END				500000000

// Time in nanoseconds at which we turn each of the lights on
#define CORE_MTA_VIDEO_LIGHTA				45510000
#define CORE_MTA_VIDEO_LIGHTB				57580000
#define CORE_MTA_VIDEO_LIGHTC				65940000
#define CORE_MTA_VIDEO_LIGHTD				79110000

// Sigmoid S-curve phase points (-6 to 6)
#define SIGMOID_START_CUT					-4						// Cuts off some of the units to speed up the incrementation
#define SIGMOID_START_CUT2					-2						// Cuts off even more
#define SIGMOID_START						-6						// Sigmoid curve start value (t)
#define SIGMOID_END							6						// Sigmoid curve end value (t)
#define SIGMOID_SPAN						12

// Scene animation properties
#define SPIN_SPEED							0.1f					// The speed at which the camera will be spinning
#define CREDIT_FOCUS_TIME					60						// Time (in frames) the credits have to be focused on

// Mask defines
#define MASK_WIDTH							1024					// Mask bitmap width
#define MASK_HEIGHT							512						// Mask bitmap height
#define MASK_SQUARES_X						16						// Number of squares in a row
#define MASK_SQUARES_Y						7						// Number of squares in a column
#define MASK_PADDING_X						4						// Position in pixels (x) at which the square content actually starts
#define MASK_PADDING_Y						4						// Position in pixels (y)

// Credit stack defines
#define NAME_MAX_LENGTH						32						// Maximum length of a name
#define MASK_CREDITS						32						// Number of credits to show on the mask
#define MASK_CREDIT_STACK					MASK_SQUARES_Y			// Number of credits to show simultaneously
#define MASK_CREDIT_FRAMECOUNT				325						// Duration of one credit cycle in frames
#define MASK_THETA_PER_SQUARE				10						// (1/x) Amount of difference in fTheta to scroll one square
#define MASK_CREDIT_MAX_LENGTH_IN_SQUARES	4						// Maximum amount of squares a credit item can occupy
#define	MASK_CREDITS_ALIGN(x)				(unsigned int)(x*(55))%MASK_SQUARES_X
																	// Macro to align the squares in a somewhat random, predefined but relative fashion

// Sigmoid curve related
#define SIGMOID_TRANSITION_PHASE			2.085f					// The length of one complete sigmoid phase
#define SIGMOID_TRANSITION_PRESWITCH		1.90f					// Value at which we start fading in (into a new stage)
#define SIGMOID_TRANSITION_POSTSWITCH		0.2f					// Value at which we stop fading out
#define SIGMOID_TRANSITION_SWITCH			2.0608f					// Value at which we actually switch stages (exact phase value is never reached)

// Miscellaneous
#define INVALID					255			// Number that marks an invalid item

// DEBUG START
DWORD dwTimerStart = 0;
DWORD dwTimerCurrent = 0;
// DEBUG END

bool bEnablePostProcessing = true;
bool bEnableVideo = true;
bool bInitialized = false;

bool bSafe = false;
bool bCredits = true;

DWORD dwTickLast = 0;

FLOAT fInc = 1;

FLOAT fLightDimA = 0;
FLOAT fLightDimB = 0;
FLOAT fLightDimC = 0;
FLOAT fLightDimD = 0;
FLOAT fBlend = 1.0f;
FLOAT fSplash = 0;
FLOAT fSplashAlpha = 0;
FLOAT fSplashTheta = 0;
FLOAT fSigmoidOffset = 0;

BOOL bShowScene = true;				// Show the scene?
BOOL bShowAll = true;				// Show the scene and the video cylinder?
BOOL bShowRoll = false;				// Show the vertical credits roll?
BOOL bFade = false;					// Black fading active?

DWORD dwFrameCount = 0;
DWORD dwFrameStart = 0;
DWORD dwFrameMid = 0;
DWORD dwFrameStop = 0;

D3DXVECTOR3 vecEyePt, vecLookatPt, vecUpPt;

// Structure for a credit stack item
struct SCreditStackItem
{
	unsigned short frames;
	unsigned short cnt;
	unsigned char j;
	unsigned short i;
	unsigned short x;
	unsigned short y;
	unsigned int mask;
};

// Structure for our custom vertex type
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;
    FLOAT       tu, tv;
};

// Vertex element declaration
const D3DVERTEXELEMENT9 decl[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0,  12, D3DDECLTYPE_FLOAT2,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0,  28, D3DDECLTYPE_FLOAT3,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	D3DDECL_END()
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

// Direct3D resources
IDirect3DTexture9 *				pTexRoll = NULL;
IDirect3DTexture9 *				pTexCGUI = NULL;
IDirect3DVertexBuffer9 *		pVBProjection = NULL;
IDirect3DTexture9 *				pMaskTexture = NULL;
IDirect3DTexture9 *				pFallbackTexture = NULL;

IDirect3DTexture9 *				pTexMaskAnim = NULL;
IDirect3DTexture9 *				pTexMask = NULL;
IDirect3DSurface9 *				pDepthNew = NULL;				// NVidia Compatibility hack!

IDirect3DVertexShader9 *		pVSHMask = NULL;
IDirect3DVertexShader9 *		pVSHLighting = NULL;

IDirect3DPixelShader9 *			pPSHMask = NULL;
IDirect3DPixelShader9 *			pPSHLighting = NULL;
IDirect3DPixelShader9 *			pPSHBloom = NULL;

IDirect3DVertexDeclaration9 *	pVSHDeclMask = NULL;
IDirect3DVertexDeclaration9 *	pVSHDeclLighting = NULL;

ID3DXConstantTable *			pVSHConstMask = NULL;
ID3DXConstantTable *			pVSHConstLighting = NULL;
ID3DXConstantTable *			pPSHConstMask = NULL;
ID3DXConstantTable *			pPSHConstLighting = NULL;
ID3DXConstantTable *			pPSHConstBloom = NULL;

ID3DXMesh *						pMesh = NULL;
D3DMATERIAL9 *					pMeshMaterials = NULL;
LPDIRECT3DTEXTURE9 *			pMeshTextures = NULL;
bool *							pbMeshTextures = NULL;

ID3DXSprite *					pScreen = NULL;
IDirect3DTexture9 *				pTexBloomSrc = NULL;
IDirect3DTexture9 *				pTexBloomTgt = NULL;
IDirect3DTexture9 *				pTexMonitor = NULL;

unsigned short					usTexBloomSrcWidth = 0, usTexBloomSrcHeight = 0;
unsigned short					usTexBloomTgtWidth = 0, usTexBloomTgtHeight = 0;
unsigned short					usTexMonitorWidth = 0, usTexMonitorHeight = 0;

DWORD							dwMaterials = 0;

float							fTheta = 0;							// Controls the base rotation and movement
float							fLight = 0;							// Controls the lighting strobe
float							fUp = 0.25f;						// Determines the initial Y (upwards) position
float							fZoom = SIGMOID_START_CUT;			// Sigmoid curve parameter used for movement
float							fFade = SIGMOID_START;				// Sigmoid curve parameter used for fading in/out

// Roll sizing variables
float							fRollScaleX = 1;					// Scaling relative to the native resolution
float							fRollScaleY = 1;
int								iRollOffset = 0;					// Horizontal positioning offset

// State enumeration
enum STATES {
	STATE_WAIT = -1,
	STATE_TO_CJ = 0,
	STATE_TO_CLAUDE = 1,
	STATE_TO_TOMMY = 2,
	STATE_TO_WALL = 3,
	STATE_SCROLL = 4,
	STATE_END = 5,
	STATE_CREDIT_FOCUS = 253,
	STATE_MAINMENU = 254
};

// State and credit related variables
short							sState			= STATE_MAINMENU;	// Current state we are at
short							sSubState		= INVALID;
unsigned char					ucLastState		= INVALID;			// Previous state we were at
unsigned char					ucCredit		= INVALID;			// Credit id currently being shown
unsigned char					ucCreditSkip	= INVALID;			// Credit id being ignored

// Monitor bezier transitions
D3DXVECTOR3						vecBTommyCJ[8];
D3DXVECTOR3						vecBCJClaude[8],	vecBCJClaudeLook[2];
D3DXVECTOR3						vecBFly[8],			vecBFlyLook[4];
D3DXVECTOR3						vecBWall[8],		vecBWallLook[4];
D3DXVECTOR3						vecBCreditEye[4];

// Monitor positioning vectors
D3DXVECTOR3						vecMTommy	= D3DXVECTOR3 ( -1.5f,		-0.4f,		0.2f );
D3DXVECTOR3						vecMClaude	= D3DXVECTOR3 ( 1.2f,		-0.245f,	0.025f );
D3DXVECTOR3						vecMCJ		= D3DXVECTOR3 ( 0.112f,		-0.370f,	-0.7f );

// Monitor normal vectors
D3DXVECTOR3						vecNTommy	= D3DXVECTOR3 ( -1.0f,	0,		0 );
D3DXVECTOR3						vecNClaude	= D3DXVECTOR3 ( 1.0f,	0.05f,	-0.03f );
D3DXVECTOR3						vecNCJ		= D3DXVECTOR3 ( 0,		-0.1f,	1.0f );

// World vectors
D3DXVECTOR3						vecUp		= D3DXVECTOR3 ( 0,		1,		0 );
D3DXVECTOR3						vecBUp		= D3DXVECTOR3 ( 0,		0.2f,	0 );

// Credit stack stuff
unsigned short					usFontDX = MASK_WIDTH / MASK_SQUARES_X;		// Width of one square in pixels
unsigned short					usFontDY = MASK_HEIGHT / MASK_SQUARES_Y;	// Height of one square in pixels
SCreditStackItem				pCreditsStack[MASK_CREDIT_STACK];			// Credit stack items
unsigned int					iCreditsSub = 0;							// Credit stack item id we are currently scrolling into the scene
char *							pCreditsSub[] = { "alienx", "arc_", "bb101", "boss", "brophy", "dragon", "driver2", "fedor", "iggy", "jbeta", "jhxp", "johnline", "joriz", "kcuf", "lil_toady", "lucif3r", "mack", "meanpanther", "nick", "norby89", "outback", "paradox", "raf", "ratt", "tvke", "slothman", "solide", "talidan", "tweak", "x16", "" };
//char *							pCreditsSub[] = { "solide", "talidan", "tweak", "vandalite", "x16", "xanarki", "xerox", "placeholder" };
unsigned int					iCreditsEnd = 0;							// Credit stack end item index
																			// Credit stack items text
// Credit sprite stuff
D3DXVECTOR3						vecCredits[10];								// Sprite placement vectors
D3DXVECTOR3						vecCreditsSpan[10];							// Sprite size/span vectors
IDirect3DTexture9 *				pCredits[10];								// Sprite textures
IDirect3DTexture9 *				pSplash;									// Splash texture

// Mask animation stuff
struct SMaskAnim {
	float fA, fB, fC, fD, fE, fF, fG, fH, fI;
	unsigned char ucType;
	unsigned char ucIndex;
	unsigned short usWait;
};

#define MASK_ANIMATIONS		5			// Number of simultaneous mask animation items
#define MASK_ITEMS			20			// Maximum number of items to pick from
#define MASK_DEFAULT_ITEMS	5			// Number of default screenshots
#define MASK_ANIM_WAIT		40			// Amount of frames to wait before fading out

SMaskAnim pMaskAnim[MASK_ANIMATIONS] = {0};
IDirect3DTexture9 * pMaskAnimTexture[MASK_ITEMS];
int iMaskAnimTextures = 0;
bool bMaskAnimSwitch = true;
float fMaskAnim = 0;

// Clamps a value between two other values ( min < x < max )
inline float Clamp( float min, float x, float max )
{
    if ( x < min ) return min; else
    if ( x > max ) return max;
    return x;
}

// Processes a bezier curve in a linear fashion as function of t (0 < t < 1)
inline void ProcessLinear ( D3DXVECTOR3& vecPos, D3DXVECTOR3* vecBezier, float fT )
{
	vecPos = (1.0f - fT) * vecBezier[0] + fT * vecBezier[1];
}

// Processes two combined cubic bezier curves as function of t (0 < t < 2)
inline void ProcessBezierDouble ( D3DXVECTOR3& vecPos, D3DXVECTOR3* vecBezier, float fT )
{
	unsigned char offset = 0;
	if ( fT > 1.0f ) {
		offset = 4;
		fT -= 1.0f;
	}

	vecPos =	(pow(1.0f - fT, 3) * vecBezier[offset+0]) +
				(3.0f*fT * pow(1.0f - fT, 2) * vecBezier[offset+1]) +
				(3.0f*pow(fT,2) * (1.0f - fT) * vecBezier[offset+2]) +
				(pow(fT,3) * vecBezier[offset+3]);
}

// Processes one cubic bezier curve as function of t (0 < t < 2)
inline void ProcessBezier ( D3DXVECTOR3& vecPos, D3DXVECTOR3* vecBezier, float fT )
{
	vecPos =	(pow(1.0f - fT, 3) * vecBezier[0]) +
				(3.0f*fT * pow(1.0f - fT, 2) * vecBezier[1]) +
				(3.0f*pow(fT,2) * (1.0f - fT) * vecBezier[2]) +
				(pow(fT,3) * vecBezier[3]);
}

unsigned char *szTune;

CMainMenuScene::CMainMenuScene ( CMainMenu * pMainMenu )
{
	m_pVideoRenderer = NULL;
	m_pGraphics = NULL;
	m_pDevice = NULL;
	m_pMainMenu = pMainMenu;
}


CMainMenuScene::~CMainMenuScene ( void )
{
	delete [] szTune;

	Destroy3DScene ();

	// Release the device
	SAFE_RELEASE ( m_pDevice );
}


void CMainMenuScene::PreStartCredits ( void )
{
	// Reset variables
	fSplash =        0;
	fSplashAlpha =   0;
	fSplashTheta =   0;
	fSigmoidOffset = 0;
	fBlend =         1.0f;

	bShowScene =     true;
	bShowAll =       true;
	bShowRoll =      false;

	fTheta =         0;
	fLight =         0;
	fZoom =          SIGMOID_START_CUT;
	fFade =          SIGMOID_START;

	ucLastState =    INVALID;
	ucCredit =       INVALID;
	ucCreditSkip =   INVALID;

	iCreditsSub =    0;

	bMaskAnimSwitch = true;

	memset ( pMaskAnim, 0, sizeof(pMaskAnim) );


	dwTimerStart = CClientTime::GetTime ();

	// Mute video
	// if ( m_pVideoRenderer ) {
	//	m_pVideoRenderer->Mute ();
	//}

	// Start in the wait state
	sState = STATE_WAIT;
	sSubState = INVALID;

	// Stop the video
	StopVideo ();

	// Enable music
//	ssInit ( s_ucSong, GetForegroundWindow () );
//	ssPlay ();
}


void CMainMenuScene::StartCredits ( void )
{
	// Switch to the first state
	sState++;
}


void CMainMenuScene::StopCredits ( void )
{
	// Start the credits
	PlayVideo ();

	// Disable music
//	ssStop ();
//	ssClose ();

	// Enable video audio
	//if ( m_pVideoRenderer ) {
	//	m_pVideoRenderer->Unmute ();
	//}

	// Reset variables
	fSplash =        0;
	fSplashAlpha =   0;
	fSplashTheta =   0;
	fSigmoidOffset = 0;
	fBlend =         1.0f;

	bShowScene =     true;
	bShowAll =       true;
	bShowRoll =      false;

	fTheta =         0;
	fLight =         0;
	fZoom =          SIGMOID_START_CUT;
	fFade =          SIGMOID_START;

	ucLastState =    INVALID;
	ucCredit =       INVALID;
	ucCreditSkip =   INVALID;

	iCreditsSub =    0;

	// Change the state
	sState = STATE_MAINMENU;
	sSubState = INVALID;

	// Bring up the main menu
	m_pMainMenu->Show ( false );
}


void CMainMenuScene::Destroy3DScene ( void )
{
// Destroy the video renderer
	if ( m_pVideoRenderer ) {
		CVideoManager::GetSingletonPtr ()->DestroyRenderer ( m_pVideoRenderer );
		m_pVideoRenderer = NULL;
	}

	// Destroy the (unmanaged) rendertarget textures we've used
	DestroyRenderTargets ();

	// Destroy the (managed) textures we've used
	SAFE_RELEASE ( pMaskTexture );
	SAFE_RELEASE ( pFallbackTexture );
	SAFE_RELEASE ( pSplash );
	SAFE_RELEASE ( pTexRoll );

	// Clear all the (managed) mask anim textures
	for ( unsigned int i = 0; i < MASK_ITEMS; i++ ) {
		SAFE_RELEASE ( pMaskAnimTexture[i] );
	}

	// Clear all the (managed) credit textures
	unsigned int j = sizeof ( pCredits ) / sizeof ( IDirect3DTexture9* );
	for ( unsigned int i = 0; i < j; i++ ) {
		SAFE_RELEASE ( pCredits[i] );
	}

	// Destroy the shaders and their resources
	DestroyShaders ();

	// Destroy the mesh and it's resources
	SAFE_RELEASE ( pMesh );
	SAFE_RELEASE ( pScreen );
	if ( pMeshMaterials ) {
		delete [] pMeshMaterials;
		pMeshMaterials = NULL;
	}
	if ( pMeshTextures ) {
		delete [] pMeshTextures;
		pMeshTextures = NULL;
	}
	if ( pbMeshTextures ) {
		delete [] pbMeshTextures;
		pbMeshTextures = NULL;
	}
}


bool CMainMenuScene::Init3DScene ( IDirect3DTexture9 * pRenderTarget, CVector2D vecScreenSize )
{
	CFilePathTranslator FileTranslator;
    string WorkingDirectory;
    char szCurDir [ MAX_PATH ];

	ID3DXBuffer * pMaterials = NULL;
	D3DXMATERIAL * pMaterialStore = NULL;

	bool bReturn = false;

	m_pGFX = CLocalGUI::GetSingleton ().GetRenderingLibrary ();

	// Randomize the seed
	srand ( GetTickCount () );

    // Set the current directory to the MTA dir so we can load files using a relative path
    FileTranslator.SetCurrentWorkingDirectory ( "MTA" );
    FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
    GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
    SetCurrentDirectory ( WorkingDirectory.c_str ( ) );

	do {
		CCore::GetSingleton ().GetConsole ()->Printf ( "Initializing vectors" );

		// Zero the credits stack
		memset ( pCreditsStack, 0, sizeof(pCreditsStack) );

		// Zero the mask animation stack
		memset ( pMaskAnim, 0, sizeof(pMaskAnim) );
		memset ( pMaskAnimTexture, 0, sizeof(pMaskAnimTexture) );

		// Determine the credits stack end item index
		iCreditsEnd = sizeof ( pCreditsSub ) / sizeof ( char* );

		// Normalize the normals
		D3DXVec3Normalize ( &vecNCJ, &vecNCJ );
		D3DXVec3Normalize ( &vecNClaude, &vecNClaude );
		D3DXVec3Normalize ( &vecNTommy, &vecNTommy );

		// Define the credit sprite positions
		vecCredits[0]		= D3DXVECTOR3 ( -1.6f,		-0.30f,		-0.1f );
		vecCreditsSpan[0]	= D3DXVECTOR3 ( 0,			0.30f,		0.30f );
		vecCredits[1]		= D3DXVECTOR3 ( 0.262f,		-0.45f,		-0.6f );
		vecCreditsSpan[1]	= D3DXVECTOR3 ( -0.30f,		0.30f,		0 );
		vecCredits[2]		= D3DXVECTOR3 ( 1.3f,		0,			-1.4f );
		vecCreditsSpan[2]	= D3DXVECTOR3 ( -0.30f,		0.30f,		0 );
		vecCredits[3]		= D3DXVECTOR3 ( 1.2f,		-0.3f,		0.15f );
		vecCreditsSpan[3]	= D3DXVECTOR3 ( 0,			0.30f,		-0.30f );
		vecCredits[4]		= D3DXVECTOR3 ( -1.24f,		-0.25f,		0.42f );
		vecCreditsSpan[4]	= D3DXVECTOR3 ( 0,			0.30f,		0.30f );
		vecCredits[5]		= D3DXVECTOR3 ( -0.60f,		-1.0f,		0.15f );
		vecCreditsSpan[5]	= D3DXVECTOR3 ( 0,			0.30f,		0.30f );
		vecCredits[6]		= D3DXVECTOR3 ( -0.74f,		-0.28f,		-1.30f );
		vecCreditsSpan[6]	= D3DXVECTOR3 ( -0.30f,		0.30f,		0 );
		vecCredits[7]		= D3DXVECTOR3 ( 0.7f,		0.52f,		-1.56f );
		vecCreditsSpan[7]	= D3DXVECTOR3 ( -0.30f,		0.30f,		0 );
		vecCredits[8]		= D3DXVECTOR3 ( -0.15f,		0.25f,		-1.50f );
		vecCreditsSpan[8]	= D3DXVECTOR3 ( -0.30f,		0.30f,		0 );
		vecCredits[9]		= D3DXVECTOR3 ( -0.85f,		-0.08f,		-1.00f );
		vecCreditsSpan[9]	= D3DXVECTOR3 ( 0,			0.30f,		0.30f );

		// Create the bezier transition curves
		vecBTommyCJ[0]		= vecMTommy + vecNTommy*1.0f					+ vecBUp*2.5f;
		vecBTommyCJ[1]		= vecMTommy + vecNTommy*0.3f + vecNCJ*0.2f		+ vecBUp*0.5f;
		vecBTommyCJ[2]		= vecBTommyCJ[1] - vecNCJ*1.0f					+ vecBUp*0.6f;
		vecBTommyCJ[3]		= vecBTommyCJ[2] - vecNTommy*0.5f				+ vecBUp*1.0f;
		vecBTommyCJ[4]		= vecBTommyCJ[3];
		vecBTommyCJ[5]		= vecBTommyCJ[3] - vecNTommy*0.5f				+ vecBUp*1.0f;
		vecBTommyCJ[6]		= vecMCJ - vecNCJ*0.75f							+ vecBUp*0.6f;
		vecBTommyCJ[7]		= vecMCJ;

		// Store the graphics subsystem and D3D device
		m_pGraphics = CGraphics::GetSingletonPtr ();
		m_pDevice = m_pGraphics->GetDevice ();

		// Store the screen size
		m_vecScreenSize = vecScreenSize;

		CCore::GetSingleton ().GetConsole ()->Printf ( "Checking for PS2.0" );

		// Check for PS 2.0 support, abort otherwise
		D3DCAPS9 Caps;
		m_pDevice->GetDeviceCaps ( &Caps );
		if ( Caps.PixelShaderVersion < D3DPS_VERSION ( 2, 0 ) )
			break;

		// Create a texture to be associated with the background
		pTexCGUI = pRenderTarget;

		// Initialize the video graph
		CCore::GetSingleton ().GetConsole ()->Printf ( "Initializing video renderer" );
		if ( !bEnableVideo || !InitVideo () ) {    // If the video init failed, or we disabled it
			// Video renderer couldn't initialize, so fall back to a static texture
			if ( D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_FALLBACK, &pFallbackTexture ) != S_OK ) {
				// Could not load the texture, so abort
				break;
			}

			// Update the config
			CMainConfig *pConfig = g_pCore->GetConfig ();
			pConfig->iMenuOptions = ( pConfig->iMenuOptions & ~CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED );
		}

		CCore::GetSingleton ().GetConsole ()->Printf ( "Creating textures" );

		// Roll texture
		if ( D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_ROLL, &pTexRoll ) != S_OK ) {
			// Could not load the texture, so abort
			break;
		}
		// Resize the roll texture based on the screen resolution, and determine the offset
		fRollScaleX = m_vecScreenSize.fX / CORE_MTA_ROLL_WIDTH_POW2;
		fRollScaleY = m_vecScreenSize.fY / CORE_MTA_ROLL_HEIGHT_POW2;
		//iRollOffset = (CORE_MTA_ROLL_WIDTH_POW2 - CORE_MTA_ROLL_WIDTH) * (m_vecScreenSize.fX/CORE_MTA_ROLL_WIDTH) * 0.5f;
		iRollOffset = 0;
 
		// Mask texture
		if ( D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_MASK, &pMaskTexture ) != S_OK ) {
			// Could not load the texture, so abort
			break;
		}

		// Mask animation textures
		unsigned int i_own = CScreenShot::GetScreenShots ();
		unsigned int i_num = i_own + MASK_DEFAULT_ITEMS;
		if ( i_num > MASK_ITEMS ) i_num = MASK_ITEMS;
		char szTexPath[MAX_PATH] = {0};

		// Grab them from the screenshot class (directory)
		for ( unsigned int i = 0; i < i_num; i++ ) {
			if ( i < i_own ) {
				CScreenShot::GetScreenShotPath ( i + 1, szTexPath, MAX_PATH );
			} else {
				// Add our default screenshots as well
				_snprintf ( szTexPath, MAX_PATH - 1, "data\\default%u.png", i - i_own );
			}
			D3DXCreateTextureFromFile ( m_pDevice, szTexPath, &pMaskAnimTexture[i] );
			iMaskAnimTextures++;
		}

		CCore::GetSingleton ().GetConsole ()->Printf ( "Creating meshes" );

		// Create the cylinder
		CreateCylinder ( m_pDevice );

		// Mesh
		if ( D3DXLoadMeshFromX ( CORE_MTA_SCENE, D3DXMESH_MANAGED, m_pDevice, NULL, &pMaterials, NULL, &dwMaterials, &pMesh ) != S_OK ) {
			// Could not load the mesh, so abort
			break;
		}
		pMaterialStore = reinterpret_cast < D3DXMATERIAL* > ( pMaterials->GetBufferPointer () );
		pMeshMaterials = new D3DMATERIAL9[dwMaterials];
		pMeshTextures = new LPDIRECT3DTEXTURE9[dwMaterials];
		pbMeshTextures = new bool[dwMaterials];

		// Copy the materials and textures
		for ( DWORD i = 0; i < dwMaterials; i++ ) {
			pMeshMaterials[i] = pMaterialStore[i].MatD3D;
			pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;
			pbMeshTextures[i] = false;

			// If there's no texture present for this material, use the plain white texture
			if ( pMaterialStore[i].pTextureFilename == NULL ) {
				pMaterialStore[i].pTextureFilename = "white32.png";
			}

			// Get the correct path
			char szPath[MAX_PATH] = {0};
			_snprintf ( &szPath[0], MAX_PATH-1, CORE_MTA_TEXTURE_PATH, pMaterialStore[i].pTextureFilename );
			
			// Create the texture, loading failure isn't fatal here (just a blank texture)
			if ( FAILED ( D3DXCreateTextureFromFile ( m_pDevice, szPath, &pMeshTextures[i] ) ) ) {
				CLogger::GetSingleton ().ErrorPrintf ( "Could not load mesh texture (%s)", szPath );
			}

			// Disable lighting for screen textures
			pbMeshTextures[i] = ( strstr ( pMaterialStore[i].pTextureFilename, "screen" ) != NULL );
		}
		pMaterials->Release ();

		CCore::GetSingleton ().GetConsole ()->Printf ( "Initializing shaders" );

		// Initialize the shaders
		if ( !InitShaders () ) break;

		CCore::GetSingleton ().GetConsole ()->Printf ( "Initializing render targets" );

		// Initialize the rendertarget textures
		if ( !InitRenderTargets () ) break;

		CCore::GetSingleton ().GetConsole ()->Printf ( "Initializing sprites" );

		// Initialize the credit sprites
		if ( !InitCreditSprites () ) break;

		// Create the screen sprite
		D3DXCreateSprite ( m_pDevice, &pScreen );

		// Function succeeded
		CCore::GetSingleton ().GetConsole ()->Printf ( "Successfully initialized scene" );
		bReturn = true;
	} while ( false );

	// Restore current directory
	SetCurrentDirectory ( szCurDir );

	bInitialized = true;

	return bReturn;
}


void CMainMenuScene::Draw3DScene ( void )
{
	LONGLONG VideoTimeCurrent, VideoTimeDuration;
	IDirect3DTexture9 * pVideoTexture = NULL;

	// Frame limiter
	DWORD dwTick = CClientTime::GetTime ();
	DWORD dwTickDiff = dwTick - dwTickLast;
	if ( dwTickDiff < CORE_MTA_SCENE_FRAME_CAP ) {
		Sleep ( CORE_MTA_SCENE_FRAME_CAP - dwTickDiff );
		dwTickDiff = CORE_MTA_SCENE_FRAME_CAP;
	}
	dwTickLast = dwTick;

	// Sound tick
//	ssDoTick ();

	// Calculate the increment rate, based on the native FPS, to use time-based animations
	fInc = (float)dwTickDiff / (float)CORE_MTA_SCENE_FRAME_TICK;

	// DEBUG START
	dwTimerCurrent = dwTick - dwTimerStart;
	char buf[128] = {0};
	_snprintf(buf,127,"inc rate: %.2f - time: %.2f\ncount: %u - (%u,%u,%u)",fInc,((float)dwTimerCurrent)/1000.0f,dwFrameCount,dwFrameStart,dwFrameMid,dwFrameStop);
	// DEBUG END

	// Check if we have a video renderer
	if ( m_pVideoRenderer ) {
		// Check if we need to loop the video (every 20 frames)
		m_pVideoRenderer->GetPositions ( VideoTimeCurrent, VideoTimeDuration );
		if ( VideoTimeCurrent >= CORE_MTA_VIDEO_LOOP_END ) {
			m_pVideoRenderer->SetPosition ( CORE_MTA_VIDEO_LOOP_START );
		}

		// Light sequence
		/*
		if ( VideoTimeCurrent >= CORE_MTA_VIDEO_LIGHTA )
			fLightDimA = 1.0f;
		if ( VideoTimeCurrent >= CORE_MTA_VIDEO_LIGHTB )
			fLightDimB = 1.0f;
		if ( VideoTimeCurrent >= CORE_MTA_VIDEO_LIGHTC )
			fLightDimC = 1.0f;
		if ( VideoTimeCurrent >= CORE_MTA_VIDEO_LIGHTD ) {
			fLightDimD = 1.0f;
		}
		*/

		// Lock the renderer and get the video texture
		m_pVideoRenderer->Lock ();
		pVideoTexture = m_pVideoRenderer->GetVideoTexture ();
	}

	// If we use a fallback texture, we don't need a light sequence
//	if ( pFallbackTexture ) {
		fLightDimA = fLightDimB = fLightDimC = fLightDimD = 1;
//	}

	if ( pTexCGUI && pTexBloomSrc ) {
		dwFrameCount++;

		// Get the CEGUI render texture and it's surface
		IDirect3DSurface9 *pD3DSurface, *pD3DBloomTarget, *pD3DBloomSource, *pD3DMonitor, *pD3DMask, *pD3DMaskAnim;
		pTexBloomSrc->GetSurfaceLevel ( 0, &pD3DBloomSource );
		pTexBloomTgt->GetSurfaceLevel ( 0, &pD3DBloomTarget );
		pTexCGUI->GetSurfaceLevel ( 0, &pD3DSurface );
		pTexMonitor->GetSurfaceLevel ( 0, &pD3DMonitor );
		pTexMask->GetSurfaceLevel ( 0, &pD3DMask );
		pTexMaskAnim->GetSurfaceLevel ( 0, &pD3DMaskAnim );

		m_pGraphics->SetRenderTarget ( pD3DSurface );
		m_pGraphics->BeginDrawing ();

		// Calculate the sigmoid S-curves
		float fSigmoid = SIGMOID_TRANSITION_PHASE / (1.0f + exp(-fZoom)) + fSigmoidOffset;			// 1/(1+e^-t)
		float fSigmoidSpin = abs(0.5f - (1.0f / (1.0f + exp(-fFade))));

		// Increment the other counters
		fLight += 0.03f * fInc;

		// Set up the camera
		float fDisplaceX = sinf ( fTheta ) * 2.5f;
		float fDisplaceZ = cosf ( fTheta ) * 2.5f;

		D3DXVECTOR3 vecMonitorEyePt		( fDisplaceX,	fUp,	-fDisplaceZ );
		D3DXVECTOR3 vecMonitorLookatPt	( -fDisplaceX,	fUp,	fDisplaceZ );

		/********************************************************************/
		/* SCENE PASS: VIDEO MASK                                           */
		/********************************************************************/
		// Force the depth buffer to a surface of the same size as the RTT we're rendering to
		// This allows RTT's that are bigger than the backbuffer on NVidia cards
		IDirect3DSurface9 * pDepth;
		m_pDevice->GetDepthStencilSurface ( &pDepth );
		m_pDevice->SetDepthStencilSurface ( NULL );

		m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE );
		m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );

		m_pDevice->SetVertexDeclaration ( NULL );
		m_pDevice->SetVertexShader ( NULL );
		m_pDevice->SetPixelShader ( NULL );

		m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, false );
		m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );

		m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		// Mask animation
		if ( sState != STATE_MAINMENU ) {
			m_pDevice->SetRenderTarget ( 0, pD3DMaskAnim );
			m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1,1,1,0), 1, 0 );

			srand(CClientTime::GetTime ());
			for ( unsigned int i = 0; i < MASK_ANIMATIONS; i++ ) {
				// If the slot is empty, fill it
				if ( pMaskAnim[i].ucType == 0 && bMaskAnimSwitch ) {
					bMaskAnimSwitch = false;

					// Start variables
					pMaskAnim[i].ucType = 1;
					pMaskAnim[i].ucIndex = rand() % iMaskAnimTextures;
					pMaskAnim[i].fA = rand() % (MASK_SQUARES_X/2);
					pMaskAnim[i].fB = rand() % (MASK_SQUARES_Y/2);
					pMaskAnim[i].fC = 0;
					pMaskAnim[i].fD = D3DX_PI / (30 + (rand() % 30));
					pMaskAnim[i].fE = Clamp ( 0.25f, ((float)(rand() % 100)) / 100.0f, 0.75f );
					pMaskAnim[i].fF = Clamp ( 0.4f, (rand() % 100)/100.0f, 1.0f );
					pMaskAnim[i].usWait = 0;

					pMaskAnim[i].fA *= usFontDX;
					pMaskAnim[i].fB *= usFontDY;
				}

				float fZ = Clamp ( 0, sinf ( pMaskAnim[i].fC ), pMaskAnim[i].fF );

				m_pGFX->Render2DSprite ( pMaskAnimTexture[pMaskAnim[i].ucIndex],
					&D3DXVECTOR2(pMaskAnim[i].fE,pMaskAnim[i].fE),
					&D3DXVECTOR2(MASK_PADDING_X + pMaskAnim[i].fA,MASK_PADDING_Y + pMaskAnim[i].fB),
					D3DXCOLOR ( 0, 0, 1, fZ ) );

				// Almost faded out
				if ( pMaskAnim[i].fC >= (D3DX_PI*0.9f) ) {
					bMaskAnimSwitch = true;
				}

				// Full phase: Faded out
				if ( pMaskAnim[i].fC >= D3DX_PI ) {
					pMaskAnim[i].ucType = 0;
					bMaskAnimSwitch = false;
				}
				// Half phase: Faded in
				else if ( pMaskAnim[i].fC >= (D3DX_PI*0.5f) && pMaskAnim[i].usWait < MASK_ANIM_WAIT  ) {
					pMaskAnim[i].usWait++;
				} else
					pMaskAnim[i].fC += pMaskAnim[i].fD;
			}
		}

		m_pDevice->SetRenderTarget ( 0, pD3DMask );
		m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
		{
			// Render the mask texture
			m_pGFX->Render2DSprite ( pMaskTexture, &D3DXVECTOR2(1,1), &D3DXVECTOR2(0,0), 0xFFFFFFFF );

			// Check that the state is STATE_SCROLL
			if ( sState == STATE_SCROLL ) {
				fInc *= 1.1f;

				// Calculate which horizontal square (based on the mask texture coordinates) is currently getting into the view
				unsigned int proj = MASK_SQUARES_X - (unsigned int)fmod(MASK_SQUARES_X + (abs(fTheta)*MASK_THETA_PER_SQUARE),MASK_SQUARES_X);

				// Loop through the credit stack to see if we have any new places to fill up
				unsigned int x;
				for ( unsigned int i = 0; i < MASK_CREDIT_STACK; i++ ) {
					// Use the function MASK_CREDITS_ALIGN(id) to determine on what horizontal square this particular credit item should go
					x = MASK_CREDITS_ALIGN(i);

					//
					if ( proj == x && pCreditsStack[i].frames == 0 && iCreditsSub < iCreditsEnd ) {
						// Move the item so it doesn't pop up in the view immediately, but gradually scrolls in instead
						x -= MASK_CREDIT_MAX_LENGTH_IN_SQUARES;

						// If this stack position is empty, fill it and increment the index
						pCreditsStack[i].x = MASK_PADDING_X + usFontDX * x;
						if ( pCreditsStack[i].y == 0 ) pCreditsStack[i].y = MASK_PADDING_Y + usFontDY * (i);
						pCreditsStack[i].i = iCreditsSub++;
						pCreditsStack[i].frames++;
						pCreditsStack[i].cnt = 20;
						pCreditsStack[i].j = 0;
					}
				}
				// Check to see if we've reached the end
				if ( iCreditsSub == iCreditsEnd ) {
					// Fade out into the new state
					fFade += 0.02f * fInc;
					if ( fFade >= 0 ) {
						ucLastState = sState++;
						bFade = true;
					}
				}

				SIZE Size;
				for ( unsigned int i = 0; i < MASK_CREDIT_STACK; i++ ) {
					// Calculate the amount of frames we still have
					short sFramesRemaining = ((float)MASK_CREDIT_FRAMECOUNT / fInc) - pCreditsStack[i].frames;

					// If this is an empty slot, skip
					if ( pCreditsStack[i].frames == 0 ) continue;

					// Text buffers
					//const char *szItemText = pCreditsSub[pCreditsStack[i].i];
					//unsigned char ucLen = Clamp ( 0, strlen ( szItemText ), 30 );
					//char szText[NAME_MAX_LENGTH] = {0};
					const char *szText = pCreditsSub[pCreditsStack[i].i];

					/*
					// Character animation logic
					for ( unsigned char j = 0; j < ucLen; j++ ) {
						unsigned int uiMask = (1 << j);

						// If it's masked, show the correct character
						// If it's not, randomize the character
						if ( pCreditsStack[i].mask & uiMask ) {
							szText[j] = szItemText[j];
						} else {
							szText[j] = ' ';
						}

						// Determine when the next character is going to show up
						if ( pCreditsStack[i].frames == pCreditsStack[i].cnt && pCreditsStack[i].j == j ) {
							srand ( time(NULL) );
							pCreditsStack[i].mask |= uiMask;
							//pCreditsStack[i].cnt = pCreditsStack[i].frames + (rand() % ((usFramesRemaining/(ucLen-j)>>4)));
							pCreditsStack[i].j++;
						}
					}
					*/

					// Increase the frame counter of this item
					pCreditsStack[i].frames++;

					// Get the text extent so we can determine how big the black square has to be
					m_pGFX->UTIL_GetTextExtent ( szText, &Size, 1.0f );
					unsigned int uiClearWidth = (unsigned int)ceil((float)Size.cx / (float)usFontDX) * usFontDX;

					// Render a black sprite to black out the squares
					m_pGFX->Render2DSprite ( NULL, &D3DXVECTOR2 ( (float)uiClearWidth, (float)usFontDY ), &D3DXVECTOR2 ( pCreditsStack[i].x, pCreditsStack[i].y ), D3DXCOLOR ( 0, 0, 0, 1.0f ) );

					// Render the text
					m_pGFX->DrawText2D ( pCreditsStack[i].x, pCreditsStack[i].y, D3DXCOLOR ( 1, 0, 0, 1.0f ), szText, 1.0f );

					// If the framecount has reached it's maximum, clear the slot
					if ( sFramesRemaining <= 0 ) pCreditsStack[i].frames = 0;
				}
			}
		}

#define BBC
#ifdef BBC

		m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_TRUE );
		m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );

		assert ( SUCCEEDED(m_pDevice->SetDepthStencilSurface ( pDepthNew )) );

		/********************************************************************/
		/* SCENE PASS: MONITOR VIEW                                         */
		/********************************************************************/
		// Only render if we're not in the main menu
		if ( sState != STATE_MAINMENU ) {			
			m_pDevice->SetRenderTarget ( 0, pD3DMonitor );
			m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
			{
				D3DXMATRIX matProj, matWorld;

				D3DXMatrixIdentity ( &matWorld );
				m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );

				D3DXMATRIXA16 matView;
				D3DXMatrixLookAtLH( &matView, &vecMonitorEyePt, &vecMonitorLookatPt, &vecUp );
				m_pDevice->SetTransform( D3DTS_VIEW, &matView );

				// Set up the frustum
				D3DXMatrixPerspectiveFovLH ( &matProj, 0.785398f, 1.33f, 0.1f, 10.0f );
				m_pDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

				// Render states
				m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );						// Lighting on
				m_pDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );			// Gouraud shading on
				m_pDevice->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_XRGB(150,150,150) );	// Ambient light
				m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_TRUE );					// Depth buffer enable
				m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );						// Depth buffer writing
				m_pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );				// Depth function
				m_pDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );				// Texture factor rest
				m_pDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );				// Solid fillmode
				m_pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
				m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
				m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x08 );
				m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );

				m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
				m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );

				m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
				m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
				m_pDevice->SetRenderState ( D3DRS_ALPHAREF, 0 );
				m_pDevice->SetRenderState ( D3DRS_NORMALIZENORMALS, FALSE );
				m_pDevice->SetRenderState ( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
				m_pDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
				m_pDevice->SetRenderState ( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				m_pDevice->SetRenderState ( D3DRS_SCISSORTESTENABLE, FALSE );
				m_pDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, 0 );
				m_pDevice->SetRenderState ( D3DRS_BLENDFACTOR, 0xFFFFFFFF );
				m_pDevice->SetRenderState ( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );

				/********************************************************************/
				/* PASS 0                                                           */
				/********************************************************************/
				// Set the worldview projection matrix ( no world transformation, world matrix = identity  )
				D3DXMATRIXA16 matWorldViewProj = matView * matProj;
				pVSHConstMask->SetMatrix ( m_pDevice, "WorldViewProj", &matWorldViewProj );
				
				unsigned int count;
				D3DXCONSTANT_DESC cTextureMask, cTextureVideo;
				D3DXHANDLE hTextureMask = pPSHConstMask->GetConstantByName ( 0, "TextureMask" );
				D3DXHANDLE hTextureVideo = pPSHConstMask->GetConstantByName ( 0, "TextureVideo" );
				pPSHConstMask->GetConstantDesc ( hTextureMask, &cTextureMask, &count );
				pPSHConstMask->GetConstantDesc ( hTextureVideo, &cTextureVideo, &count );

				// Set the vertex and pixel shaders
				m_pDevice->SetVertexDeclaration ( pVSHDeclMask );
				m_pDevice->SetVertexShader ( pVSHMask );
				m_pDevice->SetPixelShader ( pPSHMask );

				// Mipmapping options
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

				pPSHConstMask->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "Alpha" ), 1.0f );

				// Texture stages
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );

				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );

				// Set the texture
				if ( pFallbackTexture )
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pFallbackTexture );
				else if ( sState != STATE_MAINMENU )
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pTexMaskAnim );
				else
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pVideoTexture );
				m_pDevice->SetTexture ( cTextureMask.RegisterIndex, pTexMask );

				// Render the vertex buffer contents
				m_pDevice->SetStreamSource( 0, pVBProjection, 0, sizeof(CUSTOMVERTEX) );
				m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

				m_pDevice->SetTexture ( cTextureMask.RegisterIndex, NULL );
				m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, NULL );

				/********************************************************************/
				/* PASS 1                                                           */
				/********************************************************************/
				// Set the light position and power
				FLOAT lightPosA[] = { 0.0f, 0.2f, 0.5f, 1.0f };	
				FLOAT lightPosB[] = { -1.0f, -0.4f, 0.3f, 1.0f };		// Tommy
				FLOAT lightPosC[] = { 1.0f, -0.25f, 0, 1.0f };			// Claude
				FLOAT lightPosD[] = { 0.1f, -0.4f, -0.5f, 1.0f };		// CJ

				float fFlickerA = 0.45f;
				float fFlickerB = 0.25f; //0.1f + (1.0f+sinf(fTheta*8.6f))/4.0f;
				float fFlickerC = 0.25f; //0.1f + (1.0f+cosf(fTheta*7.4f))/4.0f;
				float fFlickerD = 0.25f; //0.1f + (1.0f+sinf(fTheta*6.3f+1.0f))/4.0f;

				fFlickerA *= fLightDimA;
				fFlickerB *= fLightDimB;
				fFlickerC *= fLightDimC;
				fFlickerD *= fLightDimD;

				float fRadA = 1.0f;
				float fRadB = 0.6f;
				float fRadC = 0.6f;
				float fRadD = 0.6f;

				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosA" ), &lightPosA[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosB" ), &lightPosB[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosC" ), &lightPosC[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosD" ), &lightPosD[0], 4 );

				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "Alpha" ), 1.0f );

				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerA" ), fFlickerA );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerB" ), fFlickerB );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerC" ), fFlickerC );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerD" ), fFlickerD );

				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadA" ), fRadA );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadB" ), fRadB );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadC" ), fRadC );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadD" ), fRadD );

				D3DXCONSTANT_DESC cTexture;
				D3DXHANDLE hTexture = pPSHConstLighting->GetConstantByName ( 0, "Texture" );
				pPSHConstLighting->GetConstantDesc ( hTexture, &cTexture, &count );

				// Transform the world so the mesh fits in nicely
				D3DXMATRIX matWorldInverse, matRotation;
				D3DXMatrixIdentity ( &matRotation );
				D3DXMatrixRotationX ( &matRotation, -0.5f * D3DX_PI );
				D3DXMatrixIdentity ( &matWorld );
				D3DXMatrixScaling ( &matWorld, 0.20f, 0.20f, 0.20f );
				D3DXMatrixTranslation ( &matWorld, 0, 0, -1.5f );

				matWorld = matWorld * matRotation;
				m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );

				// Set the matrices
				matWorldViewProj = matWorld * matView * matProj;
				pVSHConstLighting->SetMatrix ( m_pDevice, "World", &matWorld );
				pVSHConstLighting->SetMatrix ( m_pDevice, "WorldViewProj", &matWorldViewProj );

				// Enable the per-pixel lighting shader
				m_pDevice->SetVertexDeclaration ( pVSHDeclLighting );
				m_pDevice->SetVertexShader ( pVSHLighting );
				m_pDevice->SetPixelShader ( pPSHLighting );

				// Draw the mesh
				for ( DWORD i = 0; i < dwMaterials; i++ ) {
					m_pDevice->SetTexture ( cTexture.RegisterIndex, pMeshTextures[i] );
					// Check for monitor textures
					if ( pbMeshTextures[i] ) {
						pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightHack" ), 1.0f );
					} else {
						pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightHack" ), 0.0f );
					}
					m_pDevice->SetMaterial ( &pMeshMaterials[i] );
					pMesh->DrawSubset ( i );
				}
			}
		}


		
		/********************************************************************/
		/* SCENE PASS: SCENE VIEW                                           */
		/********************************************************************/

		m_pDevice->SetRenderTarget ( 0, pD3DBloomSource );
		m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
		{
			D3DXMATRIX matProj, matWorld;

			// Set the identity world matrix
			D3DXMatrixIdentity ( &matWorld );
			m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );

			// Store the up vector
			vecUpPt = vecUp;
#endif
			/********************************************************************/
			/* STATE: MAIN MENU                                                 */
			/********************************************************************/
			if ( sState == STATE_MAINMENU ) {
				// We want everything to show, as usual
				bShowAll = true; bShowScene = true; bShowRoll = false;

				// Increase the theta value
				fTheta += 0.005f * fInc;

				// Use the default eye and look vectors
				vecEyePt = vecMonitorEyePt;
				vecLookatPt = vecMonitorLookatPt;

				// If we're still fading, fade in
				if ( fFade <= SIGMOID_END && bFade ) {
					fFade += 0.02f * fInc;
				}
			}
			/********************************************************************/
			/* STATE: START WAIT DELAY                                          */
			/********************************************************************/
			else if ( sState == STATE_WAIT ) {
				// Check if we need to do any fading
				if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// We want everything to show, as usual
				bShowAll = true; bShowScene = true;

				// Calculate the curves
				ProcessBezierDouble ( vecEyePt, vecBTommyCJ, fSigmoid );
				vecLookatPt = vecMCJ + vecNCJ;
				fTheta = D3DX_PI/4.0f;
			}
			/********************************************************************/
			/* STATE: FROM INITIAL TO CJ                                        */
			/********************************************************************/
			else if ( sState == STATE_TO_CJ ) {
				// Check if we need to do any fading
				if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// We want everything to show, as usual
				bShowAll = true; bShowScene = true;
				fZoom += 0.025f * fInc;

				// Calculate the curves
				ProcessBezierDouble ( vecEyePt, vecBTommyCJ, fSigmoid );
				vecLookatPt = vecMCJ + vecNCJ;
				fTheta = D3DX_PI/4.0f;

				// If we've reached the switch point, switch to the next state
				if ( fSigmoid > SIGMOID_TRANSITION_SWITCH ) {
					ucLastState = sState++;
					fZoom = SIGMOID_START_CUT;

					// Create the cubic camera position bezier array
					vecBCJClaude[0] = vecMonitorEyePt;
					vecBCJClaude[1] = vecMonitorEyePt - D3DXVECTOR3(1,0,0);
					vecBCJClaude[2] = vecMClaude + vecNClaude;
					vecBCJClaude[3] = vecMClaude;

					// Create the look-at interpolation array
					vecBCJClaudeLook[0] = vecMonitorLookatPt;
					vecBCJClaudeLook[1] = vecMClaude - vecNClaude;
				}
			}

			/********************************************************************/
			/* STATE: FROM CJ TO CLAUDE                                         */
			/********************************************************************/
			else if ( sState == STATE_TO_CLAUDE ) {
				// Check if we need to do any fading
				if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH || fSigmoid < SIGMOID_TRANSITION_POSTSWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// We want everything to show, as usual
				bShowAll = true; bShowScene = true;
				fZoom += 0.025f * fInc;

				// Calculate the curves
				ProcessBezier ( vecEyePt, vecBCJClaude, fSigmoid / 2.0f );
				ProcessLinear ( vecLookatPt, vecBCJClaudeLook, fSigmoid / 2.0f );
//				vecEyePt = vecBCJClaude[0];
//				vecLookatPt = vecBCJClaudeLook[0];
				fTheta = -D3DX_PI/4.0f;

				// If we've reached the switch point, switch to the next state
				if ( fSigmoid > SIGMOID_TRANSITION_SWITCH ) {
					// Reset some values
					ucLastState = sState++;
					fZoom = SIGMOID_START_CUT;

					// Create the double cubic camera position bezier array
					vecBFly[0] = vecMonitorEyePt;
					vecBFly[1] = vecMonitorEyePt - D3DXVECTOR3(-1,0,0);
					vecBFly[2] = D3DXVECTOR3(-1.0f,-1,-1);
					vecBFly[3] = D3DXVECTOR3(-0.9f,-1,-0.2f);
					vecBFly[4] = D3DXVECTOR3(-0.9f,-1,-0.2f);
					vecBFly[5] = D3DXVECTOR3(-1.0f,-1,1);
					vecBFly[6] = D3DXVECTOR3(-2.0f,-0.1f,2);
					vecBFly[7] = D3DXVECTOR3(-1.5f,0,0);

					// Create the look-at bezier curve array
					vecBFlyLook[0] = vecMonitorLookatPt;
					vecBFlyLook[1] = D3DXVECTOR3(-0.2f, -1.0f, 1.0f);
					vecBFlyLook[2] = D3DXVECTOR3(-0.2f, 0, 0);
					vecBFlyLook[3] = D3DXVECTOR3(-0.2f, -0.2f, 1.0f);
				}
			}

			/********************************************************************/
			/* STATE: FROM CLAUDE, UNDER THE TABLE, TO TOMMY                    */
			/********************************************************************/
			else if ( sState == STATE_TO_TOMMY ) {
				// Check if we need to do any fading
				if ( fSigmoid < SIGMOID_TRANSITION_POSTSWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// We want everything to show, as usual
				bShowAll = true; bShowScene = true;
				fZoom += 0.025f * fInc;
				//fTheta = -D3DX_PI/4.0f;

				// Calculate the curves
				ProcessBezierDouble ( vecEyePt, vecBFly, fSigmoid );
				ProcessBezier ( vecLookatPt, vecBFlyLook, fSigmoid * 0.5f );

				// If we've almost reached the switch point, start speeding up the ever decreasing sigmoid curve to smoothen out the transition
				if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH ) {
					fSigmoidOffset += 0.0004f * fInc;
				}

				// If we've reached the switch point, switch to the next state
				if ( fSigmoid > SIGMOID_TRANSITION_PHASE ) {
					// Reset some values
					ucLastState = sState++;
					fZoom = SIGMOID_START;
					fSigmoidOffset = 0;

					// Create the double bezier array, based on the current eye position
					vecBWall[0] = vecEyePt - D3DXVECTOR3(0,0,-0.001f);
					vecBWall[1] = vecEyePt + D3DXVECTOR3(0,0,-1);
					vecBWall[2] = D3DXVECTOR3(-0.5f,0.75f,-2);
					vecBWall[3] = D3DXVECTOR3(0.5f,0.75f,-2);
					vecBWall[4] = vecBWall[3];
					vecBWall[5] = vecBWall[3] + D3DXVECTOR3(1,0,0);
					vecBWall[6] = D3DXVECTOR3(0.75f,0.75f,0);
					vecBWall[7] = D3DXVECTOR3(1.45f,0.75f,0);

					// Create the look-at linear interpolation array
					vecBWallLook[0] = vecLookatPt;
					vecBWallLook[1] = D3DXVECTOR3(3,0,0);
				}
			}

			/********************************************************************/
			/* STATE: FROM TOMMY TOWARDS THE WALL                               */
			/********************************************************************/
			else if ( sState == STATE_TO_WALL ) {
				// Check if we need to do any fading
				if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// Smoothen out the transition of the previous state by adding (and later, removing) an offset to (from) the sigmoid curve
				if ( fSigmoid < 0.61f ) {
					fSigmoidOffset += 0.002f * fInc;
				} else if ( fSigmoidOffset > 0 )
					fSigmoidOffset -= 0.002f * fInc;

				// We want everything to show, as usual
				bShowAll = true; bShowScene = true;
				fZoom += 0.025f * fInc;

				// Calculate the curves
				ProcessBezierDouble ( vecEyePt, vecBWall, fSigmoid );
				ProcessLinear ( vecLookatPt, vecBWallLook, fSigmoid / 2.0f );

				// If we've reached the switch point, switch to the next state
				if ( fSigmoid > SIGMOID_TRANSITION_SWITCH ) {
					ucLastState = sState++;
					fZoom = SIGMOID_START_CUT;
				}
			}

			/********************************************************************/
			/* STATE: QA NAME SCROLLER                                          */
			/********************************************************************/
			else if ( sState == STATE_SCROLL ) {
				if ( fSigmoid < SIGMOID_TRANSITION_POSTSWITCH )
					fFade += 0.07f * fInc;					// Do the fading
				else if ( fSigmoid > 0 && fSigmoid < 1 )
					fFade = SIGMOID_START;					// Need to reset the sigmoid phase

				// Turn off the scene rendering
				bShowScene = false;
				fZoom += 0.025f * fInc;

				// Phase of the eye bezier curve
				float fEyeSine = cosf ( fTheta * 2 ) * 0.5f;

				// Credits stack
				fTheta -= 0.005f * fInc;
				//fFade = SIGMOID_START;

				// Calculate a phased theta on which we base our direction vector
				float fThetaPhased = fTheta - D3DX_PI * 0.31f;

				D3DXVECTOR3 vecDirection;
				D3DXVECTOR3 vecEyeStart = D3DXVECTOR3 ( fDisplaceX * 0.5f, 0, fDisplaceZ * 0.5f );
				D3DXVec3Normalize ( &vecDirection, &D3DXVECTOR3 ( sinf ( fThetaPhased ) * 3.0f, 0, cosf ( fThetaPhased ) * 3.0f ) );

				// Adjust the bezier camera eye curve
				vecBCreditEye[0] = vecEyeStart - D3DXVECTOR3 ( 0, 0.5f, 0 );
				vecBCreditEye[1] = vecBCreditEye[0] - vecDirection * 0.2f;
				vecBCreditEye[2] = vecBCreditEye[1] + D3DXVECTOR3 ( 0, 1, 0 );
				vecBCreditEye[3] = vecEyeStart + D3DXVECTOR3 ( 0, 0.5f, 0 );

				ProcessBezier ( vecEyePt, vecBCreditEye, 0.5f + fEyeSine );
				//vecLookatPt = ( vecDirection + D3DXVECTOR3 ( 0, cosf ( fTheta * 1 ) * 0.1f, 0 ) ) * 3;
				vecLookatPt = vecDirection * 3;
			}

			/********************************************************************/
			/* STATE: VERTICAL END SCROLLER                                     */
			/********************************************************************/
			else if ( sState == STATE_END ) {
				// Turn off all scene rendering, and enable the vertical credits roll
				bShowScene = false; bShowAll = false; bShowRoll = true;
				fTheta += fRollScaleY * 3.0f * fInc;		// Used for scrolling the roll

				// Fix the camera
				vecEyePt = D3DXVECTOR3 ( 0, 0, 0 );
				vecLookatPt = D3DXVECTOR3 ( 0, 0, 1 );

				// Fade out if we need to
				if ( fFade <= SIGMOID_END && bFade ) {
					fFade += 0.02f * fInc;
				} else {
					// Don't fade out again like in the beginning
					bFade = false;

					// And fade out again if the roll is through
					if ( fTheta >= (CORE_MTA_ROLL_HEIGHT_POW2+CORE_MTA_ROLL_BOTTOM) ) {
						fFade += 0.02f * fInc;

						// If we're at the switch point, switch back to the main menu
						if ( fFade >= 0 ) {
							bFade = true;
							StopCredits ();
						}
					} else {
						fFade = SIGMOID_START;
					}
				}
			}

			/********************************************************************/
			/* STATE: CREDIT FOCUS                                              */
			/********************************************************************/
			if ( sState == STATE_CREDIT_FOCUS ) {
				// Handle the fading in/out
				if ( dwFrameCount < dwFrameMid ) {
					// If we're not half-way through, decrement the scene blender (alpha)
					//fBlend -= 0.1f;
					//fBlend = -1;

					// And set the splash alpha to some value above 1 (to delay the alpha fading below for a few frames)
					//fSplashAlpha = 2.2f; //- fBlend;
					fSplashAlpha = 1.4f;
				} else {
					// If we're half-way through, increment the scene blender (alpha)
					//fBlend += 0.1f * fInc;
					//fBlend = 1.0f;

					// Decrement the splash alpha
					fSplashAlpha -= (1.4f/((float)(dwFrameStop-dwFrameMid))) * fInc;
				}

				// If the splash isnt fully scaled yet, increase it's scale value exponentially
				if ( fSplash < 1.0f && fSplashTheta == 0 ) {
					fSplash = fSplash + (fSplash * 0.4f * fInc);
				} else {
					// If it's fully scaled, use a sine wave to make it grow and shrink
					fSplash = 1.0f + sinf ( fSplashTheta ) * 0.1f;
					fSplashTheta += 0.04f * fInc;
				}

				// We don't want to see this credit again after the focus
				ucCreditSkip = ucCredit;

				// Check if we need to switch back
				if ( dwFrameCount >= dwFrameStop ) {
					sState = ucLastState;
					//sSubState = INVALID;

					// Invalidate our current credit id (so we never see it again)
					ucCredit = INVALID;
				}
			}
#ifdef BBC
			// Set up the view matrix
			D3DXMATRIXA16 matView;
			D3DXMatrixLookAtLH( &matView, &vecEyePt, &vecLookatPt, &vecUpPt );
			m_pDevice->SetTransform ( D3DTS_VIEW, &matView );

			// Set up the frustum
			D3DXMatrixPerspectiveFovLH ( &matProj, 0.785398f, 1.33f, 0.1f, 10.0f );
			m_pDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

			// Set misc render states
			m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );						// Lighting on
			m_pDevice->SetRenderState ( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );			// Gouraud shading on
			m_pDevice->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_XRGB(150,150,150) );	// Ambient light
			m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_TRUE );					// Depth buffer enable
			m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );						// Depth buffer writing
			m_pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );				// Depth function
			m_pDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );				// Texture factor rest
			m_pDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );				// Solid fillmode
			m_pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
			m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  FALSE );
			m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x08 );
			m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );

			m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );

			m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
			m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
			m_pDevice->SetRenderState ( D3DRS_ALPHAREF, 0 );
			m_pDevice->SetRenderState ( D3DRS_NORMALIZENORMALS, FALSE );
			m_pDevice->SetRenderState ( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
			m_pDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
			m_pDevice->SetRenderState ( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			m_pDevice->SetRenderState ( D3DRS_SCISSORTESTENABLE, FALSE );
			m_pDevice->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS, 0 );
			m_pDevice->SetRenderState ( D3DRS_BLENDFACTOR, 0xFFFFFFFF );
			m_pDevice->SetRenderState ( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );


			/********************************************************************/
			/* PASS 0                                                           */
			/********************************************************************/
			// Set the worldview projection matrix ( no world transformation, world matrix = identity  )
			D3DXMATRIXA16 matWorldViewProj = matView * matProj;
			pVSHConstMask->SetMatrix ( m_pDevice, "WorldViewProj", &matWorldViewProj );
			
			if ( bShowAll ) {
				unsigned int count;
				D3DXCONSTANT_DESC cTextureMask, cTextureVideo;
				D3DXHANDLE hTextureMask = pPSHConstMask->GetConstantByName ( 0, "TextureMask" );
				D3DXHANDLE hTextureVideo = pPSHConstMask->GetConstantByName ( 0, "TextureVideo" );
				pPSHConstMask->GetConstantDesc ( hTextureMask, &cTextureMask, &count );
				pPSHConstMask->GetConstantDesc ( hTextureVideo, &cTextureVideo, &count );

				// Set the vertex and pixel shaders
				m_pDevice->SetVertexDeclaration ( pVSHDeclMask );
				m_pDevice->SetVertexShader ( pVSHMask );
				m_pDevice->SetPixelShader ( pPSHMask );

				// Mipmapping options
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				m_pDevice->SetSamplerState ( cTextureVideo.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				m_pDevice->SetSamplerState ( cTextureMask.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

				// Texture stages
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
				m_pDevice->SetTextureStageState ( cTextureVideo.RegisterIndex, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );

				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
				m_pDevice->SetTextureStageState ( cTextureMask.RegisterIndex, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );

				pPSHConstMask->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "Alpha" ), Clamp ( 0, fBlend, 1 ) );

				// Set the texture
				if ( pFallbackTexture )
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pFallbackTexture );
				else if ( sState != STATE_MAINMENU )
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pTexMaskAnim );
				else
					m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, pVideoTexture );
				m_pDevice->SetTexture ( cTextureMask.RegisterIndex, pTexMask );

				// Render the vertex buffer contents
				m_pDevice->SetStreamSource( 0, pVBProjection, 0, sizeof(CUSTOMVERTEX) );
				m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

				m_pDevice->SetTexture ( cTextureMask.RegisterIndex, NULL );
				m_pDevice->SetTexture ( cTextureVideo.RegisterIndex, NULL );

				m_pDevice->SetPixelShader ( NULL );
				m_pDevice->SetVertexDeclaration ( NULL );
				m_pDevice->SetVertexShader ( NULL );
			}

			/********************************************************************/
			/* PASS 1                                                           */
			/********************************************************************/
			if ( fBlend > 0 && bShowScene ) {
				// Set the light position and power
				FLOAT lightPosA[] = { 0.0f, 0.2f, 0.5f, 1.0f };	
				FLOAT lightPosB[] = { -1.0f, -0.4f, 0.3f, 1.0f };		// Tommy
				FLOAT lightPosC[] = { 1.0f, -0.25f, 0, 1.0f };			// Claude
				FLOAT lightPosD[] = { 0.1f, -0.4f, -0.5f, 1.0f };		// CJ

				float fFlickerA = (1.0f + sinf(fLight)) * 0.250f; //0.45f;
				float fFlickerB = 0.15f; //0.1f + (1.0f+sinf(fTheta*8.6f))/4.0f;
				float fFlickerC = 0.15f; //0.1f + (1.0f+cosf(fTheta*7.4f))/4.0f;
				float fFlickerD = 0.15f; //0.1f + (1.0f+sinf(fTheta*6.3f+1.0f))/4.0f;

				fFlickerA *= fLightDimA;
				fFlickerB *= fLightDimB;
				fFlickerC *= fLightDimC;
				fFlickerD *= fLightDimD;

				float fRadA = 1.0f;
				float fRadB = 0.6f;
				float fRadC = 0.6f;
				float fRadD = 0.6f;

				// Set light positions
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosA" ), &lightPosA[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosB" ), &lightPosB[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosC" ), &lightPosC[0], 4 );
				pPSHConstLighting->SetFloatArray ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPosD" ), &lightPosD[0], 4 );

				// Set scene blending (alpha)
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "Alpha" ), Clamp ( 0, fBlend, 1 ) );

				// Set light strength
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerA" ), fFlickerA );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerB" ), fFlickerB );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerC" ), fFlickerC );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightPowerD" ), fFlickerD );

				// Set light radius
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadA" ), fRadA );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadB" ), fRadB );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadC" ), fRadC );
				pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightRadD" ), fRadD );

				// Get the texture register indices
				unsigned int count;
				D3DXCONSTANT_DESC cTexture;
				D3DXHANDLE hTexture = pPSHConstLighting->GetConstantByName ( 0, "Texture" );
				pPSHConstLighting->GetConstantDesc ( hTexture, &cTexture, &count );

				// Transform the world so the mesh fits in nicely
				D3DXMATRIX matWorldInverse, matRotation;
				D3DXMatrixIdentity ( &matRotation );
				D3DXMatrixRotationX ( &matRotation, -0.5f * D3DX_PI );
				D3DXMatrixIdentity ( &matWorld );
				D3DXMatrixScaling ( &matWorld, 0.20f, 0.20f, 0.20f );
				D3DXMatrixTranslation ( &matWorld, 0, 0, -1.5f );
				matWorld = matWorld * matRotation;
				m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );

				// Set the matrices
				matWorldViewProj = matWorld * matView * matProj;
				pVSHConstLighting->SetMatrix ( m_pDevice, "World", &matWorld );
				pVSHConstLighting->SetMatrix ( m_pDevice, "WorldViewProj", &matWorldViewProj );

				// Enable the per-pixel lighting shader
				m_pDevice->SetVertexDeclaration ( pVSHDeclLighting );
				m_pDevice->SetVertexShader ( pVSHLighting );
				m_pDevice->SetPixelShader ( pPSHLighting );

				// Draw the mesh
				for ( DWORD i = 0; i < dwMaterials; i++ ) {
					// Check for monitor textures so we can enable the light hack on them (so they're not affected by per-pixel lighting)
					if ( pbMeshTextures[i] ) {
						// Only use the monitor pass if we're not in the main menu
						if ( sState != STATE_MAINMENU ) m_pDevice->SetTexture ( cTexture.RegisterIndex, pTexMonitor );
						else m_pDevice->SetTexture ( cTexture.RegisterIndex, pMeshTextures[i] );
						pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightHack" ), 1.0f );
					} else {
						m_pDevice->SetTexture ( cTexture.RegisterIndex, pMeshTextures[i] );
						pPSHConstLighting->SetFloat ( m_pDevice, pPSHConstLighting->GetConstantByName ( 0, "LightHack" ), 0.0f );
					}
					m_pDevice->SetMaterial ( &pMeshMaterials[i] );
					pMesh->DrawSubset ( i );
				}
			}

			// Clear all vertex and pixel shaders
			m_pDevice->SetVertexDeclaration ( NULL );
			m_pDevice->SetVertexShader ( NULL );
			m_pDevice->SetPixelShader ( NULL );

			// Render the credit sprites
			m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			m_pDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
			m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			// Set the world matrix to the identity matrix
			D3DXMatrixIdentity ( &matWorld );
			m_pDevice->SetTransform ( D3DTS_WORLD, &matWorld );

			// Take care of the credit rendering
			DrawCredit ( 0, -1, vecEyePt, 0.6f );
			DrawCredit ( 1, -1, vecEyePt, 0.6f );
			DrawCredit ( 0, 0, vecEyePt, 0.6f );
			DrawCredit ( 1, 0, vecEyePt, 0.6f );
			DrawCredit ( 2, 1, vecEyePt, 0.6f );
			DrawCredit ( 3, 1, vecEyePt, 0.6f );
			DrawCredit ( 4, 2, vecEyePt, 0.62f );
			DrawCredit ( 5, 2, vecEyePt, 0.6f );
			DrawCredit ( 6, 2, vecEyePt, 0.6f );
			DrawCredit ( 7, 3, vecEyePt, 0.51f );
			DrawCredit ( 8, 3, vecEyePt, 0.59f );

			// This credit facing backwards, so allow the camera to pass first
			DrawCredit ( 9, 3, vecEyePt, 0.48f, !( fSigmoid > 0.225f && fSigmoid < 1 ) );

			// Draw the vertical credits roll, if needed
			if ( bShowRoll )
				DrawRoll ();
		}

		
		/********************************************************************/
		/* SCENE PASS: BLOOMING EFFECT                                      */
		/********************************************************************/
		// Only enable if we've enabled post-processing
		if ( bEnablePostProcessing ) {
			m_pDevice->SetRenderTarget ( 0, pD3DBloomTarget );
			m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
			{
				unsigned int count = 0;
				D3DXCONSTANT_DESC cTexture;
				D3DXHANDLE hTexture = pPSHConstBloom->GetConstantByName ( 0, "Texture" );
				pPSHConstBloom->GetConstantDesc ( hTexture, &cTexture, &count );
				pPSHConstBloom->SetFloat ( m_pDevice, pPSHConstBloom->GetConstantByName ( 0, "alpha" ), 0.5f );

				m_pDevice->SetVertexDeclaration ( NULL );
				m_pDevice->SetVertexShader ( NULL );
				m_pDevice->SetPixelShader ( pPSHBloom );

				m_pGFX->Render2DSprite ( pTexBloomSrc, &D3DXVECTOR2((float)usTexBloomTgtWidth/(float)usTexBloomSrcWidth,(float)usTexBloomTgtHeight/(float)usTexBloomSrcHeight), &D3DXVECTOR2(0,0), 0xFFFFFFFF );
			}
		}

		
		/********************************************************************/
		/* SCENE PASS: FINAL RENDERING                                      */
		/********************************************************************/
		m_pDevice->SetRenderTarget ( 0, pD3DSurface );
		m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0 );
		{
			m_pDevice->SetVertexDeclaration ( NULL );
			m_pDevice->SetVertexShader ( NULL );
			m_pDevice->SetPixelShader ( NULL );

			// Enable plain blending
			m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, false );
			m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
			m_pDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_ONE );
			m_pDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_ONE );

			if ( bEnablePostProcessing ) {
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_GAUSSIANQUAD );
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_GAUSSIANQUAD );
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_GAUSSIANQUAD );
			} else {
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			}

			// Draw the bloom source and target planes (only if we've enabled post-processing)
			m_pGFX->Render2DSprite ( pTexBloomSrc, &D3DXVECTOR2(1,1), &D3DXVECTOR2(0,0), 0xFFFFFFFF );
			if ( bEnablePostProcessing )
				m_pGFX->Render2DSprite ( pTexBloomTgt, &D3DXVECTOR2((float)usTexBloomSrcWidth/(float)usTexBloomTgtWidth,(float)usTexBloomSrcHeight/(float)usTexBloomTgtHeight), &D3DXVECTOR2(0,0), 0xFFFFFFFF );

			// Enable blending
			m_pDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			// Draw our fade plane, in case we're fading into another state
			DWORD dwSigmoidAlpha = 0;
			DWORD dwColor = 255;
			if ( fSigmoid > SIGMOID_TRANSITION_PRESWITCH || sState > 0 ) {
				dwSigmoidAlpha = 255 - (DWORD)((fSigmoidSpin*2.0f) * 255.0f);

				// We want a black color for certain states
				if ( bFade || sState == STATE_SCROLL || sState == STATE_END || sState == STATE_MAINMENU )
					dwColor = 0;

				// Draw the sprite
				m_pGFX->Render2DSprite ( NULL, &D3DXVECTOR2((float)usTexBloomSrcWidth,(float)usTexBloomSrcHeight), &D3DXVECTOR2(0,0), D3DCOLOR_ARGB ( dwSigmoidAlpha, dwColor, dwColor, dwColor ) );
			}

			//CGraphics::GetSingletonPtr()->DrawTextCEGUI ( 200, 200, 300, 300, 0xFFFFFFFF, buf, 1.0f, DT_CENTER );
		}
#endif
		m_pDevice->SetDepthStencilSurface ( pDepth );
		SAFE_RELEASE ( pDepth );

		m_pGraphics->EndDrawing ();
		m_pGraphics->SetRenderTarget ( NULL );

		SAFE_RELEASE ( pD3DMaskAnim );
		SAFE_RELEASE ( pD3DMask );
		SAFE_RELEASE ( pD3DSurface );
		SAFE_RELEASE ( pD3DBloomSource );
		SAFE_RELEASE ( pD3DBloomTarget );
		SAFE_RELEASE ( pD3DMonitor );
	}

	// Release the video texture
	if ( pVideoTexture ) {
		m_pVideoRenderer->Unlock ();
		pVideoTexture->Release ();
	}
}


void CMainMenuScene::PlayVideo ( void )
{
	// Start the video renderer playback
	if ( m_pVideoRenderer ) {
		m_pVideoRenderer->Play ();
	}
}


void CMainMenuScene::StopVideo ( void )
{
	// Stop the video renderer playback
	if ( m_pVideoRenderer ) {
		m_pVideoRenderer->Stop ();
	}
}


bool CMainMenuScene::InitVideo ( void )
{
	// If we already have a VMR9 video renderer (e.g. after a mode change), destroy it
	if ( m_pVideoRenderer )
		CVideoManager::GetSingletonPtr ()->DestroyRenderer ( m_pVideoRenderer );

	// Initialize the VMR9 video renderer
	if ( CVideoManager::GetSingletonPtr ()->CreateRenderer ( &m_pVideoRenderer, CORE_MTA_VIDEO ) == false )
		return false;

	// Start the video renderer playback
//	m_pVideoRenderer->Play ();

	return true;
}


void CMainMenuScene::CreateCylinder ( IDirect3DDevice9 * pDevice )
{
	// Create our cylinder mesh
    assert ( pDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &pVBProjection, NULL ) == S_OK );

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
    pVBProjection->Lock( 0, 0, (void**)&pVertices, 0 );

	float fRadius = 2.5f;					// Cylinder radius
	float fHeight = 1.0f;					// Cylinder height
	float fTextureRepeat = 4.0f;			// Repeat the texture 4 times through the entire circumference

	// Create the cylinder
	for ( DWORD i = 0; i < 50; i++ ) {
        float theta = (2*D3DX_PI*i)/(50-1);

		float fPosX = sinf(theta) * fRadius;
		float fPosZ = cosf(theta) * fRadius;

        pVertices[2*i+0].position = D3DXVECTOR3 ( fPosX, -fHeight, fPosZ );
        pVertices[2*i+0].tu       = ((float)i)/((50.0f-1.0f)/fTextureRepeat);
        pVertices[2*i+0].tv       = 1.0f;

        pVertices[2*i+1].position = D3DXVECTOR3 ( fPosX, fHeight, fPosZ );
        pVertices[2*i+1].tu       = ((float)i)/((50.0f-1.0f)/fTextureRepeat);
        pVertices[2*i+1].tv       = 0.0f;
    }

    pVBProjection->Unlock();
}


void CMainMenuScene::OnInvalidate ( IDirect3DDevice9 * pDevice )
{
	if ( usTexBloomSrcWidth && usTexBloomSrcHeight )
	{
		// Notify the screen sprite
		if ( pScreen )
			pScreen->OnLostDevice ();

		// Destroy the (unmanaged) rendertarget textures we've used
		DestroyRenderTargets ();

		// Dereference the D3D device
		SAFE_RELEASE ( pDevice );
	}
}

void CMainMenuScene::OnRestore ( IDirect3DDevice9 * pDevice, CVector2D vecScreenSize, IDirect3DTexture9 * pRenderTarget )
{
//	if ( usTexBloomSrcWidth && usTexBloomSrcHeight )
	{
		m_vecScreenSize = vecScreenSize;

		// Store the device pointer
		m_pDevice = pDevice;

		// Notify the screen sprite
		if ( pScreen )
			pScreen->OnResetDevice ();

		// Store the render target pointer
		pTexCGUI = pRenderTarget;

		// Create the render targets
		InitRenderTargets ();
	}
}

void CMainMenuScene::DrawRoll ( void )
{
	m_pGFX->Render2DSprite ( pTexRoll, &D3DXVECTOR2(fRollScaleX,fRollScaleY), &D3DXVECTOR2(iRollOffset,m_vecScreenSize.fY - fTheta), 0xFFFFFFFF );
}

void CMainMenuScene::DrawCredit ( unsigned char ucIndex, short sStateRequired, D3DXVECTOR3 vecEyePt, float fDistanceThreshold, bool bIgnore )
{
	D3DXVECTOR3 vecHalfSpan	= vecCreditsSpan[ucIndex] * 0.5f;
	D3DXVECTOR3 vecCenter	= vecCredits[ucIndex] + vecHalfSpan;

	float fMinAlpha = 0.1f;
	float fDistScale = 1.2f;
	float fRGB = 1.0f;
	float fDistance = Clamp ( fMinAlpha, 1.0f - pow(D3DXVec3Length ( &(vecEyePt - vecCenter) ),1.2f) * fDistScale, 1.0f );

	if ( sState == sStateRequired || ucCredit == ucIndex ) {
		// If we're in focus mode, draw the splash
		if ( sState == STATE_CREDIT_FOCUS ) {
			// Disable the Z-buffer, so we can draw without worrying about race-issues between the splash and the credit sprites (since they're rendered at the exact same position)
			m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE );

			// Calculate the TL and BR points of the sprite, so we can scale our splash (sprites have their origin around the TL vector)
			D3DXVECTOR3 vecTopLeft		= vecCenter - ( vecHalfSpan * fSplash );
			D3DXVECTOR3 vecBottomRight	= vecCenter + ( vecHalfSpan * fSplash );

			// Render the splash sprite
			m_pGFX->DrawPlane3D ( vecTopLeft, vecBottomRight, pSplash, D3DXCOLOR ( 1, 1, 1, fSplashAlpha ) );
		}

		// Render the credit sprite
		m_pGFX->DrawPlane3D ( vecCredits[ucIndex], vecCredits[ucIndex] + vecCreditsSpan[ucIndex], pCredits[ucIndex], D3DXCOLOR ( fRGB, fRGB, fRGB, fDistance ) );

		// Make sure the Z-buffer is enabled
		m_pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_TRUE );

		// If we're close enough, switch to focus mode, unless we explicitly disabled it
		if ( fDistance > fDistanceThreshold && ucCreditSkip != ucIndex && !bIgnore ) {
			// Remember the last state, and set the current one to focus mode
			ucLastState = sState;
			sState = STATE_CREDIT_FOCUS;

			// Set our current credit index
			ucCredit = ucIndex;

			// Determine the frame start and stop times
			dwFrameStart = dwFrameCount;
			dwFrameMid = dwFrameCount + ((CREDIT_FOCUS_TIME / fInc) / 2.0f);
			dwFrameStop = dwFrameCount + (CREDIT_FOCUS_TIME / fInc);

			// Reset the splash animation values
			fSplash = 0.005f;
			fSplashAlpha = 1;
			fSplashTheta = 0;
		}
	}
}

bool CMainMenuScene::InitShaders ( void )
{
	ID3DXBuffer * pCodeBuffer = NULL;
	ID3DXBuffer * pErrors = NULL;

	// Create the vertex shaders
	m_pDevice->CreateVertexDeclaration ( decl, &pVSHDeclMask );
	if ( D3DXCompileShaderFromFile ( CORE_MTA_VSH, NULL, NULL, "vs_mask", "vs_1_1", 0, &pCodeBuffer, &pErrors, &pVSHConstMask ) != S_OK )
		return false;
	m_pDevice->CreateVertexShader ( ( (DWORD*) ( pCodeBuffer->GetBufferPointer () ) ), &pVSHMask );
	pCodeBuffer->Release ();

	m_pDevice->CreateVertexDeclaration ( decl, &pVSHDeclLighting );
	if ( D3DXCompileShaderFromFile ( CORE_MTA_VSH, NULL, NULL, "vs_lighting", "vs_2_0", 0, &pCodeBuffer, &pErrors, &pVSHConstLighting ) != S_OK )
		return false;
	m_pDevice->CreateVertexShader ( ( (DWORD*) ( pCodeBuffer->GetBufferPointer () ) ), &pVSHLighting );
	pCodeBuffer->Release ();

	// Create the pixel shaders
	if ( D3DXCompileShaderFromFile ( CORE_MTA_PSH, NULL, NULL, "ps_mask", "ps_2_0", 0, &pCodeBuffer, &pErrors, &pPSHConstMask ) != S_OK )
		return false;
	m_pDevice->CreatePixelShader ( ( (DWORD*) ( pCodeBuffer->GetBufferPointer () ) ), &pPSHMask );
	pCodeBuffer->Release();

	if ( D3DXCompileShaderFromFile ( CORE_MTA_PSH, NULL, NULL, "ps_lighting", "ps_2_0", 0, &pCodeBuffer, &pErrors, &pPSHConstLighting ) != S_OK )
		return false;
	m_pDevice->CreatePixelShader ( ( (DWORD*) ( pCodeBuffer->GetBufferPointer () ) ), &pPSHLighting );
	pCodeBuffer->Release();

	if ( D3DXCompileShaderFromFile ( CORE_MTA_PSH, NULL, NULL, "ps_bloom", "ps_2_0", 0, &pCodeBuffer, &pErrors, &pPSHConstBloom ) != S_OK )
		return false;
	m_pDevice->CreatePixelShader ( ( (DWORD*) ( pCodeBuffer->GetBufferPointer () ) ), &pPSHBloom );
	pCodeBuffer->Release();

	return true;
}

bool CMainMenuScene::InitRenderTargets ( void )
{
	// InitRenderTargets was invoked without DestroyRenderTargets, so call it ourselves
	if ( bSafe == true ) {
//		DestroyRenderTargets ();
		return true;
	}
	bSafe = true;

//	m_vecScreenSize = CVector2D(1024,768);

	// Determine the appropriate height for the blooming surfaces
	usTexBloomSrcWidth	= (unsigned short)m_vecScreenSize.fX;
	usTexBloomSrcHeight	= (unsigned short)m_vecScreenSize.fY;
	usTexBloomTgtWidth	= (unsigned short)(m_vecScreenSize.fX * 0.5f);
	usTexBloomTgtHeight	= (unsigned short)(m_vecScreenSize.fY * 0.5f);
	usTexMonitorWidth	= (unsigned short)(m_vecScreenSize.fX * 0.75f);
	usTexMonitorHeight	= (unsigned short)(m_vecScreenSize.fY * 0.75f);

	// Create the blooming source render target texture
	if ( m_pDevice->CreateTexture ( usTexBloomSrcWidth, usTexBloomSrcHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexBloomSrc, NULL ) != S_OK )
		return false;

	// Create the blooming target render target texture
	if ( m_pDevice->CreateTexture ( usTexBloomTgtWidth, usTexBloomTgtHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexBloomTgt, NULL ) != S_OK )
		return false;

	// Create the monitor render target texture
	if ( m_pDevice->CreateTexture ( usTexMonitorWidth, usTexMonitorHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexMonitor, NULL ) != S_OK )
		return false;

	// Create the mask render target texture
	if ( m_pDevice->CreateTexture ( MASK_WIDTH, MASK_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexMask, NULL ) != S_OK )
		return false;

	// Create the mask anim render target texture
	if ( m_pDevice->CreateTexture ( MASK_WIDTH, MASK_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexMaskAnim, NULL ) != S_OK )
		return false;

	if ( FAILED ( m_pDevice->CreateDepthStencilSurface ( usTexBloomSrcWidth, usTexBloomSrcHeight, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &pDepthNew, NULL ) ) ) {
		assert ( false );
		return false;
	}

	return true;
}

void CMainMenuScene::DestroyRenderTargets ( void )
{
	bSafe = false;

	SAFE_RELEASE ( pDepthNew );

	// Destroy the render targets
	SAFE_RELEASE ( pTexMaskAnim );
	SAFE_RELEASE ( pTexMask );
	SAFE_RELEASE ( pTexMonitor );
	SAFE_RELEASE ( pTexBloomTgt );
	SAFE_RELEASE ( pTexBloomSrc );
	
	// Deference the CGUI texture as well, since we created it in here
	SAFE_RELEASE ( pTexCGUI );
}

bool CMainMenuScene::InitCreditSprites ( void )
{
	/*
	char buf[64] = {0};

	// Determine the number of credits we have
	unsigned char ucNumCredits = sizeof ( vecCredits ) / sizeof ( D3DXVECTOR3 );

	// Now load all the credit textures
	for ( unsigned char i = 0; i < ucNumCredits; i++ ) {
		_snprintf ( buf, 63, CORE_MTA_CREDITS, &pCredits[i] );
		if ( FAILED(D3DXCreateTextureFromFile ( m_pDevice, buf, &pCredits[0])) ) return false;
	}

	// Load the splash texture
	if ( FAILED(D3DXCreateTextureFromFile ( m_pDevice, "data\\splash.png", &pSplash )) ) return false;
	*/

	D3DXCreateTextureFromFile ( m_pDevice, "data\\splash.png", &pSplash );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\2.png", &pCredits[0] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\3.png", &pCredits[1] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\4.png", &pCredits[2] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\5.png", &pCredits[3] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\6.png", &pCredits[4] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\7.png", &pCredits[5] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\8.png", &pCredits[6] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\9.png", &pCredits[7] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\10.png", &pCredits[8] );
	D3DXCreateTextureFromFile ( m_pDevice, "data\\11.png", &pCredits[9] );

	return true;
}

void CMainMenuScene::DestroyShaders ( void )
{
	// Destroy the pixel shader objects
	SAFE_RELEASE ( pPSHConstMask );
	SAFE_RELEASE ( pPSHConstLighting );
	SAFE_RELEASE ( pPSHConstBloom );
	SAFE_RELEASE ( pPSHMask );
	SAFE_RELEASE ( pPSHLighting );
	SAFE_RELEASE ( pPSHBloom );

	// Destroy the vertex shader objects
	SAFE_RELEASE ( pVSHConstMask );
	SAFE_RELEASE ( pVSHDeclMask );
	SAFE_RELEASE ( pVSHMask );
	SAFE_RELEASE ( pVSHConstLighting );
	SAFE_RELEASE ( pVSHDeclLighting );
	SAFE_RELEASE ( pVSHLighting );
}

inline bool CMainMenuScene::IsCreditsPlaying ( void )
{
	return (sState != STATE_MAINMENU && sState != STATE_WAIT);
}

bool CMainMenuScene::ProcessMessage ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// If we're playing the credits scene, and a key was pressed
	if ( IsCreditsPlaying () && uMsg == WM_KEYDOWN ) {
		// Stop the credits scene
		StopCredits ();
		return true;
	}
	return false;
}

void CMainMenuScene::SetPostProcessingEnabled ( bool bEnabled )
{
	bEnablePostProcessing = bEnabled;
}

bool CMainMenuScene::SetVideoEnabled ( bool bEnabled )
{
	if ( bEnableVideo == bEnabled ) return true;

	bool bResult = false;
	CFilePathTranslator FileTranslator;
    string WorkingDirectory;
    char szCurDir [ MAX_PATH ];

    // Set the current directory to the MTA dir so we can load files using a relative path
    FileTranslator.SetCurrentWorkingDirectory ( "MTA" );
    FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
    GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
    SetCurrentDirectory ( WorkingDirectory.c_str ( ) );

	do {
		if ( bEnabled ) {
			// Create the video renderer, if it doesn't already exist
			if ( bInitialized && m_pVideoRenderer == NULL ) {
				bEnableVideo = true;
				if ( InitVideo () == false ) {
					// Video renderer couldn't initialize, so fall back to a static texture (if it doesn't already exist)
					if ( pFallbackTexture == NULL && D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_FALLBACK, &pFallbackTexture ) != S_OK ) {
						break;    // Fatal error
					} else
						bEnableVideo = false;
				}
				PlayVideo ();
			}
			// Remove the fallback texture
			SAFE_RELEASE ( pFallbackTexture );
		} else {
			// Destroy the video renderer, if any
			if ( m_pVideoRenderer ) {
				CVideoManager::GetSingletonPtr ()->DestroyRenderer ( m_pVideoRenderer );
				m_pVideoRenderer = NULL;
			}

			// Fall back to a static texture (if it doesn't already exist)
			if ( pFallbackTexture == NULL && D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_FALLBACK, &pFallbackTexture ) != S_OK ) {
				// ACHTUNG: just for debugging purposes to see if the current path is incorrect, thus causing a failed texture load
				char szCurDir [ MAX_PATH ];
				GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
				CLogger::GetSingleton ().ErrorPrintf ( "Could not even load fallback texture (current dir: %s, %s)", szCurDir, CORE_MTA_FALLBACK );

//				HRESULT hr = D3DXCreateTextureFromFile ( m_pDevice, CORE_MTA_FALLBACK, &pFallbackTexture );
#ifdef MTA_DEBUG
                assert ( false );
#endif
				break;    // Fatal error
			}

			bEnableVideo = false;
		}
		bResult = true;
	} while ( false );

	// Update the config
	CMainConfig *pConfig = g_pCore->GetConfig ();
	DWORD dwSelect = (DWORD) bEnableVideo;
	pConfig->iMenuOptions = ( pConfig->iMenuOptions & ~CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED ) | ( dwSelect * CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED );

	// Restore current directory
	SetCurrentDirectory ( szCurDir );

	return bResult;
}
