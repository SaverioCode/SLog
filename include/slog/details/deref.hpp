#ifndef SLOG_DETAILS_DEREF_HPP
#define SLOG_DETAILS_DEREF_HPP

#include <slog/common.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog
{
class Logger;
}

namespace slog::details
{
    SLOG_ALWAYS_INLINE constexpr Logger& deref(Logger& ref) noexcept { return ref; }

    SLOG_ALWAYS_INLINE constexpr Logger& deref(Logger* ptr) noexcept { return *ptr; }

    SLOG_ALWAYS_INLINE Logger& deref(std::shared_ptr<Logger>& ptr) noexcept { return *ptr; }
}

#endif // SLOG_DETAILS_DEREF_HPP