// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/fusion/model_functions_fwd.h>
#include <nx/utils/uuid.h>

namespace nx::network { class HostAddress; }
namespace nx::network::http { class Request; }

namespace nx::network::rest {

struct NX_NETWORK_REST_API AuthSession
{
    nx::Uuid id;
    QString userName;
    QString userHost;
    QString userAgent;
    bool isAutoGenerated = false;

    AuthSession(nx::Uuid id_): id(std::move(id_)) {}
    AuthSession(
        nx::Uuid id_,
        const QString& userName,
        const nx::network::http::Request& request,
        const nx::network::HostAddress& hostAddress);

    bool operator==(const AuthSession& other) const = default;
    QString toString() const;
    void fromString(const QString& data);

    static std::chrono::seconds keepPeriod(bool longPeriod)
    {
        return longPeriod ? std::chrono::seconds{std::chrono::days{1}} : std::chrono::seconds{15};
    }

    std::chrono::seconds keepPeriod() const { return keepPeriod(!isAutoGenerated); }
};

#define AuthSession_Fields (id)(userName)(userHost)(userAgent)
QN_FUSION_DECLARE_FUNCTIONS(AuthSession,
    (ubjson)(xml)(json)(csv_record)(sql_record), NX_NETWORK_REST_API)
NX_REFLECTION_INSTRUMENT(AuthSession, AuthSession_Fields)

NX_NETWORK_REST_API void serialize_field(const AuthSession& authData, QVariant* target);
NX_NETWORK_REST_API void deserialize_field(const QVariant& value, AuthSession* target);

} // namespace nx::network::rest
