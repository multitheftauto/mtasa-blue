/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SharedUtil_Tests.cpp
 *  PURPOSE:     Google Test port of Shared/sdk/SharedUtil.Tests.hpp
 *
 *  Tests ported from the original assert()-based tests in SharedUtil.Tests.hpp.
 *  Only tests that require no heavy external dependencies (CryptoPP, bcrypt)
 *  are included here. Hash/crypto tests are deferred to a future iteration
 *  once those vendor libraries are linked into the test project.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.h>

using namespace SharedUtil;

///////////////////////////////////////////////////////////////
//
// SString Tests
//
///////////////////////////////////////////////////////////////

// Verify SString printf-style construction with mixed format specifiers
TEST(SString, vFormat)
{
    EXPECT_EQ(SString("XX%sAA%dBB", "la", 0), "XXlaAA0BB");
    EXPECT_EQ(SString("XX%sAA%dBB", "laQWE", 2000), "XXlaQWEAA2000BB");
}

// Verify Split() separates a string at the Nth delimiter occurrence
TEST(SString, Split)
{
    struct TestData
    {
        SString     source;
        const char* delim;
        int         index;
        bool        result;
        const char* resultLeft;
        const char* resultRight;
    };

    TestData tests[] = {
        {"a.b.c.d.e", ".", 1, true, "a", "b.c.d.e"},
        {"a.b.c.d.e", ".", 2, true, "a.b", "c.d.e"},
        {"a.b.c.d.e", ".", -2, true, "a.b.c", "d.e"},
    };

    for (const auto& t : tests)
    {
        SString strLeft, strRight;
        EXPECT_EQ(t.source.Split(t.delim, &strLeft, &strRight, t.index), t.result);
        EXPECT_EQ(strLeft, t.resultLeft);
        EXPECT_EQ(strRight, t.resultRight);
    }
}

// Verify Split() with a vector output produces all delimited parts
TEST(SString, SplitMultiple)
{
    struct TestData
    {
        SString     source;
        const char* delim;
        uint        maxAmount;
        uint        minAmount;
        const char* resultFirst;
        const char* resultLast;
        uint        resultCount;
    };

    TestData tests[] = {
        {"hello", ".", 0, 0, "hello", "hello", 1},
        {"a.#b.#c.#d.#e", ".#", 0, 0, "a", "e", 5},
    };

    for (const auto& t : tests)
    {
        std::vector<SString> parts;
        t.source.Split(t.delim, parts, t.maxAmount, t.minAmount);
        EXPECT_EQ(parts.front(), t.resultFirst);
        EXPECT_EQ(parts.back(), t.resultLast);
        EXPECT_EQ(parts.size(), t.resultCount);
    }
}

// Verify case-sensitive Replace() with various needle/replacement patterns
TEST(SString, Replace)
{
    struct TestData
    {
        SString     source;
        const char* needle;
        const char* replacement;
        const char* result;
    };

    TestData tests[] = {
        {"blah1234blah", "la", "QwE", "bQwEh1234bQwEh"},
        {"blah1234blah", "blah", "", "1234"},
        {"blahblah1234", "blah", "", "1234"},
        {"1234blahblah", "blah", "", "1234"},
        {"1234blAhblaH", "BLah", "", "1234blAhblaH"},
        {"blah1234blah", "LA", "QwE", "blah1234blah"},
        {"blah1234blah", "blAh", "", "blah1234blah"},
        {"blah////blah", "//", "/", "blah//blah"},
    };

    for (const auto& t : tests)
    {
        EXPECT_EQ(t.source.Replace(t.needle, t.replacement), t.result);
    }
}

// Verify Replace() bSearchJustReplaced flag controls re-scanning of replaced text
TEST(SString, ReplaceSearchJustReplaced)
{
    SString source = "blah////blah";
    // With bSearchJustReplaced = false (default)
    EXPECT_EQ(source.Replace("//", "/", false), "blah//blah");
    // With bSearchJustReplaced = true
    EXPECT_EQ(source.Replace("//", "/", true), "blah/blah");
}

// Verify case-insensitive ReplaceI() matches regardless of letter casing
TEST(SString, ReplaceI)
{
    struct TestData
    {
        SString     source;
        const char* needle;
        const char* replacement;
        const char* result;
    };

    TestData tests[] = {
        {"blah1234blah", "la", "QwE", "bQwEh1234bQwEh"},
        {"blah1234blah", "blah", "", "1234"},
        {"blahblah1234", "blAh", "", "1234"},
        {"1234blAhblaH", "BLah", "", "1234"},
        {"blah1234blah", "LA", "QwE", "bQwEh1234bQwEh"},
        {"blah1234blah", "blAh", "", "1234"},
    };

    for (const auto& t : tests)
    {
        EXPECT_EQ(t.source.ReplaceI(t.needle, t.replacement), t.result);
    }
}

// Verify SString::Join() reassembles split parts with a delimiter and offset/count
TEST(SString, Join)
{
    struct TestData
    {
        SString source;
        SString delim;
        int     first;
        int     count;
        SString result;
    };

    TestData tests[] = {
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

    for (const auto& t : tests)
    {
        std::vector<SString> parts;
        t.source.Split(",", parts);
        EXPECT_EQ(SString::Join(t.delim, parts, t.first, t.count), t.result);
    }
}

///////////////////////////////////////////////////////////////
//
// SharedUtil File Tests (PathJoin)
//
///////////////////////////////////////////////////////////////

#ifdef WIN32

// Verify PathJoin normalizes slashes and joins two path segments
TEST(SharedUtilFile, PathJoin2Parts)
{
    struct TestData
    {
        const char* a;
        const char* b;
        const char* result;
    };

    TestData tests[] = {
        {"///\\\\\\/\\/\\/\\/blah/\\/\\/\\", "////\\\\/////\\/fleeb///\\\\///\\/\\", "\\\\blah\\fleeb\\"},
        {"blah/\\/\\/\\", "////\\\\/////\\/fleeb", "blah\\fleeb"},
        {"blah", "fleeb", "blah\\fleeb"},
    };

    for (const auto& t : tests)
    {
        EXPECT_EQ(PathJoin(t.a, t.b), t.result);
    }
}

// Verify PathJoin normalizes and joins three path segments
TEST(SharedUtilFile, PathJoin3Parts)
{
    struct TestData
    {
        const char* a;
        const char* b;
        const char* c;
        const char* result;
    };

    TestData tests[] = {
        {"/blah/", "/\\fl\\eeb/", "//fleeeb/", "\\blah\\fl\\eeb\\fleeeb\\"},
        {"blah/", "/fl//eeb", "\\fleeeb", "blah\\fl\\eeb\\fleeeb"},
        {"blah", "fleeb", "fleeb", "blah\\fleeb\\fleeb"},
    };

    for (const auto& t : tests)
    {
        EXPECT_EQ(PathJoin(t.a, t.b, t.c), t.result);
    }
}

// Verify PathJoin preserves UNC, file://, and drive-letter prefixes
TEST(SharedUtilFile, PathJoinUNC)
{
    struct TestData
    {
        const char* a;
        const char* b;
        const char* result;
    };

    TestData tests[] = {
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

    for (const auto& t : tests)
    {
        EXPECT_EQ(PathJoin(t.a, t.b), t.result);
    }
}

#endif  // WIN32

///////////////////////////////////////////////////////////////
//
// ClassIdent Tests
//
///////////////////////////////////////////////////////////////

#ifdef WIN32

// Verify MTA's custom DynamicCast works correctly across a class hierarchy
TEST(SharedUtilClassIdent, DynamicCast)
{
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

    // Direct pointer tests
    EXPECT_TRUE(DynamicCast<SPlant>(pPlant));
    EXPECT_TRUE(DynamicCast<SPlant>(pTree));
    EXPECT_TRUE(DynamicCast<SPlant>(pFlower));
    EXPECT_TRUE(DynamicCast<SPlant>(pBlueBell));
    EXPECT_TRUE(DynamicCast<SPlant>(pDaffodil));

    EXPECT_FALSE(DynamicCast<STree>(pPlant));
    EXPECT_TRUE(DynamicCast<STree>(pTree));

    EXPECT_FALSE(DynamicCast<SFlower>(pPlant));
    EXPECT_TRUE(DynamicCast<SFlower>(pFlower));
    EXPECT_TRUE(DynamicCast<SFlower>(pBlueBell));
    EXPECT_TRUE(DynamicCast<SFlower>(pDaffodil));

    EXPECT_FALSE(DynamicCast<SBlueBell>(pPlant));
    EXPECT_FALSE(DynamicCast<SBlueBell>(pFlower));
    EXPECT_TRUE(DynamicCast<SBlueBell>(pBlueBell));

    EXPECT_FALSE(DynamicCast<SDaffodil>(pPlant));
    EXPECT_FALSE(DynamicCast<SDaffodil>(pFlower));
    EXPECT_TRUE(DynamicCast<SDaffodil>(pDaffodil));

    // Upcast pointer tests (cast to base, then check derived)
    SPlant*  pPlantTree = pTree;
    SPlant*  pPlantFlower = pFlower;
    SPlant*  pPlantBlueBell = pBlueBell;
    SPlant*  pPlantDaffodil = pDaffodil;
    SFlower* pFlowerBlueBell = pBlueBell;
    SFlower* pFlowerDaffodil = pDaffodil;

    EXPECT_TRUE(DynamicCast<SPlant>(pPlantTree));
    EXPECT_TRUE(DynamicCast<SPlant>(pPlantFlower));
    EXPECT_TRUE(DynamicCast<SPlant>(pPlantBlueBell));
    EXPECT_TRUE(DynamicCast<SPlant>(pPlantDaffodil));
    EXPECT_TRUE(DynamicCast<SPlant>(pFlowerBlueBell));
    EXPECT_TRUE(DynamicCast<SPlant>(pFlowerDaffodil));

    EXPECT_TRUE(DynamicCast<STree>(pPlantTree));
    EXPECT_FALSE(DynamicCast<STree>(pPlantFlower));
    EXPECT_FALSE(DynamicCast<STree>(pPlantBlueBell));
    EXPECT_FALSE(DynamicCast<STree>(pPlantDaffodil));

    EXPECT_FALSE(DynamicCast<SFlower>(pPlantTree));
    EXPECT_TRUE(DynamicCast<SFlower>(pPlantFlower));
    EXPECT_TRUE(DynamicCast<SFlower>(pPlantBlueBell));
    EXPECT_TRUE(DynamicCast<SFlower>(pPlantDaffodil));
    EXPECT_TRUE(DynamicCast<SFlower>(pFlowerBlueBell));
    EXPECT_TRUE(DynamicCast<SFlower>(pFlowerDaffodil));

    EXPECT_FALSE(DynamicCast<SBlueBell>(pPlantTree));
    EXPECT_FALSE(DynamicCast<SBlueBell>(pPlantFlower));
    EXPECT_TRUE(DynamicCast<SBlueBell>(pPlantBlueBell));
    EXPECT_FALSE(DynamicCast<SBlueBell>(pPlantDaffodil));
    EXPECT_TRUE(DynamicCast<SBlueBell>(pFlowerBlueBell));
    EXPECT_FALSE(DynamicCast<SBlueBell>(pFlowerDaffodil));

    EXPECT_FALSE(DynamicCast<SDaffodil>(pPlantTree));
    EXPECT_FALSE(DynamicCast<SDaffodil>(pPlantFlower));
    EXPECT_FALSE(DynamicCast<SDaffodil>(pPlantBlueBell));
    EXPECT_TRUE(DynamicCast<SDaffodil>(pPlantDaffodil));
    EXPECT_FALSE(DynamicCast<SDaffodil>(pFlowerBlueBell));
    EXPECT_TRUE(DynamicCast<SDaffodil>(pFlowerDaffodil));

    // ClassBits must be at least 64 bits
    EXPECT_GE(sizeof(ClassBits) * 8, 64u);

    delete pPlant;
    delete pTree;
    delete pFlower;
    delete pBlueBell;
    delete pDaffodil;
}

#endif  // WIN32

///////////////////////////////////////////////////////////////
//
// WildcardMatch Tests
//
///////////////////////////////////////////////////////////////

// Verify WildcardMatch handles *, ?, and various edge cases
TEST(SharedUtilWildcard, WildcardMatch)
{
    struct TestData
    {
        const char* pattern;
        const char* str;
        bool        result;
    };

    TestData tests[] = {
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

    for (const auto& t : tests)
    {
        EXPECT_EQ(WildcardMatch(t.pattern, t.str), t.result) << "Pattern: \"" << t.pattern << "\", String: \"" << t.str << "\"";
    }
}

///////////////////////////////////////////////////////////////
//
// Collection Tests (iterator behaviour)
//
///////////////////////////////////////////////////////////////

// Verify post-increment erase idiom works correctly with std::map iterators
TEST(SharedUtilCollection, MapEraseIterator)
{
    std::map<uint, SString> testMap;
    MapSet(testMap, 10, "10");
    MapSet(testMap, 20, "20");
    MapSet(testMap, 30, "30");
    MapSet(testMap, 40, "40");

    auto iter = testMap.begin();
    EXPECT_EQ(iter->first, 10u);
    iter++;
    EXPECT_EQ(iter->first, 20u);
    iter++;
    testMap.erase(iter++);
    EXPECT_EQ(iter->first, 40u);
    iter++;
    EXPECT_EQ(iter, testMap.end());
}

// Verify post-increment erase idiom works correctly with std::set iterators
TEST(SharedUtilCollection, SetEraseIterator)
{
    std::set<uint> testSet;
    MapInsert(testSet, 10);
    MapInsert(testSet, 20);
    MapInsert(testSet, 30);
    MapInsert(testSet, 40);

    auto iter = testSet.begin();
    EXPECT_EQ(*iter, 10u);
    iter++;
    EXPECT_EQ(*iter, 20u);
    iter++;
    testSet.erase(iter++);
    EXPECT_EQ(*iter, 40u);
    iter++;
    EXPECT_EQ(iter, testSet.end());
}

// Verify erase returns the next valid iterator for std::list
TEST(SharedUtilCollection, ListEraseIterator)
{
    std::list<uint> testList;
    testList.push_back(10);
    testList.push_back(20);
    testList.push_back(30);
    testList.push_back(40);

    auto iter = testList.begin();
    EXPECT_EQ(*iter, 10u);
    iter++;
    EXPECT_EQ(*iter, 20u);
    iter++;
    iter = testList.erase(iter);
    EXPECT_EQ(*iter, 40u);
    iter++;
    EXPECT_EQ(iter, testList.end());
}

// Verify erase returns the next valid iterator for std::vector
TEST(SharedUtilCollection, VectorEraseIterator)
{
    std::vector<uint> testList;
    testList.push_back(10);
    testList.push_back(20);
    testList.push_back(30);
    testList.push_back(40);

    auto iter = testList.begin();
    EXPECT_EQ(*iter, 10u);
    iter++;
    EXPECT_EQ(*iter, 20u);
    iter++;
    iter = testList.erase(iter);
    EXPECT_EQ(*iter, 40u);
    iter++;
    EXPECT_EQ(iter, testList.end());
}

///////////////////////////////////////////////////////////////
//
// String formatting and utility tests
//
///////////////////////////////////////////////////////////////

// Verify PRSinS/PRWinS/PRSinW/PRWinW cross-string-type format specifiers
TEST(SharedUtilString, SStringFormatting)
{
    SString strTemp1("name:" PRSinS, "dave");
    SString strTemp2("name:" PRWinS, L"dave");
    WString wstrTemp3(L"name:" PRSinW, "dave");
    WString wstrTemp4(L"name:" PRWinW, L"dave");

    EXPECT_EQ(strTemp1, "name:dave");
    EXPECT_EQ(strTemp2, "name:dave");
    EXPECT_TRUE(wstrTemp3 == L"name:dave");
    EXPECT_TRUE(wstrTemp4 == L"name:dave");
}

// Verify WString::ToAnsi() round-trips ASCII text correctly
TEST(SharedUtilString, WStringToAnsi)
{
    SString strTemp1 = "abcABC 123";
    WString wstrTemp2 = "defDEF 456";
    WString wstrTemp3 = L"ghiGHI 789";

    EXPECT_TRUE(wstrTemp2 == L"defDEF 456");

    SString strTemp2 = wstrTemp2.ToAnsi();
    SString strTemp3 = wstrTemp3.ToAnsi();

    EXPECT_EQ(strTemp2, "defDEF 456");
    EXPECT_EQ(strTemp3, "ghiGHI 789");
}

// Verify EscapeURLArgument percent-encodes reserved chars and UnescapeString reverses it
TEST(SharedUtilString, EscapeURLArgument)
{
    struct TestData
    {
        const char* input;
        const char* result;
    };

    TestData tests[] = {
        {"!*'();:@", "%21%2A%27%28%29%3B%3A%40"},
        {"&=+$,/?#", "%26%3D%2B%24%2C%2F%3F%23"},
        {"[] \"%<>\\", "%5B%5D%20%22%25%3C%3E%5C"},
        {"^`{|}", "%5E%60%7B%7C%7D"},
        {"AZaz09-_.~", "AZaz09-_.~"},
    };

    for (const auto& t : tests)
    {
        SStringX strInput(t.input);
        SString  strEscaped = SharedUtil::EscapeURLArgument(strInput);
        SString  strUnescaped = SharedUtil::UnescapeString(strEscaped, '%');
        EXPECT_EQ(strEscaped, t.result);
        EXPECT_EQ(strInput, strUnescaped);
    }
}

// Verify RemoveColorCodes strips valid #RRGGBB color codes from narrow strings
TEST(SharedUtilString, RemoveColorCodes)
{
    struct TestData
    {
        const char* input;
        const char* result;
    };

    TestData tests[] = {
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

    for (const auto& t : tests)
    {
        EXPECT_EQ(SharedUtil::RemoveColorCodes(t.input), t.result) << "Input: \"" << t.input << "\"";
    }
}

// Verify RemoveColorCodesInPlaceW strips color codes from wide strings in-place
TEST(SharedUtilString, RemoveColorCodesInPlaceW)
{
    struct TestData
    {
        const wchar_t* input;
        const wchar_t* result;
    };

    TestData tests[] = {
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

    for (const auto& t : tests)
    {
        WString wstr = t.input;
        SharedUtil::RemoveColorCodesInPlaceW(wstr);
        EXPECT_TRUE(wstr == t.result);
    }
}
