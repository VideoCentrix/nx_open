// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "saas_utils.h"

#include <nx/vms/api/data/saas_data.h>
#include <nx/vms/common/saas/saas_service_manager.h>
#include <nx/vms/common/system_context.h>

namespace nx::vms::common::saas {

using namespace nx::vms::api;

namespace {

bool servicesOverused(SystemContext* systemContext, const QString& seriviceType)
{
    if (!NX_ASSERT(systemContext))
        return false;

    const auto saasServiceManager = systemContext->saasServiceManager();
    if (!NX_ASSERT(saasServiceManager))
        return false;

    if (saasServiceManager->saasState() == SaasState::uninitialized)
        return false;

    return saasServiceManager->serviceStatus(seriviceType).status
        == nx::vms::api::UseStatus::overUse;
}

} // namespace

bool saasInitialized(SystemContext* systemContext)
{
    return systemContext->saasServiceManager()->saasState() != SaasState::uninitialized;
}

bool saasServicesOperational(SystemContext* systemContext)
{
    const auto state = systemContext->saasServiceManager()->saasState();
    return state == SaasState::active || state == SaasState::suspend;
}

bool localRecordingServicesOverused(SystemContext* systemContext)
{
    return servicesOverused(
        systemContext,
        nx::vms::api::SaasService::kLocalRecordingServiceType);
}

bool cloudStorageServicesOverused(SystemContext* systemContext)
{
    return servicesOverused(
        systemContext,
        nx::vms::api::SaasService::kCloudRecordingType);
}

bool integrationServicesOverused(SystemContext* systemContext)
{
    return servicesOverused(
        systemContext,
        nx::vms::api::SaasService::kAnalyticsIntegrationServiceType);
}

QString StringsHelper::shortState(nx::vms::api::SaasState state)
{
    using namespace nx::vms::api;
    switch (state)
    {
        case SaasState::uninitialized:
            return tr("SaaS uninitialized");
        case SaasState::active:
            return tr("SaaS active");
        case SaasState::suspend:
            return tr("SaaS suspended");
        case SaasState::shutdown:
            return tr("SaaS shut down");
        case SaasState::autoShutdown:
            return tr("SaaS shut down automatically");

        default:
            return QString();
    }
}

QString StringsHelper::recommendedAction(nx::vms::api::SaasState state)
{
    using namespace nx::vms::api;
    switch (state)
    {
        case SaasState::uninitialized:
        case SaasState::active:
            return QString();

        case SaasState::suspend:
        case SaasState::shutdown:
            return tr("Contact your channel partner for details.");

        case SaasState::autoShutdown:
            return tr("Check internet connection between VMS and license server.");

        default:
            return QString();
    }
}

} // nx::vms::common::saas