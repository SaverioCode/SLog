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
    std::source_location location;

    consteval FormatWithLocation(const char* str,
                                 std::source_location loc = std::source_location::current())
        : fmt(str), location(loc)
    {
    }
};

} // namespace slog

#endif // SLOG_CORE_FORMAT_WITH_LOCATION_HPP
