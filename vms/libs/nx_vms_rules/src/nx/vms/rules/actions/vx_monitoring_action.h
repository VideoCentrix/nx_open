// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/vms/rules/client_action.h>
#include <nx/vms/rules/icon.h>

#include "notification_action_base.h"

namespace nx::vms::rules {

/**
 * VX Monitoring Action - VideoCentrix custom action for triggering alerts.
 * This action is based on NotificationAction but has a distinct type identifier
 * for VideoCentrix-specific handling.
 */
class NX_VMS_RULES_API VxMonitoringAction: public NotificationActionBase
{
    Q_OBJECT
    Q_CLASSINFO("type", "vxMonitoringAction")

    // Data fields.
    FIELD(std::chrono::microseconds, interval, setInterval)
    FIELD(bool, acknowledge, setAcknowledge)

    // Notification look and feel fields.
    FIELD(nx::vms::rules::Icon, icon, setIcon)
    FIELD(QString, customIcon, setCustomIcon)
    FIELD(nx::vms::rules::ClientAction, clientAction, setClientAction)
    FIELD(QString, url, setUrl)
    FIELD(QString, extendedCaption, setExtendedCaption);

    // VX-specific fields.
    FIELD(int, alertLevel, setAlertLevel)

public:
    VxMonitoringAction() = default;

    static const ItemDescriptor& manifest();
};

} // namespace nx::vms::rules
