// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "level.h"

#include "actions/abstract_action.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace {
static const QString _alertLevelTag = "al";
}

namespace nx::vms::event {

TLevelExtended levelOf(const AbstractActionPtr &action) {
    switch (action->actionType()) {
    case ActionType::playSoundAction:
        return {Level::common};
    case ActionType::showOnAlarmLayoutAction:
        return {Level::critical};
    case ActionType::vxMonitoringAction: {
        auto json = QJsonDocument::fromJson(action->getParams().tags.toLocal8Bit());
        if (json.isObject()) {
            const auto& obj = json.object();
            const auto iter = obj.find(_alertLevelTag);
            if (iter != obj.end()) {
                const auto& levelDesc = iter.value().toString();
                if (levelDesc == QStringLiteral("Tier 1")) {
                    return TLevelExtended{Level::common, 500, 1000};
                }
                if (levelDesc == QStringLiteral("Tier 2")) {
                    return TLevelExtended{Level::important, 500, 10'000};
                }
                if (levelDesc == QStringLiteral("Tier 3")) {
                    return TLevelExtended{Level::critical, 400, 60'000};
                }
            }
        }
        // fall-through
    }
    default:
        return {levelOf(action->getRuntimeParams())};
    }
}

Level levelOf(const EventParameters &params) {
    EventType eventType = params.eventType;

    if (eventType >= EventType::userDefinedEvent)
        return Level::common;

    switch (eventType)
    {
        // Gray notifications.
        case EventType::cameraMotionEvent:
        case EventType::cameraInputEvent:
        case EventType::serverStartEvent:
        case EventType::softwareTriggerEvent:
        case EventType::analyticsSdkEvent:
        case EventType::analyticsSdkObjectDetected:
            return Level::common;

        // Yellow notifications.
        case EventType::ldapSyncIssueEvent:
        case EventType::networkIssueEvent:
        case EventType::cameraIpConflictEvent:
        case EventType::serverConflictEvent:
        case EventType::saasIssueEvent:
            return Level::important;

        // Red notifications.
        case EventType::cameraDisconnectEvent:
        case EventType::storageFailureEvent:
        case EventType::serverFailureEvent:
        case EventType::licenseIssueEvent:
        case EventType::fanErrorEvent:
        case EventType::poeOverBudgetEvent:
        case EventType::serverCertificateError:
            return Level::critical;

        case EventType::backupFinishedEvent:
            NX_ASSERT(false, "This event is deprecated");
            return Level::none;

        case EventType::pluginDiagnosticEvent:
        {
            using namespace nx::vms::api;
            switch (params.metadata.level)
            {
                case EventLevel::error:
                    return Level::critical;

                case EventLevel::warning:
                    return Level::important;

                default:
                    return Level::common;
            }
        }

        default:
            NX_ASSERT(false, "All enum values must be handled");
            return Level::none;
    }
}

} // namespace nx::vms::event
