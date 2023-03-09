/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCredits.cpp
 *  PURPOSE:     In-game credits window implementation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

namespace CCreditsGUI
{
    float fWindowX = 560.0f;
    float fWindowY = 300.0f;
}            // namespace CCreditsGUI
using namespace CCreditsGUI;

CCredits::CCredits()
{
    CGUI* pManager = g_pCore->GetGUI();

    // Define our credits string
    m_strCredits += "== Multi Theft Auto: San Andreas v" MTA_DM_BUILDTAG_SHORT
                    " ==\n"
                    "\n";

    m_strCredits += _("Programming");
    m_strCredits +=
        "\n\n"

        "Alberto \"ryden\" Alonso\n"
        "Marcus \"mabako\" Bauer\n"
        "Hendrik \"arc_\" van den Berge\n"
        "Stanislav \"lil_Toady\" Bobrov\n"
        "Dan \"Talidan\" Chowdhury\n"
        "Chris \"ccw\" Cockwanger\n"
        "Adge \"Jax\" Cutler\n"
        "Cecill \"ijs\" Etheredge\n"
        "Danish \"Saml1er\" Khan\n"
        "Marek \"botder\" Kulik\n"
        "Sebas \"x86\" Lamers\n"
        "Raphael \"Mr.Hankey\" Leiteritz\n"
        "Ed \"eAi\" Lyons\n"
        "Christian \"ChrML\" Myhre Lundheim\n"
        "Qais \"qaisjp\" Patankar\n"
        "Arushan \"aru\" Raj\n"
        "Frank \"Aim\" Spijkerman\n"
        "Pascal \"sbx320\" Stücker\n"
        "Kevin \"Kevuwk\" Whiteside\n"
        "Richard \"Cazomino05\" Whitlock\n"
        "Gamesnert\n"
        "Jusonex\n"
        "\n"
        "\n";

    m_strCredits += _("Contributors");
    m_strCredits +=
        "\n\n"

        "Patrik \"Pirulax\" Jankovics-Szmolenicki\n"
        "Patrik \"myonlake\" Juvonen\n"
        "Uladzislau \"TheNormalnij\" Nikalayevich\n"
        "Nikita \"StrixG\" Obrecht\n"
        "Arran\n"
        "colistro123\n"
        "impulze\n"
        "Iztas\n"
        "JoeBullet\n"
        "lopezloo\n"
        "Lpsd\n"
        "MX_Master\n"
        "Remp\n"
        "SugarD-x\n"
        "\n"
        "\n";

    m_strCredits += _("Game Design / Scripting");
    m_strCredits +=
        "\n\n"

        "John \"Johnline\" Borsberry\n"
        "Paul \"Brophy\" Brophy\n"
        "Maciej \"MACK\" Cieslak\n"
        "Jacek \"jhxp\" Halas vel Lagoda\n"
        "Brad \"Slothman\" Hammond\n"
        "Norbert \"norby89\" Juhos\n"
        "Kris \"KWKSND\" Kulchisky\n"
        "Dustin \"Ransom\" Morren\n"
        "Dutchman101\n"
        "erorr404\n"
        "Synecy\n"
        "\n"
        "\n";

    if (g_pLocalization->IsLocalized() && !g_pLocalization->GetTranslators().empty())
    {
        m_strCredits += _("Language Localization");
        m_strCredits += "\n\n";
        m_strCredits += g_pLocalization->GetTranslators();
        m_strCredits += "\n\n\n";
    }

    m_strCredits += _("Patch contributors");
    m_strCredits +=
        "\n\n"

        "Enes \"cleopatra\" Akıllıok\n"
        "Nawaf \"xLive\" Alqarni\n"
        "Nicolás \"PlatinMTA\" Barrios\n"
        "Amine \"UndR\" Bidah\n"
        "Lukasz \"W\" Biegaj\n"
        "Pacal \"buepas\" Buerklin\n"
        "Florian \"Flobu\" Busse\n"
        "Emiliano \"Sasu\" Castro\n"
        "Breno \"iDannz\" Danyel\n"
        "Callum \"Callum\" Dawson\n"
        "Pieter \"pieT\" Dewachter\n"
        "Mateusz \"Allerek\" Domińczak\n"
        "Philip \"Fenix\" Farquharson\n"
        "Seweryn \"Neproify\" Figura\n"
        "Alejandro \"AlexTMjugador\" González\n"
        "Kevin \"ciber96\" Gross\n"
        "Robin \"robhol\" Holm\n"
        "Bob \"NanoBob\" van Hooff\n"
        "Sebastian \"CrosRoad95\" Jura\n"
        "Daniel \"arfni\" Keller\n"
        "Gabrielius \"Dezash\" Laurinavičius\n"
        "John \"jlillis\" Lillis\n"
        "Kamil \"forkerer\" Marciniak\n"
        "Tete \"Tete\" Omar\n"
        "Marcin \"FileEX\" Pawlicki\n"
        "Ali \"Haxardous\" Qamber\n"
        "Sam \"samr46\" R.\n"
        "Fedor \"Ted\" Sinev\n"
        "Jordy \"MegadreamsBE\" Sleeubus\n"
        "Simon \"Simi23\" Tamás\n"
        "Adam \"50p\" Telega\n"
        "Ilya \"Kenix\" Volkov\n"
        "AboShanab\n"
        "Addlibs\n"
        "AfuSensi\n"
        "AleksCore\n"
        "AlιAѕѕaѕѕιN\n"
        "ApeLsiN4eG\n"
        "asturel\n"
        "Audifire\n"
        "Bonus1702\n"
        "correaAlex\n"
        "CubedDeath\n"
        "Deihim007\n"
        "delinx\n"
        "DetectedStorm\n"
        "DirtY_iCE\n"
        "Disinterpreter\n"
        "Dragon\n"
        "drop-club\n"
        "dvarnai\n"
        "Einheit-101\n"
        "emre1702\n"
        "Fabio(GNR)\n"
        "fastman92\n"
        "FrankZZ\n"
        "Gothem\n"
        "GRascm\n"
        "GTX / Timic3\n"
        "guix\n"
        "JeViCo\n"
        "John_Michael\n"
        "JR10\n"
        "Kayl\n"
        "knitz12\n"
        "LarSoWiTsH\n"
        "Lex128\n"
        "Lokote1998\n"
        "Loooop\n"
        "Lordmau5\n"
        "LosFaul\n"
        "lucasc190\n"
        "MCvarial\n"
        "mickdermack\n"
        "NeO_D-Power\n"
        "Patrick2562\n"
        "PauloKim1246\n"
        "Pawelo / 4O4\n"
        "pentaflops\n"
        "PerikiyoXD\n"
        "PhrozenByte\n"
        "rafalh\n"
        "samt2497\n"
        "SDraw\n"
        "se16n\n"
        "Sergeanur\n"
        "-ffs-Sniper\n"
        "STR6\n"
        "TAPL-93\n"
        "tederis\n"
        "theSarrum\n"
        "Totto8492\n"
        "tvc\n"
        "x0rh4x\n"
        "xerox8521\n"
        "[GP_A]XetaQuake\n"
        "xLuxy\n"
        "Zango\n"
        "zneext\n"
        "ZReC\n"
        "\n"
        "\n";

    m_strCredits += _("Special Thanks");
    m_strCredits +=
        "\n\n"

        "Derek \"slush\" Abdine\n"
        "Mertcan \"Rocketbunny\" Aydoğmuş\n"
        "Jordan \"Woovie\" Banasik\n"
        "Javier \"jbeta\" Beta\n"
        "Oliver \"Oli\" Brown\n"
        "Emiliano \"Sasu\" Castro\n"
        "Wojciech \"Wojjie\" Hlibowicki\n"
        "Chris \"Cray\" McArthur\n"
        "Rob \"Sugar Daddy\" Pooley\n"
        "max \"Hobo Pie\" Power\n"
        "Hans \"Blokker\" Roes\n"
        "Kent \"Kent747\" Simon\n"
        "Matthew \"Towncivilian\" Wolfe\n"
        "Adamix\n"
        "Aibo\n"
        "diegofkda\n"
        "leetIbrahim\n"
        "Phatlooser\n"
        "Pugwipe\n"
        "Ren712\n"
        "StifflersMom\n"
        "The_GTA\n"
        "\n"
        "\n";

    m_strCredits += _("This software makes use of the following libraries and software:");
    m_strCredits +=
        "\n"
        "breakpad (https://chromium.googlesource.com/breakpad/breakpad/)\n"
        "bzip2 (http://bzip.org/)\n"
        "CEF (https://bitbucket.org/chromiumembedded/cef/)\n"
        "CEGUI (http://www.cegui.org.uk/)\n"
        "cURL (https://curl.se/)\n"
        "Embedded HTTP Server (https://ehs.fritz-elfert.de/)\n"
        "inspect.lua by kikito (https://github.com/kikito/inspect.lua)\n"
        "libpcre (https://www.pcre.org/)\n"
        "libpng (http://www.libpng.org/)\n"
        "Lua (https://www.lua.org/)\n"
        "lunasvg (https://github.com/sammycage/lunasvg)\n"
        "PortAudio (http://www.portaudio.com/)\n"
        "RakNet (http://www.jenkinssoftware.com/)\n"
        "SA Limit adjuster source code (Credits to Sacky)\n"
        "speex (https://www.speex.org/)\n"
        "SQLite (https://www.sqlite.org/)\n"
        "tinygettext (https://github.com/tinygettext/tinygettext/)\n"
        "UnRAR (https://www.rarlab.com/)\n"
        "zlib (https://zlib.net/)\n";

    // Create our window
    CVector2D RelativeWindow = CVector2D(fWindowX / pManager->GetResolution().fX, fWindowY / pManager->GetResolution().fY);
    m_pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, "Multi Theft Auto: San Andreas " MTA_DM_BUILDTAG_SHORT));
    m_pWindow->SetCloseButtonEnabled(false);
    m_pWindow->SetMovable(false);
    m_pWindow->SetPosition(CVector2D(0.5f - RelativeWindow.fX * 0.5f, 0.5f - RelativeWindow.fY * 0.5f), true);
    m_pWindow->SetSize(CVector2D(fWindowX, fWindowY));            // relative 0.70, 0.50
    m_pWindow->SetSizingEnabled(false);
    m_pWindow->SetVisible(false);
    m_pWindow->SetAlwaysOnTop(true);

    // Credits label
    memset(m_pLabels, 0, sizeof(CGUILabel*) * 30);

    // Create one for every 15th line. This is because of some limit at 500 chars
    float        fStartPosition = 1.0f;
    const char*  szCreditsIterator = m_strCredits.c_str();
    const char*  szCreditsBegin = m_strCredits.c_str();
    unsigned int uiLineCount = 0;
    unsigned int uiLabelIndex = 0;
    while (true)
    {
        // Count every new line
        if (*szCreditsIterator == '\n')
            ++uiLineCount;

        // 15? Create a new label
        if (uiLineCount >= 15 || *szCreditsIterator == 0)
        {
            // Copy out the text we shall put in that label
            std::string strBuffer(szCreditsBegin, szCreditsIterator - szCreditsBegin);

            // Remember where we count from
            szCreditsBegin = szCreditsIterator;
            ++szCreditsBegin;

            // Create the label
            m_pLabels[uiLabelIndex] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, strBuffer.c_str()));
            m_pLabels[uiLabelIndex]->SetPosition(CVector2D(0.022f, fStartPosition), true);
            m_pLabels[uiLabelIndex]->SetSize(CVector2D(532.0f, 1200.0f));            // relative 0.95, 6.0
            m_pLabels[uiLabelIndex]->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
            ++uiLabelIndex;

            // Reset the linecount
            uiLineCount = 0;
        }

        // End the loop at 0
        if (*szCreditsIterator == 0)
            break;

        // Increase the credits iterator
        ++szCreditsIterator;
    }

    // Create the OK button
    m_pButtonOK = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, "OK"));
    m_pButtonOK->SetPosition(CVector2D(0.77f, 0.90f), true);
    m_pButtonOK->SetSize(CVector2D(112.0f, 21.0f));            // relative 0.20, 0.07
    m_pButtonOK->SetVisible(true);
    m_pButtonOK->SetAlwaysOnTop(true);

    // Set up the event handlers
    m_pButtonOK->SetClickHandler(GUI_CALLBACK(&CCredits::OnOKButtonClick, this));
    m_pWindow->SetEnterKeyHandler(GUI_CALLBACK(&CCredits::OnOKButtonClick, this));
}

CCredits::~CCredits()
{
    // Delete all the labels
    int i = 0;
    for (; i < 30; i++)
    {
        if (m_pLabels[i])
        {
            delete m_pLabels[i];
            m_pLabels[i] = NULL;
        }
    }

    // Delete the controls
    delete m_pButtonOK;

    // Delete our window
    delete m_pWindow;
}

void CCredits::Update()
{
    if (m_pWindow->IsVisible())
    {
        // Speed it up if arrow keys are being held
        if ((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0)
        {
            m_clkStart -= 150;
        }
        if ((GetAsyncKeyState(VK_UP) & 0x8000) != 0)
        {
            m_clkStart += 150;
        }

        // Calculate the current position
        float fCurrentTop = 1.0f - 0.0001f * static_cast<float>(clock() - m_clkStart);

        // If we're too far down or up, make sure we restart next pulse
        if (fCurrentTop <= -11.0f || fCurrentTop >= 1.0f)
        {
            m_clkStart = clock();
        }

        // Move everything by the time
        int i = 0;
        for (; i < 30; i++)
        {
            if (m_pLabels[i])
            {
                m_pLabels[i]->SetPosition(CVector2D(0.022f, fCurrentTop + i * 0.725f), true);
            }
        }
    }
}

void CCredits::SetVisible(bool bVisible)
{
    m_pWindow->SetVisible(bVisible);
    m_pWindow->BringToFront();
    m_clkStart = clock();
}

bool CCredits::IsVisible()
{
    return m_pWindow->IsVisible();
}

bool CCredits::OnOKButtonClick(CGUIElement* pElement)
{
    CMainMenu* pMainMenu = CLocalGUI::GetSingleton().GetMainMenu();

    // Close the window
    m_pWindow->SetVisible(false);
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}
