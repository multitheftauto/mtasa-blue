/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CChat.cpp
*  PURPOSE:		In-game chat box user interface implementation
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

CChat * g_pChat = NULL;

CChat::CChat ( CGUI* pManager, CVector2D & vecPosition )
{    
    g_pChat = this;
    m_pManager = pManager;  

    // Calculate relative position (assuming a 800x600 native resolution for our defined CCHAT_* values)
    CVector2D vecResolution = m_pManager->GetResolution ();
    m_vecScale = CVector2D ( vecResolution.fX / 800.0f, vecResolution.fY / 600.0f );
    vecPosition = vecPosition * vecResolution;
    m_vecBackgroundPosition = vecPosition;

    // Initialize variables
    m_TextColor = CHAT_TEXT_COLOR;
    m_bUseCEGUI = false;
    m_iCVarsRevision = -1;
    m_szCommand = NULL;
    m_bVisible = false;
    m_bInputVisible = false;
    m_pFont = m_pManager->GetClearFont ();
    m_pDXFont = g_pCore->GetGraphics ()->GetFont ();
    m_fNativeWidth = CHAT_WIDTH;
    m_bCanChangeWidth = true;

    // Background area
    m_pBackground = m_pManager->CreateStaticImage ();
    m_pBackgroundTexture = m_pManager->CreateTexture ();
    m_pBackground->LoadFromTexture ( m_pBackgroundTexture );
    m_pBackground->MoveToBack ();
    m_pBackground->SetPosition ( m_vecBackgroundPosition );
    m_pBackground->SetSize ( m_vecBackgroundSize );
    m_pBackground->SetEnabled ( false );
    m_pBackground->SetVisible ( false );

    // Input area
    m_pInputLine = new CChatInputLine;
    m_szInputText = NULL;
    m_pInput = m_pManager->CreateStaticImage ();
    m_pInputTexture = m_pManager->CreateTexture ();
    m_pInput->LoadFromTexture ( m_pInputTexture );
    m_pInput->MoveToBack ();
    m_pInput->SetPosition ( m_vecInputPosition );
    m_pInput->SetSize ( m_vecInputSize );
    m_pInput->SetEnabled ( false );
    m_pInput->SetVisible ( false );
    SetInputPrefix ( "Say: " );

    // Position the GUI
    UpdateGUI ();

    // Set handlers
    m_pManager->SetCharacterKeyHandler ( GUI_CALLBACK_KEY ( &CChat::CharacterKeyHandler, this ) );

    // Load cvars
    LoadCVars ();
}


CChat::~CChat ( void )
{
    Clear ();
    ClearInput ();

    SAFE_DELETE ( m_pBackground );
    SAFE_DELETE ( m_pBackgroundTexture );
    SAFE_DELETE ( m_pInput );
    SAFE_DELETE ( m_pInputTexture );
    SAFE_DELETE ( m_pInputLine );

    if ( m_szInputText ) delete [] m_szInputText;
    if ( m_szCommand ) delete [] m_szCommand;
    if ( g_pChat == this ) g_pChat = NULL;
}


void CChat::LoadCVars ( void )
{
    unsigned int Font;
    float fWidth = 1;

    CVARS_GET ( "chat_color",                   m_Color );              if( m_bCanChangeWidth ) SetColor ( m_Color );
    CVARS_GET ( "chat_input_color",             m_InputColor );         SetInputColor ( m_InputColor );
    CVARS_GET ( "chat_use_cegui",               m_bUseCEGUI );
    CVARS_GET ( "chat_lines",                   m_uiNumLines );         SetNumLines ( m_uiNumLines);
    CVARS_GET ( "chat_text_color",              m_TextColor );
    CVARS_GET ( "chat_scale",                   m_vecScale );
    CVARS_GET ( "chat_width",                   fWidth );               if( m_bCanChangeWidth ) m_fNativeWidth = fWidth * CHAT_WIDTH;
    CVARS_GET ( "chat_css_style_text",          m_bCssStyleText );
    CVARS_GET ( "chat_css_style_background",    m_bCssStyleBackground );
    CVARS_GET ( "chat_line_life",               (unsigned int &)m_ulChatLineLife );
    CVARS_GET ( "chat_line_fade_out",           (unsigned int &)m_ulChatLineFadeOut );
    CVARS_GET ( "chat_font",                    (unsigned int &)Font ); SetChatFont ( (eChatFont)Font );

    UpdateGUI ();
}


void CChat::Draw ( void )
{
    // Are we visible?
    if ( !m_bVisible )
        return;

    // Is it time to update all the chat related cvars?
    if( m_iCVarsRevision != CClientVariables::GetSingleton ().GetRevision () ) {
        m_iCVarsRevision = CClientVariables::GetSingleton ().GetRevision ();
        LoadCVars ();
    }

    float fLineDifference = CChat::GetFontHeight ( g_pChat->m_vecScale.fY );
    CVector2D vecPosition ( m_vecBackgroundPosition.fX + ( 5.0f * g_pChat->m_vecScale.fX ), m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY - ( fLineDifference * 1.25f ) );
    unsigned char ucAlpha = 0;
    unsigned long ulTime = GetTickCount ();
    unsigned long ulLineAge = 0;
    bool bShadow = ( m_Color.A == 0 || ( m_bCssStyleText && !m_bInputVisible ) );
    bool bInputShadow = ( m_InputColor.A == 0 );

    bool bDrawBackground = false;
    if ( m_bCssStyleBackground )
        bDrawBackground = ( m_bVisible && m_bInputVisible && m_Color.A != 0 );
    else
        bDrawBackground = ( m_bVisible && m_Color.A != 0 );

    if ( bDrawBackground )
    {
        // Hack to draw the background behind the text.
        m_pBackground->SetVisible ( true );
        m_pBackground->Render ();
        m_pBackground->SetVisible ( false );
    }

    if ( m_bInputVisible )
    {
        // ChrML: Hack so chatbox input always works. It might get unfocused..
        if ( !m_pBackground->IsActive () )
        {
            m_pBackground->Activate ();
        }
    }

    list < CChatLine* > ::iterator iter = m_Lines.begin ();
    for ( unsigned int i = 0 ; ( iter != m_Lines.end () && i < m_uiNumLines ) ; iter++, i++ )
    {
		// HACK: added to stop any unusual crashes for now, can't see any cause for this
        CChatLine* pLine = *iter;
        if ( pLine )
        {
            ucAlpha = 255;
            if ( m_bCssStyleText )
            {
                if ( !m_bInputVisible )
                {
                    ulLineAge = ulTime - pLine->GetCreationTime ();
                    if ( ulLineAge > m_ulChatLineLife )
                    {
                        if ( ulLineAge > ( m_ulChatLineLife + m_ulChatLineFadeOut ) )
                        {
                            ucAlpha = 0;
                        }
                        else
                        {
                            float fOver = float ( ( m_ulChatLineLife + m_ulChatLineFadeOut ) - ulLineAge );
                            fOver /= ( float ) m_ulChatLineFadeOut;
                            ucAlpha = unsigned char ( fOver * 255.0f );
                        }
                    }
                }
            }

            if ( ucAlpha > 0 )
                pLine->Draw ( vecPosition, ucAlpha, bShadow );

            vecPosition.fY -= fLineDifference;
        }
    }

    if ( m_pInputLine && m_bInputVisible )
    {
        if ( m_InputColor.A != 0 )
        {
            if ( m_pInput )
            {
                // Hack to draw the input background behind the text.
                m_pInput->SetVisible ( true );
                m_pInput->Render ();
                m_pInput->SetVisible ( false );
            }
        }

        CVector2D vecPosition ( m_vecInputPosition.fX + ( 5.0f * g_pChat->m_vecScale.fX ), m_vecInputPosition.fY + ( fLineDifference * 0.125f ) );
        if ( m_pInputLine ) m_pInputLine->Draw ( vecPosition, 255, bInputShadow );
    }    
}


void CChat::Output ( char* szText, bool bColorCoded )
{
    CChatLine* pLine = NULL;
    char* szRemainingText = szText;
    unsigned int uiCharsUsed = 0, uiTempCharsUsed = 0;
    do
    {
        pLine = new CChatLine;
        if ( pLine )
        {
            szRemainingText = pLine->Format ( szRemainingText, ( m_vecBackgroundSize.fX - ( 10.0f * g_pChat->m_vecScale.fX ) ), m_TextColor, bColorCoded, uiTempCharsUsed );
            uiCharsUsed += uiTempCharsUsed;
            m_Lines.push_front ( pLine );

            unsigned int uiLines = static_cast < unsigned int > ( m_Lines.size () );
            if ( uiLines > CHAT_MAX_LINES )
            {
                for ( unsigned int i = 0 ; i < ( uiLines - CHAT_MAX_LINES ) ; i++ )
                {
                    CChatLine* pLastLine = m_Lines.back ();
                    delete pLastLine;
                    m_Lines.pop_back ();
                }
            }
        }
    }
    while ( szRemainingText );
}


void CChat::Outputf ( bool bColorCoded, char* szText, ... )
{
    char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szText );
	_VSNPRINTF ( szBuffer, 1024, szText, ap );
	va_end ( ap );

    Output ( szBuffer, bColorCoded );
}


void CChat::Clear ( void )
{
    list < CChatLine* > ::iterator iter = m_Lines.begin ();
    for ( ; iter != m_Lines.end () ; iter++ )
    {
        delete *iter;
    }
    m_Lines.clear ();
}


void CChat::ClearInput ( void )
{
    if ( m_szInputText )
    {
        delete [] m_szInputText;
        m_szInputText = NULL;
    }
    if ( m_pInputLine )
    {
        m_pInputLine->Clear ();    
        m_vecInputSize = CVector2D ( m_vecBackgroundSize.fX, ( CChat::GetFontHeight ( g_pChat->m_vecScale.fY ) * ( ( float ) m_pInputLine->m_ExtraLines.size () + 1.25f ) ) );
        if ( m_pInput ) m_pInput->SetSize ( m_vecInputSize );
    }
}


bool CChat::CharacterKeyHandler ( CGUIKeyEventArgs KeyboardArgs )
{
    char szTemp [ CHAT_BUFFER ] = { 0 };
    char* szInputText = GetInputText ();
    unsigned int uiInputTextLength = 0;

	// Copy the input text
    if ( szInputText )
    {
		uiInputTextLength = strlen ( szInputText );
		if ( uiInputTextLength > CHAT_BUFFER )
			uiInputTextLength = CHAT_BUFFER - 1;

        strncpy ( szTemp, szInputText, uiInputTextLength );
        szTemp [ 1023 ] = 0;
    }

    // If we can take input
	if ( m_bInputVisible && CLocalGUI::GetSingleton ().GetVisibleWindows () == 0 &&
        !CLocalGUI::GetSingleton ().GetConsole ()->IsVisible () )
    {
        // Check if it's a special key like enter and backspace, if not, add it as a character to the message
        switch ( KeyboardArgs.codepoint )
        {
            // Backspace
            case 0x08:
            {                
                if ( uiInputTextLength > 0 )
                {
                    szTemp [ uiInputTextLength - 1 ] = 0;
                    SetInputText ( szTemp );
                }
                break;
            }

            // Enter
            case 0x0D:
            {
                // Empty the chat and hide the input stuff
                // If theres a command to call, call it
                if ( m_szCommand && szTemp [ 0 ] )
                    CCommands::GetSingleton().Execute ( m_szCommand, szTemp );
			
				// Deactivate the VisibleWindows counter
				CLocalGUI::GetSingleton ().SetVisibleWindows ( false );
                SetInputVisible ( false );

                break;
            }             
            
            default:
            {
                // If we haven't exceeded the maximum number of characters per chat message, append the char to the message and update the input control
                if ( uiInputTextLength < CHAT_MAX_CHAT_LENGTH )
                {                    
                    if ( KeyboardArgs.codepoint >= 32 && KeyboardArgs.codepoint <= 126 )
                    {
                        szTemp [ uiInputTextLength ] = KeyboardArgs.codepoint;
                        szTemp [ uiInputTextLength + 1 ] = 0;
                        SetInputText ( szTemp );
                    }
                }
                break;
            }
        }
    }

	return true;
}


void CChat::SetVisible ( bool bVisible )
{
    m_bVisible = bVisible;
}


void CChat::SetInputVisible ( bool bVisible )
{    
    if ( !IsVisible () )
        bVisible = false;

    if ( bVisible )
    {
        ClearInput ();
    }

    m_bInputVisible = bVisible;
}


void CChat::SetNumLines ( unsigned int uiNumLines )
{
    if ( uiNumLines <= CHAT_MAX_LINES )
    {
        m_uiNumLines = uiNumLines;
        UpdateGUI ();
    }
}


void CChat::SetChatFont ( eChatFont Font )
{
    CGUIFont* pFont = g_pCore->GetGUI ()->GetDefaultFont ();
    ID3DXFont* pDXFont = g_pCore->GetGraphics ()->GetFont ();
    switch ( Font )
    {
        case CHAT_FONT_DEFAULT:
            pFont = g_pCore->GetGUI ()->GetDefaultFont ();
			pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_DEFAULT );
            break;
        case CHAT_FONT_CLEAR:
            pFont = g_pCore->GetGUI ()->GetClearFont ();
			pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_CLEAR );
            break;
        case CHAT_FONT_BOLD:
            pFont = g_pCore->GetGUI ()->GetBoldFont ();
            pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_DEFAULT_BOLD );
            break;
        case CHAT_FONT_ARIAL:
			pDXFont = g_pCore->GetGraphics ()->GetFont ( FONT_ARIAL );
            break;                
    }

    // Set fonts
    g_pChat->m_pFont = pFont;
    g_pChat->m_pDXFont = pDXFont;
}


void CChat::UpdateGUI ( void )
{
    m_vecBackgroundSize = CVector2D (
        m_fNativeWidth * g_pChat->m_vecScale.fX,
        CChat::GetFontHeight ( g_pChat->m_vecScale.fY ) * (float(m_uiNumLines) + 0.5f)
    );
    m_pBackground->SetSize ( m_vecBackgroundSize );

    m_vecInputPosition = CVector2D (
        m_vecBackgroundPosition.fX,
        m_vecBackgroundPosition.fY + m_vecBackgroundSize.fY
    );
    m_vecInputSize = CVector2D (
        m_vecBackgroundSize.fX,
        CChat::GetFontHeight ( g_pChat->m_vecScale.fY ) * 1.25f
    );
    if ( m_pInput )
    {
        m_pInput->SetPosition ( m_vecInputPosition );
        m_pInput->SetSize ( m_vecInputSize );
    }
}


void CChat::SetColor ( CColor& Color )
{
    unsigned long ulBackgroundColor = COLOR_ARGB ( Color.A, Color.R, Color.G, Color.B );

    m_pBackgroundTexture->LoadFromMemory ( &ulBackgroundColor, 1, 1 );
    m_pBackground->LoadFromTexture ( m_pBackgroundTexture );
}


void CChat::SetInputColor ( CColor& Color )
{
    unsigned long ulInputColor = COLOR_ARGB ( Color.A, Color.R, Color.G, Color.B );

    if ( m_pInputTexture ) m_pInputTexture->LoadFromMemory ( &ulInputColor, 1, 1 );
    if ( m_pInput ) m_pInput->LoadFromTexture ( m_pInputTexture );
}


char* CChat::GetInputPrefix ( void )
{
    if ( m_pInputLine )
    {
        return m_pInputLine->m_pPrefix->GetText ();
    }
    return NULL;
}


void CChat::SetInputPrefix ( char* szPrefix )
{
    if ( m_pInputLine )
    {
        m_pInputLine->m_pPrefix->SetText ( szPrefix );
    }
}


char* CChat::GetInputText ( void )
{
    return m_szInputText;
}


void CChat::SetInputText ( char* szText )
{
    if ( !m_pInputLine ) return;

    ClearInput ();

    CColor color;
    m_pInputLine->m_pText->GetColor ( color );
    unsigned int uiCharsUsed = 0, uiTempCharsUsed = 0;
    char* szRemainingText = m_pInputLine->Format ( szText, ( m_vecInputSize.fX - ( 10.0f * g_pChat->m_vecScale.fX ) - m_pInputLine->m_pPrefix->GetWidth () ), color, false, uiTempCharsUsed );
    uiCharsUsed += uiTempCharsUsed;
    CChatLine* pLine = NULL;
    
    while ( szRemainingText && m_pInputLine->m_ExtraLines.size () < 3 )
    {
        pLine = new CChatLine;
        if ( pLine )
        {
            szRemainingText = pLine->Format ( szRemainingText, ( m_vecInputSize.fX - ( 10.0f * g_pChat->m_vecScale.fX ) ), color, false, uiTempCharsUsed );
            uiCharsUsed += uiTempCharsUsed;
            m_pInputLine->m_ExtraLines.push_back ( pLine );
        }
    }

    if ( m_szInputText )
    {
        delete [] m_szInputText;
        m_szInputText = NULL;
    }
    if ( szText )
    {
        m_szInputText = new char [ uiCharsUsed + 1 ];
        memcpy ( m_szInputText, szText, uiCharsUsed );
        m_szInputText [ uiCharsUsed ] = 0;
    }

    m_vecInputSize = CVector2D ( m_vecBackgroundSize.fX, ( CChat::GetFontHeight ( g_pChat->m_vecScale.fY ) * ( ( float ) m_pInputLine->m_ExtraLines.size () + 1.25f ) ) );
    if ( m_pInput ) m_pInput->SetSize ( m_vecInputSize );
}


void CChat::SetCommand ( char* szCommand )
{
    if ( m_szCommand )
    {
        delete [] m_szCommand;
        m_szCommand = NULL;
    }
    if ( szCommand )
    {
        m_szCommand = new char [ strlen ( szCommand ) + 1 ];
        strcpy ( m_szCommand, szCommand );

        if ( strcmp ( szCommand, "chatboxsay" ) == 0 )
        {
            SetInputPrefix ( "Say: " );
        }
        else
        {
            char* szTemp = new char [ strlen ( m_szCommand ) + 3 ];
            sprintf ( szTemp, "%s: ", m_szCommand );
            *szTemp = toupper ( *szTemp );
            SetInputPrefix ( szTemp );
            delete [] szTemp;
        }
    }
}


float CChat::GetFontHeight ( float fScale )
{
    if ( g_pChat->m_bUseCEGUI )
    {
        return g_pChat->m_pFont->GetFontHeight ( fScale );
    }
    return g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, g_pChat->m_pDXFont );
}


float CChat::GetCharacterWidth ( int iChar, float fScale )
{
    if ( g_pChat->m_bUseCEGUI )
    {
        return g_pChat->m_pFont->GetCharacterWidth ( iChar, fScale );
    }
    return g_pCore->GetGraphics ()->GetDXCharacterWidth ( ( char ) iChar, fScale, g_pChat->m_pDXFont );
}


float CChat::GetTextExtent ( const char * szText, float fScale )
{
    if ( g_pChat->m_bUseCEGUI )
    {
        return g_pChat->m_pFont->GetTextExtent ( szText, fScale );
    }
    return g_pCore->GetGraphics ()->GetDXTextExtent ( szText, fScale, g_pChat->m_pDXFont );
}


void CChat::DrawTextString ( const char * szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY )
{
    if ( g_pChat->m_bUseCEGUI )
    {
        g_pChat->m_pFont->DrawTextString ( szText, DrawArea, fZ, ClipRect, ulFormat, ulColor, fScaleX, fScaleY );
    }
    else
    {
        g_pCore->GetGraphics ()->DrawText ( ( int ) DrawArea.fX1, ( int ) DrawArea.fY1, ( int ) DrawArea.fX1, ( int ) DrawArea.fY1, ulColor, szText, fScaleX, fScaleY, DT_LEFT | DT_TOP | DT_NOCLIP, g_pChat->m_pDXFont );
    }
}


CChatLine::CChatLine ( void )
{
    m_szText = NULL;
    m_ulCreationTime = GetTickCount ();
}


CChatLine::~CChatLine ( void )
{
    if ( m_szText )
    {
        delete [] m_szText;
        m_szText = NULL;
    }
    list < CChatLineSection* > ::iterator iter = m_Sections.begin ();
    for ( ; iter != m_Sections.end () ; iter++ )
    {
        delete *iter;
    }
    m_Sections.clear ();
}


bool CChatLine::IsNumber ( char c )
{
    switch ( c )
    {
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9': case 'A': case 'B':
        case 'C': case 'D': case 'E': case 'F':
            return true;
    }
    return false;
}


unsigned char CChatLine::GetNumber ( char c )
{
    switch ( c )
    {
        case '0': return 0; case '1': return 1; case '2': return 2; case '3': return 3;
        case '4': return 4; case '5': return 5; case '6': return 6; case '7': return 7;
        case '8': return 8; case '9': return 9; case 'A': return 10; case 'B': return 11;
        case 'C': return 12; case 'D': return 13; case 'E': return 14; case 'F': return 15;
    }
    return 0;
}   


char* CChatLine::Format ( char* szString, float fWidth, CColor& color, bool bColorCoded, unsigned int& uiCharsUsed )
{
    // *#RRGGBB*
    char* szText = new char [ strlen ( szString ) + 1 ];
    strcpy ( szText, szString );    
    
    unsigned int uiTextLength = static_cast < unsigned int > ( strlen ( szText ) );

    // List the color markers
    list < SMarker* > colorMarkers;
    if ( bColorCoded )
    {
        for ( unsigned int i = 0 ; i < uiTextLength ; i++ )
        {
            char c = szText [ i ];
            if ( c == '#' )
            {
                unsigned int uiMarkerEnd = i + 7;
                if ( uiTextLength >= uiMarkerEnd )
                {
                    unsigned int j;

                    for ( j = i + 1 ; j < uiMarkerEnd ; j++ )
                    {
                        if ( !IsNumber ( szText [ j ] ) )
                        {
                            break;
                        }
                    }
                    if ( j == uiMarkerEnd )
                    {
                        SMarker* pMarker = new SMarker;
                        pMarker->uiPosition = i;
                        pMarker->m_Color.R = GetNumber ( szText [ i + 1 ] );
                        pMarker->m_Color.R = pMarker->m_Color.R << 4;
                        pMarker->m_Color.R |= GetNumber ( szText [ 1 + 2 ] );
                        pMarker->m_Color.G = GetNumber ( szText [ i + 3 ] );
                        pMarker->m_Color.G = pMarker->m_Color.G << 4;
                        pMarker->m_Color.G |= GetNumber ( szText [ 1 + 4 ] );
                        pMarker->m_Color.B = GetNumber ( szText [ i + 5 ] );
                        pMarker->m_Color.B = pMarker->m_Color.B << 4;
                        pMarker->m_Color.B |= GetNumber ( szText [ 1 + 6 ] );
                        pMarker->m_Color.A = 255;
                        colorMarkers.push_back ( pMarker );
                    }
                }
            }
        }
    }

    float fCurrentWidth = 0.0f;
    unsigned int uiStartPosition = 0;
    list < SMarker* > ::iterator iter = colorMarkers.begin ();
    for ( ; iter != colorMarkers.end () ; iter++ )
    {
        SMarker* pMarker = *iter;
        if ( pMarker->uiPosition != uiStartPosition )
        {            
            char c = szText [ pMarker->uiPosition ];
            szText [ pMarker->uiPosition ] = 0;
            // Wrap
            for ( unsigned int i = uiStartPosition ; i < pMarker->uiPosition ; i++ )
            {               
                char cTemp = szText [ i ];
                // Newline?                
                if ( cTemp == '\n' )
                {
                    szText [ i ] = 0;
                    CChatLineSection* pSection = new CChatLineSection;
                    pSection->SetText ( &szText [ uiStartPosition ] );
                    pSection->SetColor ( color );
                    m_Sections.push_back ( pSection );
                    AddText ( &szText [ uiStartPosition ] );
                    delete [] szText;
                    uiCharsUsed = i + 1;
                    return &szString [ uiCharsUsed ];
                }

                float fCharWidth = CChat::GetCharacterWidth ( cTemp, g_pChat->m_vecScale.fX );
                if ( ( fCurrentWidth + fCharWidth ) > fWidth )
                {            
                    // Word wrap: find the last space
                    int breakAt = i;
                    for ( unsigned int j = i ; j > uiStartPosition ; j-- )
                    {
                        if ( szText [ j ] == ' ' )
                        {
                            breakAt = j;
                            break;
                        }                        
                    }
                    szText [ breakAt ] = 0;
                    CChatLineSection* pSection = new CChatLineSection;
                    pSection->SetText ( &szText [ uiStartPosition ] );
                    pSection->SetColor ( color );
                    m_Sections.push_back ( pSection );
                    AddText ( &szText [ uiStartPosition ] );
                    delete [] szText;
                    uiCharsUsed = breakAt;
                    return &szString [ uiCharsUsed ];
                }                
                fCurrentWidth += fCharWidth;
            }
            CChatLineSection* pSection = new CChatLineSection;
            pSection->SetText ( &szText [ uiStartPosition ] );
            pSection->SetColor ( color );
            m_Sections.push_back ( pSection );
            AddText ( &szText [ uiStartPosition ] );
            szText [ pMarker->uiPosition ] = c;
        }
        color = pMarker->m_Color;
        uiStartPosition = pMarker->uiPosition + 7;
        delete pMarker;
    }
    colorMarkers.clear ();

    // Wrap
    for ( unsigned int i = uiStartPosition ; i < strlen ( szText ) ; i++ )
    {        
        char cTemp = szText [ i ];
        // Newline?
        if ( cTemp == '\n' )
        {
            szText [ i ] = 0;
            CChatLineSection* pSection = new CChatLineSection;
            pSection->SetText ( &szText [ uiStartPosition ] );
            pSection->SetColor ( color );
            m_Sections.push_back ( pSection );
            AddText ( &szText [ uiStartPosition ] );
            delete [] szText;
            uiCharsUsed = i + 1;
            return &szString [ uiCharsUsed ];
        }

        float fCharWidth = CChat::GetCharacterWidth ( cTemp, g_pChat->m_vecScale.fX );
        if ( ( fCurrentWidth + fCharWidth ) > fWidth )
        {
            // Word wrap: find the last space
            int breakAt = i;
            for ( unsigned int j = i ; j > uiStartPosition ; j-- )
            {
                if ( szText [ j ] == ' ' )
                {
                    breakAt = j;
                    break;
                }                        
            }
            szText [ breakAt ] = 0;
            CChatLineSection* pSection = new CChatLineSection;
            pSection->SetText ( &szText [ uiStartPosition ] );
            pSection->SetColor ( color );
            m_Sections.push_back ( pSection );
            AddText ( &szText [ uiStartPosition ] );
            delete [] szText;
            uiCharsUsed = breakAt;
            return &szString [ uiCharsUsed ];
        }
        fCurrentWidth += fCharWidth;
    }
    CChatLineSection* pSection = new CChatLineSection;
    pSection->SetText ( &szText [ uiStartPosition ] );
    pSection->SetColor ( color );
    m_Sections.push_back ( pSection );
    AddText ( &szText [ uiStartPosition ] );

    delete [] szText;
    uiCharsUsed = uiTextLength;
    return NULL;
}


void CChatLine::Draw ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow )
{
    float fCurrentX = vecPosition.fX;
    list < CChatLineSection* > ::iterator iter = m_Sections.begin ();
    for ( ; iter != m_Sections.end () ; iter++ )
    {
        CChatLineSection* pSection = *iter;
        pSection->Draw ( CVector2D ( fCurrentX, vecPosition.fY ), ucAlpha, bShadow );
        fCurrentX += pSection->GetWidth ();
    }
}


float CChatLine::GetWidth ( unsigned int uiLength )
{
    float fWidth = 0.0f;
    if ( uiLength && uiLength < m_uiLength )
    {
        char c = m_szText [ uiLength ];
        m_szText [ uiLength ] = 0;
        fWidth = CChat::GetTextExtent ( m_szText, g_pChat->m_vecScale.fX );
        m_szText [ uiLength ] = c;
    }
    else
    {
        fWidth = CChat::GetTextExtent ( m_szText, g_pChat->m_vecScale.fX );
    }
    return fWidth;
}


void CChatLine::RemoveColorCode ( char* szString, char* szReturn, unsigned int uiReturnLength )
{
    char* szText = new char [ strlen ( szString ) + 1 ];
    strcpy ( szText, szString );
    char szTemp [ 1024 ] = { 0 };
    
    unsigned int uiTextLength = static_cast < unsigned int > ( strlen ( szText ) );
    unsigned int uiLastMarkerEnd = 0;
    for ( unsigned int i = 0 ; i < uiTextLength ; i++ )
    {
        char c = szText [ i ];
        if ( c == '#' )
        {
            unsigned int uiMarkerEnd = i + 7;
            if ( uiTextLength >= uiMarkerEnd )
            {
                unsigned int j;

                for ( j = i + 1 ; j < uiMarkerEnd ; j++ )
                {
                    if ( !IsNumber ( szText [ j ] ) )
                    {
                        break;
                    }
                }
                if ( j == uiMarkerEnd )
                {
                    szText [ i ] = 0;
                    sprintf ( szTemp, "%s%s", szTemp, &szText [ uiLastMarkerEnd ] );
                    szText [ i ] = c;
                    uiLastMarkerEnd = uiMarkerEnd;
                }
            }
        }
    }
    if ( uiLastMarkerEnd != uiTextLength )
    {
        sprintf ( szTemp, "%s%s", szTemp, &szText [ uiLastMarkerEnd ] );
    }
    delete [] szText;

    strncpy ( szReturn, szTemp, uiReturnLength );
    szReturn [ uiReturnLength - 1 ] = 0;
}


void CChatLine::SetText ( char* szText )
{
    if ( m_szText )
    {
        delete [] m_szText;
        m_szText = NULL;
    }
    if ( szText )
    {
        m_uiLength = strlen ( szText );
        m_szText = new char [ m_uiLength + 1 ];
        strcpy ( m_szText, szText );
    }
}


void CChatLine::AddText ( char* szText )
{
    if ( m_szText )
    {
        m_uiLength = strlen ( m_szText ) + strlen ( szText );
        char* szTemp = new char [ m_uiLength + 1 ];
        sprintf ( szTemp, "%s%s", m_szText, szText );
        delete [] m_szText;
        m_szText = new char [ m_uiLength + 1 ];
        strcpy ( m_szText, szTemp );
        delete [] szTemp;
    }
    else
    {
        m_uiLength = strlen ( szText );
        m_szText = new char [ m_uiLength + 1 ];
        strcpy ( m_szText, szText );
    }
}


CChatInputLine::CChatInputLine ( void )
{
    m_pPrefix = new CChatLineSection;
    m_pText = new CChatLineSection;
    m_uiEditPosition = 0;
}


CChatInputLine::~CChatInputLine ( void )
{
    delete m_pPrefix;
    delete m_pText;
}


void CChatInputLine::Draw ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow )
{
    CColor colTemp;
    m_pPrefix->GetColor ( colTemp );
    if ( colTemp.A > 0 )
        m_pPrefix->Draw ( vecPosition, colTemp.A, bShadow );

    m_pText->GetColor ( colTemp );
    if ( colTemp.A > 0 )
    {
        m_pText->Draw ( CVector2D ( vecPosition.fX + m_pPrefix->GetWidth (), vecPosition.fY ), colTemp.A, bShadow );

        float fLineDifference = CChat::GetFontHeight ( g_pChat->m_vecScale.fY );

        list < CChatLine* > ::iterator iter = m_ExtraLines.begin ();
        for ( ; iter != m_ExtraLines.end () ; iter++ )
        {
            vecPosition.fY += fLineDifference;
            (*iter)->Draw ( vecPosition, colTemp.A, bShadow );        
        }
    }
}


char* CChatInputLine::Format ( char* szString, float fWidth, CColor& color, bool bColorCoded, unsigned int& uiCharsUsed )
{
    char* szText = new char [ strlen ( szString ) + 1 ];
    strcpy ( szText, szString );    
    
    unsigned int uiTextLength = static_cast < unsigned int > ( strlen ( szText ) );

    float fCurrentWidth = 0.0f;

    // Wrap
    for ( unsigned int i = 0 ; i < uiTextLength ; i++ )
    {        
        char cTemp = szText [ i ];
        float fCharWidth = CChat::GetCharacterWidth ( cTemp, g_pChat->m_vecScale.fX );
        if ( ( fCurrentWidth + fCharWidth ) > fWidth )
        {          
            // Word wrap: find the last space
            int breakAt = i;
            for ( unsigned int j = i ; j > 0 ; j-- )
            {
                if ( szText [ j ] == ' ' )
                {
                    breakAt = j;
                    break;
                }
            }
            szText [ breakAt ] = 0;            
            m_pText->SetText ( szText );
            m_pText->SetColor ( color );
            AddText ( szText );
            delete [] szText;
            uiCharsUsed = breakAt;
            return &szString [ breakAt ];
        }
        fCurrentWidth += fCharWidth;
    }
    CChatLineSection* pSection = new CChatLineSection;
    m_pText->SetText ( szText );
    m_pText->SetColor ( color );
    AddText ( szText );

    delete [] szText;
    uiCharsUsed = uiTextLength;
    return NULL;
}


void CChatInputLine::Clear ( void )
{
    list < CChatLine* > ::iterator iter = m_ExtraLines.begin ();
    for ( ; iter != m_ExtraLines.end () ; iter++ )
    {
        delete *iter;
    }
    m_ExtraLines.clear ();
    m_pText->SetText ( NULL );    
}


void CChatInputLine::SetEditPosition ( CVector2D& vecOrigin, unsigned int uiPosition )
{
    m_uiEditPosition = uiPosition;
    m_vecEditPosition = vecOrigin;
    if ( m_uiEditPosition == 0 )
    {
        m_vecEditPosition.fX += m_pPrefix->GetWidth ();
    }
    else
    {
        unsigned int uiOverallLength = 0, uiLength = 0;
        uiOverallLength = uiLength = m_pText->GetLength ();
        if ( uiOverallLength >= m_uiEditPosition )
        {
            m_vecEditPosition.fX += m_pPrefix->GetWidth () + m_pText->GetWidth ( m_uiEditPosition );
        }
        else
        {
            float fLineDifference = CChat::GetFontHeight ( g_pChat->m_vecScale.fY );

            list < CChatLine* > ::iterator iter = m_ExtraLines.begin ();
            for ( ; iter != m_ExtraLines.end () ; iter++ )
            {
                CChatLine* pLine = *iter;
                uiLength = pLine->GetLength ();
                m_vecEditPosition.fY += fLineDifference;
                if ( uiLength + uiOverallLength >= m_uiEditPosition )
                {
                    m_vecEditPosition.fX += pLine->GetWidth ( m_uiEditPosition - uiOverallLength );
                    break;
                }
                uiOverallLength += uiLength;
            }
        }
    }
}


CChatLineSection::CChatLineSection ( void )
{
    m_szText = NULL;
    m_uiLength = 0;
}


CChatLineSection::~CChatLineSection ( void )
{
    delete [] m_szText;
}


void CChatLineSection::Draw ( CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow )
{
    if ( m_szText )
    {
        if ( ucAlpha > 0 )
        {
            if ( bShadow )
            {
                CRect2D drawShadowAt ( vecPosition.fX + 1.0f, vecPosition.fY + 1.0f, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f );
                CChat::DrawTextString ( m_szText, drawShadowAt, 0.0f, drawShadowAt, 0, COLOR_ARGB ( ucAlpha, 0, 0, 0 ), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY );                
            }
            CRect2D drawAt ( vecPosition.fX, vecPosition.fY, vecPosition.fX + 1000.0f, vecPosition.fY + 1000.0f );
            CChat::DrawTextString ( m_szText, drawAt, 0.0f, drawAt, 0, COLOR_ARGB ( ucAlpha, m_Color.R, m_Color.G, m_Color.B ), g_pChat->m_vecScale.fX, g_pChat->m_vecScale.fY );
        }
    }
}


float CChatLineSection::GetWidth ( unsigned int uiLength )
{
    float fWidth = 0.0f;
    if ( m_szText )
    {
        for ( unsigned int i = 0 ; i < strlen ( m_szText ) ; i++ )
        {
            if ( uiLength && uiLength == i )
                break;
            fWidth += CChat::GetCharacterWidth ( m_szText [ i ], g_pChat->m_vecScale.fX );            
        }
    }
    return fWidth;
}


void CChatLineSection::SetText ( char* szText )
{
    if ( m_szText )
    {
        delete [] m_szText;
        m_szText = NULL;
    }
    if ( szText )
    {
        m_uiLength = strlen ( szText );
        m_szText = new char [ m_uiLength + 1 ];
        strcpy ( m_szText, szText );
    }
}