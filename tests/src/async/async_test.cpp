#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <slog/slog.hpp>

#include "async_vector_sink.hpp"

std::shared_ptr<slog::tests::AsyncVectorSink> get_or_create_sink(std::shared_ptr<slog::Logger> logger, const std::string& name)
{
    std::shared_ptr<slog::tests::AsyncVectorSink> sink{nullptr};
    if (!logger->has_sink(name)) {
        sink = std::make_shared<slog::tests::AsyncVectorSink>(name);
        logger->add_sink(sink);
    }
    else {
        sink = std::dynamic_pointer_cast<slog::tests::AsyncVectorSink>(logger->get_sink(name));
    }
    return sink;
}

TEST(AsyncLog, BasicFlow) 
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().create_logger("async_a");
    auto sink = std::make_shared<slog::tests::AsyncVectorSink>("sink_a");

    ASSERT_NE(logger, nullptr);
    ASSERT_NE(sink, nullptr);

    logger->add_sink(sink);
    logger->info("Test Message 1");

    // Verify it arrives eventually
    ASSERT_TRUE(sink->wait_for(1));
    ASSERT_EQ(sink->get(0), "Test Message 1");

    // Clear for reuse
    sink->clear();
}

TEST(AsyncLog, Ordering) 
{
    constexpr uint32_t message_count = 100;
    constexpr uint32_t wait_time = 100; // ms

    std::shared_ptr<slog::tests::AsyncVectorSink> sink{nullptr};
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().create_logger("async_a");
    sink = get_or_create_sink(logger, "sink_a");

    ASSERT_NE(logger, nullptr);
    ASSERT_NE(sink, nullptr);

    // Log messages
    for (uint32_t i = 0; i < message_count; i++) {
        logger->info("msg {}", i);
    }

    // Wait for all to arrive
    ASSERT_TRUE(sink->wait_for(wait_time));

    // Verify order
    for (uint32_t i = 0; i < message_count; i++) {
        EXPECT_EQ(sink->get(i), std::format("msg {}", i));
    }

    // Clear for reuse
    sink->clear();
}

TEST(AsyncLog, Routing) 
{
    std::shared_ptr<slog::tests::AsyncVectorSink> sink_a{nullptr};
    std::shared_ptr<slog::tests::AsyncVectorSink> sink_b{nullptr};
    std::shared_ptr<slog::Logger> logger_a = slog::Registry::instance().create_logger("async_a");
    std::shared_ptr<slog::Logger> logger_b = slog::Registry::instance().create_logger("async_b");
    sink_a = get_or_create_sink(logger_a, "sink_a");
    sink_b = get_or_create_sink(logger_b, "sink_b");

    ASSERT_NE(logger_a, nullptr);
    ASSERT_NE(logger_b, nullptr);
    ASSERT_NE(sink_a, nullptr);
    ASSERT_NE(sink_b, nullptr);

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

TEST(AsyncLog, Concurrency) 
{
    constexpr uint32_t NUM_THREADS = 4;
    constexpr uint32_t LOGS_PER_THREAD = 100;

    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().create_logger("async_a");
    std::shared_ptr<slog::tests::AsyncVectorSink> sink = get_or_create_sink(logger, "sink_a");

    ASSERT_NE(logger, nullptr);
    ASSERT_NE(sink, nullptr);

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([logger, i]() {
            for (uint32_t j = 0; j < LOGS_PER_THREAD; j++) {
                logger->info("Thread {} Msg {}", i, j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    ASSERT_TRUE(sink->wait_for(NUM_THREADS * LOGS_PER_THREAD));
    EXPECT_EQ(sink->buffer_size(), NUM_THREADS * LOGS_PER_THREAD);

    // Clear for reuse
    sink->clear();
}
