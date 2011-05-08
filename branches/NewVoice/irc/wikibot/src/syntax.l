%{
#include <stdio.h>
#include <stdarg.h>
#define MAX_RESERVED_WORDS 64
#define MAX_RESERVED_WORD_LEN 128
typedef unsigned char bool;
enum { false = 0U, true = 1U };

// Output buffering
static bool gs_bOk;
static struct
{
    char* data;
    size_t length;
    size_t curLength;
    size_t curLineLength;
} gs_output;

static void SafeAppend(const char* str, ...)
{
    if ( gs_bOk == true )
    {
        size_t len;
        va_list vl;
        va_start ( vl, str );
        len = vsnprintf ( &gs_output.data[gs_output.curLength], gs_output.length - gs_output.curLength, str, vl );
        va_end ( vl );

        // This code stinks, but it's not a critical application...
        gs_output.curLineLength += len;
        if ( gs_output.curLineLength > 300 )
        {
            gs_output.data[gs_output.curLength] = '\n';
            ++gs_output.curLength;
            gs_output.curLineLength = 0;
            va_start ( vl, str );
            len = vsnprintf ( &gs_output.data[gs_output.curLength], gs_output.length - gs_output.curLength, str, vl );
            va_end ( vl );
        }

        gs_output.curLength += len;
        if ( gs_output.curLength >= gs_output.length )
            gs_bOk = false;
        else
        {
            gs_output.data[gs_output.curLength] = '\0';
        }
    }
}

// Reserved words
static const char* gs_szReservedWordsPath = 0;
static char gs_reservedWords [ MAX_RESERVED_WORDS ][ MAX_RESERVED_WORD_LEN ];
static unsigned int gs_uiNumReservedWords;
void setReservedWordsPath ( const char* szPath )
{
    gs_szReservedWordsPath = szPath;
}
static void LoadData ()
{
    static bool s_bIsLoaded = false;
    if ( s_bIsLoaded == true ) return;

    FILE* fp;
    if ( gs_szReservedWordsPath == 0 )
        fp = fopen ( DATADIR "/reserved.txt", "r" );
    else
        fp = fopen ( gs_szReservedWordsPath, "r" );

    if ( fp != 0 )
    {
        char line [ MAX_RESERVED_WORD_LEN ];
        char* p;
        while ( !feof ( fp ) && gs_uiNumReservedWords <= MAX_RESERVED_WORDS )
        {
            fgets ( gs_reservedWords[gs_uiNumReservedWords], MAX_RESERVED_WORD_LEN, fp );
            p = &gs_reservedWords[gs_uiNumReservedWords][strlen(gs_reservedWords[gs_uiNumReservedWords])-1];
            while ( p > &gs_reservedWords[gs_uiNumReservedWords][0] && ( *p == '\n' || *p == '\r' ) )
                *p-- = '\0';
            ++gs_uiNumReservedWords;
        }
        fclose ( fp );
    }

    s_bIsLoaded = true;
}
static bool IsReserved(const char* tok)
{
    unsigned int i;
    LoadData ();

    for ( i = 0; i < gs_uiNumReservedWords; ++i )
    {
        if ( !strcmp ( gs_reservedWords[i], tok ) )
            return true;
    }

    return false;
}

%}

%%

\"[^\"]*\" {
    SafeAppend ( "\00305%s\003", yytext );
}
[0-9]+(\.[0-9]+)? {
    SafeAppend ( yytext );
}
[A-Za-z_][A-Za-z_0-9]* {
    if ( IsReserved ( yytext ) )
        SafeAppend ( "\00303%s\003", yytext );
    else
        SafeAppend ( yytext );
}
[A-Za-z_][A-Za-z_0-9]*[\t ]*\([\t ]*\) {
    SafeAppend ( yytext );
}
[\(\)\[\],] {
    SafeAppend ( "\002%s\002", yytext );
}
. {
    SafeAppend( yytext );
}

%%

int yywrap () { return 1; }

bool colorizeSyntax ( const char* source, char* dest, size_t destLength )
{
    char* sourceCopy = (char *)malloc(sizeof(char)*(strlen(source)+1));
    strcpy ( sourceCopy, source );
    gs_output.length = destLength;
    gs_output.data = dest;
    gs_output.curLength = 0;
    gs_output.curLineLength = 0;
    gs_bOk = true;
    YY_BUFFER_STATE state = yy_scan_string ( sourceCopy );
    while ( yylex() != 0 );
    yy_delete_buffer ( state );
    free ( sourceCopy );

    if ( gs_bOk == false )
        strcpy ( dest, source );
    return gs_bOk;
}

