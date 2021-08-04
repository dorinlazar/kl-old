#include "kltime.h"
#include <algorithm>
#include <chrono>
//#include <format>

using namespace kl;

Date DateTime::date() const {
  auto d = _ticks / TimeTools::TICKS_PER_DAY;
  auto fh = std::lldiv(d, TimeTools::DAYS_IN_400_YEARS);
  auto it = std::lower_bound(s_DateHelper.DeltaMonths.begin(), s_DateHelper.DeltaMonths.end(), fh.rem + 1) - 1;
  auto mo = std::div((uint32_t)std::distance(s_DateHelper.DeltaMonths.begin(), it), 12);
  // kl::log("days:", d, "fh.quot", fh.quot, "fh.rem:", fh.rem, "iterator at",
  //         (int32_t)std::distance(s_DateHelper.DeltaMonths.begin(), it), "value", *it);
  return Date{
      .year = (uint32_t)fh.quot * 400 + mo.quot + 1, .month = (uint32_t)mo.rem + 1, .day = 1 + (uint32_t)fh.rem - *it};
}

DateTime DateTime::now() {
  // auto n = std::chrono::utc_clock::now(); TODO FIX THIS WHEN THE COMPILER IMPLEMENTS utc_clock
  auto n = std::chrono::system_clock::now();
  int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(n.time_since_epoch()).count();
  return DateTime(nanos / 1'000'000'000, nanos % 1'000'000'000);
}

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
