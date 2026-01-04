// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <memory>

#include <nx/utils/property_storage/storage.h>
#include <nx/vms/client/core/resource/screen_recording/audio_device_info.h>

class QMediaDevices;

namespace nx::vms::client::core {

class NX_VMS_CLIENT_CORE_API AudioRecordingSettings: public nx::utils::property_storage::Storage
{
    Q_OBJECT

public:
    static QString kNoDevice;

    AudioRecordingSettings();
    virtual ~AudioRecordingSettings();

    Property<QString> primaryAudioDeviceName{this, "primaryAudioDevice"};
    Property<QString> secondaryAudioDeviceName{this, "secondaryAudioDevice"};

    QList<AudioDeviceInfo> availableDevices() const;

    AudioDeviceInfo primaryAudioDevice() const;
    AudioDeviceInfo secondaryAudioDevice() const;

    AudioDeviceInfo defaultPrimaryAudioDevice() const;
    AudioDeviceInfo defaultSecondaryAudioDevice() const;

signals:
    void availableDevicesChanged();

private:
    AudioDeviceInfo getDeviceByName(const QString& name) const;

private:
    std::unique_ptr<QMediaDevices> m_notifier;
    QList<AudioDeviceInfo> m_devices;
};

} // namespace nx::vms::client::core
