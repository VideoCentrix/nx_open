// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QHash>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtQuickWidgets/QQuickWidget>

#include <api/server_rest_connection.h>
#include <nx/utils/uuid.h>
#include <nx/vms/client/core/common/utils/common_module_aware.h>
#include <nx/vms/client/core/network/remote_connection_aware.h>
#include <nx/vms/client/desktop/analytics/analytics_engines_watcher.h>
#include <nx/vms/client/desktop/system_administration/models/api_integration_requests_model.h>

#include "../analytics_settings_widget.h"

namespace nx::vms::client::desktop {

class AnalyticsSettingsWidget::Private:
    public QObject,
    public nx::vms::client::core::CommonModuleAware,
    public nx::vms::client::core::RemoteConnectionAware
{
    Q_OBJECT
    Q_PROPERTY(QVariant analyticsEngines READ analyticsEngines NOTIFY analyticsEnginesChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit Private(AnalyticsSettingsWidget* q);

    Q_INVOKABLE nx::Uuid getCurrentEngineId() const { return currentEngineId; }
    Q_INVOKABLE void setCurrentEngineId(const nx::Uuid& engineId);

    Q_INVOKABLE QJsonObject settingsValues(const nx::Uuid& engineId);

    Q_INVOKABLE QVariant requestParameters(const QJsonObject& model);

    Q_INVOKABLE void setSettingsValues(
        const nx::Uuid& engineId,
        const QString& activeElement,
        const QJsonObject& values,
        const QJsonObject& parameters);

    Q_INVOKABLE QJsonObject settingsModel(const nx::Uuid& engineId);
    Q_INVOKABLE QJsonObject errors(const nx::Uuid& engineId) { return m_errors[engineId]; }

    QVariant analyticsEngines() const { return engines; }

    void updateEngines();
    void activateEngine(const nx::Uuid& engineId);

    bool loading() const { return settingsLoading; }
    void setLoading(bool loading);

    void refreshSettings(const nx::Uuid& engineId);
    void activeElementChanged(
        const nx::Uuid& engineId,
        const QString& activeElement,
        const QJsonObject& parameters);

    void applySettingsValues();

    void updateHasChanges();

    Q_INVOKABLE ApiIntegrationRequestsModel* makeApiIntegrationRequestsModel() const;

public:
    struct SettingsValues
    {
        QJsonObject values;
        bool changed = false;
    };

public:
    AnalyticsSettingsWidget* const q = nullptr;

    const QScopedPointer<QQuickWidget> view;
    const QScopedPointer<AnalyticsEnginesWatcher> enginesWatcher;
    QVariantList engines;
    bool hasChanges = false;
    bool settingsLoading = false;
    QList<rest::Handle> pendingRefreshRequests;
    QList<rest::Handle> pendingApplyRequests;

    nx::Uuid currentEngineId;

    QHash<nx::Uuid, QJsonObject> m_errors;
    QHash<nx::Uuid, SettingsValues> settingsValuesByEngineId;
    QHash<nx::Uuid, QJsonObject> settingsModelByEngineId;
    QVariant requests;

signals:
    void analyticsEnginesChanged();
    void currentSettingsStateChanged();
    void currentErrorsChanged();
    void loadingChanged();
    void licenseSummariesChanged();

private:
    void addEngine(const nx::Uuid& engineId, const AnalyticsEngineInfo& engineInfo);
    void removeEngine(const nx::Uuid& engineId);
    void updateEngine(const nx::Uuid& engineId);
    void setErrors(const nx::Uuid& engineId, const QJsonObject& errors);
    void resetSettings(
        const nx::Uuid& engineId,
        const QJsonObject& model,
        const QJsonObject& values,
        const QJsonObject& errors,
        bool changed);
};

} // namespace nx::vms::client::desktop
