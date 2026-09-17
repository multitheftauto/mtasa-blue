/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CDuplicateLineFilter_Tests.cpp
 *  PURPOSE:     Google Test suite for CDuplicateLineFilter
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.h>
#include <CDuplicateLineFilter.h>

///////////////////////////////////////////////////////////////
//
// Basic pass-through: unique lines appear immediately
//
///////////////////////////////////////////////////////////////

// Verify a single unique line passes through immediately
TEST(CDuplicateLineFilter, UniqueLinePassesThrough)
{
    CDuplicateLineFilter<SString> filter;
    filter.AddLine("hello");
    SString output;
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_STREQ(output.c_str(), "hello");
}

// Verify multiple distinct lines all pass through in order
TEST(CDuplicateLineFilter, MultipleUniqueLinesPassThrough)
{
    CDuplicateLineFilter<SString> filter;
    filter.AddLine("line1");
    filter.AddLine("line2");
    filter.AddLine("line3");

    SString output;
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_STREQ(output.c_str(), "line1");
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_STREQ(output.c_str(), "line2");
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_STREQ(output.c_str(), "line3");
    EXPECT_FALSE(filter.PopOutputLine(output));
}

///////////////////////////////////////////////////////////////
//
// Single-line duplicate detection
//
///////////////////////////////////////////////////////////////

// Verify consecutive duplicates are absorbed until a different line breaks the sequence
TEST(CDuplicateLineFilter, SingleLineDuplicateDetected)
{
    CDuplicateLineFilter<SString> filter;
    // First occurrence goes to output
    filter.AddLine("repeated");
    SString output;
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_STREQ(output.c_str(), "repeated");

    // Second and third occurrences are absorbed as duplicates
    filter.AddLine("repeated");
    filter.AddLine("repeated");
    filter.AddLine("repeated");
    // The duplicates are held until a non-matching line or flush
    EXPECT_FALSE(filter.PopOutputLine(output));

    // A different line breaks the match
    filter.AddLine("different");
    // Now the duplicate line(s) plus the new line should be available
    EXPECT_TRUE(filter.PopOutputLine(output));
    // The output should contain the repeated line with a DUP marker
    EXPECT_NE(output.find("repeated"), SString::npos);
}

///////////////////////////////////////////////////////////////
//
// Flush releases held duplicates
//
///////////////////////////////////////////////////////////////

// Verify Flush() releases held duplicate lines without requiring a different line
TEST(CDuplicateLineFilter, FlushReleasesHeldLines)
{
    CDuplicateLineFilter<SString> filter;
    filter.AddLine("alpha");
    // Consume the first output
    SString output;
    EXPECT_TRUE(filter.PopOutputLine(output));

    // Start duplicate matching
    filter.AddLine("alpha");
    filter.AddLine("alpha");
    // Without flush, nothing should be available
    EXPECT_FALSE(filter.PopOutputLine(output));

    filter.Flush();
    // After flush, the held lines should be released
    EXPECT_TRUE(filter.PopOutputLine(output));
    EXPECT_NE(output.find("alpha"), SString::npos);
}

///////////////////////////////////////////////////////////////
//
// Multi-line pattern detection
//
///////////////////////////////////////////////////////////////

// Verify multi-line repeating patterns are detected and collapsed
TEST(CDuplicateLineFilter, MultiLinePatternDetection)
{
    CDuplicateLineFilter<SString> filter(4);
    // Produce a 2-line pattern: "A", "B"
    filter.AddLine("A");
    filter.AddLine("B");

    // Consume initial output
    SString output;
    while (filter.PopOutputLine(output))
    {
    }

    // Repeat the pattern
    filter.AddLine("A");
    filter.AddLine("B");
    filter.AddLine("A");
    filter.AddLine("B");

    // Duplicates should be held
    // Flush to release
    filter.Flush();

    // Should have output with dup count
    bool foundOutput = false;
    while (filter.PopOutputLine(output))
        foundOutput = true;
    EXPECT_TRUE(foundOutput);
}

///////////////////////////////////////////////////////////////
//
// Empty output when nothing added
//
///////////////////////////////////////////////////////////////

// Verify an empty filter produces no output even after Flush()
TEST(CDuplicateLineFilter, EmptyOutputWhenNothingAdded)
{
    CDuplicateLineFilter<SString> filter;
    SString                       output;
    // PopOutputLine with no delay should return false (we can't test time-based
    // behavior reliably, so just test the immediate case)
    filter.Flush();
    EXPECT_FALSE(filter.PopOutputLine(output));
}
