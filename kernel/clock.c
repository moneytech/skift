/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/logger.h>

#include "kernel/clock.h"
#include "kernel/x86/x86.h"

/* --- CMOS/RTC ------------------------------------------------------------- */

typedef enum
{
    T_SECOND = 0x0,
    T_MINUTE = 0x2,
    T_HOUR = 0x4,
    T_DAY = 0x7,
    T_MONTH = 0x8,
    T_YEAR = 0x9,
} time_selector_t;

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define from_bcd(val) (((val) / 16) * 10 + ((val)&0xf))

#define CMOS_WAIT while (is_cmos_update())

bool is_cmos_update()
{
    out8(CMOS_ADDRESS, 0x0A);
    return (in8(CMOS_DATA) & 0x80);
}

char get_realtime_reg(int reg)
{
    out8(CMOS_ADDRESS, reg);
    return in8(CMOS_DATA);
}

/* --- Clock ---------------------------------------------------------------- */

TimeStamp clock_now(void)
{
    CMOS_WAIT;

    DateTime datetime;
    datetime.second = from_bcd(get_realtime_reg(T_SECOND));
    datetime.minute = from_bcd(get_realtime_reg(T_MINUTE));
    datetime.hour = from_bcd(get_realtime_reg(T_HOUR));
    datetime.day = from_bcd(get_realtime_reg(T_DAY));
    datetime.month = from_bcd(get_realtime_reg(T_MONTH));
    // FIXME: maybe in 2100 we should update this...
    datetime.year = from_bcd(get_realtime_reg(T_YEAR)) + 2000;

    TimeStamp timestamp = datetime_to_timestamp(datetime);

    return timestamp;
}
