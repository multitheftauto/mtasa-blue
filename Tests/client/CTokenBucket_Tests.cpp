/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CTokenBucket_Tests.cpp
 *  PURPOSE:     Google Test suite for the shared packet rate limiter
 *
 *  Covers burst capacity, refill pacing, the sub-token remainder that keeps
 *  packet jitter from eating the refill rate, and the drop counter that decides
 *  when a sender has been abusive for long enough to act on.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <CTokenBucket.h>

namespace
{
    // 30 packets per second, matching the bullet sync ceiling
    constexpr unsigned int kCapacity = 30;
    constexpr unsigned int kIntervalMs = 1000 / kCapacity;  // 33ms per token
}  // namespace

TEST(CTokenBucket, StartsFull)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);
    EXPECT_EQ(kCapacity, bucket.GetTokens());
    EXPECT_EQ(0u, bucket.GetDrops());
}

// The whole burst allowance must be usable in one go, since a lag spike delivers
// several packets at once.
TEST(CTokenBucket, AllowsFullBurstWithoutElapsedTime)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        EXPECT_TRUE(bucket.Consume(0)) << "burst packet " << i << " should pass";

    EXPECT_EQ(0u, bucket.GetTokens());
}

TEST(CTokenBucket, RejectsOnceEmpty)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    EXPECT_FALSE(bucket.Consume(0));
    EXPECT_EQ(1u, bucket.GetDrops());

    EXPECT_FALSE(bucket.Consume(0));
    EXPECT_EQ(2u, bucket.GetDrops());
}

TEST(CTokenBucket, RefillsOverTime)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    ASSERT_EQ(0u, bucket.GetTokens());

    // Five intervals buy five tokens, one of which the call itself spends
    EXPECT_TRUE(bucket.Consume(kIntervalMs * 5));
    EXPECT_EQ(4u, bucket.GetTokens());
}

TEST(CTokenBucket, RefillIsCappedAtCapacity)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    EXPECT_TRUE(bucket.Consume(kIntervalMs * 10000));
    EXPECT_EQ(kCapacity - 1, bucket.GetTokens());
}

// Sub-token time has to accumulate, otherwise a sender pacing just under one interval
// would never earn a token back.
TEST(CTokenBucket, AccumulatesSubTokenTime)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    // Three attempts of a third of an interval each add up to one token
    EXPECT_FALSE(bucket.Consume(kIntervalMs / 3));
    EXPECT_FALSE(bucket.Consume(kIntervalMs / 3));
    EXPECT_TRUE(bucket.Consume(kIntervalMs / 3 + 2));
}

// A sustained sender must keep accruing drops rather than having them cleared by the
// partial refill their own pacing earns.
TEST(CTokenBucket, DropsSurvivePartialRefill)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    unsigned int drops = 0;
    for (unsigned int i = 0; i < 200; ++i)
    {
        // Ask for two packets per interval - twice the sustainable rate
        bucket.Consume(kIntervalMs / 2);
        if (!bucket.Consume(0))
            ++drops;
    }

    EXPECT_GT(drops, 0u);
    EXPECT_GT(bucket.GetDrops(), 0u) << "an over-rate sender must accumulate drops";
}

TEST(CTokenBucket, DropsResetOnceFullyRefilled)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    for (unsigned int i = 0; i < kCapacity; ++i)
        bucket.Consume(0);

    bucket.Consume(0);
    ASSERT_GT(bucket.GetDrops(), 0u);

    // Going quiet long enough to fully restore the bucket clears the record
    EXPECT_TRUE(bucket.Consume(kIntervalMs * kCapacity * 2));
    EXPECT_EQ(0u, bucket.GetDrops());
}

// Someone firing at a plausible rate must never be throttled, however long they keep going.
TEST(CTokenBucket, SustainedLegitimateRateNeverDrops)
{
    CTokenBucket bucket(kCapacity, kIntervalMs);

    // 13 shots per second, above the fastest bullet sync weapon
    constexpr unsigned long long kShotIntervalMs = 1000 / 13;

    for (unsigned int i = 0; i < 5000; ++i)
        ASSERT_TRUE(bucket.Consume(kShotIntervalMs)) << "legitimate shot " << i << " was throttled";

    EXPECT_EQ(0u, bucket.GetDrops());
}

// Degenerate parameters must not divide by zero
TEST(CTokenBucket, HandlesZeroedParameters)
{
    CTokenBucket bucket(0, 0);
    EXPECT_TRUE(bucket.Consume(0));
    EXPECT_FALSE(bucket.Consume(0));
}
