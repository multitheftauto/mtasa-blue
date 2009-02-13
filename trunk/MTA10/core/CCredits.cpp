/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCredits.cpp
*  PURPOSE:		In-game credits window implementation
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

const char g_szCredits [] = "== The Multi Theft Auto Development Team ==\n" \
                            "Cecill \"IJs\" Etheredge\n" \
                            "Chris \"Cray\" McArthur\n" \
                            "Christian \"ChrML\" Lundheim\n" \
                            "Derek \"Slush\" Abdine\n" \
                            "Ed \"eAi\" Lyons\n" \
                            "Adge \"Jax\" Cutler\n" \
                            "Kent \"Kent747\" Simon\n" \
                            "Kevin \"Kevuwk\" Whiteside\n" \
                            "Oliver \"Oli\" Brown\n" \
                            "Stanislav \"lil_Toady\" Bobrov\n"
                            "Alberto \"ryden\" Alonso\n" \
                            "\n" \

                            "== Special Thanks ==\n" \
                            "Anne-Jan \"AJH\" Hak\n" \
                            "Alexandru \"Rzialix\" Panait\n" \
                            "Bill \"Sintax\" Blaiklock\n" \
                            "Bloodymess\n" \
                            "Chris \"RISO\" Hancock\n" \
                            "Hans \"Blokker\" Roes\n" \
                            "Headfonez\n" \
							"kktos\n" \
							"MrBump\n" \
							"ohsix\n" \
                            "Pieter \"trx\" Plantenga\n" \
							"spec\n" \
							"xerox\n" \
							"Wojjie\n" \
                            "\n" \

                            "== The Multi Theft Auto Website Development Team ==\n" \
							"AlienX\n" \
                            "Arc_\n" \
                            "driver2\n" \
                            "lil_Toady\n" \
                            "\n" \

							"== The Multi Theft Auto Quality Assurance Team Managers ==\n" \
                            "jhxp\n" \
                            "Ransom\n" \
							"\n" \

                            "== The Multi Theft Auto Quality Assurance Team ==\n" \
                            "AlienX\n" \
                            "arc_\n" \
                            "BB101\n" \
                            "Boss\n" \
                            "Brophy\n" \
                            "deez\n" \
                            "Dragon (Black Dragon)\n" \
                            "driver2\n" \
                            "erorr404\n" \
                            "Fedor\n" \
                            "Iggy\n" \
                            "jbeta\n" \
                            "Johnline\n" \
                            "Joriz\n" \
                            "Kcuf\n" \
                            "lil_Toady\n" \
                            "Lucif3r\n" \
                            "MACK\n" \
                            "MeanpantheR\n" \
                            "nick\n" \
                            "norby89\n" \
                            "Outback\n" \
                            "Paradox\n" \
                            "RAF\n" \
                            "Ratt\n" \
                            "ryden\n" \
                            "Scooby\n" \
                            "SinnerG (TvKe)\n" \
                            "Sintax\n" \
                            "Slick\n" \
                            "Slothman\n" \
                            "Solide\n" \
                            "Talidan\n" \
                            "Tweak\n" \
                            "Vandalite\n" \
                            "x16\n" \
                            "Xanarki\n" \
                            "xerox\n" \
			                "\n" \

                            "== Past contributors ==\n" \
                            "Aeron\n" \
                            "Au{R}oN\n" \
                            "Bishop\n" \
                            "Boro\n" \
                            "Boss (Thesord)\n" \
                            "Des (Marcel)\n" \
                            "Dome\n" \
                            "DominionSpy\n" \
                            "DracoBlue\n" \
                            "Dross\n" \
                            "Fusion\n" \
                            "Gamefreek\n" \
                            "Harry\n" \
                            "HAZJ\n" \
                            "Hedning\n" \
                            "JackC\n" \
                            "Jigga\n" \
                            "Kalle\n" \
                            "KriS\n" \
                            "MAD_BOY\n" \
                            "Mif\n" \
                            "Monk\n" \
                            "ParoXum\n" \
                            "PhatLooser\n" \
                            "Prontera\n" \
                            "rebel (l2ebel)\n" \
                            "Samppa\n" \
                            "Samurai_Ohk\n" \
                            "SGB\n" \
                            "TheBoas\n" \
                            "Twig\n" \
                            "VRocker\n" \
                            "Welder\n" \
                            "Zerbian\n" \
                            "Zircon (Blue Zircon)\n";

namespace CCreditsGUI
{
	float fWindowX =			560.0f;
	float fWindowY =			300.0f;
}
using namespace CCreditsGUI;

CCredits::CCredits ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Create our window
	CVector2D RelativeWindow = CVector2D ( fWindowX / pManager->GetResolution ().fX, fWindowY / pManager->GetResolution ().fY );
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, MTA_FORMAL_VERSION_TAG ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetMovable ( false );
    m_pWindow->SetPosition ( CVector2D ( 0.5f - RelativeWindow.fX*0.5f, 0.5f - RelativeWindow.fY*0.5f ), true );
    m_pWindow->SetSize ( CVector2D ( fWindowX, fWindowY ) );		// relative 0.70, 0.50
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetVisible ( false );
    m_pWindow->SetAlwaysOnTop ( true );

    // Credits label
	memset ( m_pLabels, 0, sizeof ( CGUILabel* ) * 30 );

	// Create one for every 15th line. This is because of some limit at 500 chars
    float fStartPosition = 1.0f;
	const char* szCreditsIterator = g_szCredits;
    const char* szCreditsBegin = g_szCredits;
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
	        m_pLabels [uiLabelIndex]->SetSize ( CVector2D ( 532.0f, 1200.0f ) );			// relative 0.95, 6.0
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
    m_pButtonOK->SetSize ( CVector2D ( 112.0f, 21.0f ) );		// relative 0.20, 0.07
    m_pButtonOK->SetVisible ( true );

    // Set up the event handlers
    m_pButtonOK->SetOnClickHandler ( GUI_CALLBACK ( &CCredits::OnOKButtonClick, this ) );
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
        if ( GetAsyncKeyState ( VK_CONTROL ) != 0 )
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
