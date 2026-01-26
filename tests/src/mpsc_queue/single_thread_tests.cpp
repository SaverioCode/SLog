#include <vector>

#include <gtest/gtest.h>
#include <slog/slog.hpp>

#include "common.hpp"

TEST(MPSCQueue_Functional, BasicPushPop)
{
    // Create a queue of size 4
    sl::MPSCQueue<int, 4> queue;

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
    sl::MPSCQueue<int, 8> queue;
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
    sl::MPSCQueue<int, SIZE> queue;
    constexpr int            CYCLES = 10;
    int                      counter = 0;
    
    for (int c = 0; c < CYCLES; c++) {
        // Fill
        for (size_t i = 0; i < SIZE; i++) {
            EXPECT_TRUE(queue.push(std::move(counter)));
            counter++;
        }
        // Empty
        for (size_t i = 0; i < SIZE; i++) {
            int val;

            EXPECT_TRUE(queue.pop(val));
            int expected = (c * SIZE) + i;
            EXPECT_EQ(val, expected);
        }
        // Verify Empty
        int dummy;
        EXPECT_FALSE(queue.pop(dummy));
    }
    ASSERT_EQ(counter, CYCLES * SIZE);
}

TEST(MPSCQueue_Functional, DiscardOnFull)
{
    // Queue size 2
    sl::MPSCQueue<int, 2, sl::DiscardOnFull> queue;

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
    sl::MPSCQueue<TestRecord, 4> queue;
    
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
