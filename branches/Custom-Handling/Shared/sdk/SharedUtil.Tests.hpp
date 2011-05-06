/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Tests.hpp
*  PURPOSE:
*
*  DEVELOPERS:
*
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifdef MTA_DEBUG

// Fwd decl
void    SString_Tests               ( void );
void    SharedUtil_File_Tests       ( void );


///////////////////////////////////////////////////////////////
//
// Run tests entry point
//
//
//
///////////////////////////////////////////////////////////////
void SharedUtil_Tests ( void )
{
    SString_Tests ();
    SharedUtil_File_Tests ();
}


///////////////////////////////////////////////////////////////
//
// Helper macros
//
//
//
///////////////////////////////////////////////////////////////
#define TEST_FUNCTION \
        struct testStruct { \
            void Test ( int testIndex ) \
            { \
                testStruct dataCopy = *this;    // Info when debugging

#define TEST_VARS \
            }

#define TEST_DATA \
        } testData[] = {


#define TEST_END \
            }; \
        for ( uint i = 0 ; i < NUMELMS( testData ) ; i++ ) \
            testData[i].Test ( i );


///////////////////////////////////////////////////////////////
//
// SString_Tests
//
// Tests for functions from SString.hpp
//
///////////////////////////////////////////////////////////////
void SString_Tests ( void )
{
    // vFormat
    {
        TEST_FUNCTION
            assert ( SString ( a, b, c ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            int c;
            const char* result;
        TEST_DATA
            "XX%sAA%dBB", "la",     0,      "XXlaAA0BB",
            "XX%sAA%dBB", "laQWE",  2000,   "XXlaQWEAA2000BB",
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
            SString strLeft, strRight;
            assert ( source.Split ( delim, &strLeft, &strRight, index ) == result );
            assert ( strLeft == resultLeft );
            assert ( strRight == resultRight );
        TEST_VARS
            SString source;
            const char* delim;
            int index;
            bool result;
            const char* resultLeft;
            const char* resultRight;
        TEST_DATA
            "a.b.c.d.e",  ".",    1,    true,   "a",        "b.c.d.e",
            "a.b.c.d.e",  ".",    2,    true,   "a.b",      "c.d.e",
            "a.b.c.d.e",  ".",    -2,   true,   "a.b.c",    "d.e",
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
            std::vector < SString > parts;
            source.Split ( delim, parts, maxAmmount, minAmmount );
            assert ( parts.front () == resultFirst );
            assert ( parts.back () == resultLast );
            assert ( parts.size () == resultCount );
        TEST_VARS
            SString source;
            const char* delim;
            uint maxAmmount;
            uint minAmmount;
            const char* resultFirst;
            const char* resultLast;
            uint resultCount;
        TEST_DATA
            "hello",            ".",    0,  0,  "hello",    "hello",    1,
            "a.#b.#c.#d.#e",    ".#",   0,  0,  "a",        "e",        5,
        TEST_END
    }

    // SplitLeft
    // SplitRight

    // Replace
    {
        TEST_FUNCTION
            assert ( source.Replace ( needle, newneedle ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            const char* result;
        TEST_DATA
            "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blah", "",     "1234",
            "blahblah1234", "blah", "",     "1234",
            "1234blahblah", "blah", "",     "1234",
            "1234blAhblaH", "BLah", "",     "1234blAhblaH",
            "blah1234blah", "LA",   "QwE",  "blah1234blah",
            "blah1234blah", "blAh", "",     "blah1234blah",
            "blah////blah", "//",   "/",    "blah//blah",
        TEST_END
    }


    // Replace with option
    {
        TEST_FUNCTION
            assert ( source.Replace ( needle, newneedle, bSearchJustReplaced ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            bool bSearchJustReplaced;
            const char* result;
        TEST_DATA
            "blah////blah", "//",   "/",  false,    "blah//blah",
            "blah////blah", "//",   "/",  true,     "blah/blah",
        TEST_END
    }


    // ReplaceI
    {
        TEST_FUNCTION
            assert ( source.ReplaceI ( needle, newneedle ) == result );
        TEST_VARS
            SString source;
            const char* needle;
            const char* newneedle;
            const char* result;
        TEST_DATA
            "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blah", "",     "1234",
            "blahblah1234", "blAh", "",     "1234",
            "1234blAhblaH", "BLah", "",     "1234",
            "blah1234blah", "LA",   "QwE",  "bQwEh1234bQwEh",
            "blah1234blah", "blAh", "",     "1234",
        TEST_END
    }

    // TrimStart
    // TrimEnd
    // ToLower
    // ToUpper
    // ConformLineEndings
    // Contains
    // ContainsI
    // SubStr
    // Left
    // Right
    // EndsWith
    // EndsWithI
    // BeginsWith
    // BeginsWithI

    // Join
    {
        TEST_FUNCTION
            std::vector < SString > parts;
            source.Split ( ",", parts );
            assert ( SString::Join ( delim, parts, first, count ) == result );
        TEST_VARS
            SString source;
            SString delim;
            int first;
            int count;
            SString result;
        TEST_DATA
            "",             "/",    0, 999, "",
            "A",            "/",    0, 999, "A",
            "A,B",          "/",    0, 999, "A/B",
            "A,B,C,D,E",    "/",    0, 999, "A/B/C/D/E",
            "A,B,C,D,E",    "/",   -5, 7,   "A/B",
            "",             "/",    0, 0,   "",
            "A",            "/",    0, 0,   "",
            "A,B",          "/",    0, 0,   "",
            "A,B,C,D,E",    "/",    0, 0,   "",
            "",             "/",    0, 1,   "",
            "A",            "/",    0, 1,   "A",
            "A,B",          "/",    0, 1,   "A",
            "A,B,C,D,E",    "/",    0, 1,   "A",
            "",             "/",    0, 2,   "",
            "A",            "/",    0, 2,   "A",
            "A,B",          "/",    0, 2,   "A/B",
            "A,B,C,D,E",    "/",    0, 2,   "A/B",
            "",             "/",    1, 2,   "",
            "A",            "/",    1, 2,   "",
            "A,B",          "/",    1, 2,   "B",
            "A,B,C,D,E",    "/",    1, 2,   "B/C",
            "A,B,C,D,E",    "/",    1, 4,   "B/C/D/E",
            "A,B,C,D,E",    "/",    1, 5,   "B/C/D/E",
        TEST_END
    }


}


///////////////////////////////////////////////////////////////
//
// SharedUtil_File_Tests
//
// Tests for functions from SharedUtil.File.hpp
//
///////////////////////////////////////////////////////////////
void SharedUtil_File_Tests ( void )
{

#ifdef WIN32

    // PathJoin 2 parts
    {
        TEST_FUNCTION
            assert ( PathJoin ( a, b ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            const char* result;
        TEST_DATA
            "///\\\\\\/\\/\\/\\/blah/\\/\\/\\", "////\\\\/////\\/fleeb///\\\\///\\/\\",    "\\\\blah\\fleeb\\",
            "blah/\\/\\/\\",                    "////\\\\/////\\/fleeb",                   "blah\\fleeb",
            "blah",                             "fleeb",                                    "blah\\fleeb",
        TEST_END
    }

    // PathJoin 3 parts
    {
        TEST_FUNCTION
            assert ( PathJoin ( a, b, c ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            const char* c;
            const char* result;
        TEST_DATA
            "/blah/",   "/\\fl\\eeb/",  "//fleeeb/",    "\\blah\\fl\\eeb\\fleeeb\\",
            "blah/",    "/fl//eeb",     "\\fleeeb",     "blah\\fl\\eeb\\fleeeb",
            "blah",     "fleeb",        "fleeb",        "blah\\fleeb\\fleeb",
        TEST_END
    }

    // UNC type tests
    {
        TEST_FUNCTION
            assert ( PathJoin ( a, b ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            const char* result;
        TEST_DATA
            "//blah/",          "//fleeeb/",    "\\\\blah\\fleeeb\\",
            "//?/blah/",        "//fleeeb/",    "\\\\?\\blah\\fleeeb\\",
            "//?/C:\blah/",     "//fleeeb/",    "\\\\?\\C:\blah\\fleeeb\\",
            "///?/C:\blah/",    "//fleeeb/",    "\\\\?\\C:\blah\\fleeeb\\",
            "file://blah/",     "//fleeeb/",    "file:\\\\blah\\fleeeb\\",
            "file:///blah\\/",  "//fleeeb/",    "file:\\\\blah\\fleeeb\\",
            "fil:e///blah\\/",  "//fleeeb/",    "fil:e\\blah\\fleeeb\\",
            "fi/le:///blah\\/",  "//fleeeb/",    "fi\\le:\\blah\\fleeeb\\",
        TEST_END
    }

#endif  // WIN32

}

#endif  // MTA_DEBUG
