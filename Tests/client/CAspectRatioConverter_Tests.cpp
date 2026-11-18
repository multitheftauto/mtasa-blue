/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CAspectRatioConverter_Tests.cpp
 *  PURPOSE:     CAspectRatioConverter horizontal aspect-ratio scaling tests
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include "../../Client/core/Graphics/CAspectRatioConverter.h"

namespace
{
    constexpr float RATIO_4_3 = 4.0f / 3.0f;
    constexpr float RATIO_16_9 = 16.0f / 9.0f;
    constexpr float RATIO_32_9 = 32.0f / 9.0f;
}

TEST(CAspectRatioConverter, DisabledWhenUnconfiguredOrSuspended)
{
    CAspectRatioConverter converter;
    EXPECT_FALSE(converter.IsEnabled());
    EXPECT_FLOAT_EQ(converter.ConvertPositionForAspectRatio(120.0f), 120.0f);

    float x = 120.0f;
    float width = 288.0f;
    converter.ConvertSideForAspectRatio(&x, &width);
    EXPECT_FLOAT_EQ(x, 120.0f);
    EXPECT_FLOAT_EQ(width, 288.0f);

    converter.Init(1920.0f);
    converter.Pulse(RATIO_16_9);
    EXPECT_FALSE(converter.IsEnabled());

    converter.SetSourceRatioValue(RATIO_4_3);
    EXPECT_TRUE(converter.IsEnabled());

    converter.SetSuspended(true);
    EXPECT_FALSE(converter.IsEnabled());
    EXPECT_FLOAT_EQ(converter.ConvertPositionForAspectRatio(120.0f), 120.0f);

    converter.SetSuspended(false);
    converter.SetSourceRatioValue(0.0f);
    EXPECT_FALSE(converter.IsEnabled());
}

TEST(CAspectRatioConverter, IdentityWhenSourceMatchesTarget)
{
    CAspectRatioConverter converter;
    converter.Init(1920.0f);
    converter.Pulse(RATIO_4_3);
    converter.SetSourceRatioValue(RATIO_4_3);

    EXPECT_TRUE(converter.IsEnabled());
    EXPECT_FLOAT_EQ(converter.ConvertPositionForAspectRatio(120.0f), 120.0f);
    EXPECT_FLOAT_EQ(converter.ConvertPositionForAspectRatio(1700.0f), 1700.0f);

    float x = 120.0f;
    float width = 288.0f;
    converter.ConvertSideForAspectRatio(&x, &width);
    EXPECT_FLOAT_EQ(x, 120.0f);
    EXPECT_FLOAT_EQ(width, 288.0f);
}

TEST(CAspectRatioConverter, AlignsLeftAnchoredRadarFrom4By3To16By9And32By9)
{
    CAspectRatioConverter converter;

    // Radar box in 4:3 fractions: left = 40/640 (0.0625), width = 96/640 (0.15)
    converter.Init(1920.0f);
    converter.Pulse(RATIO_16_9);
    converter.SetSourceRatioValue(RATIO_4_3);

    float x16_9 = 1920.0f * (40.0f / 640.0f);
    float w16_9 = 1920.0f * (96.0f / 640.0f);
    converter.ConvertSideForAspectRatio(&x16_9, &w16_9);
    EXPECT_NEAR(x16_9, 90.0f, 1e-3f);
    EXPECT_NEAR(w16_9, 216.0f, 1e-3f);

    // Same 4:3 proportions on 32:9 ultrawide (3840x1080): scale = (4/3)/(32/9) = 0.375
    converter.Init(3840.0f);
    converter.Pulse(RATIO_32_9);

    float x32_9 = 3840.0f * (40.0f / 640.0f);
    float w32_9 = 3840.0f * (96.0f / 640.0f);
    converter.ConvertSideForAspectRatio(&x32_9, &w32_9);
    EXPECT_NEAR(x32_9, 90.0f, 1e-3f);
    EXPECT_NEAR(w32_9, 216.0f, 1e-3f);
}

TEST(CAspectRatioConverter, AlignsRightAnchoredHudElementRelativeToRightEdge)
{
    CAspectRatioConverter converter;
    converter.Init(1920.0f);
    converter.Pulse(RATIO_16_9);
    converter.SetSourceRatioValue(RATIO_4_3);

    // Weapon icon in 4:3 virtual space: x = 529/640, width = 47/640 -> offset from right = 64/640
    float x = 1920.0f * (529.0f / 640.0f);
    float width = 1920.0f * (47.0f / 640.0f);
    converter.ConvertSideForAspectRatio(&x, &width);

    const float scale = RATIO_4_3 / RATIO_16_9;
    const float expectedRight = 1920.0f - (1920.0f * (64.0f / 640.0f)) * scale;
    const float expectedWidth = (1920.0f * (47.0f / 640.0f)) * scale;
    EXPECT_NEAR(x + width, expectedRight, 1e-3f);
    EXPECT_NEAR(width, expectedWidth, 1e-3f);
}

TEST(CAspectRatioConverter, PreservesExactCenterAndAvoidsInversionAcrossMidpoint)
{
    CAspectRatioConverter converter;
    converter.Init(1920.0f);
    converter.Pulse(RATIO_16_9);
    converter.SetSourceRatioValue(RATIO_4_3);

    EXPECT_FLOAT_EQ(converter.ConvertPositionForAspectRatio(960.0f), 960.0f);

    float centeredX = 860.0f;
    float centeredWidth = 200.0f;
    converter.ConvertSideForAspectRatio(&centeredX, &centeredWidth);
    EXPECT_NEAR(centeredWidth, 150.0f, 1e-3f);
    EXPECT_NEAR(centeredX + centeredWidth * 0.5f, 960.0f, 1e-3f);
    EXPECT_NEAR(centeredX, 885.0f, 1e-3f);

    // Slightly straddling right side of center
    float straddlingX = 880.0f;
    float straddlingWidth = 200.0f;
    converter.ConvertSideForAspectRatio(&straddlingX, &straddlingWidth);
    EXPECT_GT(straddlingWidth, 0.0f);
    EXPECT_NEAR(straddlingWidth, 150.0f, 1e-3f);
}

TEST(CAspectRatioConverter, Stretches16By9ScriptWhenTargetIsForcedTo4By3)
{
    CAspectRatioConverter converter;
    converter.Init(1920.0f);
    converter.Pulse(RATIO_4_3);
    converter.SetSourceRatioValue(RATIO_16_9);

    // Align 16:9 UI coordinates with 4:3 HUD proportions when hud_match_aspect_ratio is disabled
    float x = 90.0f;
    float width = 216.0f;
    converter.ConvertSideForAspectRatio(&x, &width);
    EXPECT_NEAR(x, 120.0f, 1e-3f);
    EXPECT_NEAR(width, 288.0f, 1e-3f);
}
