#pragma once
#include <iostream>
#include <source_location>

namespace kl {
inline void _perform_log(std::ostream& os) { os << "\n"; }

template <typename T, typename... Other>
inline void _perform_log(std::ostream& os, const T& val, const Other&... args) {
  os << val << " ";
  _perform_log(os, args...);
}

template <typename... Args>
inline void log(const Args&... args) {
  _perform_log(std::cout, args...);
}

template <typename... Args>
inline void err(const Args&... args) {
  _perform_log(std::cerr, args...);
}
} // namespace kl

template <typename... Args>
[[noreturn]] void FATAL(const Args&... args) {
  kl::err(args...);
  std::cout.flush();
  std::cerr.flush();
  std::abort();
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
