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

struct TimeTools {
  static const int64_t MIN_TICKS = 0LL;                   // 0001-01-01 00:00:00 UTC (if that makes any sense)
  static const int64_t MAX_TICKS = 3155378975999999999LL; // 9999-12-31 23:59:59 UTC
  static const int64_t TICKS_PER_SECOND = 10'000'000LL;
  static const int64_t TICKS_PER_DAY = TICKS_PER_SECOND * 24 * 3600;
  static const uint32_t DAYS_IN_400_YEARS = 365 * 400 + 97;

  static constexpr bool leap_year(uint32_t year) { // Leap year, for one-based 1-400 interval
    return ((year & 0x03) == 0) && year != 100 && year != 200 && year != 300;
  }

  static constexpr std::array<uint32_t, 12> MSIZES = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  static constexpr std::array<uint32_t, 12> MSIZES_LEAP = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  static constexpr const std::array<uint32_t, 12>& month_sizes(int32_t year) { // year in the interval 1-400.
    return leap_year(year) ? MSIZES_LEAP : MSIZES;
  }
};

// Interface inspired after the C# Timespan
struct TimeSpan {
  int64_t ticks;
  static TimeSpan fromHours(int64_t h) { return {.ticks = h * 3600 * TimeTools::TICKS_PER_SECOND}; }
  static TimeSpan fromMinutes(int64_t m) { return {.ticks = m * 60 * TimeTools::TICKS_PER_SECOND}; }
  static TimeSpan fromSeconds(int64_t s) { return {.ticks = s * TimeTools::TICKS_PER_SECOND}; }
  static TimeSpan fromDays(int64_t d) { return {.ticks = d * TimeTools::TICKS_PER_DAY}; }
  static TimeSpan fromNanos(int64_t d) { return {.ticks = d / 100}; }

  int64_t totalHours() const { return ticks / (TimeTools::TICKS_PER_SECOND * 3600); }
  int64_t totalMinutes() const { return ticks / (TimeTools::TICKS_PER_SECOND * 60); }
  int64_t totalSeconds() const { return ticks / TimeTools::TICKS_PER_SECOND; }
  int64_t totalMilliseconds() const { return ticks / (TimeTools::TICKS_PER_SECOND / 1000); }
  int64_t hours() const { return (ticks / (TimeTools::TICKS_PER_SECOND * 3600)) % 24; }
  int64_t minutes() const { return (ticks / (TimeTools::TICKS_PER_SECOND * 60)) % 60; }
  int64_t seconds() const { return (ticks / TimeTools::TICKS_PER_SECOND) % 60; }
  int64_t days() const { return ticks / TimeTools::TICKS_PER_DAY; }
  int64_t milliseconds() const { return (ticks / (TimeTools::TICKS_PER_SECOND / 1000)) % 1000; }

  auto operator<=>(const TimeSpan&) const = default;
  TimeSpan operator-(const TimeSpan& ts) const { return {.ticks = ticks - ts.ticks}; }
  TimeSpan operator+(const TimeSpan& ts) const { return {.ticks = ticks + ts.ticks}; }
};

// Interface inspired after the C# Datetime
class DateTime {
public:
private:
  // One Tick represents 100 nanoseconds.
  int64_t _ticks;

  struct DateHelperStatic {
    std::array<int32_t, 400 * 12 + 1> DeltaMonths;
    constexpr DateHelperStatic() {
      int32_t totalDays = 0;
      DeltaMonths[0] = 0;
      int32_t index = 1;
      for (int32_t y = 0; y < 400; y++) {
        auto& sizes = TimeTools::month_sizes(y + 1);
        for (int32_t m = 0; m < 12; m++, index++) {
          totalDays += sizes[m];
          DeltaMonths[index] = totalDays;
        }
      }
    }
  };
  const DateHelperStatic s_DateHelper;

public:
  int64_t ticks() const { return _ticks; }
  int32_t days() const { return _ticks / TimeTools::TICKS_PER_DAY; }
  TimeOfDay timeOfDay() const {
    int64_t dayTicks = _ticks % TimeTools::TICKS_PER_DAY;
    auto sec = lldiv(dayTicks, TimeTools::TICKS_PER_SECOND);
    auto mn = div(sec.quot, 60);
    auto hr = div(mn.quot, 60);
    return {.hour = hr.quot, .min = hr.rem, .sec = mn.rem, .nanos = (int32_t)sec.rem * 100};
  }
  Date date() const;
  constexpr static DateTime fromTicks(int64_t ticks) {
    DateTime dt;
    dt._ticks = ticks < TimeTools::MIN_TICKS   ? TimeTools::MIN_TICKS
                : ticks > TimeTools::MAX_TICKS ? TimeTools::MAX_TICKS
                                               : ticks;
    return dt;
  }

public:
  constexpr DateTime() { _ticks = 0; }
  constexpr DateTime(time_t seconds, int32_t nsec = 0);
  constexpr DateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour = 0, uint32_t minute = 0,
                     uint32_t sec = 0, uint32_t nsec = 0) {
    if (year < 1 || year > 9999 || month < 1 || month > 12 || day < 1 || nsec > 999'999'999ULL || sec > 59 ||
        minute > 59 || hour > 23) [[unlikely]] {
      _ticks = 0;
      return;
    }
    year--;
    month--;
    day--;
    // This isn't constexpr in C++20 :( auto years = std::div(year - 1, 400);
    ldiv_t years{.quot = year / 400, .rem = year % 400};
    auto& monthsizes = TimeTools::month_sizes(years.rem + 1);
    if (day >= monthsizes[month]) [[unlikely]] {
      _ticks = 0;
      return;
    }
    auto seconds = sec + minute * 60 + hour * 3600;
    auto days = s_DateHelper.DeltaMonths[years.rem * 12 + month] + day + years.quot * TimeTools::DAYS_IN_400_YEARS;
    _ticks = nsec / 100;
    _ticks += seconds * TimeTools::TICKS_PER_SECOND;
    _ticks += days * TimeTools::TICKS_PER_DAY;
  }

  DateTime(const DateTime&) = default;

  static DateTime now();

public:
  const DateTime& operator=(const DateTime d) {
    _ticks = d._ticks;
    return *this;
  }
  TimeSpan operator-(const DateTime d) { return {.ticks = _ticks - d._ticks}; }
  DateTime operator-(TimeSpan ts) { return fromTicks(_ticks - ts.ticks); }
  DateTime operator+(TimeSpan ts) { return fromTicks(_ticks + ts.ticks); }

  auto operator<=>(const DateTime& other) const { return _ticks <=> other._ticks; };
};

static constexpr DateTime s_UnixEpoch(1970, 1, 1);

constexpr DateTime::DateTime(time_t seconds, int32_t nsec) {
  _ticks = s_UnixEpoch.ticks() + seconds * TimeTools::TICKS_PER_SECOND + nsec / 100;
}

} // namespace kl

#include <ostream>

std::ostream& operator<<(std::ostream& os, kl::DateTime t);
std::ostream& operator<<(std::ostream& os, kl::TimeSpan t);
