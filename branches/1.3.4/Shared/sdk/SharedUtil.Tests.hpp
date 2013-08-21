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

// Fwd decl
void    SString_Tests                   ( void );
void    SharedUtil_File_Tests           ( void );
void    SharedUtil_ClassIdent_Tests     ( void );
void 	SharedUtil_WildcardMatch_Tests  ( void );
void    SharedUtil_Collection_Tests     ( void );
void    SharedUtil_String_Tests         ( void );

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
    SharedUtil_ClassIdent_Tests ();
    SharedUtil_WildcardMatch_Tests ();
    SharedUtil_Collection_Tests ();
    SharedUtil_String_Tests ();
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
            { "XX%sAA%dBB", "la",     0,      "XXlaAA0BB" },
            { "XX%sAA%dBB", "laQWE",  2000,   "XXlaQWEAA2000BB" },
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
            { "a.b.c.d.e",  ".",    1,    true,   "a",        "b.c.d.e" },
            { "a.b.c.d.e",  ".",    2,    true,   "a.b",      "c.d.e" },
            { "a.b.c.d.e",  ".",    -2,   true,   "a.b.c",    "d.e" },
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
            { "hello",            ".",    0,  0,  "hello",    "hello",    1 },
            { "a.#b.#c.#d.#e",    ".#",   0,  0,  "a",        "e",        5 },
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
            { "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh" },
            { "blah1234blah", "blah", "",     "1234" },
            { "blahblah1234", "blah", "",     "1234" },
            { "1234blahblah", "blah", "",     "1234" },
            { "1234blAhblaH", "BLah", "",     "1234blAhblaH" },
            { "blah1234blah", "LA",   "QwE",  "blah1234blah" },
            { "blah1234blah", "blAh", "",     "blah1234blah" },
            { "blah////blah", "//",   "/",    "blah//blah" },
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
            { "blah////blah", "//",   "/",  false,    "blah//blah" },
            { "blah////blah", "//",   "/",  true,     "blah/blah" },
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
            { "blah1234blah", "la",   "QwE",  "bQwEh1234bQwEh" },
            { "blah1234blah", "blah", "",     "1234" },
            { "blahblah1234", "blAh", "",     "1234" },
            { "1234blAhblaH", "BLah", "",     "1234" },
            { "blah1234blah", "LA",   "QwE",  "bQwEh1234bQwEh" },
            { "blah1234blah", "blAh", "",     "1234" },
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
            { "",             "/",    0, 999, "" },
            { "A",            "/",    0, 999, "A" },
            { "A,B",          "/",    0, 999, "A/B" },
            { "A,B,C,D,E",    "/",    0, 999, "A/B/C/D/E" },
            { "A,B,C,D,E",    "/",   -5, 7,   "A/B" },
            { "",             "/",    0, 0,   "" },
            { "A",            "/",    0, 0,   "" },
            { "A,B",          "/",    0, 0,   "" },
            { "A,B,C,D,E",    "/",    0, 0,   "" },
            { "",             "/",    0, 1,   "" },
            { "A",            "/",    0, 1,   "A" },
            { "A,B",          "/",    0, 1,   "A" },
            { "A,B,C,D,E",    "/",    0, 1,   "A" },
            { "",             "/",    0, 2,   "" },
            { "A",            "/",    0, 2,   "A" },
            { "A,B",          "/",    0, 2,   "A/B" },
            { "A,B,C,D,E",    "/",    0, 2,   "A/B" },
            { "",             "/",    1, 2,   "" },
            { "A",            "/",    1, 2,   "" },
            { "A,B",          "/",    1, 2,   "B" },
            { "A,B,C,D,E",    "/",    1, 2,   "B/C" },
            { "A,B,C,D,E",    "/",    1, 4,   "B/C/D/E" },
            { "A,B,C,D,E",    "/",    1, 5,   "B/C/D/E" },
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
            { "///\\\\\\/\\/\\/\\/blah/\\/\\/\\", "////\\\\/////\\/fleeb///\\\\///\\/\\",    "\\\\blah\\fleeb\\" },
            { "blah/\\/\\/\\",                    "////\\\\/////\\/fleeb",                   "blah\\fleeb" },
            { "blah",                             "fleeb",                                    "blah\\fleeb" },
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
            { "/blah/",   "/\\fl\\eeb/",  "//fleeeb/",    "\\blah\\fl\\eeb\\fleeeb\\" },
            { "blah/",    "/fl//eeb",     "\\fleeeb",     "blah\\fl\\eeb\\fleeeb" },
            { "blah",     "fleeb",        "fleeb",        "blah\\fleeb\\fleeb" },
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
            { "//blah/",          "//fleeeb/",    "\\\\blah\\fleeeb\\" },
            { "//?/blah/",        "//fleeeb/",    "\\\\?\\blah\\fleeeb\\" },
            { "//?/C:\blah/",     "//fleeeb/",    "\\\\?\\C:\blah\\fleeeb\\" },
            { "///?/C:\blah/",    "//fleeeb/",    "\\\\?\\C:\blah\\fleeeb\\" },
            { "file://blah/",     "//fleeeb/",    "file:\\\\blah\\fleeeb\\" },
            { "file:///blah\\/",  "//fleeeb/",    "file:\\\\blah\\fleeeb\\" },
            { "fil:e///blah\\/",  "//fleeeb/",    "fil:e\\blah\\fleeeb\\" },
            { "fi/le:///blah\\/",  "//fleeeb/",    "fi\\le:\\blah\\fleeeb\\" },
            { "c:///blah\\/",      "//fleeeb/",    "c:\\blah\\fleeeb\\" },
        TEST_END
    }

#endif  // WIN32

}


///////////////////////////////////////////////////////////////
//
// SharedUtil_ClassIdent_Tests
//
// Tests for macros in SharedUtil.ClassIdent.h
//
///////////////////////////////////////////////////////////////
void SharedUtil_ClassIdent_Tests ( void )
{
#if WIN32
    // Setup
    enum eTestClassTypes
    {
        CLASS_SPlant,
        CLASS_STree,
        CLASS_SFlower,
        CLASS_SBlueBell,
        CLASS_SDaffodil,
    };

    struct SPlant
    {
        DECLARE_BASE_CLASS( SPlant )
        SPlant ( void ) : ClassInit ( this ) {}
    };

    struct STree : SPlant
    {
        DECLARE_CLASS( STree, SPlant )
        STree ( void ) : ClassInit ( this ) {}
    };

    struct SFlower : SPlant
    {
        DECLARE_CLASS( SFlower, SPlant )
        SFlower ( void ) : ClassInit ( this ) {}
    };

    struct SBlueBell : SFlower
    {
        DECLARE_CLASS( SBlueBell, SFlower )
        SBlueBell ( void ) : ClassInit ( this ) {}
    };

    struct SDaffodil : SFlower
    {
        DECLARE_CLASS( SDaffodil, SFlower )
        SDaffodil ( void ) : ClassInit ( this ) {}
    };

    // Create test data
    SPlant* pPlant = new SPlant ();
    STree* pTree = new STree ();
    SFlower* pFlower = new SFlower ();
    SBlueBell* pBlueBell = new SBlueBell ();
    SDaffodil* pDaffodil = new SDaffodil ();

    // Tests
    assert ( DynamicCast < SPlant > ( pPlant ) );
    assert ( DynamicCast < SPlant > ( pTree ) );
    assert ( DynamicCast < SPlant > ( pFlower ) );
    assert ( DynamicCast < SPlant > ( pBlueBell ) );
    assert ( DynamicCast < SPlant > ( pDaffodil ) );

    assert ( !DynamicCast < STree > ( pPlant ) );
    assert ( DynamicCast < STree > ( pTree ) );

    assert ( !DynamicCast < SFlower > ( pPlant ) );
    assert ( DynamicCast < SFlower > ( pFlower ) );
    assert ( DynamicCast < SFlower > ( pBlueBell ) );
    assert ( DynamicCast < SFlower > ( pDaffodil ) );

    assert ( !DynamicCast < SBlueBell > ( pPlant ) );
    assert ( !DynamicCast < SBlueBell > ( pFlower ) );
    assert ( DynamicCast < SBlueBell > ( pBlueBell ) );

    assert ( !DynamicCast < SDaffodil > ( pPlant ) );
    assert ( !DynamicCast < SDaffodil > ( pFlower ) );
    assert ( DynamicCast < SDaffodil > ( pDaffodil ) );

    // Create test data
    SPlant* pPlantTree = pTree;
    SPlant* pPlantFlower = pFlower;
    SPlant* pPlantBlueBell = pBlueBell;
    SPlant* pPlantDaffodil = pDaffodil;
    SFlower* pFlowerBlueBell = pBlueBell;
    SFlower* pFlowerDaffodil = pDaffodil;

    // Tests
    assert ( DynamicCast < SPlant > ( pPlantTree ) );
    assert ( DynamicCast < SPlant > ( pPlantFlower ) );
    assert ( DynamicCast < SPlant > ( pPlantBlueBell ) );
    assert ( DynamicCast < SPlant > ( pPlantDaffodil ) );
    assert ( DynamicCast < SPlant > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SPlant > ( pFlowerDaffodil ) );

    assert ( DynamicCast < STree > ( pPlantTree ) );
    assert ( !DynamicCast < STree > ( pPlantFlower ) );
    assert ( !DynamicCast < STree > ( pPlantBlueBell ) );
    assert ( !DynamicCast < STree > ( pPlantDaffodil ) );

    assert ( !DynamicCast < SFlower > ( pPlantTree ) );
    assert ( DynamicCast < SFlower > ( pPlantFlower ) );
    assert ( DynamicCast < SFlower > ( pPlantBlueBell ) );
    assert ( DynamicCast < SFlower > ( pPlantDaffodil ) );
    assert ( DynamicCast < SFlower > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SFlower > ( pFlowerDaffodil ) );

    assert ( !DynamicCast < SBlueBell > ( pPlantTree ) );
    assert ( !DynamicCast < SBlueBell > ( pPlantFlower ) );
    assert ( DynamicCast < SBlueBell > ( pPlantBlueBell ) );
    assert ( !DynamicCast < SBlueBell > ( pPlantDaffodil ) );
    assert ( DynamicCast < SBlueBell > ( pFlowerBlueBell ) );
    assert ( !DynamicCast < SBlueBell > ( pFlowerDaffodil ) );

    assert ( !DynamicCast < SDaffodil > ( pPlantTree ) );
    assert ( !DynamicCast < SDaffodil > ( pPlantFlower ) );
    assert ( !DynamicCast < SDaffodil > ( pPlantBlueBell ) );
    assert ( DynamicCast < SDaffodil > ( pPlantDaffodil ) );
    assert ( !DynamicCast < SDaffodil > ( pFlowerBlueBell ) );
    assert ( DynamicCast < SDaffodil > ( pFlowerDaffodil ) );

    // Check ClassBits type is at least 64 bits
    assert ( sizeof ( ClassBits ) * 8 >= 64 );

    // Cleanup
    delete pPlant;
    delete pTree;
    delete pFlower;
    delete pBlueBell;
    delete pDaffodil;
#endif
}


///////////////////////////////////////////////////////////////
//
// SharedUtil_WildcardMatch_Tests
//
// Tests for WildcardMatch
//
///////////////////////////////////////////////////////////////
void SharedUtil_WildcardMatch_Tests ( void )
{
    // WildcardMatch
    {
        TEST_FUNCTION
            assert ( WildcardMatch ( a, b ) == result );
        TEST_VARS
            const char* a;
            const char* b;
            bool result;
        TEST_DATA
            { "*bd*",         "abcbde",               true },
            { "*bd?f*",       "abcbdef_bdgh",         true },
            { "*bd?h*",       "abcbdef_bdgh",         true },
            { "*bd?g*",       "abcbdef_bdgh",         false },
            { "scr*w?d",      "screeeewywxd",         true },
            { "A*B",          "A_B_B",                true },
            { "",             "",                     true },
            { "*",            "",                     true },
            { "*",            "A",                    true },
            { "",             "A",                    false },
            { "A*",           "",                     false },
            { "A*",           "AAB",                  true },
            { "A*",           "BAA",                  false },
            { "A*",           "A",                    true },
            { "A*B",          "",                     false },
            { "A*B",          "AAB",                  true },
            { "A*B",          "AB",                   true },
            { "A*B",          "AABA",                 false },
            { "A*B",          "ABAB",                 true },
            { "A*B",          "ABBBB",                true },
            { "A*B*C",        "",                     false },
            { "A*B*C",        "ABC",                  true },
            { "A*B*C",        "ABCC",                 true },
            { "A*B*C",        "ABBBC",                true },
            { "A*B*C",        "ABBBBCCCC",            true },
            { "A*B*C",        "ABCBBBCBCCCBCBCCCC",   true },
            { "A*B*",         "AB",                   true },
            { "A*B*",         "AABA",                 true },
            { "A*B*",         "ABAB",                 true },
            { "A*B*",         "ABBBB",                true },
            { "A*B*C*",       "",                     false },
            { "A*B*C*",       "ABC",                  true },
            { "A*B*C*",       "ABCC",                 true },
            { "A*B*C*",       "ABBBC",                true },
            { "A*B*C*",       "ABBBBCCCC",            true },
            { "A*B*C*",       "ABCBBBCBCCCBCBCCCC",   true },
            { "A?",           "AAB",                  false },
            { "A?B",          "AAB",                  true },
            { "A?*",          "A",                    false },
            { "A?*",          "ABBCC",                true },
            { "A?*",          "BAA",                  false },
        TEST_END
    }
}


///////////////////////////////////////////////////////////////
//
// SharedUtil_Collection_Tests
//
// Test behaviour of iterators and such
//
///////////////////////////////////////////////////////////////
void SharedUtil_Collection_Tests ( void )
{
    // std::map
    {
        std::map < uint, SString > testMap;
        MapSet ( testMap, 10, "10" );
        MapSet ( testMap, 20, "20" );
        MapSet ( testMap, 30, "30" );
        MapSet ( testMap, 40, "40" );

        std::map < uint, SString >::iterator iter = testMap.begin ();
        assert ( iter->first == 10 );
        iter++;
        assert ( iter->first == 20 );
        iter++;
        testMap.erase ( iter++ );
        assert ( iter->first == 40 );
        iter++;
        assert ( iter == testMap.end () );
    }

    // std::set
    {
        std::set < uint > testMap;
        MapInsert ( testMap, 10 );
        MapInsert ( testMap, 20 );
        MapInsert ( testMap, 30 );
        MapInsert ( testMap, 40 );

        std::set < uint >::iterator iter = testMap.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        testMap.erase ( iter++ );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testMap.end () );
    }

    // std::list
    {
        std::list < uint > testList;
        testList.push_back ( 10 );
        testList.push_back ( 20 );
        testList.push_back ( 30 );
        testList.push_back ( 40 );

        std::list < uint >::iterator iter = testList.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        iter = testList.erase ( iter );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testList.end () );
    }

    // std::vector
    {
        std::vector < uint > testList;
        testList.push_back ( 10 );
        testList.push_back ( 20 );
        testList.push_back ( 30 );
        testList.push_back ( 40 );

        std::vector < uint >::iterator iter = testList.begin ();
        assert ( *iter == 10 );
        iter++;
        assert ( *iter == 20 );
        iter++;
        iter = testList.erase ( iter );
        assert ( *iter == 40 );
        iter++;
        assert ( iter == testList.end () );
    }

}


///////////////////////////////////////////////////////////////
//
// SharedUtil_String_Tests
//
// Test behaviour of strings
//
///////////////////////////////////////////////////////////////
void SharedUtil_String_Tests ( void )
{
    // Formatting
    {
        SString strTemp1 ( "name:" PRSinS, "dave" );
        SString strTemp2 ( "name:" PRWinS, L"dave" );
        WString wstrTemp3 ( L"name:" PRSinW, "dave" );
        WString wstrTemp4 ( L"name:" PRWinW, L"dave" );

        assert ( strTemp1 == "name:dave" );
        assert ( strTemp2 == "name:dave" );
        assert ( wstrTemp3 == L"name:dave" );
        assert ( wstrTemp4 == L"name:dave" );
    }

    // To/From Ansi
    {
        SString strTemp1 = "abcABC 123";
        WString wstrTemp2 = "defDEF 456";
        WString wstrTemp3 = L"ghiGHI 789";

        assert ( wstrTemp2 == L"defDEF 456" );

        SString strTemp2 = wstrTemp2.ToAnsi ();
        SString strTemp3 = wstrTemp3.ToAnsi ();

        assert ( strTemp2 == "defDEF 456" );
        assert ( strTemp3 == "ghiGHI 789" );
    }

    // Escaping URL arguments
    {
        TEST_FUNCTION
            SStringX strInputA( (const char*)a, sizeof( a ) );
            SString strEscaped = EscapeURLArgument( strInputA );
            SString strUnescaped = UnescapeString ( strEscaped, '%' );
            assert ( strEscaped == result );
            assert ( strInputA == strUnescaped );
        TEST_VARS
            const uchar a[5];
            const char* result;
        TEST_DATA
            { {0x00, 0x10, 0x20, 0x21, 0x22},       "%00%10%20%21%22" },
            { {0x7F, 0x80, 0x81, 0xFE, 0xFF},       "%7F%80%81%FE%FF" },
        TEST_END
    }

    {
        TEST_FUNCTION
            SStringX strInputA( a );
            SString strEscaped = EscapeURLArgument( strInputA );
            SString strUnescaped = UnescapeString ( strEscaped, '%' );
            assert ( strEscaped == result );
            assert ( strInputA == strUnescaped );
        TEST_VARS
            const char* a;
            const char* result;
        TEST_DATA
            { "!*'();:@",           "%21%2A%27%28%29%3B%3A%40" },
            { "&=+$,/?#",           "%26%3D%2B%24%2C%2F%3F%23" },
            { "[] \"%<>\\",         "%5B%5D%20%22%25%3C%3E%5C" },
            { "^`{|}",              "%5E%60%7B%7C%7D" },
            { "AZaz09-_.~",         "AZaz09-_.~" },
        TEST_END
    }

    // RemoveColorCodes
    {
        TEST_FUNCTION
            SString strRemoved = RemoveColorCodes( a );
            assert ( strRemoved == result );
        TEST_VARS
            const char* a;
            const char* result;
        TEST_DATA
            { "aa #0f0F34 bb",                          "aa  bb" },
            { "aa #0f0F34#AABBBB bb",                   "aa  bb" },
            { "aa #0f0F3G#AABBBB bb",                   "aa #0f0F3G bb" },
            { "aa #0f0F34#AABBB bb",                    "aa #AABBB bb" },
            { "#0f0F34#AABBB1",                         "" },
            { "#0f0F34 x #AABBB1",                      " x " },
            { "#0f0F34#0f0F34 x #AABBB1#AABBB1",        " x " },
            { "#123456#12345G#123456#12345G",           "#12345G#12345G" },
            { "#123456#12345#123456#125G",              "#12345#125G" },
            { "##123456#125G##123456#12345",            "##125G##12345" },
        TEST_END
    }

    // RemoveColorCodesInPlaceW
    {
        TEST_FUNCTION
            WString wstrString = a;
            RemoveColorCodesInPlaceW( wstrString );
            assert ( wstrString == result );
        TEST_VARS
            const wchar_t* a;
            const wchar_t* result;
        TEST_DATA
            { L"aa #0f0F34 bb",                          L"aa  bb" },
            { L"aa #0f0F34#AABBBB bb",                   L"aa  bb" },
            { L"aa #0f0F3G#AABBBB bb",                   L"aa #0f0F3G bb" },
            { L"aa #0f0F34#AABBB bb",                    L"aa #AABBB bb" },
            { L"#0f0F34#AABBB1",                         L"" },
            { L"#0f0F34 x #AABBB1",                      L" x " },
            { L"#0f0F34#0f0F34 x #AABBB1#AABBB1",        L" x " },
            { L"#123456#12345G#123456#12345G",           L"#12345G#12345G" },
            { L"#123456#12345#123456#125G",              L"#12345#125G" },
            { L"##123456#125G##123456#12345",            L"##125G##12345" },
        TEST_END
    }

}
