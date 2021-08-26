#pragma once
#include "kl.h"
#include <array>
#include <compare>
#include <cstdint>

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
  static const int64_t MIN_TICKS = 0LL;                   // 0001-01-01 00:00:00 UTC (if that makes any sense)
  static const int64_t MAX_TICKS = 3155378975999999999LL; // 9999-12-31 23:59:59 UTC
  static const int64_t TICKS_PER_SECOND = 10'000'000LL;
  static const int64_t TICKS_PER_DAY = TICKS_PER_SECOND * 24 * 3600;
  static const uint32_t DAYS_IN_400_YEARS = 365 * 400 + 97;
};

// Interface inspired after the C# Timespan
struct TimeSpan {
  int64_t ticks;

  static constexpr TimeSpan fromHours(int64_t h) { return {.ticks = h * 3600 * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromMinutes(int64_t m) { return {.ticks = m * 60 * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromSeconds(int64_t s) { return {.ticks = s * TimeLimits::TICKS_PER_SECOND}; }
  static constexpr TimeSpan fromDays(int64_t d) { return {.ticks = d * TimeLimits::TICKS_PER_DAY}; }
  static constexpr TimeSpan fromNanos(int64_t d) { return {.ticks = d / 100}; }

  constexpr int64_t totalHours() const { return ticks / (TimeLimits::TICKS_PER_SECOND * 3600); }
  constexpr int64_t totalMinutes() const { return ticks / (TimeLimits::TICKS_PER_SECOND * 60); }
  constexpr int64_t totalSeconds() const { return ticks / TimeLimits::TICKS_PER_SECOND; }
  constexpr int64_t totalMilliseconds() const { return ticks / (TimeLimits::TICKS_PER_SECOND / 1000); }
  constexpr int64_t hours() const { return (ticks / (TimeLimits::TICKS_PER_SECOND * 3600)) % 24; }
  constexpr int64_t minutes() const { return (ticks / (TimeLimits::TICKS_PER_SECOND * 60)) % 60; }
  constexpr int64_t seconds() const { return (ticks / TimeLimits::TICKS_PER_SECOND) % 60; }
  constexpr int64_t days() const { return ticks / TimeLimits::TICKS_PER_DAY; }
  constexpr int64_t milliseconds() const { return (ticks / (TimeLimits::TICKS_PER_SECOND / 1000)) % 1000; }

  auto operator<=>(const TimeSpan&) const = default;
  constexpr TimeSpan operator-(const TimeSpan& ts) const { return {.ticks = ticks - ts.ticks}; }
  constexpr TimeSpan operator+(const TimeSpan& ts) const { return {.ticks = ticks + ts.ticks}; }
};

// Interface inspired after the C# Datetime
class DateTime {
private:
  // One Tick represents 100 nanoseconds.
  int64_t _ticks = 0;

public:
  int64_t ticks() const;
  int32_t days() const;
  TimeOfDay timeOfDay() const;
  Date date() const;
  static DateTime fromTicks(int64_t ticks);

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
  std::strong_ordering operator<=>(const DateTime& other) const;

  static const DateTime UnixEpoch;
  static const DateTime MAX;
  static const DateTime MIN;
  static DateTime now();
};

} // namespace kl

#include <ostream>

std::ostream& operator<<(std::ostream& os, kl::DateTime t);
std::ostream& operator<<(std::ostream& os, kl::TimeSpan t);
