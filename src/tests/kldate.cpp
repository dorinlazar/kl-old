#include "kl/kl.h"
#include "kl/kltime.h"

void sanity_checks() {
  CHECKST((25 * kl::TimeLimits::DAYS_IN_400_YEARS - 366) * kl::TimeLimits::TICKS_PER_DAY ==
          kl::TimeLimits::MAX_TICKS + 1);
  kl::log("DateTime sanity checks [OK]");
}

void test_ticks() {
  CHECKST(kl::DateTime::fromTicks(kl::TimeLimits::MIN_TICKS - 100).ticks() == kl::TimeLimits::MIN_TICKS);
  CHECKST(kl::DateTime::fromTicks(kl::TimeLimits::MIN_TICKS).ticks() == kl::TimeLimits::MIN_TICKS);
  CHECKST(kl::DateTime::fromTicks(kl::TimeLimits::MAX_TICKS).ticks() == kl::TimeLimits::MAX_TICKS);
  CHECKST(kl::DateTime::fromTicks(kl::TimeLimits::MAX_TICKS + 1000).ticks() == kl::TimeLimits::MAX_TICKS);
  kl::log("Ticks tests [OK]");
}

void check_year(uint32_t year) {
  uint32_t delta_leap = ((year & 0x03) == 0 && (year % 100 != 0 || year % 400 == 0)) ? 1 : 0;
  uint32_t months[] = {31, 28 + delta_leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for (uint32_t month = 0; month < 12; month++) {
    for (uint32_t day = 0; day < months[month]; day++) {
      auto dt = kl::DateTime(year, month + 1, day + 1);
      auto d = dt.date();
      auto t = dt.timeOfDay();
      CHECKST(d.year == year);
      CHECKST(d.month == month + 1);
      CHECKST(d.day == day + 1);
      CHECKST(t.hour == 0);
      CHECKST(t.min == 0);
      CHECKST(t.sec == 0);
      CHECKST(t.nanos == 0);

      dt = kl::DateTime(year, month + 1, day + 1, 23, 59, 59, 999'999'999);
      d = dt.date();
      t = dt.timeOfDay();
      CHECKST(d.year == year);
      CHECKST(d.month == month + 1);
      CHECKST(d.day == day + 1);
      CHECKST(t.hour == 23);
      CHECKST(t.min == 59);
      CHECKST(t.sec == 59);
      CHECKST(t.nanos == 999'999'900);

      dt = kl::DateTime(year, month + 1, day + 1, 14, 37, 21, 123'456'789);
      d = dt.date();
      t = dt.timeOfDay();
      CHECKST(d.year == year);
      CHECKST(d.month == month + 1);
      CHECKST(d.day == day + 1);
      CHECKST(t.hour == 14);
      CHECKST(t.min == 37);
      CHECKST(t.sec == 21);
      CHECKST(t.nanos == 123'456'700);
    }
  }
  kl::log("Testing year", year, "leap:", delta_leap, "[OK]");
}

void test_date_time() {
  auto dt = kl::DateTime(9999, 12, 31, 23, 59, 59, 999999999);
  CHECKST(dt.ticks() == kl::TimeLimits::MAX_TICKS);
  CHECKST(dt.days() == 3652058);
  auto d = dt.date();
  CHECKST(d.year == 9999);
  CHECKST(d.month == 12);
  CHECKST(d.day == 31);

  check_year(2021);
  check_year(2020);
  check_year(2000);
  check_year(9021);
  check_year(9020);
  check_year(9000);
  kl::log("Basic DateTime tests [OK]");
}

int main() {
  auto start = kl::DateTime::now();
  sanity_checks();
  test_date_time();
  test_ticks();
  auto end = kl::DateTime::now();
  kl::log("Time spent testing:", end - start, "or, in reverse", start - end);
  return 0;
}
