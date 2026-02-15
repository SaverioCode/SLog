#include <atomic>
#include <set>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include <slog/async/mpsc_queue.hpp>
#include <slog/async/policies.hpp>

#include "common.hpp"

TEST(MPSCQueue_Concurrent, MultiProducerSingleConsumer_TestRecord)
{
    constexpr size_t QUEUE_SIZE = 1024 * 16;
    constexpr size_t NUM_PRODUCERS = 8;
    constexpr size_t ITEMS_PER_PRODUCER = 10000;
    
    // Use BlockOnFull to ensure we don't drop items during this integrity test
    slog::async::MPSCQueue<TestRecord, QUEUE_SIZE, slog::async::BlockOnFull> queue;

    std::atomic<bool> start_flag{false};
    std::vector<std::thread> producers;

    // Launch Producers
    for (size_t t = 0; t < NUM_PRODUCERS; t++) {
        producers.emplace_back([&, t]() {
            while (!start_flag.load(std::memory_order_acquire));

            for (size_t i = 0; i < ITEMS_PER_PRODUCER; i++) {
                queue.push(TestRecord{"payload", t, i});
            }
        });
    }

    // Start!
    start_flag.store(true, std::memory_order_release);

    size_t total_consumed = 0;
    std::vector<size_t> last_seen_seq(NUM_PRODUCERS, (size_t)-1);
    
    while (total_consumed < NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
        TestRecord rec;

        if (queue.pop(rec)) {
            total_consumed++;
            
            // Validate Integrity
            ASSERT_LT(rec.thread_id, NUM_PRODUCERS) << "Received invalid thread ID";
            if (last_seen_seq[rec.thread_id] == (size_t)-1) {
                ASSERT_EQ(rec.sequence, 0) << "Thread " << rec.thread_id << " started with wrong sequence";
            }
            else {
                ASSERT_EQ(rec.sequence, last_seen_seq[rec.thread_id] + 1) 
                    << "Gap or disorder detected for thread " << rec.thread_id;
            }
            last_seen_seq[rec.thread_id] = rec.sequence;
        }
        else {
            std::this_thread::yield();
        }
    }
    for (auto& p : producers) {
        if (p.joinable()) p.join();
    }
    EXPECT_EQ(total_consumed, NUM_PRODUCERS * ITEMS_PER_PRODUCER);
}

TEST(MPSCQueue_Concurrent, DiscardOnFull_Stress)
{
    // Use a small queue relative to the volume of data to FORCE drops
    constexpr size_t QUEUE_SIZE = 256; 
    constexpr size_t NUM_PRODUCERS = 8;
    constexpr size_t ITEMS_PER_PRODUCER = 100000;
    
    slog::async::MPSCQueue<TestRecord, QUEUE_SIZE, slog::async::DiscardOnFull> queue;

    std::atomic<bool> start_flag{false};
    std::vector<std::thread> producers;
    std::atomic<size_t> producer_finished_count{0};

    // Launch Producers
    for (size_t t = 0; t < NUM_PRODUCERS; t++) {
        producers.emplace_back([&, t]() {
            while (!start_flag.load(std::memory_order_acquire));

            for (size_t i = 0; i < ITEMS_PER_PRODUCER; i++) {
                queue.push(TestRecord{"drop_test", t, i});
            }
            producer_finished_count.fetch_add(1, std::memory_order_release);
        });
    }

    // Start!
    start_flag.store(true, std::memory_order_release);

    size_t total_consumed = 0;
    std::vector<size_t> last_seen_seq(NUM_PRODUCERS, (size_t)-1);
    
    // Consume until all producers are done AND queue is empty
    while (true) {
        TestRecord rec;

        if (queue.pop(rec)) {
            total_consumed++;
            ASSERT_LT(rec.thread_id, NUM_PRODUCERS);
            // KEY CHECK: Gaps are allowed, but we must never see an OLDER sequence number than what we've already seen.
            // sequence > last_seen OR (sequence == 0 && last_seen == -1)
            if (last_seen_seq[rec.thread_id] != (size_t)-1) {
                ASSERT_GT(rec.sequence, last_seen_seq[rec.thread_id]) 
                    << "Reordering detected! Thread " << rec.thread_id 
                    << " seq " << rec.sequence << " came after " << last_seen_seq[rec.thread_id];
            }
            last_seen_seq[rec.thread_id] = rec.sequence;
        }
        else {
            if (producer_finished_count.load(std::memory_order_acquire) == NUM_PRODUCERS) {
                break; 
            }
            std::this_thread::yield();
        }
    }
    for (auto& p : producers) {
        if (p.joinable()) p.join();
    }
    // We expect total_consumed to be LESS than total produced due to drops
    EXPECT_LT(total_consumed, NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    // But we should have consumed AT LEAST something (sanity check)
    EXPECT_GT(total_consumed, 0);
}
