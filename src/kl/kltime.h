#pragma once
#include "kl.h"
#include "kltext.h"
#include "klexcept.h"

#include <array>
#include <compare>
#include <cstdint>
#include <sys/time.h>

namespace kl {

struct TimeOfDay {
  uint32_t hour;
  uint32_t min;
  uint32_t sec;
  uint32_t nanos;
};

struct Date {
  uint32_t year;
  uint32_t month;
  uint32_t day;
};

struct TimeLimits {
  static constexpr int64_t MIN_TICKS = 0LL;                   // 0001-01-01 00:00:00 UTC (if that makes any sense)
  static constexpr int64_t MAX_TICKS = 3155378975999999999LL; // 9999-12-31 23:59:59 UTC
  static constexpr int64_t TICKS_PER_SECOND = 10'000'000LL;
  static constexpr int64_t TICKS_PER_MILLISECOND = 10'000LL;
  static constexpr int64_t TICKS_PER_MICROSECOND = 10LL;
  static constexpr int64_t TICKS_PER_DAY = TICKS_PER_SECOND * 24 * 3600;
  static constexpr uint32_t DAYS_IN_400_YEARS = 365 * 400 + 97;
};

// Interface inspired after the C# Timespan
struct TimeSpan {
  int64_t ticks;

  static constexpr TimeSpan fromHours(int64_t h) { return {.ticks = h * 3600 * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromMinutes(int64_t m) { return {.ticks = m * 60 * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromSeconds(int64_t s) { return {.ticks = s * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromDays(int64_t d) { return {.ticks = d * TimeLimits::TICKS_PER_DAY}; }
  static constexpr TimeSpan fromNanos(int64_t d) { return {.ticks = d / 100}; }
  static constexpr TimeSpan fromTimeval(struct timeval tv) {
    return {.ticks = (int64_t)tv.tv_sec * TimeLimits::TICKS_PER_SECOND +
                     (int64_t)tv.tv_usec * TimeLimits::TICKS_PER_MICROSECOND};
  }

  constexpr int64_t totalHours() const { return ticks / (TimeLimits::TICKS_PER_SECOND * 3600); }
  constexpr int64_t totalMinutes() const { return ticks / (TimeLimits::TICKS_PER_SECOND * 60); }
  constexpr int64_t totalSeconds() const { return ticks / TimeLimits::TICKS_PER_SECOND; }
  constexpr int64_t totalMilliseconds() const { return ticks / (TimeLimits::TICKS_PER_SECOND / 1000); }
  constexpr int64_t hours() const { return (ticks / (TimeLimits::TICKS_PER_SECOND * 3600)) % 24; }
  constexpr int64_t minutes() const { return (ticks / (TimeLimits::TICKS_PER_SECOND * 60)) % 60; }
  constexpr int64_t seconds() const { return (ticks / TimeLimits::TICKS_PER_SECOND) % 60; }
  constexpr int64_t days() const { return ticks / TimeLimits::TICKS_PER_DAY; }
  constexpr int64_t milliseconds() const { return (ticks / (TimeLimits::TICKS_PER_MILLISECOND)) % 1000; }
  constexpr struct timeval timeval() const {
    return {.tv_sec = totalSeconds(),
            .tv_usec = (ticks % TimeLimits::TICKS_PER_SECOND) / TimeLimits::TICKS_PER_MICROSECOND};
  }

  auto operator<=>(const TimeSpan&) const = default;
  constexpr TimeSpan operator-(const TimeSpan& ts) const { return {.ticks = ticks - ts.ticks}; }
  constexpr TimeSpan operator+(const TimeSpan& ts) const { return {.ticks = ticks + ts.ticks}; }
};

// Interface inspired after the C# Datetime
class DateTime {
  // One Tick represents 100 nanoseconds.
  int64_t _ticks = TimeLimits::MIN_TICKS;

public:
  int64_t ticks() const;
  int32_t days() const;
  TimeOfDay timeOfDay() const;
  Date date() const;
  static DateTime fromTicks(int64_t ticks);
  static DateTime parse(const kl::Text& src);

public:
  DateTime() = default;
  DateTime(time_t seconds, int32_t nsec = 0);
  DateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour = 0, uint32_t minute = 0, uint32_t sec = 0,
           uint32_t nsec = 0);
  DateTime(const DateTime&) = default;

public:
  const DateTime& operator=(const DateTime d);
  TimeSpan operator-(const DateTime d);
  DateTime operator-(TimeSpan ts);
  DateTime operator+(TimeSpan ts);
  friend std::strong_ordering operator<=>(const kl::DateTime& x, const kl::DateTime& y) = default;
  friend bool operator==(const kl::DateTime& x, const kl::DateTime& y) = default;
  friend bool operator!=(const kl::DateTime& x, const kl::DateTime& y) = default;

  static const DateTime UnixEpoch;
  static const DateTime MAX;
  static const DateTime MIN;
  static DateTime now();
};

} // namespace kl

template <>
struct fmt::formatter<kl::TimeSpan> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && *it != '}') {
      throw fmt::format_error("invalid format");
    }
    return it;
  }
  template <typename FormatContext>
  auto format(const kl::TimeSpan& ts, FormatContext& ctx) const -> decltype(ctx.out()) {
    std::string s;
    kl::TimeSpan t = ts;
    if (t.ticks < 0) {
      s = "-";
      t.ticks = -t.ticks;
    }
    uint32_t days = t.days();
    uint32_t hours = t.hours();
    uint32_t minutes = t.minutes();
    uint32_t seconds = t.seconds();
    uint32_t millis = t.milliseconds();

    if (days > 0) {
      return fmt::format_to(ctx.out(), "{}{}d {:0>2}:{:0>2}:{:0>2}.{:0>3}", s, days, hours, minutes, seconds, millis);
    }

    if (hours > 0) {
      return fmt::format_to(ctx.out(), "{}{:0>2}:{:0>2}:{:0>2}.{:0>3}", s, hours, minutes, seconds, millis);
    }

    if (minutes > 0) {
      return fmt::format_to(ctx.out(), "{}{:0>2}:{:0>2}.{:0>3}", s, minutes, seconds, millis);
    }

    return fmt::format_to(ctx.out(), "{}{:0>2}.{:0>3}", s, seconds, millis);
  }
};

template <>
struct fmt::formatter<kl::DateTime> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && *it != '}') {
      throw fmt::format_error("invalid format");
    }
    return it;
  }
  template <typename FormatContext>
  auto format(const kl::DateTime& dt, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto d = dt.date();
    auto t = dt.timeOfDay();
    return fmt::format_to(ctx.out(), "{:0>4}.{:0>2}.{:0>2}T{:0>2}:{:0>2}:{:0>2}.{:0>3}Z", d.year, d.month, d.day,
                          t.hour, t.min, t.sec, t.nanos / 1'000'000);
  }
};
