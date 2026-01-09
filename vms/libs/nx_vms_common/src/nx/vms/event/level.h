// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/reflect/enum_instrument.h>

namespace nx::vms::event {

/**
 * Importance level of a notification.
 */
NX_REFLECTION_ENUM_CLASS(Level,
    none,
    common,
    other,
    success,
    important,
    critical,
    count
);

struct TLevelExtended {
    operator Level() const noexcept { return level; }

    Level level = Level::none;
    int splashPeriodMs = 500;
    int splashTotalLengthMs = 1000;
};

} // namespace nx::vms::event
