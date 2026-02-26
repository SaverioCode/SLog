#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <slog/slog.hpp>

#include "async_vector_sink.hpp"

class AsyncTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        logger_a = slog::Registry::instance().create_logger("async_a");
        logger_b = slog::Registry::instance().create_logger("async_b");
        sink_a = std::make_shared<slog::tests::AsyncVectorSink>("sink_a");
        sink_b = std::make_shared<slog::tests::AsyncVectorSink>("sink_b");

        logger_a->add_sink(sink_a);
        logger_b->add_sink(sink_b);

        sink_a->set_pattern("%v");
        sink_b->set_pattern("%v");
    }

    std::shared_ptr<slog::Logger> logger_a;
    std::shared_ptr<slog::Logger> logger_b;
    std::shared_ptr<slog::tests::AsyncVectorSink> sink_a;
    std::shared_ptr<slog::tests::AsyncVectorSink> sink_b;
};

TEST_F(AsyncTest, BasicFlow) 
{
    logger_a->info("Test Message 1");

    // Verify it arrives eventually
    ASSERT_TRUE(sink_a->wait_for(1));
    ASSERT_EQ(sink_a->get(0), "Test Message 1");

    // Clear for reuse
    sink_a->clear();
}

TEST_F(AsyncTest, Ordering) 
{
    constexpr uint32_t message_count = 100;
    constexpr uint32_t wait_time = 100; // ms

    // Log messages
    for (uint32_t i = 0; i < message_count; i++) {
        logger_a->info("msg {}", i);
    }

    // Wait for all to arrive
    ASSERT_TRUE(sink_a->wait_for(wait_time));

    // Verify order
    for (uint32_t i = 0; i < message_count; i++) {
        EXPECT_EQ(sink_a->get(i), std::format("msg {}", i));
    }

    // Clear for reuse
    sink_a->clear();
}

TEST_F(AsyncTest, Routing) 
{
    logger_b->add_sink(sink_b);
    logger_a->info("Hello A");
    logger_b->info("Hello B");

    ASSERT_TRUE(sink_a->wait_for(1));
    ASSERT_TRUE(sink_b->wait_for(1));

    EXPECT_EQ(sink_a->get(0), "Hello A");
    EXPECT_EQ(sink_b->get(0), "Hello B");

    // Ensure no cross-talk
    EXPECT_EQ(sink_a->buffer_size(), 1);
    EXPECT_EQ(sink_b->buffer_size(), 1);

    // Clear for reuse
    sink_a->clear();
    sink_b->clear();
}

TEST_F(AsyncTest, Concurrency) 
{
    constexpr uint32_t NUM_THREADS = 4;
    constexpr uint32_t LOGS_PER_THREAD = 100;

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([this, i]() {
            for (uint32_t j = 0; j < LOGS_PER_THREAD; j++) {
                logger_a->info("Thread {} Msg {}", i, j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    ASSERT_TRUE(sink_a->wait_for(NUM_THREADS * LOGS_PER_THREAD));
    EXPECT_EQ(sink_a->buffer_size(), NUM_THREADS * LOGS_PER_THREAD);

    // Clear for reuse
    sink_a->clear();
}
