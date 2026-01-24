#ifndef MPSC_QUEUE_TEST_COMMON_HPP
#define MPSC_QUEUE_TEST_COMMON_HPP

#include <string>
#include <iostream>

struct TestRecord
{
    std::string data;
    size_t thread_id;
    size_t sequence;

    TestRecord() = default;
    TestRecord(std::string d, size_t tid = 0, size_t seq = 0) : data(std::move(d)), thread_id(tid), sequence(seq) {}

    bool operator==(const TestRecord& other) const
    {
        return data == other.data && thread_id == other.thread_id && sequence == other.sequence;
    }
    friend std::ostream& operator<<(std::ostream& os, const TestRecord& rec)
    {
        return os << "{ data: '" << rec.data << "', tid: " << rec.thread_id << ", seq: " << rec.sequence << " }";
    }
};

#endif // MPSC_QUEUE_TEST_COMMON_HPP
