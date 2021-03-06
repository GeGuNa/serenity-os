/*
 * Copyright (c) 2020-2021, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/StringBuilder.h>
#include <LibCore/DateTime.h>
#include <LibJS/Runtime/Date.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <time.h>

namespace JS {

Date* Date::create(GlobalObject& global_object, Core::DateTime datetime, i16 milliseconds, bool is_invalid)
{
    return global_object.heap().allocate<Date>(global_object, datetime, milliseconds, is_invalid, *global_object.date_prototype());
}

Date::Date(Core::DateTime datetime, i16 milliseconds, bool is_invalid, Object& prototype)
    : Object(prototype)
    , m_datetime(datetime)
    , m_milliseconds(milliseconds)
    , m_is_invalid(is_invalid)
{
}

Date::~Date()
{
}

tm Date::to_utc_tm() const
{
    time_t timestamp = m_datetime.timestamp();
    struct tm tm;
    gmtime_r(&timestamp, &tm);
    return tm;
}

int Date::utc_date() const
{
    return to_utc_tm().tm_mday;
}

int Date::utc_day() const
{
    return to_utc_tm().tm_wday;
}

int Date::utc_full_year() const
{
    return to_utc_tm().tm_year + 1900;
}

int Date::utc_hours() const
{
    return to_utc_tm().tm_hour;
}

int Date::utc_minutes() const
{
    return to_utc_tm().tm_min;
}

int Date::utc_month() const
{
    return to_utc_tm().tm_mon;
}

int Date::utc_seconds() const
{
    return to_utc_tm().tm_sec;
}

String Date::gmt_date_string() const
{
    // Mon, 18 Dec 1995 17:28:35 GMT
    // FIXME: Note that we're totally cheating with the timezone part here..
    return datetime().to_string("%a, %e %b %Y %T GMT");
}

String Date::iso_date_string() const
{
    auto tm = to_utc_tm();
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;

    StringBuilder builder;
    if (year < 0)
        builder.appendff("-{:06}", -year);
    else if (year > 9999)
        builder.appendff("+{:06}", year);
    else
        builder.appendff("{:04}", year);
    builder.append('-');
    builder.appendff("{:02}", month);
    builder.append('-');
    builder.appendff("{:02}", tm.tm_mday);
    builder.append('T');
    builder.appendff("{:02}", tm.tm_hour);
    builder.append(':');
    builder.appendff("{:02}", tm.tm_min);
    builder.append(':');
    builder.appendff("{:02}", tm.tm_sec);
    builder.append('.');
    builder.appendff("{:03}", m_milliseconds);
    builder.append('Z');

    return builder.build();
}

// https://tc39.es/ecma262/#eqn-HoursPerDay
static constexpr double HOURS_PER_DAY = 24;
// https://tc39.es/ecma262/#eqn-MinutesPerHour
static constexpr double MINUTES_PER_HOUR = 60;
// https://tc39.es/ecma262/#eqn-SecondsPerMinute
static constexpr double SECONDS_PER_MINUTE = 60;
// https://tc39.es/ecma262/#eqn-msPerSecond
static constexpr double MS_PER_SECOND = 1000;
// https://tc39.es/ecma262/#eqn-msPerMinute
static constexpr double MS_PER_MINUTE = 60000;
// https://tc39.es/ecma262/#eqn-msPerHour
static constexpr double MS_PER_HOUR = 3600000;
// https://tc39.es/ecma262/#eqn-msPerDay
static constexpr double MS_PER_DAY = 86400000;

// DayWithinYear(t), https://tc39.es/ecma262/#eqn-DayWithinYear
u16 day_within_year(double t)
{
    // Day(t) - DayFromYear(YearFromTime(t))
    return static_cast<u16>(day(t) - day_from_year(year_from_time(t)));
}

// DateFromTime(t), https://tc39.es/ecma262/#sec-date-number
u8 date_from_time(double t)
{
    switch (month_from_time(t)) {
    // DayWithinYear(t) + 1???? if MonthFromTime(t) = +0????
    case 0:
        return day_within_year(t) + 1;
    // DayWithinYear(t) - 30???? if MonthFromTime(t) = 1????
    case 1:
        return day_within_year(t) - 30;
    // DayWithinYear(t) - 58???? - InLeapYear(t) if MonthFromTime(t) = 2????
    case 2:
        return day_within_year(t) - 58 - in_leap_year(t);
    // DayWithinYear(t) - 89???? - InLeapYear(t) if MonthFromTime(t) = 3????
    case 3:
        return day_within_year(t) - 89 - in_leap_year(t);
    // DayWithinYear(t) - 119???? - InLeapYear(t) if MonthFromTime(t) = 4????
    case 4:
        return day_within_year(t) - 119 - in_leap_year(t);
    // DayWithinYear(t) - 150???? - InLeapYear(t) if MonthFromTime(t) = 5????
    case 5:
        return day_within_year(t) - 150 - in_leap_year(t);
    // DayWithinYear(t) - 180???? - InLeapYear(t) if MonthFromTime(t) = 6????
    case 6:
        return day_within_year(t) - 180 - in_leap_year(t);
    // DayWithinYear(t) - 211???? - InLeapYear(t) if MonthFromTime(t) = 7????
    case 7:
        return day_within_year(t) - 211 - in_leap_year(t);
    // DayWithinYear(t) - 242???? - InLeapYear(t) if MonthFromTime(t) = 8????
    case 8:
        return day_within_year(t) - 242 - in_leap_year(t);
    // DayWithinYear(t) - 272???? - InLeapYear(t) if MonthFromTime(t) = 9????
    case 9:
        return day_within_year(t) - 272 - in_leap_year(t);
    // DayWithinYear(t) - 303???? - InLeapYear(t) if MonthFromTime(t) = 10????
    case 10:
        return day_within_year(t) - 303 - in_leap_year(t);
    // DayWithinYear(t) - 333???? - InLeapYear(t) if MonthFromTime(t) = 11????
    case 11:
        return day_within_year(t) - 333 - in_leap_year(t);
    default:
        VERIFY_NOT_REACHED();
    }
}

// DaysInYear(y), https://tc39.es/ecma262/#eqn-DaysInYear
u16 days_in_year(i32 y)
{
    // 365???? if (???(y) modulo 4) ??? 0
    if (y % 4 != 0)
        return 365;
    // 366???? if (???(y) modulo 4) = 0 and (???(y) modulo 100) ??? 0
    if (y % 4 == 0 && y % 100 != 0)
        return 366;
    // 365???? if (???(y) modulo 100) = 0 and (???(y) modulo 400) ??? 0
    if (y % 100 == 0 && y % 400 != 0)
        return 365;
    // 366???? if (???(y) modulo 400) = 0
    if (y % 400 == 0)
        return 366;
    VERIFY_NOT_REACHED();
}

// DayFromYear(y), https://tc39.es/ecma262/#eqn-DaysFromYear
double day_from_year(i32 y)
{
    // ????(365 ?? (???(y) - 1970) + floor((???(y) - 1969) / 4) - floor((???(y) - 1901) / 100) + floor((???(y) - 1601) / 400))
    return 365 * (y - 1970) + floor((y - 1969) / 4.0) - floor((y - 1901) / 100.0) + floor((y - 1601) / 400.0);
}

// TimeFromYear(y), https://tc39.es/ecma262/#eqn-TimeFromYear
double time_from_year(i32 y)
{
    // msPerDay ?? DayFromYear(y)
    return MS_PER_DAY * day_from_year(y);
}

// YearFromTime(t), https://tc39.es/ecma262/#eqn-YearFromTime
i32 year_from_time(double t)
{
    // the largest integral Number y (closest to +???) such that TimeFromYear(y) ??? t

    // Approximation using average number of milliseconds per year. We might have to adjust this guess afterwards.
    auto year = static_cast<i32>(t / (365.2425 * MS_PER_DAY) + 1970);

    auto year_t = time_from_year(year);
    if (year_t > t)
        year--;
    else if (year_t + days_in_year(year) * MS_PER_DAY <= t)
        year++;

    return year;
}

// InLeapYear(t), https://tc39.es/ecma262/#eqn-InLeapYear
bool in_leap_year(double t)
{
    // +0???? if DaysInYear(YearFromTime(t)) = 365????
    // 1???? if DaysInYear(YearFromTime(t)) = 366????
    return days_in_year(year_from_time(t)) == 366;
}

// MonthFromTime(t), https://tc39.es/ecma262/#eqn-MonthFromTime
u8 month_from_time(double t)
{
    auto in_leap_year = JS::in_leap_year(t);
    auto day_within_year = JS::day_within_year(t);

    // +0???? if +0???? ??? DayWithinYear(t) < 31????
    if (day_within_year < 31)
        return 0;
    // 1???? if 31???? ??? DayWithinYear(t) < 59???? + InLeapYear(t)
    if (31 <= day_within_year && day_within_year < 59 + in_leap_year)
        return 1;
    // 2???? if 59???? + InLeapYear(t) ??? DayWithinYear(t) < 90???? + InLeapYear(t)
    if (59 + in_leap_year <= day_within_year && day_within_year < 90 + in_leap_year)
        return 2;
    // 3???? if 90???? + InLeapYear(t) ??? DayWithinYear(t) < 120???? + InLeapYear(t)
    if (90 + in_leap_year <= day_within_year && day_within_year < 120 + in_leap_year)
        return 3;
    // 4???? if 120???? + InLeapYear(t) ??? DayWithinYear(t) < 151???? + InLeapYear(t)
    if (120 + in_leap_year <= day_within_year && day_within_year < 151 + in_leap_year)
        return 4;
    // 5???? if 151???? + InLeapYear(t) ??? DayWithinYear(t) < 181???? + InLeapYear(t)
    if (151 + in_leap_year <= day_within_year && day_within_year < 181 + in_leap_year)
        return 5;
    // 6???? if 181???? + InLeapYear(t) ??? DayWithinYear(t) < 212???? + InLeapYear(t)
    if (181 + in_leap_year <= day_within_year && day_within_year < 212 + in_leap_year)
        return 6;
    // 7???? if 212???? + InLeapYear(t) ??? DayWithinYear(t) < 243???? + InLeapYear(t)
    if (212 + in_leap_year <= day_within_year && day_within_year < 243 + in_leap_year)
        return 7;
    // 8???? if 243???? + InLeapYear(t) ??? DayWithinYear(t) < 273???? + InLeapYear(t)
    if (243 + in_leap_year <= day_within_year && day_within_year < 273 + in_leap_year)
        return 8;
    // 9???? if 273???? + InLeapYear(t) ??? DayWithinYear(t) < 304???? + InLeapYear(t)
    if (273 + in_leap_year <= day_within_year && day_within_year < 304 + in_leap_year)
        return 9;
    // 10???? if 304???? + InLeapYear(t) ??? DayWithinYear(t) < 334???? + InLeapYear(t)
    if (304 + in_leap_year <= day_within_year && day_within_year < 334 + in_leap_year)
        return 10;
    // 11???? if 334???? + InLeapYear(t) ??? DayWithinYear(t) < 365???? + InLeapYear(t)
    if (334 + in_leap_year <= day_within_year && day_within_year < 365 + in_leap_year)
        return 11;
    VERIFY_NOT_REACHED();
}

// HourFromTime(t), https://tc39.es/ecma262/#eqn-HourFromTime
u8 hour_from_time(double t)
{
    // ????(floor(???(t / msPerHour)) modulo HoursPerDay)
    return static_cast<u8>(fmod(floor(t / MS_PER_HOUR), HOURS_PER_DAY));
}

// MinFromTime(t), https://tc39.es/ecma262/#eqn-MinFromTime
u8 min_from_time(double t)
{
    // ????(floor(???(t / msPerMinute)) modulo MinutesPerHour)
    return static_cast<u8>(fmod(floor(t / MS_PER_MINUTE), MINUTES_PER_HOUR));
}

// SecFromTime(t), https://tc39.es/ecma262/#eqn-SecFromTime
u8 sec_from_time(double t)
{
    // ????(floor(???(t / msPerSecond)) modulo SecondsPerMinute)
    return static_cast<u8>(fmod(t / MS_PER_SECOND, SECONDS_PER_MINUTE));
}

// msFromTime(t), https://tc39.es/ecma262/#eqn-msFromTime
u16 ms_from_time(double t)
{
    // ????(???(t) modulo msPerSecond)
    return static_cast<u16>(fmod(t, MS_PER_SECOND));
}

// 21.4.1.11 MakeTime ( hour, min, sec, ms ), https://tc39.es/ecma262/#sec-maketime
Value make_time(GlobalObject& global_object, Value hour, Value min, Value sec, Value ms)
{
    // 1. If hour is not finite or min is not finite or sec is not finite or ms is not finite, return NaN.
    if (!hour.is_finite_number() || !min.is_finite_number() || !sec.is_finite_number() || !ms.is_finite_number())
        return js_nan();

    // 2. Let h be ????(! ToIntegerOrInfinity(hour)).
    auto h = MUST(hour.to_integer_or_infinity(global_object));
    // 3. Let m be ????(! ToIntegerOrInfinity(min)).
    auto m = MUST(min.to_integer_or_infinity(global_object));
    // 4. Let s be ????(! ToIntegerOrInfinity(sec)).
    auto s = MUST(sec.to_integer_or_infinity(global_object));
    // 5. Let milli be ????(! ToIntegerOrInfinity(ms)).
    auto milli = MUST(ms.to_integer_or_infinity(global_object));
    // 6. Let t be ((h * msPerHour + m * msPerMinute) + s * msPerSecond) + milli, performing the arithmetic according to IEEE 754-2019 rules (that is, as if using the ECMAScript operators * and +).
    // NOTE: C++ arithmetic abides by IEEE 754 rules
    auto t = ((h * MS_PER_HOUR + m * MS_PER_MINUTE) + s * MS_PER_SECOND) + milli;
    // 7. Return t.
    return Value(t);
}

// Day(t), https://tc39.es/ecma262/#eqn-Day
double day(double time_value)
{
    return floor(time_value / MS_PER_DAY);
}

// 21.4.1.12 MakeDay ( year, month, date ), https://tc39.es/ecma262/#sec-makeday
Value make_day(GlobalObject& global_object, Value year, Value month, Value date)
{
    // 1. If year is not finite or month is not finite or date is not finite, return NaN.
    if (!year.is_finite_number() || !month.is_finite_number() || !date.is_finite_number())
        return js_nan();

    // 2. Let y be ????(! ToIntegerOrInfinity(year)).
    auto y = MUST(year.to_integer_or_infinity(global_object));
    // 3. Let m be ????(! ToIntegerOrInfinity(month)).
    auto m = MUST(month.to_integer_or_infinity(global_object));
    // 4. Let dt be ????(! ToIntegerOrInfinity(date)).
    auto dt = MUST(date.to_integer_or_infinity(global_object));
    // 5. Let ym be y + ????(floor(???(m) / 12)).
    auto ym = Value(y + floor(m / 12));
    // 6. If ym is not finite, return NaN.
    if (!ym.is_finite_number())
        return js_nan();
    // 7. Let mn be ????(???(m) modulo 12).
    // NOTE: This calculation has no side-effects and is unused, so we omit it

    // 8. Find a finite time value t such that YearFromTime(t) is ym and MonthFromTime(t) is mn and DateFromTime(t) is 1????; but if this is not possible (because some argument is out of range), return NaN.
    if (!AK::is_within_range<int>(y) || !AK::is_within_range<int>(m + 1))
        return js_nan();
    // FIXME: Core::DateTime assumes the argument values are in local time, which is not the case here.
    //        Let mktime() think local time is UTC by temporarily overwriting the TZ environment variable,
    //        so that the values are not adjusted. Core::DateTime should probably learn to deal with both
    //        local time and UTC time itself.
    auto* tz = getenv("TZ");
    VERIFY(setenv("TZ", "UTC", 1) == 0);
    auto t = Core::DateTime::create(static_cast<int>(y), static_cast<int>(m + 1), 1).timestamp() * 1000;
    tz ? setenv("TZ", tz, 1) : unsetenv("TZ");
    // 9. Return Day(t) + dt - 1????.
    return Value(day(static_cast<double>(t)) + dt - 1);
}

// 21.4.1.13 MakeDate ( day, time ), https://tc39.es/ecma262/#sec-makedate
Value make_date(Value day, Value time)
{
    // 1. If day is not finite or time is not finite, return NaN.
    if (!day.is_finite_number() || !time.is_finite_number())
        return js_nan();

    // 2. Let tv be day ?? msPerDay + time.
    auto tv = Value(day.as_double() * MS_PER_DAY + time.as_double());

    // 3. If tv is not finite, return NaN.
    if (!tv.is_finite_number())
        return js_nan();

    // 4. Return tv.
    return tv;
}

}
