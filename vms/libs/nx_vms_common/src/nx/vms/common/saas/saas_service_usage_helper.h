// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QObject>

#include <core/resource/resource_fwd.h>
#include <nx/vms/api/data/license_data.h>
#include <nx/vms/common/system_context_aware.h>
#include <nx/utils/thread/mutex.h>
#include <nx/utils/uuid.h>

namespace nx::vms::common::saas {

/*
 * This class is intended to calculate service usage for SaaS services
 * that VMS reads from Channel Partner Server.
 */
class NX_VMS_COMMON_API CloudServiceUsageHelper:
    public QObject,
    public /*mixin*/ SystemContextAware
{
public:
    CloudServiceUsageHelper(
        SystemContext* context,
        QObject* parent = nullptr);

protected:
    QnVirtualCameraResourceList getAllCameras() const;
    static void sortCameras(QnVirtualCameraResourceList* inOutCameras);
};

 /*
  *  Helper class to calculate integration licenses usages.
  */
class NX_VMS_COMMON_API IntegrationServiceUsageHelper: public CloudServiceUsageHelper
{
public:
    IntegrationServiceUsageHelper(
        SystemContext* context,
        QObject* parent = nullptr);

    /*
     *  @param id Integration id.
     *  @return Information about available licenses per integration.
     */
    nx::vms::api::LicenseSummaryDataEx info(const QString& integrationId);

    std::map<QString, nx::vms::api::LicenseSummaryDataEx> allInfo() const;

    /*
     *  @return true if there are not enough licenses for any integration.
     */
    bool isOverflow() const;

    //std::set<nx::Uuid> usedDevices() const;

    /*
     * @return Information about cameras which consume SAAS integration services.
     * key - serviceId, value - the physicalId list of consumed cameras.
     */
    std::map<nx::Uuid, std::set<QString>> camerasByService() const;

    struct Propose
    {
        nx::Uuid resourceId;
        QSet<nx::Uuid> integrations;
    };

    /* Propose change in integration usage for some resource.
     *  @param resourceId Resource Id.
     *  @param integrations Set of used integration.
     */
    void proposeChange(const nx::Uuid& resourceId, const QSet<nx::Uuid>& integrations);

    void proposeChange(const std::vector<Propose>& data);

    void invalidateCache();
private:
    void updateCache();
    void updateCacheUnsafe() const;
private:
    //Summary by integrationId.
    mutable std::optional<std::map<QString, nx::vms::api::LicenseSummaryDataEx>> m_cache;
    mutable nx::Mutex m_mutex;
};

/*
  *  Helper class to calculate integration licenses usages.
  */
class NX_VMS_COMMON_API CloudStorageServiceUsageHelper: public CloudServiceUsageHelper
{
public:
    CloudStorageServiceUsageHelper(
        SystemContext* context,
        QObject* parent = nullptr);

    /*
     *  @return true if there are not enough licenses for any integration.
     */
    bool isOverflow() const;

    /*
     * @return Information about available license usages per service.
     *     key - megapixels, value - service usage info.
     */
    std::map<int, nx::vms::api::LicenseSummaryData> allInfoByMegapixels() const;

    /*
     * @return Services usage summary accumulated for cloud storage services with resolution
     *     greater than or euqal to the given value.
     */
    nx::vms::api::LicenseSummaryData allInfoForResolution(int megapixels) const;

    /*
     * @return Information about available license usages per service.
     *     key - serviceId, value - service usage info.
     */
    std::map<nx::Uuid, nx::vms::api::LicenseSummaryData> allInfoByService() const;

    /*
     * @return Information about cameras which consumes SAAS services.
     * key - camera physicalId, value - serviceId. If there is not enough services for some camera
     * then the service value is an empty UUID.
     */
    std::map<QString, nx::Uuid> servicesByCameras() const;

    /* Propose change that resources are used for cloud storage.
     *  @param devices New full set of resources that is going to be used.
     */
    void setUsedDevices(const QSet<nx::Uuid>& devices);

    /* Propose change that resources are used for cloud storage. Contains delta only.
     *  @param devicesToAdd New devices to  add to the list.
     *  @param devicesToRemove Devices to  remove from the list.
     */
    void proposeChange(
        const std::set<nx::Uuid>& devicesToAdd,
        const std::set<nx::Uuid>& devicesToRemove);

    void invalidateCache();

private:
    void updateCache();
    void updateCacheUnsafe() const;
    void borrowUsages() const;
    void calculateAvailableUnsafe() const;
    void processUsedDevicesUnsafe(const QnVirtualCameraResourceList& cameras) const;
    void countCameraAsUsedUnsafe(const QnVirtualCameraResourcePtr& camera) const;
private:

    struct ServiceInfo
    {
        bool operator<(const ServiceInfo& other) const;

        int megapixels = 0;
        nx::Uuid serviceId;
    };

    //Summary by megapixels and serviceId.
    mutable std::optional<std::map<ServiceInfo, nx::vms::api::LicenseSummaryData>> m_cache;
    mutable nx::Mutex m_mutex;
};

/*
  *  Helper class to calculate local recording usages.
  */
class NX_VMS_COMMON_API LocalRecordingUsageHelper: public CloudServiceUsageHelper
{
public:
    LocalRecordingUsageHelper(
        SystemContext* context,
        QObject* parent = nullptr);

    /*
     * @return Information about cameras which consumes SAAS services.
     *     key - serviceId, value - camera groupId list. If there is not enough services for some camera
     *     then the service value is an empty UUID.
     *     groupId is the camera physicalId for regular cameras and groupId for NVR. It is used to
     *     spend a single license for NVR.
     */
    std::map<nx::Uuid, std::set<QString>> cameraGroupsByService() const;

};

} // nx::vms::common::saas
