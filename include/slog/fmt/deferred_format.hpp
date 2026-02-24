#ifndef SLOG_FMT_DEFERRED_FORMAT_HPP
#define SLOG_FMT_DEFERRED_FORMAT_HPP

#include <any>
#include <format>
#include <string>
#include <string_view>
#include <tuple>

namespace slog::fmt
{

template<typename... Args>
void format_deferred(std::string_view sv, std::any& stored, std::string& out)
{
    auto& tup = std::any_cast<std::tuple<Args...>&>(stored);
    std::apply([&](auto&... a) {
        out = std::vformat(sv, std::make_format_args(a...));
    }, tup);
}

} // namespace slog::fmt

#endif // SLOG_FMT_DEFERRED_FORMAT_HPP
