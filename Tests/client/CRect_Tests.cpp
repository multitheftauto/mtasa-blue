/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CRect_Tests.cpp
 *  PURPOSE:     Google Test suite for CRect (GTA SA bounding rect)
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CRect.h>

///////////////////////////////////////////////////////////////
//
// Construction
//
///////////////////////////////////////////////////////////////

// Verify default constructor uses large sentinel values for StretchToPoint compatibility
TEST(CRect, DefaultConstructorHasSentinelValues)
{
    // CRect uses large sentinel values so StretchToPoint can shrink them
    CRect r;
    EXPECT_FLOAT_EQ(r.left, 1000000.0f);
    EXPECT_FLOAT_EQ(r.top, 1000000.0f);
    EXPECT_FLOAT_EQ(r.right, -1000000.0f);
    EXPECT_FLOAT_EQ(r.bottom, -1000000.0f);
}

// Verify four-argument constructor assigns left, top, right, bottom
TEST(CRect, ExplicitConstructor)
{
    CRect r(10.0f, 20.0f, 30.0f, 40.0f);
    EXPECT_FLOAT_EQ(r.left, 10.0f);
    EXPECT_FLOAT_EQ(r.top, 20.0f);
    EXPECT_FLOAT_EQ(r.right, 30.0f);
    EXPECT_FLOAT_EQ(r.bottom, 40.0f);
}

///////////////////////////////////////////////////////////////
//
// StretchToPoint
//
///////////////////////////////////////////////////////////////

// Verify first StretchToPoint collapses sentinel to a zero-size rect at the point
TEST(CRect, StretchToPointFromDefault)
{
    // First point sets both left/right and top/bottom
    CRect r;
    r.StretchToPoint(5.0f, 10.0f);
    EXPECT_FLOAT_EQ(r.left, 5.0f);
    EXPECT_FLOAT_EQ(r.right, 5.0f);
    EXPECT_FLOAT_EQ(r.top, 10.0f);
    EXPECT_FLOAT_EQ(r.bottom, 10.0f);
}

// Verify second StretchToPoint expands to encompass both points
TEST(CRect, StretchToPointExpandsBounds)
{
    CRect r;
    r.StretchToPoint(0.0f, 0.0f);
    r.StretchToPoint(10.0f, 20.0f);
    EXPECT_FLOAT_EQ(r.left, 0.0f);
    EXPECT_FLOAT_EQ(r.right, 10.0f);
    EXPECT_FLOAT_EQ(r.top, 0.0f);
    EXPECT_FLOAT_EQ(r.bottom, 20.0f);
}

// Verify StretchToPoint handles negative coordinates correctly
TEST(CRect, StretchToPointNegativeCoords)
{
    CRect r;
    r.StretchToPoint(-5.0f, -10.0f);
    r.StretchToPoint(5.0f, 10.0f);
    EXPECT_FLOAT_EQ(r.left, -5.0f);
    EXPECT_FLOAT_EQ(r.right, 5.0f);
    EXPECT_FLOAT_EQ(r.top, -10.0f);
    EXPECT_FLOAT_EQ(r.bottom, 10.0f);
}

///////////////////////////////////////////////////////////////
//
// Restrict
//
///////////////////////////////////////////////////////////////

// Verify Restrict() widens the rect to fully contain a larger rect
TEST(CRect, RestrictExpandsToContainOther)
{
    CRect a(5.0f, 5.0f, 15.0f, 15.0f);
    CRect b(0.0f, 0.0f, 20.0f, 20.0f);
    a.Restrict(&b);
    EXPECT_FLOAT_EQ(a.left, 0.0f);
    EXPECT_FLOAT_EQ(a.top, 0.0f);
    EXPECT_FLOAT_EQ(a.right, 20.0f);
    EXPECT_FLOAT_EQ(a.bottom, 20.0f);
}

// Verify Restrict() is a no-op when the rect already contains the other
TEST(CRect, RestrictDoesNotShrink)
{
    CRect a(0.0f, 0.0f, 20.0f, 20.0f);
    CRect b(5.0f, 5.0f, 15.0f, 15.0f);
    a.Restrict(&b);
    // a already contains b, so no change
    EXPECT_FLOAT_EQ(a.left, 0.0f);
    EXPECT_FLOAT_EQ(a.top, 0.0f);
    EXPECT_FLOAT_EQ(a.right, 20.0f);
    EXPECT_FLOAT_EQ(a.bottom, 20.0f);
}

///////////////////////////////////////////////////////////////
//
// IsCircleInside
//
///////////////////////////////////////////////////////////////

// Verify IsCircleInside returns true for a circle fully within the rect
TEST(CRect, IsCircleInsideCentered)
{
    CRect     r(0.0f, 0.0f, 10.0f, 10.0f);
    CVector2D center(5.0f, 5.0f);
    EXPECT_TRUE(r.IsCircleInside(&center, 1.0f));
}

// Verify IsCircleInside returns true when the circle overlaps the rect edge
TEST(CRect, IsCircleInsidePartiallyOverlapping)
{
    CRect     r(0.0f, 0.0f, 10.0f, 10.0f);
    CVector2D center(11.0f, 5.0f);
    // Circle center is outside right edge, but radius 2 reaches inside
    EXPECT_TRUE(r.IsCircleInside(&center, 2.0f));
}

// Verify IsCircleInside returns false for a circle entirely outside the rect
TEST(CRect, IsCircleInsideCompletelyOutside)
{
    CRect     r(0.0f, 0.0f, 10.0f, 10.0f);
    CVector2D center(20.0f, 20.0f);
    EXPECT_FALSE(r.IsCircleInside(&center, 1.0f));
}

///////////////////////////////////////////////////////////////
//
// Reset
//
///////////////////////////////////////////////////////////////

// Verify Reset() zeroes all four fields
TEST(CRect, ResetZeroesAllFields)
{
    CRect r(10.0f, 20.0f, 30.0f, 40.0f);
    r.Reset();
    EXPECT_FLOAT_EQ(r.left, 0.0f);
    EXPECT_FLOAT_EQ(r.top, 0.0f);
    EXPECT_FLOAT_EQ(r.right, 0.0f);
    EXPECT_FLOAT_EQ(r.bottom, 0.0f);
}

///////////////////////////////////////////////////////////////
//
// FixIncorrectTopLeft
//
///////////////////////////////////////////////////////////////

// Verify FixIncorrectTopLeft swaps left/right and top/bottom when inverted
TEST(CRect, FixIncorrectTopLeftCorrectsBadValues)
{
    // Simulate corrupted bounding box where left > right
    CRect r;
    r.left = 20.0f;
    r.right = 10.0f;
    r.top = 30.0f;
    r.bottom = 5.0f;
    r.FixIncorrectTopLeft();
    EXPECT_LT(r.left, r.right);
    EXPECT_LT(r.top, r.bottom);
    EXPECT_FLOAT_EQ(r.left, 9.0f);  // right - 1.0
    EXPECT_FLOAT_EQ(r.top, 4.0f);   // bottom - 1.0
}

// Verify FixIncorrectTopLeft is a no-op when the rect is already correct
TEST(CRect, FixIncorrectTopLeftNoOpWhenCorrect)
{
    CRect r(0.0f, 0.0f, 10.0f, 10.0f);
    r.FixIncorrectTopLeft();
    // Already correct, values should not change
    EXPECT_FLOAT_EQ(r.left, 0.0f);
    EXPECT_FLOAT_EQ(r.top, 0.0f);
    EXPECT_FLOAT_EQ(r.right, 10.0f);
    EXPECT_FLOAT_EQ(r.bottom, 10.0f);
}
