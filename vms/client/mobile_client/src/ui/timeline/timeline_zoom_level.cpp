// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "timeline_zoom_level.h"

#include <nx/utils/log/assert.h>
#include <nx/vms/time/formatter.h>

bool QnTimelineZoomLevel::testTick(qint64 tick) const
{
    if (isMonotonic())
        return tick % interval == 0;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(tick);
    bool isStartOfDay = dateTime.time() == QTime(0, 0);
    if (!isStartOfDay)
        return false;

    QDate date = dateTime.date();

    if (type == Days) {
        if (interval > 1 && date.day() > 25)
            return false;

        return date.day() == 1 || date.day() % interval == 0;
    }

    if (date.day() != 1)
        return false;

    if (type == Months)
        return (date.month() - 1) % interval == 0;

    if (dateTime.date().month() != 1)
        return false;

    return date.year() % interval == 0;
}

qint64 QnTimelineZoomLevel::nextTick(qint64 tick) const {
    switch (type) {
    case Days: {
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(tick);

        if (interval == 1)
            return dateTime.addDays(interval).toMSecsSinceEpoch();

        dateTime = dateTime.addDays(dateTime.date().day() == 1 ? interval - 1 : interval);
        QDate date = dateTime.date();
        if (date.day() > 25)
            dateTime.setDate(QDate(date.year(), date.month(), 1).addMonths(1));
        return dateTime.toMSecsSinceEpoch();
    }
    case Months:
        return QDateTime::fromMSecsSinceEpoch(tick).addMonths(interval).toMSecsSinceEpoch();
    case Years:
        return QDateTime::fromMSecsSinceEpoch(tick).addYears(interval).toMSecsSinceEpoch();
    default:
        return tick + interval;
    }
}

qint64 QnTimelineZoomLevel::averageTickLength() const
{
    static constexpr auto kDayMsecs = 1000ll * 60 * 60 * 24;

    switch (type)
    {
        case Days:
            return kDayMsecs * interval;
        case Months:
            return kDayMsecs * 30 * interval;
        case Years:
            return kDayMsecs * 30 * 12 * interval;
        default:
            return interval;
    }
}

qint64 QnTimelineZoomLevel::alignTick(qint64 tick) const {
    if (isMonotonic())
        return tick - tick % interval;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(tick);
    QDate date = dateTime.date();
    dateTime.setTime(QTime(0, 0));

    switch (type) {
    case Days:
        if (interval > 1) {
            int day = qMax(1, date.day() - date.day() % int(interval));
            if (day > 25)
                day -= interval;
            date = QDate(date.year(), date.month(), day);
            dateTime.setDate(date);
        }
        break;
    case Months: {
        int month = date.month() - 1;
        month -= month % interval;
        dateTime.setDate(QDate(date.year(), month + 1, 1));
        break;
    }
    case Years: {
        int year = date.year() - date.year() % interval;
        dateTime.setDate(QDate(year, 1, 1));
        break;
    }
    default:
        break;
    } // switch (type)

    return dateTime.toMSecsSinceEpoch();
}

int QnTimelineZoomLevel::tickCount(qint64 start, qint64 end) const {
    if (start >= end)
        return 1;

    if (isMonotonic())
        return qMax(static_cast<int>((end - start) / interval), 1);

    QDate startDate = QDateTime::fromMSecsSinceEpoch(start).date();
    QDate endDate = QDateTime::fromMSecsSinceEpoch(end).date();

    switch (type) {
    case Days:
        return startDate.daysTo(endDate) / interval;
    case Months:
        return qMax((12 - startDate.month()) + endDate.month() + 12 * (endDate.year() - startDate.year() - 1) - 1, 1) / interval;
    case Years:
        return qMax((endDate.year() - startDate.year()) / static_cast<int>(interval), 1);
    default:
        NX_ASSERT(0);
    }
    return 0;
}

bool QnTimelineZoomLevel::isMonotonic() const
{
    return type < Days;
}

QString QnTimelineZoomLevel::value(qint64 tick) const
{
    const auto dateTime = QDateTime::fromMSecsSinceEpoch(tick);
    switch (type) {
    case Milliseconds:
        return dateTime.toString(QStringLiteral("zzz"));
    case Seconds:
        return dateTime.toString(QStringLiteral("s"));
    case Minutes:
    case Hours:
        return QString("%1:%2").arg(
            nx::vms::time::toString(dateTime, nx::vms::time::Format::h),
            nx::vms::time::toString(dateTime, nx::vms::time::Format::m));
    case Days:
        return dateTime.toString(QStringLiteral("d"));
    case Months:
        return QString();
    case Years:
        return dateTime.toString(QStringLiteral("yyyy"));
    }
    return QString();
}

QString QnTimelineZoomLevel::suffix(qint64 tick) const
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(tick);
    switch (type) {
    case Days:
        return QLocale().monthName(dateTime.date().month(), QLocale::ShortFormat);
    case Months:
        return QLocale().standaloneMonthName(dateTime.date().month(), QLocale::ShortFormat);
    case Milliseconds:
        return QStringLiteral("ms");
    case Seconds:
        return QStringLiteral("s");
    case Minutes:
    case Hours:
        return nx::vms::time::toString(dateTime, nx::vms::time::Format::a);
    default:
        return QString();
    }
}

QString QnTimelineZoomLevel::longestText() const
{
    switch (type)
    {
        case Milliseconds:
            return QStringLiteral("000 ms");
        case Seconds:
            return QStringLiteral("00 s");
        case Minutes:
        case Hours:
            return nx::vms::time::is24HoursTimeFormat()
                ? QStringLiteral("00:00")
                : QStringLiteral("00:00 MM");
        case Years:
            return QStringLiteral("0000");
        case Days:
        case Months:
            // Cannot be evaluated accurately.
            // The caller should measure text strings by itself.
            return QString();
        default:
            return QString();
    }
}
