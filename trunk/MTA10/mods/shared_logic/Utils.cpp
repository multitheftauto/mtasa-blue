/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CUtils.cpp
*  PURPOSE:     Util functions
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Derek Abdine <>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

const char* GetFilenameFromPath ( const char* szPath )
{
    if ( szPath && szPath [ 0 ] )
    {
        int uiLength = static_cast < int > ( strlen ( szPath ) );
        for ( int i = uiLength - 1 ; i >= 1 ; i-- )
        {
            if ( szPath [ i ] == '/' || szPath [ i ] == '\\' )
            {
                return &szPath [ i + 1 ];
            }
        }
    }
    return NULL;
}

bool DoesFileExist ( const char* szFilename )
{
    // Check that the file exists
    WIN32_FIND_DATA wFind;
    HANDLE hFind = FindFirstFile ( szFilename, &wFind );
    if ( hFind == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    // Clean up
    FindClose ( hFind );
    return true;
}


char* ReplaceAnyStringOccurrence ( char* szBuffer, const char* szWhat, const char* szWith, size_t sizeMax )
{
    // TODO: Check for max size
    // Copy the input buffer
    char* szTemp = new char [ strlen ( szBuffer ) + 1 ];
    strcpy ( szTemp, szBuffer );

    // Clear the original string
    strcpy ( szBuffer, "" );

    // Grab the size of what to replace and what to replace with
    size_t sizeWhat = strlen ( szWhat );
    size_t sizeWith = strlen ( szWith );

    // Iterate through the original string
    char* szSourceIterator = szTemp;
    char* szDestIterator = szBuffer;
    unsigned int uiCurrentSize = 0;
    while ( *szSourceIterator )
    {
        if ( strncmp ( szSourceIterator, szWhat, sizeWhat ) == 0 )
        {
            strcpy ( szDestIterator, szWith );

            // Inc the stuff
            uiCurrentSize += static_cast < unsigned int > ( sizeWith );
            szSourceIterator += sizeWhat;
            szDestIterator += sizeWith;
        }
        else
        {
            *szDestIterator = *szSourceIterator;
            ++uiCurrentSize;
            ++szSourceIterator;
            ++szDestIterator;
        }
    }

    // Free the copy and return the buffer
    delete [] szTemp;
    return szBuffer;
}


unsigned int StripUnwantedCharacters ( char* szText, unsigned char cReplace )
{
    // Replace any unwanted character with a space
    unsigned int uiReplaced = 0;
    char* szTemp = szText;
    while ( *szTemp != 0 )
    {
        if ( !IsWantedCharacter ( *szTemp ) )
        {
            *szTemp = cReplace;
            ++uiReplaced;
        }

        ++szTemp;
    }

    // Return how many characters we replaced
    return uiReplaced;
}


bool IsWantedCharacter ( unsigned char c )
{
    // 32..126 are visible characters
    return c >= 32 && c <= 126;
}


bool IsValidFilePath ( const char *szDir )
{
	if ( szDir == NULL ) return false;

	unsigned int uiLen = strlen ( szDir );
	unsigned char c, c_d;
	
	// iterate through the char array
	for ( unsigned int i = 0; i < uiLen; i++ ) {
		c = szDir[i];										// current character
		c_d = ( i < ( uiLen - 1 ) ) ? szDir[i+1] : 0;		// one character ahead, if any
		if ( !IsWantedCharacter ( c ) || c == ':' || ( c == '.' && c_d == '.' ) || ( c == '\\' && c_d == '\\' ) )
			return false;
	}
	return true;
}

void ReplaceOccurrencesInString ( std::string &s, const char *a, const char *b )
{
    int idx = 0;
    while( (idx=s.find_first_of(a, idx)) >= 0 )
        s.replace(idx, 1, b);
}

void RaiseFatalError ( unsigned int uiCode )
{
#ifdef MTA_DEBUG
    assert ( 0 );
#endif

    // Populate the message and show the box
    SString strBuffer ( "Fatal error (%u). If this problem persists, please check out mtasa.com for support.", uiCode );
    g_pCore->ShowMessageBox ( "Fatal error", strBuffer, MB_BUTTON_OK | MB_ICON_ERROR );

    // Request the mod unload
    g_pCore->GetModManager ()->RequestUnload ();
}


void RaiseProtocolError ( unsigned int uiCode )
{
//#ifdef MTA_DEBUG
//    assert ( 0 );
//#endif

    // Populate the message and show the box
    SString strBuffer ( "Protocol error (%u). If this problem persists, please check out mtasa.com for support.", uiCode );
    g_pCore->ShowMessageBox ( "Connection error", strBuffer, MB_BUTTON_OK | MB_ICON_ERROR );

    // Request the mod unload
    g_pCore->GetModManager ()->RequestUnload ();
}


void RotateVector ( CVector& vecLine, const CVector& vecRotation )
{
    // Rotate it along the X axis
    // [ 1     0        0    0 ]
    // [ 0   cos a   sin a   0 ]
    // [ 0  -sin a   cos a   0 ]

    float fLineY = vecLine.fY;
    vecLine.fY = cos ( vecRotation.fX ) * fLineY  + sin ( vecRotation.fX ) * vecLine.fZ;
    vecLine.fZ = -sin ( vecRotation.fX ) * fLineY + cos ( vecRotation.fX ) * vecLine.fZ;

    // Rotate it along the Y axis
    // [ cos a   0   -sin a   0 ]
    // [   0     1     0      0 ]
    // [ sin a   0    cos a   0 ]

    float fLineX = vecLine.fX;
    vecLine.fX = cos ( vecRotation.fY ) * fLineX - sin ( vecRotation.fY ) * vecLine.fZ;
    vecLine.fZ = sin ( vecRotation.fY ) * fLineX + cos ( vecRotation.fY ) * vecLine.fZ;

    // Rotate it along the Z axis
    // [  cos a   sin a   0   0 ]
    // [ -sin a   cos a   0   0 ]
    // [    0       0     1   0 ]

    fLineX = vecLine.fX;
    vecLine.fX = cos ( vecRotation.fZ ) * fLineX  + sin ( vecRotation.fZ ) * vecLine.fY;
    vecLine.fY = -sin ( vecRotation.fZ ) * fLineX + cos ( vecRotation.fZ ) * vecLine.fY;
}
void AttachedMatrix ( CMatrix & matrix, CMatrix & returnMatrix, CVector vecDirection, CVector vecRotation )
{    
    CVector vecMatRotation;
    g_pMultiplayer->ConvertMatrixToEulerAngles ( matrix, vecMatRotation );

    RotateVector ( vecDirection, vecMatRotation );

    returnMatrix.vPos = matrix.vPos + vecDirection;
   
    vecMatRotation += vecRotation;
    g_pMultiplayer->ConvertEulerAnglesToMatrix ( returnMatrix, vecMatRotation );
}


void LongToDottedIP ( unsigned long ulIP, char* szDottedIP )
{
	in_addr in;
	in.s_addr = ulIP;;
	char* szTemp = inet_ntoa ( in );
    if ( szTemp )
    {
        strncpy ( szDottedIP, szTemp, 22 );
    }
    else
    {
        szDottedIP [0] = 0;
    }
}

float GetRandomFloat ( void )
{
    return static_cast < float > ( rand () ) / ( static_cast < float > ( RAND_MAX ) + 1.0f );
}


double GetRandomDouble ( void )
{
    return static_cast < double > ( rand () ) / ( static_cast < double > ( RAND_MAX ) + 1.0 );
}


unsigned int GetRandom ( unsigned int uiLow, unsigned int uiHigh )
{
    double dLow = static_cast < double > ( uiLow );
    double dHigh = static_cast < double > ( uiHigh );

    return static_cast < unsigned int > ( floor ( ( dHigh - dLow + 1.0 ) * GetRandomDouble () ) ) + uiLow;
}


SString GetDataUnit ( unsigned int uiInput )
{
    // Convert it to a float
    float fInput = static_cast < float > ( uiInput );

    // Bytes per sec?
    if ( fInput < 1024 )
    {
        return SString ( "%u B", uiInput );
    }

    // Kilobytes per sec?
    fInput /= 1024;
    if ( fInput < 1024 )
    {
        return SString ( "%.2f kB", fInput );
    }

    // Megabytes per sec?
    fInput /= 1024;
    if ( fInput < 1024 )
    {
        return SString ( "%.2f MB", fInput );
    }

    // Gigabytes per sec?
    fInput /= 1024;
    if ( fInput < 1024 )
    {
        return SString ( "%.2f GB", fInput );
    }

    // Terrabytes per sec?
    fInput /= 1024;
    if ( fInput < 1024 )
    {
        return SString ( "%.2f TB", fInput );
    }

    // Unknown
    SString strUnknown = "X";
    return strUnknown;
}



#include <direct.h>
void MakeSureDirExists ( const char* szPath )
{
    // Copy the path
    char szCopy [MAX_PATH];
    strncpy ( szCopy, szPath, MAX_PATH );

    // Begin from the start
    char cChar = 0;
    char* szIter = szCopy;
    while ( *szIter != 0 )
    {
        // Met a slash?
        cChar = *szIter;
        if ( cChar == '\\' ||
             cChar == '/' )
        {
            // Replace it temprarily with 0
            *szIter = 0;

            // Call mkdir on this path
            mkdir ( szCopy );

            // Make it a slash again
            *szIter = cChar;
        }

        // Increment iterator
        ++szIter;
    }
}


#ifdef MTA_DEBUG
HMODULE RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath)
{
    /* Called correctly? */
    if ( szLibPath == NULL )
    {
        return 0;
    }

    /* Allocate memory in the remote process for the library path */
    HANDLE hThread = 0;
    void* pLibPathRemote = NULL;
    HMODULE hKernel32 = GetModuleHandle( "Kernel32" );
    pLibPathRemote = VirtualAllocEx( hProcess, NULL, strlen ( szLibPath ) + 1, MEM_COMMIT, PAGE_READWRITE );
    
    if ( pLibPathRemote == NULL )
    {
        DWORD dwError = GetLastError();
        return 0;
    }

    /* Make sure pLibPathRemote is always freed */
    __try {

        /* Write the DLL library path to the remote allocation */
        DWORD byteswritten = 0;
        WriteProcessMemory ( hProcess, pLibPathRemote, (void*)szLibPath, strlen ( szLibPath ) + 1, &byteswritten );

        if ( byteswritten != strlen ( szLibPath ) + 1 )
        {
            return 0;
        }

        /* Start a remote thread executing LoadLibraryA exported from Kernel32. Passing the
           remotly allocated path buffer as an argument to that thread (and also to LoadLibraryA)
           will make the remote process load the DLL into it's userspace (giving the DLL full
           access to the game executable).*/
        hThread = CreateRemoteThread(   hProcess,
                                        NULL,
                                        0,
                                        reinterpret_cast < LPTHREAD_START_ROUTINE > ( GetProcAddress ( hKernel32, "LoadLibraryA" ) ),
                                        pLibPathRemote,
                                        0,
                                        NULL);

        if ( hThread == 0 )
        {
            return 0;
        }


    } __finally {
        VirtualFreeEx( hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );
    }

    /*  We wait for the created remote thread to finish executing. When it's done, the DLL
        is loaded into the game's userspace, and we can destroy the thread-handle. We wait
        5 seconds which is way longer than this should take to prevent this application
        from deadlocking if something goes really wrong allowing us to kill the injected
        game executable and avoid user inconvenience.*/
    WaitForSingleObject ( hThread, INFINITE );

    /* Get the handle of the remotely loaded DLL module */
    DWORD hLibModule = 0;
    GetExitCodeThread ( hProcess, &hLibModule );

    // Wait for the LoadLibrary thread to finish
    WaitForSingleObject( hThread, INFINITE );

    /* Clean up the resources we used to inject the DLL */
    VirtualFreeEx (hProcess, pLibPathRemote, strlen ( szLibPath ) + 1, MEM_RELEASE );

    /* Success */
    return ( HINSTANCE )( 1 );
}

#endif

unsigned int HexToInt ( const char * szHex )
{
    unsigned int value = 0;
    
    if ( szHex[0] == '0' && ( szHex[0] == 'x' || szHex[0] == 'X' ) )
    {
        szHex += 2;
    }

    while ( *szHex != 0 )
    {
        char c = *szHex++;

        value <<= 4;

        if ( c >= 'A' && c <= 'F' )
        {
            value += c - 'A' + 10;
        }
        else if ( c >= 'a' && c <= 'f' )
        {
            value += c - 'a' + 10;
        }
        else
        {
            value += c - '0';
        }
    }

    return value;
}


bool XMLColorToInt ( const char* szColor, unsigned long& ulColor )
{
    const char validHexChars[] = "0123456789ABCDEFabcdef";

    unsigned int uiLength = strlen ( szColor );
    // Make sure it starts with #
    if ( szColor [0] == '#' )
    {
        // #RGB #RGBA #RRGGBB #RRGGBBAA
        if ( uiLength >= 4 && szColor [4] == 0 )
        {
            // Make a RRGGBBAA string
            unsigned char szTemp [12];
            szTemp [0] = 'F';
            szTemp [1] = 'F';
            szTemp [2] = szColor [3];
            szTemp [3] = szColor [3];
            szTemp [4] = szColor [2];
            szTemp [5] = szColor [2];
            szTemp [6] = szColor [1];
            szTemp [7] = szColor [1];
            szTemp [8] = 0;

            // Make sure it only contains 0-9 and A-F
            if ( strspn ( (const char*) szTemp, validHexChars ) == 8 )
            {
                ulColor = HexToInt ( (const char*) szTemp );
                return true;
            }
        }
        else if ( uiLength >= 5 && szColor [5] == 0 )
        {
            // Make a RRGGBBAA string
            unsigned char szTemp [12];
            szTemp [0] = szColor [4];
            szTemp [1] = szColor [4];
            szTemp [2] = szColor [3];
            szTemp [3] = szColor [3];
            szTemp [4] = szColor [2];
            szTemp [5] = szColor [2];
            szTemp [6] = szColor [1];
            szTemp [7] = szColor [1];
            szTemp [8] = 0;

            // Make sure it only contains 0-9 and A-F
            if ( strspn ( (const char*) szTemp, validHexChars ) == 8 )
            {
                ulColor = HexToInt ( (const char*) szTemp );
                return true;
            }
        }
        else if ( uiLength >= 7 && szColor [7] == 0 )
        {
            // Make a RRGGBBAA string
            unsigned char szTemp [12];
            szTemp [0] = 'F';
            szTemp [1] = 'F';
            szTemp [2] = szColor [5];
            szTemp [3] = szColor [6];
            szTemp [4] = szColor [3];
            szTemp [5] = szColor [4];
            szTemp [6] = szColor [1];
            szTemp [7] = szColor [2];
            szTemp [8] = 0;

            // Make sure it only contains 0-9 and A-F
            if ( strspn ( (const char*) szTemp, validHexChars ) == 8 )
            {
                ulColor = HexToInt ( (const char*) szTemp );
                return true;
            }
        }
        else if ( uiLength >= 9 && szColor [9] == 0 )
        {
            // Copy the string without the pre-#
            unsigned char szTemp [12];
            szTemp [0] = szColor [7];
            szTemp [1] = szColor [8];
            szTemp [2] = szColor [5];
            szTemp [3] = szColor [6];
            szTemp [4] = szColor [3];
            szTemp [5] = szColor [4];
            szTemp [6] = szColor [1];
            szTemp [7] = szColor [2];
            szTemp [8] = 0;

            // Make sure it only contains 0-9 and A-F
            if ( strspn ( (const char*) szTemp, validHexChars ) == 8 )
            {
                ulColor = HexToInt ( (const char*) szTemp );
                return true;
            }
        }
    }

    return false;
}

bool XMLColorToInt ( const char* szColor, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha )
{
    // Convert it to an integer first
    unsigned long ulColor;
    if ( !XMLColorToInt ( szColor, ulColor ) )
    {
        return false;
    }

    // Convert it to red, green, blue and alpha
    ucRed = static_cast < unsigned char > ( ulColor );
    ucGreen = static_cast < unsigned char > ( ulColor >> 8 );
    ucBlue = static_cast < unsigned char > ( ulColor >> 16 );
    ucAlpha = static_cast < unsigned char > ( ulColor >> 24 );
    return true;
}

unsigned int HashString ( const char* szString )
{
    // Implementation of Bob Jenkin's awesome hash function
    // Ref: http://burtleburtle.net/bob/hash/doobs.html

    register const char* k;             //< pointer to the string data to be hashed
    register unsigned int a, b, c;      //< temporary variables
    register unsigned int len;          //< length of the string left

    unsigned int length = (unsigned int) strlen ( szString );

    k = szString;
    len = length;
    a = b = 0x9e3779b9;
    c = 0xabcdef89;                     // initval, arbitrarily set

    while ( len >= 12 )
    {
        a += ( k[0] + ((unsigned int)k[1]<<8) + ((unsigned int)k[2]<<16)  + ((unsigned int)k[3]<<24) );
        b += ( k[4] + ((unsigned int)k[5]<<8) + ((unsigned int)k[6]<<16)  + ((unsigned int)k[7]<<24) );
        c += ( k[8] + ((unsigned int)k[9]<<8) + ((unsigned int)k[10]<<16) + ((unsigned int)k[11]<<24) );

        // Mix
        a -= b; a -= c; a ^= ( c >> 13 );
        b -= c; b -= a; b ^= ( a << 8 );
        c -= a; c -= b; c ^= ( b >> 13 );
        a -= b; a -= c; a ^= ( c >> 12 );
        b -= c; b -= a; b ^= ( a << 16 );
        c -= a; c -= b; c ^= ( b >> 5 );
        a -= b; a -= c; a ^= ( c >> 3 );
        b -= c; b -= a; b ^= ( a << 10 );
        c -= a; c -= b; c ^= ( b >> 15 );

        k += 12;
        len -= 12;
    }

    // Handle the last 11 remaining bytes
    // Note: All cases fall through

    c += length;        // Lower byte of c gets used for length

    switch ( len )
    {
    case 11: 
        c += ((unsigned int)k[10]<<24);
    case 10: 
        c += ((unsigned int)k[9]<<16);
    case 9: 
        c += ((unsigned int)k[8]<<8);
    case 8: 
        b += ((unsigned int)k[7]<<24);
    case 7: 
        b += ((unsigned int)k[6]<<16);
    case 6: 
        b += ((unsigned int)k[5]<<8);
    case 5: 
        b += k[4];
    case 4: 
        a += ((unsigned int)k[3]<<24);
    case 3: 
        a += ((unsigned int)k[2]<<16);
    case 2: 
        a += ((unsigned int)k[1]<<8);
    case 1: 
        a += k[0];
    }

    // Mix
    a -= b; a -= c; a ^= ( c >> 13 );
    b -= c; b -= a; b ^= ( a << 8 );
    c -= a; c -= b; c ^= ( b >> 13 );
    a -= b; a -= c; a ^= ( c >> 12 );
    b -= c; b -= a; b ^= ( a << 16 );
    c -= a; c -= b; c ^= ( b >> 5 );
    a -= b; a -= c; a ^= ( c >> 3 );
    b -= c; b -= a; b ^= ( a << 10 );
    c -= a; c -= b; c ^= ( b >> 15 );

    return c;
}


//
// Safely read a ushort sized string from a NetBitStreamInterface
//
bool BitStreamReadUsString( class NetBitStreamInterface& bitStream, SString& strOut )
{
    bool bResult = false;

    // Read out the string length
	unsigned short usLength;
	if ( bitStream.Read ( usLength ) )
	{
        // Allocate a buffer and read the string into it
        char* szValue = new char [ usLength + 1 ];
        // String with a length of zero is considered a success
        if ( !usLength || bitStream.Read ( szValue, usLength ) )
        {
            // Put it into us
            szValue [ usLength ] = 0;
			strOut = szValue;
            bResult = true;
        }

        // Delete the buffer
        delete [] szValue;
	}

    // Clear output on fail
    if ( !bResult )
        strOut = "";

    return bResult;
}
