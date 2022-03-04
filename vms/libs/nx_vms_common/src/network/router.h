// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QObject>

#include <core/resource/resource_fwd.h>
#include <nx/network/http/http_types.h>
#include <nx/network/socket_common.h>
#include <nx/utils/uuid.h>

namespace nx::vms::common { class ResourceContext; }
namespace nx::vms::discovery { class Manager; }

struct NX_VMS_COMMON_API QnRoute
{
    /** Address for the connect. */
    nx::network::SocketAddress addr;

    /** Target Server Id. */
    QnUuid id;

    /** Proxy Server Id. */
    QnUuid gatewayId;

    /** Whether direct connection is not available. */
    bool reverseConnect = false;

    /** Distance in peers. */
    int distance = 0;

    bool isValid() const { return !addr.isNull(); }
    QString toString() const;
};

class NX_VMS_COMMON_API QnRouter: public QObject
{
    Q_OBJECT
public:
    explicit QnRouter(
        nx::vms::discovery::Manager* moduleManager,
        QObject* parent = nullptr);

    QnRoute routeTo(const QnUuid& serverId, nx::vms::common::ResourceContext* context);
    QnRoute routeTo(const QnMediaServerResourcePtr& server);

private:
    const nx::vms::discovery::Manager* m_moduleManager;
};
