// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <ui/widgets/business/abstract_business_params_widget.h>

namespace Ui {
class VxMonitoringBusinessActionWidget;
}

namespace nx::vms::client::desktop {

class VxMonitoringBusinessActionWidget : public QnAbstractBusinessParamsWidget {
    Q_OBJECT
    typedef QnAbstractBusinessParamsWidget base_type;

public:
    explicit VxMonitoringBusinessActionWidget(SystemContext *systemContext, QWidget *parent = nullptr);
    ~VxMonitoringBusinessActionWidget() override;

    void updateTabOrder(QWidget *before, QWidget *after) override;

protected:
    virtual void at_model_dataChanged(Fields fields) override;
    void setParamsToModel();
    void setModelToParams();

private:
    QScopedPointer<Ui::VxMonitoringBusinessActionWidget> ui;
};

} // namespace nx::vms::client::desktop
