#include "kltime.h"
#include <algorithm>
#include <chrono>
//#include <format>

using namespace kl;

static constexpr bool _leap_year(uint32_t year) { // Leap year, for one-based 1-400 interval
  return ((year & 0x03) == 0) && year != 100 && year != 200 && year != 300;
}

static constexpr std::array<uint32_t, 12> MSIZES = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static constexpr std::array<uint32_t, 12> MSIZES_LEAP = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static constexpr const std::array<uint32_t, 12>& _month_sizes(int32_t year) { // year in the interval 1-400.
  return _leap_year(year) ? MSIZES_LEAP : MSIZES;
}

static constexpr std::array<int32_t, 400 * 12 + 1> _calculateDeltaMonths() {
  std::array<int32_t, 400 * 12 + 1> res;
  int32_t totalDays = 0;
  res[0] = 0;
  int32_t index = 1;
  for (int32_t y = 0; y < 400; y++) {
    auto& sizes = _month_sizes(y + 1);
    for (int32_t m = 0; m < 12; m++, index++) {
      totalDays += sizes[m];
      res[index] = totalDays;
    }
  }
  return res;
}

static const std::array<int32_t, 400 * 12 + 1> DeltaMonths = _calculateDeltaMonths();

const DateTime DateTime::UnixEpoch(1970, 1, 1);
const DateTime DateTime::MAX = DateTime::fromTicks(TimeLimits::MAX_TICKS);
const DateTime DateTime::MIN = DateTime::fromTicks(TimeLimits::MIN_TICKS);

Date DateTime::date() const {
  auto d = _ticks / TimeLimits::TICKS_PER_DAY;
  auto fh = std::lldiv(d, TimeLimits::DAYS_IN_400_YEARS);
  auto it = std::lower_bound(DeltaMonths.begin(), DeltaMonths.end(), fh.rem + 1) - 1;
  auto mo = std::div((uint32_t)std::distance(DeltaMonths.begin(), it), 12);
  // kl::log("days:", d, "fh.quot", fh.quot, "fh.rem:", fh.rem, "iterator at",
  //         (int32_t)std::distance(s_DateHelper.DeltaMonths.begin(), it), "value", *it);
  return Date{.year = (uint32_t)fh.quot * 400 + mo.quot + 1,
              .month = (uint32_t)mo.rem + 1,
              .day = 1 + (uint32_t)fh.rem - *it};
}

DateTime DateTime::now() {
  // auto n = std::chrono::utc_clock::now(); TODO FIX THIS WHEN THE COMPILER IMPLEMENTS utc_clock
  auto n = std::chrono::system_clock::now();
  int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(n.time_since_epoch()).count();
  return DateTime(nanos / 1'000'000'000, nanos % 1'000'000'000);
}

int64_t DateTime::ticks() const { return _ticks; }
int32_t DateTime::days() const { return _ticks / TimeLimits::TICKS_PER_DAY; }
TimeOfDay DateTime::timeOfDay() const {
  int64_t dayTicks = _ticks % TimeLimits::TICKS_PER_DAY;
  auto sec = lldiv(dayTicks, TimeLimits::TICKS_PER_SECOND);
  auto mn = div(sec.quot, 60);
  auto hr = div(mn.quot, 60);
  return {.hour = (uint32_t)hr.quot,
          .min = (uint32_t)hr.rem,
          .sec = (uint32_t)mn.rem,
          .nanos = (uint32_t)sec.rem * 100};
}

DateTime DateTime::fromTicks(int64_t ticks) {
  DateTime dt;
  dt._ticks = ticks < TimeLimits::MIN_TICKS   ? TimeLimits::MIN_TICKS
              : ticks > TimeLimits::MAX_TICKS ? TimeLimits::MAX_TICKS
                                              : ticks;
  return dt;
}

DateTime::DateTime(time_t seconds, int32_t nsec) {
  _ticks = DateTime::UnixEpoch.ticks() + seconds * TimeLimits::TICKS_PER_SECOND + nsec / 100;
}

DateTime::DateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t sec,
                   uint32_t nsec) {
  if (year < 1 || year > 9999 || month < 1 || month > 12 || day < 1 || nsec > 999'999'999ULL || sec > 59 ||
      minute > 59 || hour > 23) [[unlikely]] {
    _ticks = 0;
    return;
  }
  year--;
  month--;
  day--;
  // This isn't constexpr in C++20 :( auto years = std::div(year - 1, 400);
  // ldiv_t years{.quot = year / 400, .rem = year % 400};
  auto years = std::div(year - 1, 400);
  auto& monthsizes = _month_sizes(years.rem + 1);
  if (day >= monthsizes[month]) [[unlikely]] {
    _ticks = 0;
    return;
  }
  auto seconds = sec + minute * 60 + hour * 3600;
  auto days = DeltaMonths[years.rem * 12 + month] + day + years.quot * TimeLimits::DAYS_IN_400_YEARS;
  _ticks = nsec / 100;
  _ticks += seconds * TimeLimits::TICKS_PER_SECOND;
  _ticks += days * TimeLimits::TICKS_PER_DAY;
}

const DateTime& DateTime::operator=(const DateTime d) {
  _ticks = d._ticks;
  return *this;
}
TimeSpan DateTime::operator-(const DateTime d) { return {.ticks = _ticks - d._ticks}; }
DateTime DateTime::operator-(TimeSpan ts) { return fromTicks(_ticks - ts.ticks); }
DateTime DateTime::operator+(TimeSpan ts) { return fromTicks(_ticks + ts.ticks); }

std::strong_ordering DateTime::operator<=>(const DateTime& other) const { return _ticks <=> other._ticks; };

std::ostream& operator<<(std::ostream& os, kl::DateTime t) {
  auto date = t.date();
  auto time = t.timeOfDay();
  // TODO when gcc implements std::format
  // os << std::format("{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}", date.year, date.month, date.day, time.hour,
  //                   time.min, time.sec, time.nanos / 1'000'000);
  char buffer[128];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d", date.year, date.month, date.day, time.hour, time.min, time.sec,
          time.nanos / 1'000'000);
  return os << buffer;
}

std::ostream& operator<<(std::ostream& os, TimeSpan t) {
  if (t.ticks < 0) {
    os << "-";
    t.ticks = -t.ticks;
  }
  uint32_t days = t.days();
  uint32_t hours = t.hours();
  uint32_t minutes = t.minutes();
  uint32_t seconds = t.seconds();
  uint32_t millis = t.milliseconds();
  // TODO when gcc implements std::format
  // os << std::format("{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}", date.year, date.month, date.day, time.hour,
  //                   time.min, time.sec, time.nanos / 1'000'000);
  if (days != 0) {
    os << days << "d ";
  }
  if (hours != 0 || days != 0) {
    os << hours << ":";
  }

  char buffer[128];
  sprintf(buffer, "%02d:%02d.%03d", minutes, seconds, millis);
  return os << buffer;
}
