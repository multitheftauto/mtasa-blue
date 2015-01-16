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
void    SharedUtil_Hash_Tests           ( void );

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
    SharedUtil_Hash_Tests ();
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


///////////////////////////////////////////////////////////////
//
// SharedUtil_Hash_Tests
//
// Test behaviour of hashing/crypt related functions
//
///////////////////////////////////////////////////////////////
void SharedUtil_Hash_Tests ( void )
{
    // ConvertHexStringToData/ConvertDataToHexString
    {
        TEST_FUNCTION
            char buffer[256];
            uint length = a.length() / 2;
            ConvertHexStringToData( a, buffer, length );
            SString strResult = ConvertDataToHexString( buffer, length );
            assert ( strResult == b );
        TEST_VARS
            const SString a;
            const char* b;
        TEST_DATA
            { "66B9139D8C424BE2BCF224706B48FEB8", "66B9139D8C424BE2BCF224706B48FEB8" },
            { "E7C7253C74275F2DC2DC8C6828816C18301636949369F3bad87666C81E71B309", "E7C7253C74275F2DC2DC8C6828816C18301636949369F3BAD87666C81E71B309" },
            { "61", "61" },
            { "\x01""A""\x1F\x80""BC""\xFE\xFF", "0A00BC00" },
        TEST_END
    }

    // TeaEncode/TeaDecode
    {
        TEST_FUNCTION
            SString strEncoded;
            TeaEncode( a, b, &strEncoded );
            if ( !result.empty() )
                assert ( strEncoded == result );
            SString strDecoded;
            TeaDecode( strEncoded, b, &strDecoded );
            assert ( a == *strDecoded );
        TEST_VARS
            const SString a;
            const SString b;
            const SString result;
        TEST_DATA
            { "1234", "AB12£$_ ", "\xD2\xB4\x75\x5C\xDC\x15\x54\xC9" },
            { "Hello thereHello there", "78111E998C42243285635E39AFDD614B\0 AB12£$_ ", "" },
            { "78111E998C42243285635E39AFD\0D614B AB12£$_ ", "Hello thereHello there", "" },
        TEST_END
    }

    // MD5
    {
        TEST_FUNCTION
            SString strResult = CMD5Hasher::CalculateHexString( a.c_str(), a.length() );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "D41D8CD98F00B204E9800998ECF8427E" },
            { "Hello there",    "E8EA7A8D1E93E8764A84A0F3DF4644DE" },
            { "AB12£$_\0 ",     "78111E998C42243285635E39AFDD614B" },
        TEST_END
    }

    // SHA256
    {
        TEST_FUNCTION
            SString strResult = GenerateSha256HexString( a );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855" },
            { "Hello there",    "4E47826698BB4630FB4451010062FADBF85D61427CBDFAED7AD0F23F239BED89" },
            { "AB12£$_\0 ",     "A427AEDD32E652FED23901406BCC49BF78B906E69699A68932638502E8C1138B" },
        TEST_END
    }

    // SHA1
    {
        TEST_FUNCTION
            SString strResult = GenerateHashHexString( EHashFunction::SHA1, a );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709" },
            { "Hello there",    "726C76553E1A3FDEA29134F36E6AF2EA05EC5CCE" },
            { "AB12£$_\0 ",     "CA7B95DF48B83232FCA0FE3FAE7A787784F54225" },
        TEST_END
    }

    // SHA224
    {
        TEST_FUNCTION
            SString strResult = GenerateHashHexString( EHashFunction::SHA224, a );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "D14A028C2A3A2BC9476102BB288234C415A2B01F828EA62AC5B3E42F" },
            { "Hello there",    "40AACC4967ECA7730A1A069539D78AE7782480802E481F1ECC26927D" },
            { "AB12£$_\0 ",     "73420F2E80A236DD7C836C68177E282BC8E86CD8BB497E5F443F1FAE" },
        TEST_END
    }

    // SHA384
    {
        TEST_FUNCTION
            SString strResult = GenerateHashHexString( EHashFunction::SHA384, a );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "38B060A751AC96384CD9327EB1B1E36A21FDB71114BE07434C0CC7BF63F6E1DA274EDEBFE76F65FBD51AD2F14898B95B" },
            { "Hello there",    "7438E0294C534D6CA6CC2EFB04A60DB488C86B66C4CBD3C00D11D58C8020274AB0A2A720C88986968D894F26B16C461F" },
            { "AB12£$_\0 ",     "DFCAF84C21F93CC88DE6CB54D838FEE5ACF592DCF392883708BAD1CFF7B847DEA9A175C5E87014D5829E66E17571F7E6" },
        TEST_END
    }

    // SHA512
    {
        TEST_FUNCTION
            SString strResult = GenerateHashHexString( EHashFunction::SHA512, a );
            assert ( strResult == result );
        TEST_VARS
            const SString a;
            const char* result;
        TEST_DATA
            { "",               "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E" },
            { "Hello there",    "567683DDBA1F5A576B68EC26F41FFBCC7E718D646839AC6C2EF746FE952CEF4CBE6DEA635BC2F098B92B65CAACF482333BB9D1D9A3089BC4F01CB86F7A2FBC18" },
            { "AB12£$_\0 ",     "EE57E02866026848F988E793E9B099931CBCA8773B13EA5055ABA462885E8B044CE5C72CB528712CE2A442707F2BC25A52CC91F8C09DD1C6A6A5C9A63D52F320" },
        TEST_END
    }

}
