// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "help_topic.h"

#include <QtQml/QtQml>

#include <nx/utils/log/assert.h>

namespace nx::vms::client::desktop {

void HelpTopic::registerQmlType()
{
    qmlRegisterUncreatableType<HelpTopic>("nx.vms.client.desktop", 1, 0, "HelpTopic",
        "Cannot create an instance of HelpTopic.");
}

QString HelpTopic::relativeUrlForTopic(Id topic)
{
    switch (topic)
    {
        case Id::Empty:
        case Id::Forced_Empty:
            return QString();

        case Id::About:
            return "collecting_additional_informat.html";
        case Id::Administration_General_CamerasList:
            return "viewing_full_cameras_list.html";
        case Id::Administration:
            return "system-wide_configurations.html";
        case Id::Administration_RoutingManagement:
            return "configuring_routing_within_mul.html";
        case Id::Administration_TimeSynchronization:
            return "configuring_time_synchronizati.html";
        case Id::Administration_Update:
            return "upgrading_vms.html";
        case Id::AuditTrail:
            return "viewing_users_actions_log_audi.html";
        case Id::Bookmarks_Editing:
            return "creating_and_setting_up_bookma.html";
        case Id::Bookmarks_Search:
            return "search_using_bookmarks.html";
        case Id::Bookmarks_Usage:
            return "using_bookmarks.html";
        case Id::CameraDiagnostics:
            return "diagnosing_offline_cameras.html";
        case Id::CameraList:
            return "viewing_full_cameras_list.html";
        case Id::CameraReplacementDialog:
            return "replacing-a-camera.html";
        case Id::CameraSettingsExpertPage:
            return "working_around_device_issues_e.html";
        case Id::CameraSettingsRecordingPage:
            return "editing_recording_schedule.html";
        case Id::CameraSettingsWebPage:
            return "configuring_devices_using_thei.html";
        case Id::CameraSettings_AspectRatio:
            return "setting_camera_aspect_ratio.html";
        case Id::CameraSettings_Dewarping:
            return "dewarp-controls.html";
        case Id::CameraSettings_Expert_DisableArchivePrimary:
            return "disabling_recording_of_primary.html";
        case Id::CameraSettings_Expert_Rtp:
            return "setting_up_camera_transport_pr.html";
        case Id::CameraSettings_Expert_SettingsControl:
            return "preventing_vms_from_c.html";
        case Id::CameraSettings:
            return "setting_up_cameras_and_devices.html";
        case Id::CameraSettings_Motion:
            return "setting_up_motion_mask_and_motion_sensitivity_(adm.html";
        case Id::CameraSettings_Recording_ArchiveLength:
            return "configuring-maxi-and-min-archive-storage.html";
        case Id::CameraSettings_Recording_Export:
            return "copying_recording_schedule_fro.html";
        case Id::CameraSettings_Rotation:
            return "setting_camera_orientation.html";
        case Id::CameraSettings_SecondStream:
            return "dual_streaming.html";
        case Id::CameraSettings_Onvif:
            return "using-the-specific-onvif-profi.html";
        case Id::CertificateValidation:
            return "certificate-validation.html";
        case Id::CloudLayoutsIntroduction:
            return "adding-items-to-layout.html";
        case Id::CloudLayoutsIntroductionAssign:
            return "layout-management.html";
        case Id::ConnectToCamerasOverOnlyHttps:
            return "to-connect-to-cameras-over-onl.html";
        case Id::EnableArchiveEncryption:
            return "to-enable-archive-encryption.html";
        case Id::EnableEncryptedVideoTraffic:
            return "to-enable-encrypted-video-traf.html";
        case Id::EventLog:
            return "viewing_events_log.html";
        case Id::EventsActions_BackupFinished:
            return "archive_backup_finished.html";
        case Id::EventsActions_Bookmark:
            return "create_bookmark.html";
        case Id::EventsActions_CameraDisconnected:
            return "camera_disconnection_malfuncti.html";
        case Id::CollectingLogs:
            return "collecting_logs.html";
        case Id::EventsActions_CameraInput:
            return "input_signal_on_camera.html";
        case Id::EventsActions_CameraIpConflict:
            return "camera_ip_conflict.html";
        case Id::EventsActions_CameraMotion:
            return "motion_on_camera.html";
        case Id::EventsActions_CameraOutput:
            return "trigger_camera_output.html";
        case Id::EventsActions_Diagnostics:
            return "write_to_log.html";
        case Id::EventsActions_EmailNotSet:
            return "e-mail_is_not_set_for_users.html";
        case Id::EventsActions_EmailServerNotSet:
            return "e-mail_server_is_not_configure.html";
        case Id::EventsActions_ExecHttpRequest:
            return "perform_http_request.html";
        case Id::EventsActions_ExecutePtzPreset:
            return "execute_ptz_preset.html";
        case Id::EventsActions_Generic:
            return "generic_event.html";
        case Id::EventsActions:
            return "configuring_events_and_actions.html";
        case Id::EventsActions_LdapSyncIssue:
            return "ldap-sync-failure.html";
        case Id::EventsActions_LicenseIssue:
            return "license_issue.html";
        case Id::EventsActions_MediaServerConflict:
            return "media_servers_conflict.html";
        case Id::EventsActions_MediaServerFailure:
            return "media_server_failure.html";
        case Id::EventsActions_MediaServerStarted:
            return "media_server_started.html";
        case Id::EventsActions_NetworkIssue:
            return "network_issue.html";
        case Id::EventsActions_NoLicenses:
            return "licenses_are_not_configured.html";
        case Id::EventsActions_PlaySoundRepeated:
            return "repeat_sound.html";
        case Id::EventsActions_PlaySound:
            return "play_sound.html";
        case Id::EventsActions_Schedule:
            return "setting_up_schedule_for_tracki.html";
        case Id::EventsActions_SendMailError:
            return "error_while_sending_e-mail.html";
        case Id::EventsActions_SendMail:
            return "mail_notifications.html";
        case Id::EventsActions_SendMobileNotification:
            return "send_mobile_notification.html";
        case Id::EventsActions_ShowDesktopNotification:
            return "notifications.html";
        case Id::EventsActions_ShowOnAlarmLayout:
            return "showing_cameras_on_alarm_layou.html";
        case Id::EventsActions_ShowIntercomInformer:
            return "working-with-intercoms.html";
        case Id::EventsActions_ShowTextOverlay:
            return "display_text_on_cameras.html";
        case Id::EventsActions_SoftTrigger:
            return "soft_triggers.html";
        case Id::EventsActions_Speech:
            return "say_text.html";
        case Id::EventsActions_StartPanicRecording:
            return "start_panic_recording.html";
        case Id::EventsActions_StartRecording:
            return "start_recording_on_camera.html";
        case Id::EventsActions_StorageFailure:
            return "storage_failure.html";
        case Id::EventsActions_StoragesMisconfigured:
            return "storages_misconfiguration.html";
        case Id::EventsActions_VideoAnalytics:
            return "analytics_event.html";
        case Id::Exporting:
            return "exporting.html";
        case Id::ForceSecureConnections:
            return "to-force-secure-connections.html";
        case Id::IOModules:
            return "setting_up_i_o_modules.html";
        case Id::ImageEnhancement:
            return "color_correction.html";
        case Id::LaunchingAndClosing:
            return "launching_and_closing_vms_client.html";
        case Id::LayoutSettings_EMapping:
            return "layout-backgrounds.html";
        case Id::LayoutSettings_Locking:
            return "saving-locking-layouts.html";
        case Id::Ldap:
            return "adding_users_from_ldap_server.html";
        case Id::Licenses:
            return "obtaining_and_activating_vms_licenses.html";
        case Id::Login:
            return "connecting_to_enterprise_contr.html";
        case Id::MainWindow_Calendar:
            return "using_calendar.html";
        case Id::MainWindow_ContextHelp:
            return "getting_context_help.html";
        case Id::MainWindow_DayTimePicker:
            return "using_calendar.html";
        case Id::MainWindow_Fullscreen:
            return "full_screen_and_windowed_mode.html";
        case Id::MainWindow_MediaItem_AnalogCamera:
            return "setting_up_analog_cameras.html";
        case Id::MainWindow_MediaItem_Dewarping:
            return "working_with_fish-eye_cameras.html";
        case Id::MainWindow_MediaItem_Diagnostics:
            return "diagnosing_offline_cameras.html";
        case Id::MainWindow_MediaItem:
            return "viewing_camera_stream_information.html";
        case Id::MainWindow_MediaItem_ImageEnhancement:
            return "color_correction.html";
        case Id::MainWindow_MediaItem_Local:
            return "playing_back_local_files_in_vms.html";
        case Id::MainWindow_MediaItem_Ptz:
            return "ptz-controls.html";
        case Id::MainWindow_MediaItem_Rotate:
            return "rotate.html";
        case Id::MainWindow_MediaItem_Screenshot:
            return "taking_a_screenshot.html";
        case Id::MainWindow_MediaItem_SmartSearch:
            return "performing_smart_search.html";
        case Id::MainWindow_MediaItem_Unauthorized:
            return "configuring_camera_authentication_(admin_only).html";
        case Id::MainWindow_MediaItem_AccessDenied:
            return "configuring_camera_authentication_(admin_only).html";
        case Id::MainWindow_MediaItem_ZoomWindows:
            return "zoom_windows.html";
        case Id::MainWindow_MonitoringItem:
            return "monitoring_media_servers.html";
        case Id::MainWindow_MonitoringItem_Log:
            return "viewing_events_log.html";
        case Id::MainWindow_Navigation:
            return "navigating_through_archive_and.html";
        case Id::MainWindow_Notifications_EventLog:
            return "viewing_events_log.html";
        case Id::MainWindow_Pin:
            return "showing_and_hiding_side_panels.html";
        case Id::MainWindow_Playback:
            return "playback_panel.html";
        case Id::MainWindow_Scene_EMapping:
            return "layout-backgrounds.html";
        case Id::MainWindow_Scene:
            return "index.html";
        case Id::MainWindow_Scene_PreviewSearch:
            return "thumbnail_search.html";
        case Id::MainWindow_Scene_TourInProgress:
            return "tours.html";
        case Id::MainWindow_Slider_Timeline:
            return "pan-temporal_timeline.html";
        case Id::MainWindow_Slider_Volume:
            return "adjusting_volume_while_viewing_local_files.html";
        case Id::MainWindow_Sync:
            return "navigating_through_several_cameras_synchronously.html";
        case Id::MainWindow_Thumbnails:
            return "using_thumbnails_for_better_navigation.html";
        case Id::MainWindow_TitleBar_Cloud:
            return "connecting_as_cloud_user.html";
        case Id::MainWindow_TitleBar_MainMenu:
            return "main_menu.html";
        case Id::MainWindow_TitleBar_NewLayout:
            return "creating-and-sharing-layouts.html";
        case Id::MainWindow_Tree_Camera:
            return "configuring_cameras.html";
        case Id::MainWindow_Tree_Exported:
            return "viewing_videos_exported_from_a.html";
        case Id::MainWindow_Tree:
            return "resource-panel.html";
        case Id::MainWindow_Tree_Layouts:
            return "customizing_product_name.html";
        case Id::MainWindow_Tree_MultiVideo:
            return "changing_multi-channel_video_a.html";
        case Id::MainWindow_Tree_Recorder:
            return "setting_up_analog_cameras.html";
        case Id::MainWindow_Tree_Servers:
            return "configuring_media_servers_additional_settings_(ad.html";
        case Id::MainWindow_Tree_Users:
            return "users_management.html";
        case Id::MainWindow_Tree_WebPage:
            return "managing-web-pages-and-integrations.html";
        case Id::MainWindow_WebPageItem:
            return "managing-web-pages-and-integrations.html";
        case Id::MediaFolders:
            return "configuring_media_folders.html";
        case Id::NewUser:
            return "adding-users.html";
        case Id::NotificationsPanel:
            return "notifications_panel.html";
        case Id::ObjectSearch:
            return "objects_tab.html";
        case Id::OtherSystems:
            return "configuring_multi-server_envir.html";
        case Id::PluginsAndAnalytics:
            return "plugins-and-analytics.html";
        case Id::PtzManagement_Tour:
            return "setting_up_ptz_tours.html";
        case Id::PtzPresets:
            return "saving_and_restoring_ptz_posit.html";
        case Id::Rapid_Review:
            return "rapid_review_export.html";
        case Id::SaveLayout:
            return "saving-locking-layouts.html";
        case Id::SecureConnection:
            return "configuring-secure-connections.html";
        case Id::ServerSettings_ArchiveRestoring:
            return "restoring_broken_archive.html";
        case Id::ServerSettings_Backup:
            return "configuring_redundant_storage_.html";
        case Id::ServerSettings_Failover:
            return "configuring_failover.html";
        case Id::ServerSettings_General:
            return "configuring_media_servers_additional_settings_(ad.html";
        case Id::ServerSettings_StorageAnalitycs:
            return "analyzingandanticipatingstorageusage.html";
        case Id::ServerSettings_StoragesBackup:
            return "configuring_redundant_storage_.html";
        case Id::ServerSettings_Storages:
            return "configuring_media_server_storages_(admin_only).html";
        case Id::ServerSettings_WebClient:
            return "using_media_servers_web_interf.html";
        case Id::SessionAndDigestAuth:
            return "session-and-digest-authentication.html";
        case Id::Setup_Wizard:
            return "vms_quick_start.html";
        case Id::Showreel:
            return "showreel_layout_tours.html";
        case Id::SystemSettings_Cloud:
            return "connecting_system_to_cloud.html";
        case Id::SystemSettings_General_AnonymousUsage:
            return "sending_anonymous_usage_and_cr.html";
        case Id::SystemSettings_General_AutoPause:
            return "cpu_and_bandwidth_saving_durin.html";
        case Id::SystemSettings_General_Customizing:
            return "customizing_product_name.html";
        case Id::SystemSettings_General_DoubleBuffering:
            return "double_buffering.html";
        case Id::SystemSettings_General_Logs:
            return "collecting_logs.html";
        case Id::SystemSettings_General_ShowIpInTree:
            return "resource-panel.html";
        case Id::SystemSettings_General_TourCycleTime:
            return "tours.html";
        case Id::SystemSettings_LogsManagement:
            return "collecting_logs.html";
        case Id::SystemSettings_Notifications:
            return "notifications.html";
        case Id::SystemSettings_ScreenRecording:
            return "setting_up_screen_recording.html";
        case Id::SystemSettings_Server_Backup:
            return "backing_up_and_restoring_produ.html";
        case Id::SystemSettings_Server_CameraAutoDiscovery:
            return "discovering_cameras_automatica.html";
        case Id::SystemSettings_Server_Mail:
            return "configuring_mail_server_for_e-.html";
        case Id::SystemSettings_UserManagement:
            return "manage-users.html";
        case Id::SystemSettings_UserManagement_Groups:
            return "managing-groups.html";
        case Id::Systems_ConnectToCurrentSystem:
            return "join_server_from_different_sys.html";
        case Id::Systems_MergeSystems:
            return "merge_all_servers_to_another_s.html";
        case Id::UserRoles:
            return "roles_management.html";
        case Id::UserSettings_DisableUser:
            return "disabling_user.html";
        case Id::UserSettings_Permissions:
            return "permissions-management.html";
        case Id::UserSettings_UserRoles:
            return "built-in-groups.html";
        case Id::UserWatermark:
            return "user_watermark_on_exports.html";
        case Id::UsingJoystick:
            return "using-joysticks.html";
        case Id::VersionMismatch:
            return "launching_product_name_in_comp.html";
        case Id::Videowall_Attach:
            return "creating_new_video_wall.html";
        case Id::Videowall_Display:
            return "controlling_video_wall_display.html";
        case Id::Videowall:
            return "configuring_video_wall_several.html";
        case Id::Videowall_Management:
            return "video_wall_management.html";
        case Id::VirtualCamera:
            return "virtual_camera.html";
        case Id::Watermark:
            return "viewing_and_checking_the_validity_of_exported_vide.html";

        default:
            NX_ASSERT(false, "Unhandled help topic %1", (int) topic);
            return QString();
    }
}

} // namespace nx::vms::client::desktop
