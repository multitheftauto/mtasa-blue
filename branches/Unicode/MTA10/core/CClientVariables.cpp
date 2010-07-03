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


void CClientVariables::LoadDefaults ( void )
{
    #define DEFAULT(__x,__y)    if(!Exists(__x)) \
                                Set(__x,__y)
    #define _S(__x)             std::string(__x)

    DEFAULT ( "nick",                       _S("Player") );                 // nickname
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
    DEFAULT ( "text_scale",                 1.0f );                         // text scale
    DEFAULT ( "invert_mouse",               false );                        // mouse inverting
    DEFAULT ( "fly_with_mouse",             false );                        // flying with mouse controls
    DEFAULT ( "steer_with_mouse",           false );                        // steering with mouse controls
    DEFAULT ( "classic_controls",           false );                        // classic/standard controls
    DEFAULT ( "mtavolume",                  1.0f );                         // custom sound's volume
    DEFAULT ( "async_loading",              1 );                            // loading while crashing
    DEFAULT ( "mapalpha",                   155 );                          // map alpha
}
