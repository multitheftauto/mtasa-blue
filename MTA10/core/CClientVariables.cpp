/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CClientVariables.cpp
*  PURPOSE:     Managed storage of client variables (cvars)
*  DEVELOPERS:  Derek Abdine <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

template<> CClientVariables * CSingleton< CClientVariables >::m_pSingleton = NULL;

CClientVariables::CClientVariables ( void )
{
    m_pStorage = NULL;
    m_bLoaded = false;
    m_iRevision = 1;
}


CClientVariables::~CClientVariables ( void )
{

}

bool CClientVariables::Load ( void )
{
    // Get the root node
    CXMLNode *pRoot = CCore::GetSingleton ().GetConfig ();
    if ( !pRoot )
        return false;
    m_iRevision++;

    // Load the cvars
    m_pStorage = pRoot->FindSubNode ( CONFIG_NODE_CVARS );
    if ( !m_pStorage ) {
        // Non-existant, create a new node
        m_pStorage = pRoot->CreateSubNode ( CONFIG_NODE_CVARS );
    }

    // Verify that at least all the defaults are in
    LoadDefaults ();

    return true;
}


bool CClientVariables::Get ( const std::string& strVariable, CVector &val )
{
    std::stringstream ss;
    std::string strVal;

    if ( !Node ( strVariable ) ) return false;
    strVal = Node ( strVariable )->GetTagContent ();
    ss.str ( strVal );

    try {
        ss >> val.fX >> val.fY >> val.fZ;
    } catch (...) {
        return false;
    }

    return true;
}


bool CClientVariables::Get ( const std::string& strVariable, CVector2D &val )
{
    std::stringstream ss;
    std::string strVal;

    if ( !Node ( strVariable ) ) return false;
    strVal = Node ( strVariable )->GetTagContent ();
    ss.str ( strVal );

    try {
        ss >> val.fX >> val.fY;
    } catch (...) {
        return false;
    }

    return true;
}


bool CClientVariables::Get ( const std::string& strVariable, CColor &val )
{
    std::stringstream ss;
    std::string strVal;
    int iR, iG, iB, iA;

    if ( !Node ( strVariable ) ) return false;
    strVal = Node ( strVariable )->GetTagContent ();
    ss.str ( strVal );

    try {
        ss >> iR >> iG >> iB >> iA;
        val.R = iR;
        val.G = iG;
        val.B = iB;
        val.A = iA;
    } catch (...) {
        return false;
    }

    return true;
}


void CClientVariables::Set ( const std::string& strVariable, CVector val )
{
    std::stringstream ss;
    if ( !m_pStorage ) return;
    m_iRevision++;

    ss << val.fX << " " << val.fY << " " << val.fZ;

    std::string strVal = ss.str ();
    const char *szVal = strVal.c_str ();
    Node ( strVariable )->SetTagContent ( szVal );
}


void CClientVariables::Set ( const std::string& strVariable, CVector2D val )
{
    std::stringstream ss;
    if ( !m_pStorage ) return;
    m_iRevision++;

    ss << val.fX << " " << val.fY;

    std::string strVal = ss.str ();
    const char *szVal = strVal.c_str ();
    Node ( strVariable )->SetTagContent ( szVal );
}


void CClientVariables::Set ( const std::string& strVariable, CColor val )
{
    std::stringstream ss;
    if ( !m_pStorage ) return;
    m_iRevision++;

    ss << (unsigned int)val.R << " " << (unsigned int)val.G << " " << (unsigned int)val.B << " " << (unsigned int)val.A;

    std::string strVal = ss.str ();
    const char *szVal = strVal.c_str ();
    Node ( strVariable )->SetTagContent ( szVal );
}


CXMLNode* CClientVariables::Node ( const std::string& strVariable )
{
    CXMLNode *pNode;
    if ( !m_pStorage ) return NULL;

    // Try and grab the sub node
    pNode = m_pStorage->FindSubNode ( strVariable.c_str () );
    if ( !pNode ) {
        // Non-existant, create a new sub node
        pNode = m_pStorage->CreateSubNode ( strVariable.c_str () );
    }
    return pNode;
}


bool CClientVariables::Exists ( const std::string& strVariable )
{
    // Check whether this node exists, thus if it is a valid cvar
    if ( !m_pStorage ) return false;
    return (m_pStorage->FindSubNode ( strVariable.c_str () ) != NULL);
}


// Clamp int variable
void CClientVariables::ClampValue ( const std::string& strVariable, int iMinValue, int iMaxValue  )
{
    int iTemp;
    CVARS_GET ( strVariable, iTemp );
    iTemp = Clamp ( iMinValue, iTemp, iMaxValue );
    CVARS_SET ( strVariable, iTemp );
}


// Clamp float variable
void CClientVariables::ClampValue ( const std::string& strVariable, float fMinValue, float fMaxValue  )
{
    float fTemp;
    CVARS_GET ( strVariable, fTemp );
    fTemp = Clamp ( fMinValue, fTemp, fMaxValue );
    CVARS_SET ( strVariable, fTemp );
}


// Clamp CColor variable
void CClientVariables::ClampValue ( const std::string& strVariable, CColor minValue, CColor maxValue  )
{
    CColor temp;
    CVARS_GET ( strVariable, temp );
    temp.R = Clamp ( minValue.R, temp.R, maxValue.R );
    temp.G = Clamp ( minValue.G, temp.G, maxValue.G );
    temp.B = Clamp ( minValue.B, temp.B, maxValue.B );
    temp.A = Clamp ( minValue.A, temp.A, maxValue.A );
    CVARS_SET ( strVariable, temp );
}


// Clamp CVector2D variable
void CClientVariables::ClampValue ( const std::string& strVariable, CVector2D minValue, CVector2D maxValue  )
{
    CVector2D temp;
    CVARS_GET ( strVariable, temp );
    temp.fX = Clamp ( minValue.fX, temp.fX, maxValue.fX );
    temp.fY = Clamp ( minValue.fY, temp.fY, maxValue.fY );
    CVARS_SET ( strVariable, temp );
}


// Ensure CVars are within reasonable limits
void CClientVariables::ValidateValues ( void )
{
    uint uiViewportWidth = CCore::GetSingleton().GetGraphics ()->GetViewportWidth ();
    uint uiViewportHeight = CCore::GetSingleton().GetGraphics ()->GetViewportHeight ();

    ClampValue ( "console_pos",             CVector2D ( 0, 0 ),         CVector2D ( uiViewportWidth - 32, uiViewportHeight - 32 ) );
    ClampValue ( "console_size",            CVector2D ( 50, 50 ),       CVector2D ( uiViewportWidth - 32, uiViewportHeight - 32 ) );
    ClampValue ( "fps_limit",               0,                          100 );
    ClampValue ( "chat_font",               0,                          3 );
    ClampValue ( "chat_lines",              3,                          62 );
    ClampValue ( "chat_color",              CColor (0,0,0,0),           CColor (255,255,255,255) );
    ClampValue ( "chat_text_color",         CColor (0,0,0,128),         CColor (255,255,255,255) );
    ClampValue ( "chat_input_color",        CColor (0,0,0,0),           CColor (255,255,255,255) );
    ClampValue ( "chat_input_prefix_color", CColor (0,0,0,128),         CColor (255,255,255,255) );
    ClampValue ( "chat_input_text_color",   CColor (0,0,0,128),         CColor (255,255,255,255) );
    ClampValue ( "chat_scale",              CVector2D ( 0.5f, 0.5f ),   CVector2D ( 3, 3 ) );
    ClampValue ( "chat_width",              0.5f,                       4.f );
    ClampValue ( "chat_line_life",          1000,                       120000000 );
    ClampValue ( "chat_line_fade_out",      1000,                       30000000 );
    ClampValue ( "text_scale",              0.8f,                       3.0f );
    ClampValue ( "mtavolume",               0.0f,                       1.0f );
    ClampValue ( "voicevolume",             0.0f,                       1.0f );
    ClampValue ( "mapalpha",                0,                          255 );
}


void CClientVariables::LoadDefaults ( void )
{
    #define DEFAULT(__x,__y)    if(!Exists(__x)) \
                                Set(__x,__y)
    #define _S(__x)             std::string(__x)

    if(!Exists("nick"))
    {
        DEFAULT ( "nick",                       _S(CNickGen::GetRandomNickname()) );       // nickname
        CCore::GetSingleton ().RequestNewNickOnStart();  // Request the user to set a new nickname
    }

    DEFAULT ( "host",                       _S("127.0.0.1") );              // hostname
    DEFAULT ( "port",                       22003 );                        // port
    DEFAULT ( "password",                   _S("") );                       // password
    DEFAULT ( "qc_host",                    _S("127.0.0.1") );              // quick connect hostname
    DEFAULT ( "qc_port",                    22003 );                        // quick connect port
    DEFAULT ( "qc_password",                _S("") );                       // quick connect password
    DEFAULT ( "debugfile",                  _S("") );                       // debug filename
    DEFAULT ( "console_pos",                CVector2D ( 0, 0 ) );           // console position
    DEFAULT ( "console_size",               CVector2D ( 200, 200 ) );       // console size
    DEFAULT ( "serverbrowser_size",         CVector2D ( 720.0f, 495.0f ) ); // serverbrowser size
    DEFAULT ( "fps_limit",                  100 );                          // frame limiter
    DEFAULT ( "chat_font",                  0 );                            // chatbox font type
    DEFAULT ( "chat_lines",                 7 );                            // chatbox lines
    DEFAULT ( "chat_color",                 CColor (0,0,128,100) );         // chatbox color
    DEFAULT ( "chat_text_color",            CColor (172,213,254,255) );     // chatbox text color
    DEFAULT ( "chat_input_color",           CColor (0,0,191,110) );         // chatbox input color
    DEFAULT ( "chat_input_prefix_color",    CColor (172,213,254,255) );     // chatbox prefix input color
    DEFAULT ( "chat_input_text_color",      CColor (172,213,254,255) );     // chatbox text input color
    DEFAULT ( "chat_scale",                 CVector2D ( 1.0f, 1.0f ) );     // chatbox scale
    DEFAULT ( "chat_width",                 1.0f );                         // chatbox width
    DEFAULT ( "chat_css_style_text",        false );                        // chatbox css/hl style text
    DEFAULT ( "chat_css_style_background",  false );                        // chatbox css/hl style background
    DEFAULT ( "chat_line_life",             12000 );                        // chatbox line life time
    DEFAULT ( "chat_line_fade_out",         3000 );                         // chatbox line fade out time
    DEFAULT ( "chat_use_cegui",             false );                        // chatbox uses cegui
    DEFAULT ( "chat_nickcompletion",        true );                         // chatbox nick completion
    DEFAULT ( "server_can_flash_window",    true );                         // allow server to flash the window
    DEFAULT ( "text_scale",                 1.0f );                         // text scale
    DEFAULT ( "invert_mouse",               false );                        // mouse inverting
    DEFAULT ( "fly_with_mouse",             false );                        // flying with mouse controls
    DEFAULT ( "steer_with_mouse",           false );                        // steering with mouse controls
    DEFAULT ( "classic_controls",           false );                        // classic/standard controls
    DEFAULT ( "mtavolume",                  1.0f );                         // custom sound's volume
    DEFAULT ( "voicevolume",                1.0f );                         // voice chat output volume
    DEFAULT ( "mapalpha",                   155 );                          // map alpha
    DEFAULT ( "browser_speed",              1 );                            // Browser speed
    DEFAULT ( "single_download",            0 );                            // Single connection for downloads
    DEFAULT ( "packet_tag",                 0 );                            // Tag network packets
    DEFAULT ( "progress_animation",         1 );                            // Progress spinner at the bottom of the screen
    DEFAULT ( "update_build_type",          0 );                            // 0-stable 1-test 2-nightly
    DEFAULT ( "update_auto_install",        1 );                            // 0-off 1-on
    DEFAULT ( "volumetric_shadows",         0 );                            // Enable volumetric shadows
    DEFAULT ( "aspect_ratio",               0 );                            // Display aspect ratio
    DEFAULT ( "hud_match_aspect_ratio",     1 );                            // GTA HUD should match the display aspect ratio
    DEFAULT ( "anisotropic",                0 );                            // Anisotropic filtering
    DEFAULT ( "grass",                      1 );                            // Enable grass
    DEFAULT ( "heat_haze",                  1 );                            // Enable heat haze
    DEFAULT ( "tyre_smoke_enabled",         1 );                            // Enable tyre smoke
    DEFAULT ( "fast_clothes_loading",       1 );                            // 0-off 1-auto 2-on
    DEFAULT ( "allow_screen_upload",        1 );                            // 0-off 1-on
    DEFAULT ( "max_clientscript_log_kb",    5000 );                         // Max size in KB (0-No limit)
    DEFAULT ( "display_fullscreen_style",   0 );                            // 0-standard 1-borderless 2-borderless keep res 3-borderless stretch
    DEFAULT ( "display_windowed",           0 );                            // 0-off 1-on
    DEFAULT ( "multimon_fullscreen_minimize", 1 );                          // 0-off 1-on
    DEFAULT ( "vertical_aim_sensitivity",   0.0015f );                      // 0.0015f is GTA default setting
    DEFAULT ( "process_priority",           0 );                            // 0-normal 1-above normal 2-high
    DEFAULT ( "mute_sfx_when_minimized",    0 );                            // 0-off 1-on
    DEFAULT ( "mute_radio_when_minimized",  0 );                            // 0-off 1-on
    DEFAULT ( "mute_mta_when_minimized",    0 );                            // 0-off 1-on
    DEFAULT ( "mute_voice_when_minimized",  0 );                            // 0-off 1-on
    DEFAULT ( "share_file_cache",           1 );                            // 0-no 1-share client resource file cache with other MTA installs
    DEFAULT ( "show_unsafe_resolutions",    0 );                            // 0-off 1-show resolutions that are higher that the desktop
    DEFAULT ( "fov",                        70 );                           // Camera field of view
    DEFAULT ( "browser_remote_websites",    true );                         // Load remote websites?
    DEFAULT ( "browser_remote_javascript",  true );                         // Execute javascript on remote websites?
    DEFAULT ( "browser_plugins",            false );                         // Enable browser plugins?
    DEFAULT ( "filter_duplicate_log_lines", true );                         // Filter duplicate log lines for debug view and clientscript.log

    if (!Exists("locale"))
    {
        SString strLangCode = GetApplicationSetting ( "locale" );
        Set ( "locale", !strLangCode.empty() ? strLangCode : _S("en_US") );
    }

    // Set default resolution to native resolution
    if ( !Exists ( "display_resolution" ) )
    {
        RECT rect;
        GetWindowRect( GetDesktopWindow (), &rect );
        Set ( "display_resolution", SString ( "%dx%dx32", rect.right, rect.bottom ) );
    }

    // We will default this one at CClientGame.cpp, because we need a valid direct3d device to give a proper default value.
#if 0
    DEFAULT ( "streaming_memory",           50 );                           // Streaming memory
#endif
}
