#include "vx_monitoring_business_action_widget.h"
#include "ui_vx_monitoring_business_action_widget.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore/QScopedValueRollback>

namespace nx::vms::client::desktop {

namespace {

const QString _alertLevelTag = "al"; // legacy
const QString _alertLevelIntTag = "ali";

const QStringList alertLevels{
    "Tier 1",
    "Tier 2",
    "Tier 3",
};

} // namespace

VxMonitoringBusinessActionWidget::VxMonitoringBusinessActionWidget(SystemContext *systemContext, QWidget *parent)
    : base_type(systemContext, parent)
    , ui(new Ui::VxMonitoringBusinessActionWidget) {
    ui->setupUi(this);

    ui->alertLevelComboBox->clear();
    ui->alertLevelComboBox->insertItems(0, alertLevels);
    connect(ui->alertLevelComboBox, &QComboBox::currentIndexChanged, this, &VxMonitoringBusinessActionWidget::setParamsToModel);
}

VxMonitoringBusinessActionWidget::~VxMonitoringBusinessActionWidget() = default;

void VxMonitoringBusinessActionWidget::updateTabOrder(QWidget *before, QWidget *after) {
    setTabOrder(before, ui->alertLevelComboBox);
    setTabOrder(ui->alertLevelComboBox, after);
}

void VxMonitoringBusinessActionWidget::at_model_dataChanged(Fields fields) {
    if (!model() || m_updating)
        return;

    const QScopedValueRollback<bool> guard(m_updating, true);

    if (fields.testFlag(Field::eventType)) {
    }

    if (fields.testFlag(Field::actionParams)) {
        setModelToParams();
    }
    base_type::at_model_dataChanged(fields);
}

void VxMonitoringBusinessActionWidget::setModelToParams() {
    const auto &actionParams = model()->actionParams();
    const auto tags = QJsonDocument::fromJson(actionParams.tags.toLocal8Bit());
    if (!tags.isObject())
        return;
    const auto &tagsJson = tags.object();

    if (const auto iterI = tagsJson.find(_alertLevelIntTag); iterI != tagsJson.end())
        ui->alertLevelComboBox->setCurrentIndex(iterI.value().toInt(1) - 1);
    else if (const auto iter = tagsJson.find(_alertLevelTag); iter != tagsJson.end()) {
        const auto idx = std::ranges::find(alertLevels, iter.value().toString());
        if (idx != alertLevels.end())
            ui->alertLevelComboBox->setCurrentIndex(std::distance(alertLevels.begin(), idx));
    }
}

void VxMonitoringBusinessActionWidget::setParamsToModel() {
    if (!model() || m_updating)
        return;

    QScopedValueRollback<bool> guard(m_updating, true);
    auto actionParams = model()->actionParams();

    auto obj = QJsonDocument::fromJson(actionParams.tags.toLocal8Bit()).object();
    const auto alertLevel = ui->alertLevelComboBox->currentIndex() + 1;
    obj[_alertLevelIntTag] = alertLevel;
    actionParams.tags = QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact);

    model()->setActionParams(actionParams);
}

} // namespace nx::vms::client::desktop
