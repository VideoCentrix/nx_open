// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "server_certificate_warning.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <network/system_helpers.h>
#include <nx/network/socket_common.h>
#include <nx/network/ssl/certificate.h>
#include <nx/vms/api/data/module_information.h>
#include <nx/vms/common/html/html.h>
#include <ui/statistics/modules/certificate_statistics_module.h>
#include <ui/workbench/workbench_context.h>

#include "server_certificate_viewer.h"

namespace nx::vms::client::desktop {

ServerCertificateWarning::ServerCertificateWarning(
    const nx::vms::api::ModuleInformation& target,
    const nx::network::SocketAddress& primaryAddress,
    const nx::network::ssl::CertificateChain& certificates,
    Reason reason,
    QWidget* parent)
    :
    base_type(parent),
    QnWorkbenchContextAware(parent)
{
    // Prepare target description text.
    QStringList knownData;
    const static QString kTemplate("<b>%1</b> %2");

    if (!target.systemName.isEmpty())
        knownData <<  kTemplate.arg(tr("System:"), helpers::getSystemName(target));

    QString serverStr;
    if (target.name.isEmpty())
        serverStr = QString::fromStdString(primaryAddress.address.toString());
    else if (primaryAddress.isNull())
        serverStr = target.name;
    else
        serverStr = nx::format("%1 (%2)", target.name, primaryAddress.address);

    if (!serverStr.isEmpty())
        knownData << kTemplate.arg(tr("Server:"), serverStr);

    knownData << kTemplate.arg(tr("Server ID:"), target.id.toSimpleString());

    const auto targetInfo =
        QString("<p style='margin-top: 8px; margin-bottom: 8px;'>%1</p>")
            .arg(knownData.join(common::html::kLineBreak));

    // Prepare data.
    QnMessageBox::Icon icon = QnMessageBox::Icon::NoIcon;
    QString header, details, advice;
    switch (reason)
    {
        case Reason::unknownServer:
        {
            icon = QnMessageBox::Icon::Question;
            header = tr("Trust this server?");
            details = tr("You attempted to connect to this Server, but it presented a certificate "
                "that cannot be verified automatically.");

            advice = tr("Review the certificate's details to make sure you are connecting to the "
                "correct Server.");
            break;
        }

        case Reason::invalidCertificate:
        case Reason::serverCertificateChanged:
        {
            icon = QnMessageBox::Icon::Warning;
            header = tr("Cannot verify the identity of %1").arg(target.name);
            details = tr("Someone may be impersonating this Server to steal your personal "
                "information.");
            advice = tr("Do not connect to this Server unless instructed by your VMS "
                "administrator.");

            break;
        }

        default:
            NX_ASSERT("Unreachable");
    }


    // Load data into UI.
    setIcon(icon);
    setText(header);
    setInformativeText(targetInfo + details);

    // Add this text as a separate label to make a proper spacing.
    auto additionalText = new QLabel(advice);
    additionalText->setWordWrap(true);
    addCustomWidget(additionalText);

    auto layout = findChild<QVBoxLayout*>("verticalLayout");
    if (NX_ASSERT(layout))
        layout->setSpacing(12);

    auto statisticsName =
        [reason](const QString& name)
        {
            switch (reason)
            {
                case Reason::unknownServer:
                    return "unkn_dialog_" + name;
                case Reason::invalidCertificate:
                case Reason::serverCertificateChanged:
                    return "invl_dialog_" + name;
            }
            return QString();
        };
    auto statistics = context()->instance<QnCertificateStatisticsModule>();
    statistics->registerClick(statisticsName("open"));

    // Init server certificate `link`
    auto link = new QLabel(common::html::localLink(tr("View certificate")));
    connect(link, &QLabel::linkActivated, this,
        [=, this]
        {
            auto viewer = new ServerCertificateViewer(
                target,
                primaryAddress,
                certificates,
                ServerCertificateViewer::Mode::presented,
                this);

            // Show modal.
            viewer->open();
            statistics->registerClick(statisticsName("view_cert"));
        });
    addCustomWidget(link);

    // Create 'Connect' button.
    auto connectButton = addButton(
        tr("Connect Anyway"),
        QDialogButtonBox::AcceptRole,
        Qn::ButtonAccent::Warning);

    if (reason == Reason::invalidCertificate || reason == Reason::serverCertificateChanged)
    {
        // Create mandatory checkbox for additional safety.
        auto checkbox = new QCheckBox(tr("I trust this server"));
        auto updateButtonState =
            [connectButton, checkbox]
            {
                connectButton->setEnabled(checkbox->isChecked());
            };
        connect(checkbox, &QCheckBox::toggled, this, updateButtonState);

        addCustomWidget(checkbox);
        updateButtonState();
    }

    connect(connectButton, &QPushButton::clicked,
        statistics, [=] { statistics->registerClick(statisticsName("connect")); });

    // Create 'Cancel' button.
    setStandardButtons({QDialogButtonBox::Cancel});
}

void ServerCertificateWarning::showEvent(QShowEvent *event)
{
    // Set focus to 'Cancel' button for some additional safety.
    button(QDialogButtonBox::Cancel)->setFocus();
    base_type::showEvent(event);
}

} // namespace nx::vms::client::desktop
