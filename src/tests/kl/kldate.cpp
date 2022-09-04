#include "kl/kl.h"
#include "kl/kltime.h"
#include <gtest/gtest.h>

TEST(kldate, sanity_checks) {
  EXPECT_EQ((25 * kl::TimeLimits::DAYS_IN_400_YEARS - 366) * kl::TimeLimits::TICKS_PER_DAY,
            kl::TimeLimits::MAX_TICKS + 1);
}

TEST(kldate, test_ticks) {
  EXPECT_TRUE(kl::DateTime::fromTicks(kl::TimeLimits::MIN_TICKS - 100).ticks() == kl::TimeLimits::MIN_TICKS);
  EXPECT_TRUE(kl::DateTime::fromTicks(kl::TimeLimits::MIN_TICKS).ticks() == kl::TimeLimits::MIN_TICKS);
  EXPECT_TRUE(kl::DateTime::fromTicks(kl::TimeLimits::MAX_TICKS).ticks() == kl::TimeLimits::MAX_TICKS);
  EXPECT_TRUE(kl::DateTime::fromTicks(kl::TimeLimits::MAX_TICKS + 1000).ticks() == kl::TimeLimits::MAX_TICKS);
}

void check_year(uint32_t year) {
  uint32_t delta_leap = ((year & 0x03) == 0 && (year % 100 != 0 || year % 400 == 0)) ? 1 : 0;
  uint32_t months[] = {31, 28 + delta_leap, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for (uint32_t month = 0; month < 12; month++) {
    for (uint32_t day = 0; day < months[month]; day++) {
      auto dt = kl::DateTime(year, month + 1, day + 1);
      auto d = dt.date();
      auto t = dt.timeOfDay();
      EXPECT_TRUE(d.year == year);
      EXPECT_TRUE(d.month == month + 1);
      EXPECT_TRUE(d.day == day + 1);
      EXPECT_TRUE(t.hour == 0);
      EXPECT_TRUE(t.min == 0);
      EXPECT_TRUE(t.sec == 0);
      EXPECT_TRUE(t.nanos == 0);

      dt = kl::DateTime(year, month + 1, day + 1, 23, 59, 59, 999'999'999);
      d = dt.date();
      t = dt.timeOfDay();
      EXPECT_TRUE(d.year == year);
      EXPECT_TRUE(d.month == month + 1);
      EXPECT_TRUE(d.day == day + 1);
      EXPECT_TRUE(t.hour == 23);
      EXPECT_TRUE(t.min == 59);
      EXPECT_TRUE(t.sec == 59);
      EXPECT_TRUE(t.nanos == 999'999'900);

      dt = kl::DateTime(year, month + 1, day + 1, 14, 37, 21, 123'456'789);
      d = dt.date();
      t = dt.timeOfDay();
      EXPECT_TRUE(d.year == year);
      EXPECT_TRUE(d.month == month + 1);
      EXPECT_TRUE(d.day == day + 1);
      EXPECT_TRUE(t.hour == 14);
      EXPECT_TRUE(t.min == 37);
      EXPECT_TRUE(t.sec == 21);
      EXPECT_TRUE(t.nanos == 123'456'700);
    }
  }
}

TEST(kldate, test_date_time) {
  auto dt = kl::DateTime(9999, 12, 31, 23, 59, 59, 999999999);
  EXPECT_TRUE(dt.ticks() == kl::TimeLimits::MAX_TICKS);
  EXPECT_TRUE(dt.days() == 3652058);
  auto d = dt.date();
  EXPECT_TRUE(d.year == 9999);
  EXPECT_TRUE(d.month == 12);
  EXPECT_TRUE(d.day == 31);

  check_year(2021);
  check_year(2020);
  check_year(2000);
  check_year(9021);
  check_year(9020);
  check_year(9000);
}

TEST(kldate, test_parsing) {
  auto dt = kl::DateTime::parse("2019-03-03T10:21Z");
  EXPECT_TRUE(dt == kl::DateTime(2019, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2019-03-03T10:21");
  EXPECT_TRUE(dt == kl::DateTime(2019, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 10, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21:14");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 10, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781321");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 10, 21, 14, 781'321'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 10, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781Z");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 10, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+02:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 8, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+03:21");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 7, 00, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781+03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 7, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-02:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 12, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-03:21");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 13, 42, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14.781-03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 13, 21, 14, 781'000'000));
  dt = kl::DateTime::parse("2020-03-03 10:21:14-03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 13, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21:14+03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 7, 21, 14));
  dt = kl::DateTime::parse("2020-03-03 10:21-03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 13, 21));
  dt = kl::DateTime::parse("2020-03-03 10:21+03:00");
  EXPECT_TRUE(dt == kl::DateTime(2020, 03, 03, 7, 21));
}
