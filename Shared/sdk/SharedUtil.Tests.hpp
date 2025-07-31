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
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
            {"1234", "AB12£$_ ", "\x08\x56\x3B\x4F\x31\x2D\xE4\x4A"},
            {"Hello thereHello there", "78111E998C42243285635E39AFDD614B\0 AB12£$_ ", ""},
            {"78111E998C42243285635E39AFD\0D614B AB12£$_ ", "Hello thereHello there", ""},
        };
        TEST_END
    }

    // Hmac Test
    {
        TEST_FUNCTION
        SString hmacResult;
        switch (algorithm)
        {
            case HmacAlgorithm::MD5:
                hmacResult = SharedUtil::Hmac<CryptoPP::MD5>(data, key);
                break;
            case HmacAlgorithm::SHA1:
                hmacResult = SharedUtil::Hmac<CryptoPP::SHA1>(data, key);
                break;
            case HmacAlgorithm::SHA224:
                hmacResult = SharedUtil::Hmac<CryptoPP::SHA224>(data, key);
                break;
            case HmacAlgorithm::SHA256:
                hmacResult = SharedUtil::Hmac<CryptoPP::SHA256>(data, key);
                break;
            case HmacAlgorithm::SHA384:
                hmacResult = SharedUtil::Hmac<CryptoPP::SHA384>(data, key);
                break;
            case HmacAlgorithm::SHA512:
                hmacResult = SharedUtil::Hmac<CryptoPP::SHA512>(data, key);
                break;
        }
        assert(!hmacResult.empty());
        assert(hmacResult == result);
        TEST_VARS
        const SString       data;
        const SString       key;
        const SString       result;
        const HmacAlgorithm algorithm;
        TEST_DATA = {
            {"Hello world", "hecker was there", "657C7088ADEA11E6482EE794D3E5489C", HmacAlgorithm::MD5},
            {"cstddef", "", "0339B2CA65A63209C656047C5B11ADA73B63A367", HmacAlgorithm::SHA1},
            {"Hello thereHello there", "!@#$%^&*()_+|:<>", "A7A00E964617DFB59324502786BB28AEEF22898C00B226A7B4A1D607", HmacAlgorithm::SHA224},
            {"!@#$%^&*()_+|:<>", "cppsymbol", "46105B670A55EA8808B16FFC8B88507EAEA3E9D1F5A55891CD04136FB2AADA15", HmacAlgorithm::SHA256},
            {"value", "sha384", "CEC945A598261608218BA685EEC02D773F57AFD6410AF67D2A2D1B0D22DAE8624D0F369E55C8C7E774805204A2B5A75A", HmacAlgorithm::SHA384},
            {"", "gHtySkGerYnhDxAs",
             "4E6E87CE637808642B902A07F43CA6A1CFE4346054C0C8C542A67C4BF206708CF5AFE3F1BB6D53DCE3469CDEA1CE11A0892EE2F95322C45D2CB809F165AD3BB3",
             HmacAlgorithm::SHA512},
        };
        TEST_END
    }

    // RSA keypair generation and encryption test
    {
        TEST_FUNCTION
        auto keyPair = GenerateRsaKeyPair(keysize);
        assert(!keyPair.privateKey.empty());
        assert(!keyPair.publicKey.empty());
        SString encryptedData = RsaEncode(data, keyPair.publicKey);
        assert(!encryptedData.empty());
        SString decryptedData = RsaDecode(encryptedData, keyPair.privateKey);
        assert(!decryptedData.empty());
        assert(data == decryptedData);
        TEST_VARS
        const SString data;
        const int     keysize;
        TEST_DATA = {
            {"Hello world", 1024},
            {"!@#$%^&*()_+|:<>", 2048},
            {"Hello there", 4096},
        };
        TEST_END
    }

    // RSA decryption test
    {
        TEST_FUNCTION
        SString rawEncodedData = Base64decode(encryptedData);
        SString rawPrivateKey = Base64decode(privateKey);
        assert(!rawEncodedData.empty());
        assert(!rawPrivateKey.empty());
        SString decryptedData = RsaDecode(rawEncodedData, rawPrivateKey);
        assert(!decryptedData.empty());
        assert(exptectedData == decryptedData);
        TEST_VARS
        const SString exptectedData;
        const SString encryptedData;
        const SString privateKey;
        TEST_DATA = {
            {"Hello world",
             "ei2zOjHZLpp5/xhc5GyUoFIdmpj8BZFOtLOHafunis3KVYcEoZmm/JxMMnGMiAemqWCmGhkNin76Jy84G5O9pHCFR499cEvAq92Or1cX8tiM3nqJrgbeNj6SM3QWHGhnx"
             "NDaxHXbXAvh9M2dyR12+X5bQVhex5O0R2XGv1XaaRw=",
             "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAJOYjQmpsmZb8KafeeUtaEydwjM2rRNrdR3BvKdNa4MMx5+YxweWrbsyDycW4AIfU5jcHN61QHaRx53zS"
             "F59cV79ARKoIY+Csi3yD9mgmN81O043/SuU+HqpIsjapTqvT3w4o0dFbJAcoxTvcJf0ZjVhkzK1V6Jucf+4k8OwqeAFAgMBAAECgYBF+HMcWpudlQaQJ1hlQlKIx91R5CD"
             "G7RF3iuoBsl0+omdcjNQGD/PKo9+1G04VE37RH8B8ABdQXqnwgm4ThYqh0XOcL2QcpqzTxCLJI9XsjA5T8EUpETZYl4CNTyVqaEHNywPPVgo7M+dJjfHaKZAiKlpzxJ34M"
             "aG/fmfH7p+U3QJBANQhOLugLnz2zPN7ek9uPRlJlp1KGLA7n773fQ9lccBwxmQcec7wshlyOiH6QzYfyry+gH8WmWcs23NvXh+x/p8CQQCyHrcR3r4y7l7ziKc2NsAvL1v"
             "PD+hyluNae87XTEN14mSc2qFyjIBRRDS+2Y+nZBLWXOwpTaj7MO24yu1SFjLbAkA3D6R9Nxe6AgmyzYMy6OddD/fTPe9olyFg3TaN74P2Mo+cXrPTXjCsYDTsqtwVSW1DT"
             "qNl2YTZ/g+8R9UDnxz7AkEAma+83/NkAwf/73tyY2m8Szo+qhucwfqFMtZusv+/mWB81YYjFXY7ZWNHa4DG3XSQeFt2/XBfpFUkWHTnbhNvTwJBAJq5Gl/CM6Zozhr9yTC"
             "S1i3n3ZuX2V0F8X8QlTC90P5GJLAaGadY0DEJICNhmbaRVZ6W4GSmi3nxnibmz936/1k="},
            {"!@#$%^&*()_+|:<>",
             "Juwvq3z+ykIIdThZIUgMo3VjX4QqnT16CcJLf6ueCLHAK6E9tneLSSHmMjWr1NexPEVHr9BD3lRnmV78d2p/9yIPIF9tcDcMAqNnNQNcQ+LSgg8urDIWhT+ORCatZsM20"
             "7A/W+sQCBqyEsSMflgdc9+K7bmxksucclGyAbjpOvUK7IEH65LJDWFIG4q2uUnzHZkl8UTh26SPbZWecBWME1/+CMR62zJdM43Wm31bFzOPCQCU/5ecJX9brpOX1lGPI6e"
             "Eg895ZwSdLH+JaG6wA9BGc2Z/6Fg+EOxNRy8ZzPD7zq6+Z1If3m3ieVVaV0uraVGXUz4ygKgDDTmSwL5vLQ==",
             "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCddRAukUoE4EfHv5RO1rHU1s8PLwzAE1eUf4uOcrfNNA677L15RD3ZC4RfdpA4Ya41mMQAy4BimIXHJ"
             "Ko0JcygV2Jo9W0268afsJpDgz69uZiXp4OFG1CfKrDNG7oyM310OBkrsQdBPoBNveeBqjWf88oq95nHf5h8MMPL+x2WRf4LELnx1VlaGaw0DfVPHnNtpnv0cMFEc/saCBu"
             "UtethD/1HataFucwNotr77tQy1kK8K9nHrFhC69kvPti2unL0IQ3OYMjgZoHpv1Ecz5l7zm2DyRa9RIeeZXnADOwaS/9e6hvriirvs7aqlioVIK1wi82hrZLuOh0rHt5WL"
             "+i3AgMBAAECggEAD/DpGX3WG5ZFTiDEOBMeX+j7f5MI3G1Y4YzkKSDOIPVTRI1cRWm2RUIYPb34Le0dwQldVqNDtTex246HuM3W4KjYKr4mN3g5XLN5erpP4M9odnocI56"
             "3U9XznmBfQnPaEKkZS0YZ8O4N+VDvnj4IPdBluGHm7L6rRm99cypSPYNgxCyYX0JGEfOYXmvnVqlIlnohVy4IoQ8toPE8WIChN+kXfkcluj++YDvF+7ir7vh8EVgOsgtoz"
             "80CS+sn6lUAh+TUFmbHqJvzMn9LovhkMxhq+U55bxpVlzsWQWKKPQsU2F7g3akCux2zapI0C8rIGOqmoO8cY3hkXvAG/LpWuQKBgQD98dJ66cMfGH5uS3XeOadS9JSqyql"
             "jejWg7LhFWqpK2UAqE8hxmAhJIRexEH7jxYhCGSro2sLsbiZw00t94cNIjUdfm2urLTNBk+LNExlGMwJmVzNCSIx4085JFLRMIf9XAAOF62gZGW3yRKSl+lfG2aEMN/oQR"
             "rm2IVqpLnEsiwKBgQCeu1FJB7zUgOVZEg+DwEm+SalhERwQY200UnkIW0Q9T+SHVS1VGBQHa3oQIICotufLPn1LIRKEl+6mP+ITbxvdFbwCueNnaukGSfVvdno0vwfoFjC"
             "PD5nlwWuSf4/kAiA2zX21Hc7yO3+VPSzy9FzLcEQrpg41CnnjVzntl71eBQKBgHuuPpQsrCLF/DXdvM96BvuJMaspP2y6xcnV4T2uFciUnQKhVfu1TzvGyVIstZBb0axXs"
             "6VrfopGb6R91IO/5Cu+22c+PCdHwXIMA+dbiPC+wUl06Ps8eGYUyqCfW9zEpicKU6zy5WVsFDdtpT9NeVjY/+4CiWFKAQMqpYxGmUpXAoGAOIq/NMWF1SipCeZuEl9/pUH"
             "2DPacSZYxQ08RgBGKqFB6Nl74C/TMxavaSj5Ztj+ZKAP1e7qnLwYz45KkBIoyoblpRItU3vTtVuknfXH1rC5UMa8d0wgaqQYSPJgl+HFtajgezFJ28kCRfObSySSUnX87O"
             "VUogGpf6sSahPDlasUCgYBz2t0Qh7aILYiqIu9VaqmSAUQV21tk8rnfD5DmwZchFmVsb1TqogRLWHRz78ciV/RlWt1+QI1scUKHvYUdxrsmR2zq2AO7qWE4csDtYNgnuwZ"
             "vTpg9Nn0v9257IPf6O2ihBStJhh18tyR5VIjY5ftAWMZpF67TDdlNuKxBtkUmdg=="},
            {"Hecker there",
             "Yxt6G+BHWxutrtZ0ecuOFEZT1NexrLqzOCB1I4p09xlIzepI6j69jqwVdGLJQdlZq2K4O8PoFXgc2o5g9mGpB8zq8KDULkbLwcaNjK0WcRkOPrdveesDUzCCJc7Qm2/BE"
             "ItQeTe6kS1joV/VSy8xbyauViJUSA1EcyaIm0WI7JrP/f8phjPlFeSujlAYezyUArBIdvokO6lFJuhTPgMxJ0NOczOrbELszzUaa3ikHm6nkoK7zZQG+o4wlDeshZg1xux"
             "qKu0IfvnwQBnP0BttV0aiglGqUonZdy9CvJ14t8jyOo7rLnDkftdyV8ITG5WEoOnSmzcUR8O7xakem2aJhPFZEFcCFu1n3L0kM+L2lPu3QtgMacbtP7Qsk+UNubpSP/t/4"
             "1hBnIeQ9CYhvb08PN322OA1ZeAaXVVlyOyM6uTqhD7h4tzUi30/mKqurnrXhzEE2Thn0OZ9rIYcKHHxsE0/dBiOpgngw7I0qCqG7+N0z2NP4Cqj1e/KTEd/7kJQ4/wA7oF"
             "PzzNpq70+f/YlQQUyvpmq8kKt3MTwTuWiBDxOiEMHeoJoLEQAIB3obqymE76vfzuOc26fXNmVBsaNkm3wzVbpE9TtktHFCrFwAdvlk5/treUCIjIwoD2PHxJwOs6eFNuOP"
             "SZ+WGQ77oZYDHDoDwWi/fNzkPbBT2Cr0Vs=",
             "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"},
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
            {"AB12£$_\0 ", "135C3423B76DEECA446666DE48912C3B"},
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
            {"AB12£$_\0 ", "46CD8EB20A879864D5B7968679B6D60615BC64592D376FA10A90C064D9BA269F"},
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
            {"AB12£$_\0 ", "EDD865E6301DC1F3B578E12ACAB9A6363F1F0598"},
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
            {"AB12£$_\0 ", "5CD74BB63D1F75CF4E4EB3967D101EC05DBF2E9E101BE932ABD18D27"},
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
            {"AB12£$_\0 ", "D097C56CEB17C6553E216C42DA60EF85917738EAB9D5A7A62674945CF51F5945C3A573A1A7E9B40A9B427789110A2C14"},
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
            {"AB12£$_\0 ", "6838166F7AE21CF8344B2F3A83AF2EAF34309AE3A84369BF2E59C7F0FA2971A5975CBC24CF079074FB579BF75920B2E83B0E01AFEDFDB5522240979D3EAA0B6C"},
        };
        TEST_END
    }

    FileDelete(szTempFilename);
}
