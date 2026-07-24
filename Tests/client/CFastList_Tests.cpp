/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CFastList_Tests.cpp
 *  PURPOSE:     Google Test suite for CFastList (pointer-only ordered map list)
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CFastList.h>
#include <vector>

///////////////////////////////////////////////////////////////
//
// Helpers — CFastList only works with pointers
//
///////////////////////////////////////////////////////////////

struct Dummy
{
    int value;
};

///////////////////////////////////////////////////////////////
//
// Basic operations
//
///////////////////////////////////////////////////////////////

// Verify a newly constructed list is empty with size 0
TEST(CFastList, EmptyOnConstruction)
{
    CFastList<Dummy*> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

// Verify push_back appends elements and increments size
TEST(CFastList, PushBack)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);
    EXPECT_EQ(list.size(), 3u);
    EXPECT_FALSE(list.empty());
}

// Verify push_front inserts at the beginning of the list
TEST(CFastList, PushFront)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_front(&b);
    // front() should be the item pushed to front
    EXPECT_EQ(list.front(), &b);
}

// Verify front() returns the first element and its value
TEST(CFastList, Front)
{
    Dummy             a{10};
    CFastList<Dummy*> list;
    list.push_back(&a);
    EXPECT_EQ(list.front(), &a);
    EXPECT_EQ(list.front()->value, 10);
}

// Verify pop_front removes the first element and shifts front
TEST(CFastList, PopFront)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.pop_front();
    EXPECT_EQ(list.size(), 1u);
    EXPECT_EQ(list.front(), &b);
}

// Verify contains() detects present elements and rejects absent ones
TEST(CFastList, Contains)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    EXPECT_TRUE(list.contains(&a));
    EXPECT_TRUE(list.contains(&b));
    EXPECT_FALSE(list.contains(&c));
}

// Verify remove() deletes a specific element while keeping others
TEST(CFastList, Remove)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);
    list.remove(&b);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_FALSE(list.contains(&b));
    EXPECT_TRUE(list.contains(&a));
    EXPECT_TRUE(list.contains(&c));
}

// Verify clear() empties the list and invalidates containment
TEST(CFastList, Clear)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
    EXPECT_FALSE(list.contains(&a));
}

///////////////////////////////////////////////////////////////
//
// Iteration order
//
///////////////////////////////////////////////////////////////

// Verify forward iteration follows push_back insertion order
TEST(CFastList, IterationOrderPushBack)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    std::vector<Dummy*> result;
    for (auto it = list.begin(); it != list.end(); ++it)
        result.push_back(*it);

    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], &a);
    EXPECT_EQ(result[1], &b);
    EXPECT_EQ(result[2], &c);
}

// Verify mixed push_front/push_back produces expected iteration order
TEST(CFastList, IterationOrderPushFrontThenBack)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&b);
    list.push_front(&a);
    list.push_back(&c);

    std::vector<Dummy*> result;
    for (auto it = list.begin(); it != list.end(); ++it)
        result.push_back(*it);

    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], &a);
    EXPECT_EQ(result[1], &b);
    EXPECT_EQ(result[2], &c);
}

// Verify reverse iteration visits elements in back-to-front order
TEST(CFastList, ReverseIteration)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    std::vector<Dummy*> result;
    for (auto it = list.rbegin(); it != list.rend(); ++it)
        result.push_back(*it);

    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], &c);
    EXPECT_EQ(result[1], &b);
    EXPECT_EQ(result[2], &a);
}

///////////////////////////////////////////////////////////////
//
// Erase via iterator
//
///////////////////////////////////////////////////////////////

// Verify erase via iterator removes the element and returns the next iterator
TEST(CFastList, IteratorErase)
{
    Dummy             a{1}, b{2}, c{3};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    // Erase middle element
    auto it = list.begin();
    ++it;  // points to &b
    it = list.erase(it);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_FALSE(list.contains(&b));
    // Iterator should advance to next element (&c)
    EXPECT_EQ(*it, &c);
}

///////////////////////////////////////////////////////////////
//
// Suspend / Resume operations
//
///////////////////////////////////////////////////////////////

// Verify push_back during suspended operations is deferred until resume
TEST(CFastList, SuspendedPushBackIsDeferred)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.SuspendModifyOperations();
    list.push_back(&b);
    // b should not be in the list yet
    EXPECT_FALSE(list.contains(&b));
    EXPECT_EQ(list.size(), 1u);
    list.ResumeModifyOperations();
    // Now b should be present
    EXPECT_TRUE(list.contains(&b));
    EXPECT_EQ(list.size(), 2u);
}

// Verify remove during suspended operations is deferred until resume
TEST(CFastList, SuspendedRemoveIsDeferred)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    list.SuspendModifyOperations();
    list.remove(&a);
    // a should still be in the list
    EXPECT_TRUE(list.contains(&a));
    list.ResumeModifyOperations();
    EXPECT_FALSE(list.contains(&a));
    EXPECT_EQ(list.size(), 1u);
}

///////////////////////////////////////////////////////////////
//
// Revision tracking
//
///////////////////////////////////////////////////////////////

// Verify the revision counter increments on each push_back and remove
TEST(CFastList, RevisionIncrementsOnModify)
{
    Dummy             a{1};
    CFastList<Dummy*> list;
    uint              rev0 = list.GetRevision();
    list.push_back(&a);
    uint rev1 = list.GetRevision();
    EXPECT_GT(rev1, rev0);
    list.remove(&a);
    uint rev2 = list.GetRevision();
    EXPECT_GT(rev2, rev1);
}

///////////////////////////////////////////////////////////////
//
// ListContains / ListRemove free functions
//
///////////////////////////////////////////////////////////////

// Verify the ListContains free function works like the member contains()
TEST(CFastList, ListContainsFreeFunction)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    EXPECT_TRUE(ListContains(list, &a));
    EXPECT_FALSE(ListContains(list, &b));
}

// Verify the ListRemove free function works like the member remove()
TEST(CFastList, ListRemoveFreeFunction)
{
    Dummy             a{1}, b{2};
    CFastList<Dummy*> list;
    list.push_back(&a);
    list.push_back(&b);
    ListRemove(list, &a);
    EXPECT_FALSE(list.contains(&a));
    EXPECT_EQ(list.size(), 1u);
}
