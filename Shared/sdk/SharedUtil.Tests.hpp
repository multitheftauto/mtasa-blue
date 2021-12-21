/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Tests.hpp
 *  PURPOSE:
 *
 *
 *
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// Fwd decl
void SString_Tests();
void SharedUtil_File_Tests();
void SharedUtil_ClassIdent_Tests();
void SharedUtil_WildcardMatch_Tests();
void SharedUtil_Collection_Tests();
void SharedUtil_String_Tests();
void SharedUtil_Hash_Tests();

///////////////////////////////////////////////////////////////
//
// Run tests entry point
//
//
//
///////////////////////////////////////////////////////////////
void SharedUtil_Tests()
{
    SString_Tests();
    SharedUtil_File_Tests();
    SharedUtil_ClassIdent_Tests();
    SharedUtil_WildcardMatch_Tests();
    SharedUtil_Collection_Tests();
    SharedUtil_String_Tests();
    SharedUtil_Hash_Tests();
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
            void Test(int testIndex) \
            { \
                testStruct dataCopy = *this;    // Info when debugging

#define TEST_VARS \
            }

#define TEST_DATA \
        } testData[]

#define TEST_END \
        for (uint i = 0 ; i < NUMELMS(testData) ; i++) \
            testData[i].Test(i);

///////////////////////////////////////////////////////////////
//
// SString_Tests
//
// Tests for functions from SString.hpp
//
///////////////////////////////////////////////////////////////
void SString_Tests()
{
    // vFormat
    {
        TEST_FUNCTION
        assert(SString(a, b, c) == result);
        TEST_VARS
        const char* a;
        const char* b;
        int         c;
        const char* result;
        TEST_DATA = {
            {"XX%sAA%dBB", "la", 0, "XXlaAA0BB"},
            {"XX%sAA%dBB", "laQWE", 2000, "XXlaQWEAA2000BB"},
        };
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
        SString strLeft, strRight;
        assert(source.Split(delim, &strLeft, &strRight, index) == result);
        assert(strLeft == resultLeft);
        assert(strRight == resultRight);
        TEST_VARS
        SString     source;
        const char* delim;
        int         index;
        bool        result;
        const char* resultLeft;
        const char* resultRight;
        TEST_DATA = {
            {"a.b.c.d.e", ".", 1, true, "a", "b.c.d.e"},
            {"a.b.c.d.e", ".", 2, true, "a.b", "c.d.e"},
            {"a.b.c.d.e", ".", -2, true, "a.b.c", "d.e"},
        };
        TEST_END
    }

    // Split
    {
        TEST_FUNCTION
        std::vector<SString> parts;
        source.Split(delim, parts, maxAmmount, minAmmount);
        assert(parts.front() == resultFirst);
        assert(parts.back() == resultLast);
        assert(parts.size() == resultCount);
        TEST_VARS
        SString     source;
        const char* delim;
        uint        maxAmmount;
        uint        minAmmount;
        const char* resultFirst;
        const char* resultLast;
        uint        resultCount;
        TEST_DATA = {
            {"hello", ".", 0, 0, "hello", "hello", 1},
            {"a.#b.#c.#d.#e", ".#", 0, 0, "a", "e", 5},
        };
        TEST_END
    }

    // SplitLeft
    // SplitRight

    // Replace
    {
        TEST_FUNCTION
        assert(source.Replace(needle, newneedle) == result);
        TEST_VARS
        SString     source;
        const char* needle;
        const char* newneedle;
        const char* result;
        TEST_DATA = {
            {"blah1234blah", "la", "QwE", "bQwEh1234bQwEh"},
            {"blah1234blah", "blah", "", "1234"},
            {"blahblah1234", "blah", "", "1234"},
            {"1234blahblah", "blah", "", "1234"},
            {"1234blAhblaH", "BLah", "", "1234blAhblaH"},
            {"blah1234blah", "LA", "QwE", "blah1234blah"},
            {"blah1234blah", "blAh", "", "blah1234blah"},
            {"blah////blah", "//", "/", "blah//blah"},
        };
        TEST_END
    }

    // Replace with option
    {
        TEST_FUNCTION
        assert(source.Replace(needle, newneedle, bSearchJustReplaced) == result);
        TEST_VARS
        SString     source;
        const char* needle;
        const char* newneedle;
        bool        bSearchJustReplaced;
        const char* result;
        TEST_DATA = {
            {"blah////blah", "//", "/", false, "blah//blah"},
            {"blah////blah", "//", "/", true, "blah/blah"},
        };
        TEST_END
    }

    // ReplaceI
    {
        TEST_FUNCTION
        assert(source.ReplaceI(needle, newneedle) == result);
        TEST_VARS
        SString     source;
        const char* needle;
        const char* newneedle;
        const char* result;
        TEST_DATA = {
            {"blah1234blah", "la", "QwE", "bQwEh1234bQwEh"},
            {"blah1234blah", "blah", "", "1234"},
            {"blahblah1234", "blAh", "", "1234"},
            {"1234blAhblaH", "BLah", "", "1234"},
            {"blah1234blah", "LA", "QwE", "bQwEh1234bQwEh"},
            {"blah1234blah", "blAh", "", "1234"},
        };
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
        std::vector<SString> parts;
        source.Split(",", parts);
        assert(SString::Join(delim, parts, first, count) == result);
        TEST_VARS
        SString source;
        SString delim;
        int     first;
        int     count;
        SString result;
        TEST_DATA = {
            {"", "/", 0, 999, ""},
            {"A", "/", 0, 999, "A"},
            {"A,B", "/", 0, 999, "A/B"},
            {"A,B,C,D,E", "/", 0, 999, "A/B/C/D/E"},
            {"A,B,C,D,E", "/", -5, 7, "A/B"},
            {"", "/", 0, 0, ""},
            {"A", "/", 0, 0, ""},
            {"A,B", "/", 0, 0, ""},
            {"A,B,C,D,E", "/", 0, 0, ""},
            {"", "/", 0, 1, ""},
            {"A", "/", 0, 1, "A"},
            {"A,B", "/", 0, 1, "A"},
            {"A,B,C,D,E", "/", 0, 1, "A"},
            {"", "/", 0, 2, ""},
            {"A", "/", 0, 2, "A"},
            {"A,B", "/", 0, 2, "A/B"},
            {"A,B,C,D,E", "/", 0, 2, "A/B"},
            {"", "/", 1, 2, ""},
            {"A", "/", 1, 2, ""},
            {"A,B", "/", 1, 2, "B"},
            {"A,B,C,D,E", "/", 1, 2, "B/C"},
            {"A,B,C,D,E", "/", 1, 4, "B/C/D/E"},
            {"A,B,C,D,E", "/", 1, 5, "B/C/D/E"},
        };
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
void SharedUtil_File_Tests()
{
#ifdef WIN32

    // PathJoin 2 parts
    {
        TEST_FUNCTION
        assert(PathJoin(a, b) == result);
        TEST_VARS
        const char* a;
        const char* b;
        const char* result;
        TEST_DATA = {
            {"///\\\\\\/\\/\\/\\/blah/\\/\\/\\", "////\\\\/////\\/fleeb///\\\\///\\/\\", "\\\\blah\\fleeb\\"},
            {"blah/\\/\\/\\", "////\\\\/////\\/fleeb", "blah\\fleeb"},
            {"blah", "fleeb", "blah\\fleeb"},
        };
        TEST_END
    }

    // PathJoin 3 parts
    {
        TEST_FUNCTION
        assert(PathJoin(a, b, c) == result);
        TEST_VARS
        const char* a;
        const char* b;
        const char* c;
        const char* result;
        TEST_DATA = {
            {"/blah/", "/\\fl\\eeb/", "//fleeeb/", "\\blah\\fl\\eeb\\fleeeb\\"},
            {"blah/", "/fl//eeb", "\\fleeeb", "blah\\fl\\eeb\\fleeeb"},
            {"blah", "fleeb", "fleeb", "blah\\fleeb\\fleeb"},
        };
        TEST_END
    }

    // UNC type tests
    {
        TEST_FUNCTION
        assert(PathJoin(a, b) == result);
        TEST_VARS
        const char* a;
        const char* b;
        const char* result;
        TEST_DATA = {
            {"//blah/", "//fleeeb/", "\\\\blah\\fleeeb\\"},
            {"//?/blah/", "//fleeeb/", "\\\\?\\blah\\fleeeb\\"},
            {"//?/C:\blah/", "//fleeeb/", "\\\\?\\C:\blah\\fleeeb\\"},
            {"///?/C:\blah/", "//fleeeb/", "\\\\?\\C:\blah\\fleeeb\\"},
            {"file://blah/", "//fleeeb/", "file:\\\\blah\\fleeeb\\"},
            {"file:///blah\\/", "//fleeeb/", "file:\\\\blah\\fleeeb\\"},
            {"fil:e///blah\\/", "//fleeeb/", "fil:e\\blah\\fleeeb\\"},
            {"fi/le:///blah\\/", "//fleeeb/", "fi\\le:\\blah\\fleeeb\\"},
            {"c:///blah\\/", "//fleeeb/", "c:\\blah\\fleeeb\\"},
        };
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
void SharedUtil_ClassIdent_Tests()
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
        DECLARE_BASE_CLASS(SPlant)
        SPlant() : ClassInit(this) {}
    };

    struct STree : SPlant
    {
        DECLARE_CLASS(STree, SPlant)
        STree() : ClassInit(this) {}
    };

    struct SFlower : SPlant
    {
        DECLARE_CLASS(SFlower, SPlant)
        SFlower() : ClassInit(this) {}
    };

    struct SBlueBell : SFlower
    {
        DECLARE_CLASS(SBlueBell, SFlower)
        SBlueBell() : ClassInit(this) {}
    };

    struct SDaffodil : SFlower
    {
        DECLARE_CLASS(SDaffodil, SFlower)
        SDaffodil() : ClassInit(this) {}
    };

    // Create test data
    SPlant*    pPlant = new SPlant();
    STree*     pTree = new STree();
    SFlower*   pFlower = new SFlower();
    SBlueBell* pBlueBell = new SBlueBell();
    SDaffodil* pDaffodil = new SDaffodil();

    // Tests
    assert(DynamicCast<SPlant>(pPlant));
    assert(DynamicCast<SPlant>(pTree));
    assert(DynamicCast<SPlant>(pFlower));
    assert(DynamicCast<SPlant>(pBlueBell));
    assert(DynamicCast<SPlant>(pDaffodil));

    assert(!DynamicCast<STree>(pPlant));
    assert(DynamicCast<STree>(pTree));

    assert(!DynamicCast<SFlower>(pPlant));
    assert(DynamicCast<SFlower>(pFlower));
    assert(DynamicCast<SFlower>(pBlueBell));
    assert(DynamicCast<SFlower>(pDaffodil));

    assert(!DynamicCast<SBlueBell>(pPlant));
    assert(!DynamicCast<SBlueBell>(pFlower));
    assert(DynamicCast<SBlueBell>(pBlueBell));

    assert(!DynamicCast<SDaffodil>(pPlant));
    assert(!DynamicCast<SDaffodil>(pFlower));
    assert(DynamicCast<SDaffodil>(pDaffodil));

    // Create test data
    SPlant*  pPlantTree = pTree;
    SPlant*  pPlantFlower = pFlower;
    SPlant*  pPlantBlueBell = pBlueBell;
    SPlant*  pPlantDaffodil = pDaffodil;
    SFlower* pFlowerBlueBell = pBlueBell;
    SFlower* pFlowerDaffodil = pDaffodil;

    // Tests
    assert(DynamicCast<SPlant>(pPlantTree));
    assert(DynamicCast<SPlant>(pPlantFlower));
    assert(DynamicCast<SPlant>(pPlantBlueBell));
    assert(DynamicCast<SPlant>(pPlantDaffodil));
    assert(DynamicCast<SPlant>(pFlowerBlueBell));
    assert(DynamicCast<SPlant>(pFlowerDaffodil));

    assert(DynamicCast<STree>(pPlantTree));
    assert(!DynamicCast<STree>(pPlantFlower));
    assert(!DynamicCast<STree>(pPlantBlueBell));
    assert(!DynamicCast<STree>(pPlantDaffodil));

    assert(!DynamicCast<SFlower>(pPlantTree));
    assert(DynamicCast<SFlower>(pPlantFlower));
    assert(DynamicCast<SFlower>(pPlantBlueBell));
    assert(DynamicCast<SFlower>(pPlantDaffodil));
    assert(DynamicCast<SFlower>(pFlowerBlueBell));
    assert(DynamicCast<SFlower>(pFlowerDaffodil));

    assert(!DynamicCast<SBlueBell>(pPlantTree));
    assert(!DynamicCast<SBlueBell>(pPlantFlower));
    assert(DynamicCast<SBlueBell>(pPlantBlueBell));
    assert(!DynamicCast<SBlueBell>(pPlantDaffodil));
    assert(DynamicCast<SBlueBell>(pFlowerBlueBell));
    assert(!DynamicCast<SBlueBell>(pFlowerDaffodil));

    assert(!DynamicCast<SDaffodil>(pPlantTree));
    assert(!DynamicCast<SDaffodil>(pPlantFlower));
    assert(!DynamicCast<SDaffodil>(pPlantBlueBell));
    assert(DynamicCast<SDaffodil>(pPlantDaffodil));
    assert(!DynamicCast<SDaffodil>(pFlowerBlueBell));
    assert(DynamicCast<SDaffodil>(pFlowerDaffodil));

    // Check ClassBits type is at least 64 bits
    assert(sizeof(ClassBits) * 8 >= 64);

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
void SharedUtil_WildcardMatch_Tests()
{
    // WildcardMatch
    {
        TEST_FUNCTION
        assert(WildcardMatch(a, b) == result);
        TEST_VARS
        const char* a;
        const char* b;
        bool        result;
        TEST_DATA = {
            {"*bd*", "abcbde", true},
            {"*bd?f*", "abcbdef_bdgh", true},
            {"*bd?h*", "abcbdef_bdgh", true},
            {"*bd?g*", "abcbdef_bdgh", false},
            {"scr*w?d", "screeeewywxd", true},
            {"A*B", "A_B_B", true},
            {"", "", true},
            {"*", "", true},
            {"*", "A", true},
            {"", "A", false},
            {"A*", "", false},
            {"A*", "AAB", true},
            {"A*", "BAA", false},
            {"A*", "A", true},
            {"A*B", "", false},
            {"A*B", "AAB", true},
            {"A*B", "AB", true},
            {"A*B", "AABA", false},
            {"A*B", "ABAB", true},
            {"A*B", "ABBBB", true},
            {"A*B*C", "", false},
            {"A*B*C", "ABC", true},
            {"A*B*C", "ABCC", true},
            {"A*B*C", "ABBBC", true},
            {"A*B*C", "ABBBBCCCC", true},
            {"A*B*C", "ABCBBBCBCCCBCBCCCC", true},
            {"A*B*", "AB", true},
            {"A*B*", "AABA", true},
            {"A*B*", "ABAB", true},
            {"A*B*", "ABBBB", true},
            {"A*B*C*", "", false},
            {"A*B*C*", "ABC", true},
            {"A*B*C*", "ABCC", true},
            {"A*B*C*", "ABBBC", true},
            {"A*B*C*", "ABBBBCCCC", true},
            {"A*B*C*", "ABCBBBCBCCCBCBCCCC", true},
            {"A?", "AAB", false},
            {"A?B", "AAB", true},
            {"A?*", "A", false},
            {"A?*", "ABBCC", true},
            {"A?*", "BAA", false},
        };
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
void SharedUtil_Collection_Tests()
{
    // std::map
    {
        std::map<uint, SString> testMap;
        MapSet(testMap, 10, "10");
        MapSet(testMap, 20, "20");
        MapSet(testMap, 30, "30");
        MapSet(testMap, 40, "40");

        std::map<uint, SString>::iterator iter = testMap.begin();
        assert(iter->first == 10);
        iter++;
        assert(iter->first == 20);
        iter++;
        testMap.erase(iter++);
        assert(iter->first == 40);
        iter++;
        assert(iter == testMap.end());
    }

    // std::set
    {
        std::set<uint> testMap;
        MapInsert(testMap, 10);
        MapInsert(testMap, 20);
        MapInsert(testMap, 30);
        MapInsert(testMap, 40);

        std::set<uint>::iterator iter = testMap.begin();
        assert(*iter == 10);
        iter++;
        assert(*iter == 20);
        iter++;
        testMap.erase(iter++);
        assert(*iter == 40);
        iter++;
        assert(iter == testMap.end());
    }

    // std::list
    {
        std::list<uint> testList;
        testList.push_back(10);
        testList.push_back(20);
        testList.push_back(30);
        testList.push_back(40);

        std::list<uint>::iterator iter = testList.begin();
        assert(*iter == 10);
        iter++;
        assert(*iter == 20);
        iter++;
        iter = testList.erase(iter);
        assert(*iter == 40);
        iter++;
        assert(iter == testList.end());
    }

    // std::vector
    {
        std::vector<uint> testList;
        testList.push_back(10);
        testList.push_back(20);
        testList.push_back(30);
        testList.push_back(40);

        std::vector<uint>::iterator iter = testList.begin();
        assert(*iter == 10);
        iter++;
        assert(*iter == 20);
        iter++;
        iter = testList.erase(iter);
        assert(*iter == 40);
        iter++;
        assert(iter == testList.end());
    }
}

///////////////////////////////////////////////////////////////
//
// SharedUtil_String_Tests
//
// Test behaviour of strings
//
///////////////////////////////////////////////////////////////
void SharedUtil_String_Tests()
{
    // Formatting
    {
        SString strTemp1("name:" PRSinS, "dave");
        SString strTemp2("name:" PRWinS, L"dave");
        WString wstrTemp3(L"name:" PRSinW, "dave");
        WString wstrTemp4(L"name:" PRWinW, L"dave");

        assert(strTemp1 == "name:dave");
        assert(strTemp2 == "name:dave");
        assert(wstrTemp3 == L"name:dave");
        assert(wstrTemp4 == L"name:dave");
    }

    // To/From Ansi
    {
        SString strTemp1 = "abcABC 123";
        WString wstrTemp2 = "defDEF 456";
        WString wstrTemp3 = L"ghiGHI 789";

        assert(wstrTemp2 == L"defDEF 456");

        SString strTemp2 = wstrTemp2.ToAnsi();
        SString strTemp3 = wstrTemp3.ToAnsi();

        assert(strTemp2 == "defDEF 456");
        assert(strTemp3 == "ghiGHI 789");
    }

    // Escaping URL arguments
    {
        TEST_FUNCTION
        SStringX strInputA((const char*)a, sizeof(a));
        SString  strEscaped = EscapeURLArgument(strInputA);
        SString  strUnescaped = UnescapeString(strEscaped, '%');
        assert(strEscaped == result);
        assert(strInputA == strUnescaped);
        TEST_VARS
        const uchar a[5];
        const char* result;
        TEST_DATA = {
            {{0x00, 0x10, 0x20, 0x21, 0x22}, "%00%10%20%21%22"},
            {{0x7F, 0x80, 0x81, 0xFE, 0xFF}, "%7F%80%81%FE%FF"},
        };
        TEST_END
    }

    {
        TEST_FUNCTION
        SStringX strInputA(a);
        SString  strEscaped = EscapeURLArgument(strInputA);
        SString  strUnescaped = UnescapeString(strEscaped, '%');
        assert(strEscaped == result);
        assert(strInputA == strUnescaped);
        TEST_VARS
        const char* a;
        const char* result;
        TEST_DATA = {
            {"!*'();:@", "%21%2A%27%28%29%3B%3A%40"},
            {"&=+$,/?#", "%26%3D%2B%24%2C%2F%3F%23"},
            {"[] \"%<>\\", "%5B%5D%20%22%25%3C%3E%5C"},
            {"^`{|}", "%5E%60%7B%7C%7D"},
            {"AZaz09-_.~", "AZaz09-_.~"},
        };
        TEST_END
    }

    // RemoveColorCodes
    {
        TEST_FUNCTION
        SString strRemoved = RemoveColorCodes(a);
        assert(strRemoved == result);
        TEST_VARS
        const char* a;
        const char* result;
        TEST_DATA = {
            {"aa #0f0F34 bb", "aa  bb"},
            {"aa #0f0F34#AABBBB bb", "aa  bb"},
            {"aa #0f0F3G#AABBBB bb", "aa #0f0F3G bb"},
            {"aa #0f0F34#AABBB bb", "aa #AABBB bb"},
            {"#0f0F34#AABBB1", ""},
            {"#0f0F34 x #AABBB1", " x "},
            {"#0f0F34#0f0F34 x #AABBB1#AABBB1", " x "},
            {"#123456#12345G#123456#12345G", "#12345G#12345G"},
            {"#123456#12345#123456#125G", "#12345#125G"},
            {"##123456#125G##123456#12345", "##125G##12345"},
        };
        TEST_END
    }

    // RemoveColorCodesInPlaceW
    {
        TEST_FUNCTION
        WString wstrString = a;
        RemoveColorCodesInPlaceW(wstrString);
        assert(wstrString == result);
        TEST_VARS
        const wchar_t* a;
        const wchar_t* result;
        TEST_DATA = {
            {L"aa #0f0F34 bb", L"aa  bb"},
            {L"aa #0f0F34#AABBBB bb", L"aa  bb"},
            {L"aa #0f0F3G#AABBBB bb", L"aa #0f0F3G bb"},
            {L"aa #0f0F34#AABBB bb", L"aa #AABBB bb"},
            {L"#0f0F34#AABBB1", L""},
            {L"#0f0F34 x #AABBB1", L" x "},
            {L"#0f0F34#0f0F34 x #AABBB1#AABBB1", L" x "},
            {L"#123456#12345G#123456#12345G", L"#12345G#12345G"},
            {L"#123456#12345#123456#125G", L"#12345#125G"},
            {L"##123456#125G##123456#12345", L"##125G##12345"},
        };
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
void SharedUtil_Hash_Tests()
{
    // ConvertHexStringToData/ConvertDataToHexString
    {
        TEST_FUNCTION
        char buffer[256];
        uint length = a.length() / 2;
        ConvertHexStringToData(a, buffer, length);
        SString strResult = ConvertDataToHexString(buffer, length);
        assert(strResult == b);
        TEST_VARS
        const SString a;
        const char*   b;
        TEST_DATA = {
            {"66B9139D8C424BE2BCF224706B48FEB8", "66B9139D8C424BE2BCF224706B48FEB8"},
            {"E7C7253C74275F2DC2DC8C6828816C18301636949369F3bad87666C81E71B309", "E7C7253C74275F2DC2DC8C6828816C18301636949369F3BAD87666C81E71B309"},
            {"61", "61"},
            {"\x01"
             "A"
             "\x1F\x80"
             "BC"
             "\xFE\xFF",
             "0A00BC00"},
        };
        TEST_END
    }

    // TeaEncode/TeaDecode
    {
        TEST_FUNCTION
        SString strEncoded;
        TeaEncode(a, b, &strEncoded);
        if (!result.empty())
            assert(strEncoded == result);
        SString strDecoded;
        TeaDecode(strEncoded, b, &strDecoded);
        assert(a == *strDecoded);
        TEST_VARS
        const SString a;
        const SString b;
        const SString result;
        TEST_DATA = {
            {"1234", "AB12£$_ ", "\xD2\xB4\x75\x5C\xDC\x15\x54\xC9"},
            {"Hello thereHello there", "78111E998C42243285635E39AFDD614B\0 AB12£$_ ", ""},
            {"78111E998C42243285635E39AFD\0D614B AB12£$_ ", "Hello thereHello there", ""},
        };
        TEST_END
    }

    // RSA keypair generation and encryption test
    {
        TEST_FUNCTION
        auto [privateKey, publicKey] = GenerateRsaKeyPair(keysize);
        assert(!privateKey.empty());
        assert(!publicKey.empty());
        SString encryptedData = RsaEncode(data, publicKey);
        assert(!encryptedData.empty());
        SString decryptedData = RsaDecode(encryptedData, privateKey);
        assert(!decryptedData.empty());
        assert(data == decryptedData);
        TEST_VARS
        const SString data;
        const int     keysize;
        TEST_DATA = {
            {"Hello world", 128},
            {"!@#$%^&*()_+|:<>", 2048},
            {"Hello there", 4096},
        };
        TEST_END
    }

    // RSA decryption test
    {
        TEST_FUNCTION
        SString decryptedData = RsaDecode(encryptedData, privateKey);
        assert(!decryptedData.empty());
        assert(exptectedData == decryptedData);
        TEST_VARS
        const SString exptectedData;
        const SString encryptedData;
        const SString privateKey;
        TEST_DATA = {
            {"Hello world",
             "\x3A\xFE\xAF\x3B\xE3\x79\xFE\x63\xDB\xFA\x72\x6C\xFB\x10\x17\xA5\x6B\x6E\x89\x99\x47\x67\x86\xE0\x76\x53\xD7\x6B\x81\x83\xA5\x21\xF3\xB6\xDA\x12"
             "\xC1\xB5\x18\xC9\x1A\x6B\x35\x62\x22\x05\xBC\x70\xDD\xB2\x60\x8D\x06\x45\x35\xFF\xB0\x7C\x09\xF8\xA5\x27\x65\xEA",
             "\x30\x82\x01\x38\x02\x01\x00\x02\x40\x79\x30\xCA\xEF\x9A\x2C\xC1\xE8\x9C\x7E\x7A\x31\xD0\xCC\x60\xA3\x23\xD2\xB0\x41\xD9\xDC\xD5\xA7\xAA\x75\x9E"
             "\x17\x45\x38\xDA\x21\xC6\x29\xDA\x56\xD7\xAD\x5C\x66\x93\x48\x46\xF6\x31\x82\xEA\x0B\x44\x81\x94\xF3\x55\x17\x63\xE8\x7F\xC4\x8F\xF6\x5A\x6A\x9C"
             "\x1D\x02\x03\x01\x00\x01\x02\x40\x06\x7B\x3E\x8F\x3B\xEE\xA2\xDC\x3E\xEA\x60\xBC\xA3\x4F\x1A\xE7\xFE\xB5\x78\xE9\xBC\xAE\x1B\xDB\x20\xB9\xB3\x9F"
             "\xC8\xFA\xF6\x67\x7E\x9D\x9A\x8E\x89\xEE\x8E\xDB\x9D\x24\x11\xAC\xCD\xB7\x56\x22\xFC\xE3\x93\x6E\x6B\x2B\x99\x89\xAE\xFD\x75\xC9\x59\x42\x98\x5D"
             "\x02\x21\x00\xE0\x5A\xA7\x8C\x7E\x47\x4E\x18\x91\x81\x83\x06\xAD\xBB\xA6\x91\xF9\x1B\x6A\x51\xFF\xEC\x41\x22\x58\x83\x22\x7A\x02\xAC\xD5\x57\x02"
             "\x21\x00\x8A\x48\xF0\xE6\x36\xD8\x28\x4B\x3B\x47\xE9\x31\xCE\x1F\x11\x0D\xB4\xF8\xBB\x3F\x32\xB1\x66\xE5\xF5\xA6\xE4\xFD\x9E\x14\xDD\xAB\x02\x20"
             "\x51\xE9\xC7\x72\xCC\x48\x4C\x83\x3B\xDD\x05\x71\xD2\xCC\xC8\xAC\x53\x3F\x67\x3A\x39\x2E\x91\x39\x71\x07\xAF\xF6\xCC\x33\x3E\x51\x02\x20\x52\xC3"
             "\x85\x2A\xC3\xF8\x6B\x65\x5B\x7D\x96\x9C\x30\x79\x77\x83\x74\xA4\xC9\xAC\xF1\x72\x2B\xDB\xC6\x42\xB5\x1A\xF5\xDA\x17\x31\x02\x20\x16\x0E\xBE\xD3"
             "\x12\x6E\x30\xE2\x01\x96\x26\x0B\xCC\x89\xC1\xD1\xF1\x0C\x25\x9C\x18\x47\x89\x04\xAA\x00\x5A\x45\x26\x81\xD1\x60"},
            {"!@#$%^&*()_+|:<>",
             "\xBD\xA6\x40\x01\xD4\x53\xC3\x20\x2C\xCE\x9E\x5C\x91\x7A\x58\xCD\x5F\x79\xCF\x39\x98\x2C\xD0\x15\xA3\x81\x9D\x07\x15\x89\x2C\x97\xC3\x65\x9B\xD6"
             "\xF7\xD0\xBA\x6D\x3E\x1A\xFB\x39\xFE\x4A\xBB\xC8\x08\xB6\xD6\x46\xA3\xD5\x5E\xB6\x74\x5E\xE5\xBE\x68\xBC\xBE\xA8",
             "\x30\x82\x01\x3B\x02\x01\x00\x02\x41\x00\xD9\xE7\x6B\x26\x9A\xA7\xF0\x1E\x99\x78\x59\x4A\xDB\xD1\x28\x70\x05\x2D\x63\x08\x1A\xED\x4C\x2E\x2A\xF2"
             "\x3D\xA8\x3E\x75\x3B\x99\xFB\x4F\xA8\x7A\xD8\x3B\x6A\xE7\x6E\xD4\x64\x38\xCC\x58\xE8\x84\x05\x4B\x0A\xC7\xE5\xA7\x18\x18\xA3\x09\x6F\x14\xA1\x40"
             "\x42\xFD\x02\x03\x01\x00\x01\x02\x41\x00\x97\x02\xA3\xE5\x6B\x04\xBB\x89\xD9\x3D\xE8\x53\xD2\xA6\xCE\xE9\x63\x0D\x23\x36\x78\xC2\xBF\x2A\xB7\xA4"
             "\x8D\x4A\x4D\x8E\x9D\x30\x3F\xEA\x6F\xA3\x6B\x00\x0D\x90\x7A\x1C\xAE\x38\xC9\x1D\xCE\xD4\xB2\xD6\x2A\x87\x57\x20\x99\xD9\xFA\x00\xB8\x5C\x3B\x9C"
             "\xEC\xD9\x02\x21\x00\xF0\x45\x13\x08\xD0\x31\xD8\x91\xC1\x82\x8A\x37\x4C\x16\x27\xD3\xB5\xC4\x87\x31\x95\x82\x81\x1B\x6A\xF9\x6E\x72\xFC\xED\x57"
             "\x13\x02\x21\x00\xE8\x2B\x7E\x0B\x1A\xCF\x5D\x98\xA6\xF1\xA5\x37\x94\xE5\xFC\xA8\x97\x29\x30\x1A\x04\xA8\xA8\xD4\xC5\xA5\xF4\x67\xF2\x4D\xEF\xAF"
             "\x02\x20\x61\x6F\x91\x9D\x84\x58\x82\x7B\xF1\x5E\x8F\xD0\x86\xF1\x81\x2B\x30\xCE\x47\x09\x0B\x99\x85\xE3\xE9\x88\xC7\x8A\xA8\x19\x53\xB3\x02\x21"
             "\x00\xC2\x8C\x37\x69\x8C\xC4\xB9\x34\x82\xA8\x38\xFF\x13\x59\x89\x57\xA8\x9B\x4E\x00\x6F\x68\xF9\x58\x9A\x6A\xCF\x1C\x0F\x71\xF9\x67\x02\x20\x37"
             "\x17\x3D\x0F\xE6\xB4\x3A\x33\x9D\x0B\xCB\x2E\x16\x04\xD7\xA6\x6A\x16\x22\x13\x24\xBD\xD8\x0D\x50\x3C\xD7\x95\xC5\xC1\xC1\x00"},
            {"Hello there",
             "\x1B\x22\x51\x7F\x6F\xF3\x7A\x4B\x99\x8A\x4C\xA1\x8A\x54\x06\x9E\x43\x30\x90\x68\x58\x4F\x4F\x82\xFA\x2B\xB8\x70\x0C\x5B\xCD\x83\xEA\x32\x2E\x69"
             "\xF5\x29\x67\xAA\xF1\x2A\x89\x39\x04\xFE\x4C\x98\x32\x73\xC1\x31\x6B\xB0\x48\x4F\x1B\xE5\x7C\xC0\x2F\xDB\xFF\xB7",
             "\x30\x82\x01\x3C\x02\x01\x00\x02\x41\x00\x8F\x35\x6A\xD0\x22\x30\xD3\xC1\xF3\x97\xEB\x45\xDB\x8B\xA4\x40\xCB\x5A\x37\x1E\xC0\xA0\x58\x5D\x39\x41"
             "\xE5\x6F\xBD\xD9\xA6\xB2\x98\x63\xE9\x13\xC2\xFC\x1B\x2F\x3B\x6D\x5C\x70\xA7\x60\x5C\x7E\x9C\x30\x73\xDF\xD7\x84\x3C\xC2\x7D\xBE\x83\x48\xFD\x2B"
             "\x60\x8B\x02\x03\x01\x00\x01\x02\x41\x00\x80\x8B\x48\x27\x2E\xA9\x79\x4A\x1C\xA6\xFE\xAE\x23\xD5\x02\x48\x8E\xBF\x99\xBE\x66\xD5\x06\xBC\x17\x0B"
             "\x09\x30\xF5\xCE\x8A\x0B\x61\x5A\x4A\x28\x78\x49\x3D\x42\xBD\x2A\x8E\x3A\xFC\x73\x68\xA7\xE5\xC6\x24\x59\xE7\x2D\x23\x72\x77\x17\x48\xF1\xD3\xDA"
             "\x07\xB1\x02\x21\x00\xED\xD2\x48\xA9\xD5\xD0\x78\x06\xA9\x71\x01\x58\xA4\xE1\xD1\x96\xDE\xA9\xA9\xD6\x0B\x2A\x6E\xFB\xB6\x73\xE9\x19\xBC\xFF\x56"
             "\xAF\x02\x21\x00\x9A\x27\xBD\x86\x2D\x2F\xDC\x4E\x7F\xB3\xD6\xFB\xBA\x8A\x25\xFF\x91\xAB\x32\x2B\x19\xB4\x44\x44\xFC\xCF\xA7\x93\x1A\xA0\x0A\xE5"
             "\x02\x21\x00\x88\x45\xD3\x8F\x45\xCE\x73\xC8\x55\x75\x44\x71\x37\xE1\x91\xC8\x62\x84\x53\xA6\x2B\xB5\xBF\xEC\x8A\x2A\xB6\xEA\x21\xAB\x2A\x4F\x02"
             "\x20\x3D\x8F\x1F\x1D\x04\xD0\x76\xFE\x95\xFA\x80\x20\x58\x36\xB0\xC3\xE5\x2C\xE3\x44\xD8\xE7\xE6\xD7\x51\xC9\xAE\xD6\x8C\xAC\x5A\x8D\x02\x21\x00"
             "\xDD\x5B\x93\xFA\xB8\x93\x03\x67\x7A\x3B\x2B\x00\x0E\xB0\xB7\xAB\xD1\xD5\x38\xE4\xF3\x62\x7B\xDD\x96\x1B\x90\xDB\x9C\xF7\xA7\x3F"},
        };
        TEST_END
    }

    // Aes128encode/Aes128decode
    {
        TEST_FUNCTION
        std::pair<std::string, std::string> res = Aes128encode(data, key);
        assert(!res.first.empty());
        assert(!res.second.empty());
        SString strDecoded = Aes128decode(res.first, key, res.second);
        assert(data == strDecoded);
        TEST_VARS
        const SString key;
        const SString data;
        TEST_DATA = {
            {"5347123412340000", "AB12£$_ "},
            {"gHtySkGerYnhDxAs", "78111E998C42243285635E39AFDD614B\0 AB12£$_ "},
            {"!@#$%^&*()_+|:<>", "Hello thereHello there"},
        };
        TEST_END
    }

    #define szTempFilename "hash_""\xD0""\x98""_test"

    // MD5
    {
        TEST_FUNCTION
        SString strResult = CMD5Hasher::CalculateHexString(a.c_str(), a.length());
        assert(strResult == result);

        strResult = GenerateHashHexString(EHashFunction::MD5, a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::MD5, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "D41D8CD98F00B204E9800998ECF8427E"},
            {"Hello there", "E8EA7A8D1E93E8764A84A0F3DF4644DE"},
            {"AB12£$_\0 ", "78111E998C42243285635E39AFDD614B"},
        };
        TEST_END
    }

    // SHA256
    {
        TEST_FUNCTION
        SString strResult = GenerateSha256HexString(a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::SHA256, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855"},
            {"Hello there", "4E47826698BB4630FB4451010062FADBF85D61427CBDFAED7AD0F23F239BED89"},
            {"AB12£$_\0 ", "A427AEDD32E652FED23901406BCC49BF78B906E69699A68932638502E8C1138B"},
        };
        TEST_END
    }

    // SHA1
    {
        TEST_FUNCTION
        SString strResult = GenerateHashHexString(EHashFunction::SHA1, a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::SHA1, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709"},
            {"Hello there", "726C76553E1A3FDEA29134F36E6AF2EA05EC5CCE"},
            {"AB12£$_\0 ", "CA7B95DF48B83232FCA0FE3FAE7A787784F54225"},
        };
        TEST_END
    }

    // SHA224
    {
        TEST_FUNCTION
        SString strResult = GenerateHashHexString(EHashFunction::SHA224, a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::SHA224, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "D14A028C2A3A2BC9476102BB288234C415A2B01F828EA62AC5B3E42F"},
            {"Hello there", "40AACC4967ECA7730A1A069539D78AE7782480802E481F1ECC26927D"},
            {"AB12£$_\0 ", "73420F2E80A236DD7C836C68177E282BC8E86CD8BB497E5F443F1FAE"},
        };
        TEST_END
    }

    // SHA384
    {
        TEST_FUNCTION
        SString strResult = GenerateHashHexString(EHashFunction::SHA384, a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::SHA384, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "38B060A751AC96384CD9327EB1B1E36A21FDB71114BE07434C0CC7BF63F6E1DA274EDEBFE76F65FBD51AD2F14898B95B"},
            {"Hello there", "7438E0294C534D6CA6CC2EFB04A60DB488C86B66C4CBD3C00D11D58C8020274AB0A2A720C88986968D894F26B16C461F"},
            {"AB12£$_\0 ", "DFCAF84C21F93CC88DE6CB54D838FEE5ACF592DCF392883708BAD1CFF7B847DEA9A175C5E87014D5829E66E17571F7E6"},
        };
        TEST_END
    }

    // SHA512
    {
        TEST_FUNCTION
        SString strResult = GenerateHashHexString(EHashFunction::SHA512, a);
        assert(strResult == result);

        FileSave(szTempFilename, a);
        strResult = GenerateHashHexStringFromFile(EHashFunction::SHA512, szTempFilename);
        assert(strResult == result);
        TEST_VARS
        const SString a;
        const char*   result;
        TEST_DATA = {
            {"", "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"},
            {"Hello there", "567683DDBA1F5A576B68EC26F41FFBCC7E718D646839AC6C2EF746FE952CEF4CBE6DEA635BC2F098B92B65CAACF482333BB9D1D9A3089BC4F01CB86F7A2FBC18"},
            {"AB12£$_\0 ", "EE57E02866026848F988E793E9B099931CBCA8773B13EA5055ABA462885E8B044CE5C72CB528712CE2A442707F2BC25A52CC91F8C09DD1C6A6A5C9A63D52F320"},
        };
        TEST_END
    }

    FileDelete(szTempFilename);
}
