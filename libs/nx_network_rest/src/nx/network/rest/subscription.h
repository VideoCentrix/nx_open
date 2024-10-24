// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include <QtCore/QJsonValue>
#include <QtCore/QString>

#include <nx/utils/move_only_func.h>
#include <nx/utils/scope_guard.h>
#include <nx/utils/thread/mutex.h>

#include "handler.h"

namespace nx::network::rest {

class NX_NETWORK_REST_API SubscriptionHandler
{
public:
    using AddMonitor = nx::utils::MoveOnlyFunc<nx::utils::Guard()>;
    using NotifyType = Handler::NotifyType;
    using SubscriptionCallback = Handler::SubscriptionCallback;

    SubscriptionHandler(AddMonitor addMonitor): m_addMonitor(std::move(addMonitor)) {}
    virtual ~SubscriptionHandler() = default;
    nx::utils::Guard addSubscription(const QString& id, SubscriptionCallback callback);
    void notify(const QString& id, NotifyType notifyType, QJsonValue payload = {});

    void setAddMonitor(AddMonitor addMonitor)
    {
        NX_ASSERT(!m_guard);
        m_addMonitor = std::move(addMonitor);
    }

private:
    void removeSubscription(const std::shared_ptr<SubscriptionCallback>& subscription);

private:
    nx::Mutex m_mutex;
    nx::utils::Guard m_guard;
    std::unordered_map<std::shared_ptr<SubscriptionCallback>, QString> m_subscriptions;
    std::unordered_map<QString, std::set<std::shared_ptr<SubscriptionCallback>>> m_subscribedIds;
    AddMonitor m_addMonitor;
};

} // namespace nx::network::rest
