#pragma once
#include <iostream>
#include <functional>
#include <source_location>
#include <fmt/format.h>
#include <fmt/ranges.h>

namespace kl {
template <typename... Args>
inline void log(std::string_view frmt, Args&&... args) {
  // std::cout << fmt::format(frmt, std::forward<Args>(args)...) << "\n";
  std::cout << fmt::vformat(frmt, fmt::make_format_args(std::forward<Args>(args)...)) << "\n";
}

template <typename... Args>
inline void err(std::string_view frmt, Args&&... args) {
  std::cerr << fmt::vformat(frmt, fmt::make_format_args(std::forward<Args>(args)...)) << "\n";
}

} // namespace kl

template <typename... Args>
[[noreturn]] void FATAL(std::string_view frmt, Args&&... args) {
  kl::err(frmt, std::forward<Args>(args)...);
  std::cout.flush();
  std::cerr.flush();
  std::abort();
}

inline void CHECK(bool value) {
  if (value) [[likely]] {
    return;
  }
  FATAL("");
}

template <typename... Ts>
inline void CHECK(bool value, const Ts&... args) {
  if (value) [[likely]] {
    return;
  }
  FATAL(args...);
}

inline void CHECKST(bool value, const std::source_location& location = std::source_location::current()) {
  if (value) {
    return;
  }
  FATAL("Failure at:", location.file_name(), location.line());
}

template <typename EX>
inline void EXPECTEX(std::function<void()> op, const std::source_location& location = std::source_location::current()) {
  try {
    op();
    FATAL("Expected exception was not triggered at: {}:{}", location.file_name(), location.line());
  } catch (const EX&) {
  } catch (...) {
    FATAL("Invalid exception triggered at: {}:{}", location.file_name(), location.line());
  }
}
