#pragma once
#include <cstdint>
#include <concepts>
#include <utility>

template <typename Enumeration, typename Underlying = uint32_t>
  requires(std::is_enum<Enumeration>::value && std::is_integral<Underlying>::value &&
           !std::is_signed<Underlying>::value)
class KLFlags {
  Underlying m_flags = 0;

public:
  KLFlags() = default;
  explicit KLFlags(Enumeration initial_value) { set_flag(initial_value); }

  void set_flag(Enumeration value) { m_flags |= (1 << std::to_underlying(value)); }
  void clear_flag(Enumeration value) { m_flags &= ~(1 << std::to_underlying(value)); }
  bool is_set(Enumeration value) const { return m_flags & (1 << std::to_underlying(value)); }
};