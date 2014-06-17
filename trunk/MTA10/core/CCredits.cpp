/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCredits.cpp
*  PURPOSE:     In-game credits window implementation
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

namespace CCreditsGUI
{
    float fWindowX =            560.0f;
    float fWindowY =            300.0f;
}
using namespace CCreditsGUI;

CCredits::CCredits ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Define our credits string
    m_strCredits +=           "== Multi Theft Auto: San Andreas v" MTA_DM_BUILDTAG_SHORT " ==\n"
                              "\n";

    m_strCredits +=           _("Programming");
    m_strCredits +=           "\n\n"

                              "Alberto \"ryden\" Alonso\n"
                              "Marcus \"mabako\" Bauer\n"
                              "Hendrik \"arc_\" van den Berge\n"
                              "Stanislav \"lil_Toady\" Bobrov\n"
                              "Dan \"Talidan\" Chowdhury\n"
                              "Chris \"ccw\" Cockwanger\n"
                              "Adge \"Jax\" Cutler\n"
                              "Cecill \"ijs\" Etheredge\n"
                              "Sebas \"x86\" Lamers\n"
                              "Raphael \"Mr.Hankey\" Leiteritz\n"
                              "Ed \"eAi\" Lyons\n"
                              "Christian \"ChrML\" Myhre Lundheim\n"
                              "Arushan \"aru\" Raj\n"
                              "Frank \"Aim\" Spijkerman\n"
                              "Kevin \"Kevuwk\" Whiteside\n"
                              "Richard \"Cazomino05\" Whitlock\n"
                              "Gamesnert\n"
                              "Jusonex\n"
                              "sbx320\n"
                              "\n"
                              "\n";

    m_strCredits +=           _("Contributors");
    m_strCredits +=           "\n\n"

                              "Arran\n"
                              "Remp\n"
                              "MX_Master\n"
                              "Iztas\n"
                              "impulze\n"
                              "JoeBullet\n"
                              "qaisjp\n"
                              "\n";

    m_strCredits +=           _("Game Design / Scripting");
    m_strCredits +=           "\n\n"

                              "John \"Johnline\" Borsberry\n"
                              "Paul \"Brophy\" Brophy\n"
                              "Maciej \"MACK\" Cieslak\n"
                              "Jacek \"jhxp\" Halas vel Lagoda\n"
                              "Brad \"Slothman\" Hammond\n"
                              "Norbert \"norby89\" Juhos\n"
                              "Kris \"KWKSND\" Kulchisky\n"
                              "Dustin \"Ransom\" Morren\n"
                              "erorr404\n"
                              "Synecy\n"
                              "\n"
                              "\n";

    m_strCredits +=           _("Patch contributors");
    m_strCredits +=           "\n\n"
                              
                              "Lukasz \"W\" Biegaj\n"
                              "Florian \"Flobu\" Busse\n"
                              "Philip \"Fenix\" Farquharson\n"
                              "Robin \"robhol\" Holm\n"
                              "Adam \"50p\" Telega\n"
                              "Callum \"Callum\" Dawson\n"
                              "Ilya \"Kenix\" Volkov\n"
                              "Gothem\n"
                              "rafalh\n"
                              "Totto8492\n"
                              "CubedDeath\n"
                              "GRascm\n"
                              "NeO_D-Power\n"
                              "Dragon\n"
                              "DirtY_iCE\n"
                              "Zango\n"
                              "lucasc190\n"
                              "samt2497\n"
                              "MCvarial\n"
                              "tvc\n"
                              "Audifire\n"
                              "Kayl\n"
                              "mickdermack\n"
                              "Fabio(GNR)\n"
                              "LarSoWiTsH\n"
                              "dvarnai\n"
                              "Loooop\n"
                              "Lex128\n"
                              "x0rh4x\n"
                              "guix\n"
                              "[GP_A]XetaQuake\n"
                              "asturel\n"
                              "FrankZZ\n"
                              "GA_Justus\n"
                              "John_Michael\n"
                              "-ffs-Sniper\n"
                              "W\n"
                              "TheKiller 'Dutchman' 101\n"
                              "Fedor Sinev\n"
                              "\n";
                              "\n";

    if ( g_pLocalization->IsLocalized() && !g_pLocalization->GetTranslators().empty() ) 
    {
        m_strCredits +=           _("Language Localization");
        m_strCredits +=           "\n\n";
        m_strCredits +=           g_pLocalization->GetTranslators();
        m_strCredits +=           "\n\n\n";
    }

    m_strCredits +=           _("Special Thanks");
    m_strCredits +=           "\n\n"

                              "Derek \"slush\" Abdine\n"
                              "Javier \"jbeta\" Beta\n"
                              "Oliver \"Oli\" Brown\n"
                              "Wojciech \"Wojjie\" Hlibowicki\n"
                              "Chris \"Cray\" McArthur\n"
                              "Hans \"Blokker\" Roes\n"
                              "Kent \"Kent747\" Simon\n"
                              "Matthew \"Towncivilian\" Wolfe\n"
                              "Adamix\n"
                              "Aibo\n"
                              "Pugwipe\n"
                              "The_GTA\n"
                              "Phatlooser\n"
                              "Dwayne 'The 'Woovie' Rock' Johnson\n"
                              "max 'Hobo Pie' Power\n"
                              "Rob 'Sugar Daddy' Pooley\n"
                              "diegofkda\n"
                              "Ren712\n"
                              "\n"
                              "\n"
                             
                              "\n";
    m_strCredits +=           _("This software makes use of the following libraries and software:");
    m_strCredits +=           "\n"
                              "RakNet (http://www.jenkinssoftware.com)\n"
                              "SA Limit adjuster source code (Credits to Sacky)\n"
                              "CEGUI (http://www.cegui.org.uk/wiki/index.php/Main_Page)\n"
                              "cURL (http://curl.haxx.se/)\n"
                              "libpcre (http://www.pcre.org/)\n"
                              "Lua (http://www.lua.org/)\n"
                              "SQLite (http://www.sqlite.org/)\n"
                              "libpng (http://www.libpng.org/)\n"
                              "Embedded HTTP Server (http://ehs.fritz-elfert.de/)\n"
                              "zlib (http://zlib.net/)\n"
                              "bzip2 (http://bzip.org/)\n"
                              "UnRAR (http://www.rarlab.com/)\n"
                              "tinygettext (http://code.google.com/p/tinygettext/)\n"
                              "PortAudio (http://www.portaudio.com/)\n"
                              "speex (http://www.speex.org/)\n"
                              ;

    // Create our window
    CVector2D RelativeWindow = CVector2D ( fWindowX / pManager->GetResolution ().fX, fWindowY / pManager->GetResolution ().fY );
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Multi Theft Auto: San Andreas " MTA_DM_BUILDTAG_SHORT ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetMovable ( false );
    m_pWindow->SetPosition ( CVector2D ( 0.5f - RelativeWindow.fX*0.5f, 0.5f - RelativeWindow.fY*0.5f ), true );
    m_pWindow->SetSize ( CVector2D ( fWindowX, fWindowY ) );        // relative 0.70, 0.50
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetVisible ( false );
    m_pWindow->SetAlwaysOnTop ( true );

    // Credits label
    memset ( m_pLabels, 0, sizeof ( CGUILabel* ) * 30 );

    // Create one for every 15th line. This is because of some limit at 500 chars
    float fStartPosition = 1.0f;
    const char* szCreditsIterator = m_strCredits.c_str();
    const char* szCreditsBegin = m_strCredits.c_str();
    unsigned int uiLineCount = 0;
    unsigned int uiLabelIndex = 0;
    while ( true )
    {
        // Count every new line
        if ( *szCreditsIterator == '\n' )
            ++uiLineCount;

        // 15? Create a new label
        if ( uiLineCount >= 15 || *szCreditsIterator == 0 )
        {
            // Copy out the text we shall put in that label
            char szBuffer [512];
            unsigned int uiCreditsSize = (szCreditsIterator - szCreditsBegin);
            if ( uiCreditsSize >= 512 )
                uiCreditsSize = 511;
            memcpy ( szBuffer, szCreditsBegin, uiCreditsSize );
            szBuffer [uiCreditsSize] = 0;

            // Remember where we count from
            szCreditsBegin = szCreditsIterator;
            ++szCreditsBegin;

            // Create the label
            m_pLabels [uiLabelIndex] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, szBuffer ) );
            m_pLabels [uiLabelIndex]->SetPosition ( CVector2D ( 0.022f, fStartPosition ), true );
            m_pLabels [uiLabelIndex]->SetSize ( CVector2D ( 532.0f, 1200.0f ) );            // relative 0.95, 6.0
            m_pLabels [uiLabelIndex]->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
            ++uiLabelIndex;
            
            // Reset the linecount
            uiLineCount = 0;
        }

        // End the loop at 0
        if ( *szCreditsIterator == 0 )
            break;

        // Increase the credits iterator
        ++szCreditsIterator;
    }

    // Create the OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "OK" ) );
    m_pButtonOK->SetPosition ( CVector2D ( 0.77f, 0.90f ), true );
    m_pButtonOK->SetSize ( CVector2D ( 112.0f, 21.0f ) );       // relative 0.20, 0.07
    m_pButtonOK->SetVisible ( true );
    m_pButtonOK->SetAlwaysOnTop ( true );

    // Set up the event handlers
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CCredits::OnOKButtonClick, this ) );
    m_pWindow->SetEnterKeyHandler ( GUI_CALLBACK ( &CCredits::OnOKButtonClick, this ) );
}


CCredits::~CCredits ( void )
{
    // Delete all the labels
    int i = 0;
    for ( ; i < 30; i++ )
    {
        if ( m_pLabels [i] )
        {
            delete m_pLabels [i];
            m_pLabels [i] = NULL;
        }
    }

    // Delete the controls
    delete m_pButtonOK;

    // Delete our window
    delete m_pWindow;
}

void CCredits::Update ( void )
{
    if ( m_pWindow->IsVisible () )
    {
        // Speed it up if control is being held
        if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) != 0 )
        {
            m_clkStart -= 50;
        }

        // Calculate the current position
        float fCurrentTop = 1.0f - 0.0001f * static_cast < float > ( clock () - m_clkStart );

        // If we're too far down, make sure we restart next pulse
        if ( fCurrentTop <= -6.0f )
        {
            m_clkStart = clock ();
        }

        // Move everything by the time
        int i = 0;
        for ( ; i < 30; i++ )
        {
            if ( m_pLabels [i] )
            {
                m_pLabels [i] -> SetPosition ( CVector2D ( 0.022f, fCurrentTop + i * 0.725f ), true );
            }
        }
    }
}

void CCredits::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    m_pWindow->BringToFront ();
    m_clkStart = clock ();
}


bool CCredits::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


bool CCredits::OnOKButtonClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Close the window
    m_pWindow->SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}
