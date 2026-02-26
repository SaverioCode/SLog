#include <vector>
#include <limits>

#include <gtest/gtest.h>
#include <slog/async/mpsc_queue.hpp>
#include <slog/async/policies.hpp>

#include "common.hpp"

TEST(MPSCQueue_Functional, BasicPushPop)
{
    // Create a queue of size 4
    slog::async::MPSCQueue<int, 4, slog::async::BlockOnFull> queue;

    EXPECT_TRUE(queue.push(10));
    EXPECT_TRUE(queue.push(20));

    int val;
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 10);

    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 20);

    // Queue should be empty now
    EXPECT_FALSE(queue.pop(val));
}

TEST(MPSCQueue_Functional, FIFOOrder)
{
    slog::async::MPSCQueue<int, 8, slog::async::BlockOnFull> queue;
    std::vector<int> inputs = {1, 2, 3, 4, 5};

    for (int i : inputs) {
        EXPECT_TRUE(queue.push(std::move(i)));
    }

    int val;
    for (int expected : inputs) {
        ASSERT_TRUE(queue.pop(val));
        EXPECT_EQ(val, expected);
    }
}

TEST(MPSCQueue_Functional, WrapAround)
{
    // Use a small queue size to force wrap-around quickly
    constexpr size_t         SIZE = 4;
    slog::async::MPSCQueue<size_t, SIZE, slog::async::BlockOnFull> queue;
    constexpr size_t           CYCLES = 10;
    size_t                     counter = 0;
    
    for (size_t c = 0; c < CYCLES; c++) {
        // Fill
        for (size_t i = 0; i < SIZE; i++) {
            EXPECT_TRUE(queue.push(std::move(counter)));
            counter++;
        }
        // Empty
        for (size_t i = 0; i < SIZE; i++) {
            size_t val = std::numeric_limits<size_t>::max();

            EXPECT_TRUE(queue.pop(val));
            size_t expected = (c * SIZE) + i;
            EXPECT_EQ(val, expected);
        }
        // Verify Empty
        size_t dummy;
        EXPECT_FALSE(queue.pop(dummy));
    }
    ASSERT_EQ(counter, CYCLES * SIZE);
}

TEST(MPSCQueue_Functional, DiscardOnFull)
{
    // Queue size 2
    slog::async::MPSCQueue<int, 2, slog::async::DiscardOnFull> queue;

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));

    // this should fail because queue is full
    EXPECT_FALSE(queue.push(3));

    int val;
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 1);
    
    // Now we should be able to push again
    EXPECT_TRUE(queue.push(4));
    
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 2);
    
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 4);
}

TEST(MPSCQueue_Functional, ComplexTypes)
{
    slog::async::MPSCQueue<TestRecord, 4, slog::async::BlockOnFull> queue;
    
    TestRecord d1{"test", 1, 100};
    TestRecord d2{"foo", 1, 101};

    queue.push(TestRecord{"test", 1, 100});
    queue.push(TestRecord{"foo", 1, 101});

    TestRecord out;
    EXPECT_TRUE(queue.pop(out));
    EXPECT_EQ(out, d1);

    EXPECT_TRUE(queue.pop(out));
    EXPECT_EQ(out, d2);
}
