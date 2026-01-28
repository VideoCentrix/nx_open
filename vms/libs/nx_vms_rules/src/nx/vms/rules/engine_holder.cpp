// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "engine_holder.h"

#include <api/common_message_processor.h>
#include <nx/utils/async_handler_executor.h>
#include <nx/utils/log/log.h>
#include <nx/vms/api/rules/rule.h>
#include <nx/vms/common/system_context.h>
#include <nx/vms/rules/actions/vx_monitoring_action.h>
#include <nx/vms/rules/utils/type.h>
#include <nx_ec/abstract_ec_connection.h>
#include <nx_ec/managers/abstract_vms_rules_manager.h>

#include "plugin.h"
#include "router.h"
#include "rule.h"

namespace nx::vms::rules {

namespace {

// VX fix: Transform rules with virtual action types from server.
// The server may send old-migrated rules with "desktopNotification" type that should
// actually be "vxMonitoringAction" based on the "1008" marker in the caption field.
nx::vms::api::rules::RuleList transformVirtualActionTypes(const nx::vms::api::rules::RuleList &rules) {
    static const QString kDesktopNotificationType = QStringLiteral("desktopNotification");
    static const QString kVxMonitoringType = utils::type<VxMonitoringAction>();
    static const QString kVxMonitoringMarker = QStringLiteral("1008");
    static const QString kCaptionFieldName = QStringLiteral("caption");
    static const QString kTextPropName = QStringLiteral("text");

    nx::vms::api::rules::RuleList result;
    result.reserve(rules.size());

    for (auto rule : rules) {
        for (auto &action : rule.actionList) {
            if (action.type != kDesktopNotificationType)
                continue;

            // Check if the caption field contains the "1008" marker
            auto captionIt = action.fields.find(kCaptionFieldName);
            if (captionIt == action.fields.end())
                continue;

            auto textIt = captionIt->second.props.find(kTextPropName);
            if (textIt == captionIt->second.props.end())
                continue;

            const QString textValue = textIt->toString();
            if (textValue == kVxMonitoringMarker) {
                NX_DEBUG(NX_SCOPE_TAG, "Transforming rule %1 action from %2 to %3", rule.id, kDesktopNotificationType, kVxMonitoringType);

                action.type = kVxMonitoringType;
                // Clear the caption field since vxMonitoring has its own field structure
                action.fields.clear();
            }
        }

        result.push_back(std::move(rule));
    }

    return result;
}

} // namespace

EngineHolder::EngineHolder(
    nx::vms::common::SystemContext* context,
    std::unique_ptr<Router> router,
    std::unique_ptr<Plugin> plugin,
    bool separateThread)
    :
    m_builtinPlugin(std::move(plugin)),
    m_engine(std::make_unique<Engine>(context, std::move(router)))
{
    m_builtinPlugin->initialize(m_engine.get());

    if (separateThread)
    {
        m_thread = std::make_unique<QThread>();
        m_thread->setObjectName("VmsRulesEngine");
        m_thread->start();

        m_engine->moveToThread(m_thread.get());
    }
}

EngineHolder::~EngineHolder()
{
    stop();
}

void EngineHolder::stop()
{
    if (!m_thread)
        return;

    NX_DEBUG(this, "Stopping Engine thread");
    nx::utils::AsyncHandlerExecutor(m_thread.get()).submit(
        [this]
        {
            m_engine.reset();
            m_thread->quit();
        });

    m_thread->wait();
    m_thread.reset();
    NX_DEBUG(this, "Engine thread stopped");
}

Engine* EngineHolder::engine() const
{
    return m_engine.get();
}

void EngineHolder::connectEngine(
    Engine* engine,
    const QnCommonMessageProcessor* processor,
    Qt::ConnectionType connectionType)
{
    if (!NX_ASSERT(processor))
        return;

    connect(processor, &QnCommonMessageProcessor::vmsRulesReset, engine,
        [engine](nx::Uuid /*peerId*/, const nx::vms::api::rules::RuleList& rules)
        {
            // VX fix: Transform virtual action types before adding to engine
            engine->resetRules(transformVirtualActionTypes(rules));
        },
        connectionType);

    connect(
        processor, &QnCommonMessageProcessor::vmsRuleUpdated, engine,
        [engine](const nx::vms::api::rules::Rule& ruleData, ec2::NotificationSource /*source*/)
        {
            // VX fix: Transform virtual action types before updating engine
            auto transformedRules = transformVirtualActionTypes({ruleData});
            if (!transformedRules.empty())
                engine->updateRule(transformedRules.front());
        },
        connectionType);

    connect(processor, &QnCommonMessageProcessor::vmsRuleRemoved, engine,
        [engine](nx::Uuid id)
        {
            engine->removeRule(id);
        },
        connectionType);

    engine->router()->init(processor);
}

} // namespace nx::vms::rules
