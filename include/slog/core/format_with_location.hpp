#ifndef SLOG_CORE_FORMAT_WITH_LOCATION_HPP
#define SLOG_CORE_FORMAT_WITH_LOCATION_HPP

#include <format>
#include <source_location>

namespace slog
{

template<typename... Args>
struct FormatWithLocation
{
    std::format_string<Args...> fmt;
    std::source_location loc;

    template<typename T>
    consteval FormatWithLocation(const T& s,
                                 std::source_location l = std::source_location::current())
        : fmt(s), loc(l)
    {
    }
};

} // namespace slog

#endif // SLOG_CORE_FORMAT_WITH_LOCATION_HPP
