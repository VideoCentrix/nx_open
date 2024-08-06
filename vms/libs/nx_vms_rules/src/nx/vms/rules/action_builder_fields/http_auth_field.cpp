// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "http_auth_field.h"

namespace nx::vms::rules {

namespace {

std::optional<std::string> optionalString(const QString& str)
{
    if (str.isEmpty())
        return {};

    return str.toStdString();
}

} // namespace

nx::network::http::AuthType HttpAuthField::authType() const
{
    return m_auth.authType;
}

void HttpAuthField::setAuthType(const nx::network::http::AuthType& authType)
{
    if (m_auth.authType != authType)
    {
        m_auth.authType = authType;
        emit authTypeChanged();
    }
}

QString HttpAuthField::login() const
{
    return QString::fromStdString(m_auth.credentials.user);
}

void HttpAuthField::setLogin(const QString& value)
{
    if (auto login = value.toStdString(); m_auth.credentials.user != login)
    {
        m_auth.credentials.user = std::move(login);
        emit loginChanged();
    }
}

QString HttpAuthField::password() const
{
    return QString::fromStdString(m_auth.credentials.password.value_or(std::string()));
}

void HttpAuthField::setPassword(const QString& value)
{
    if (auto password = optionalString(value); m_auth.credentials.password != password)
    {
        m_auth.credentials.password = std::move(password);
        emit passwordChanged();

        if (m_auth.credentials.password)
            setToken({});
    }
}

QString HttpAuthField::token() const
{
    return QString::fromStdString(m_auth.credentials.token.value_or(std::string()));
}

void HttpAuthField::setToken(const QString& value)
{
    if (auto token = optionalString(value); m_auth.credentials.token != token)
    {
        m_auth.credentials.token = std::move(token);
        emit tokenChanged();

        if (m_auth.credentials.token)
            setPassword({});
    }
}

const AuthenticationInfo& HttpAuthField::auth() const
{
    return m_auth;
}

QVariant HttpAuthField::build(const AggregatedEventPtr& /*eventAggregator*/) const
{
    return QVariant::fromValue(auth());
}

} // namespace nx::vms::rules
