// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/vms/rules/event_filter_fields/text_lookup_field.h>

#include "field_picker_widget.h"

class QComboBox;
class QStackedWidget;
class QLineEdit;

namespace nx::vms::client::desktop::rules {

class TextLookupPicker: public TitledFieldPickerWidget<vms::rules::TextLookupField>
{
    Q_OBJECT

public:
    TextLookupPicker(
        vms::rules::TextLookupField* field, SystemContext* context, ParamsWidget* parent);

    void setValidity(const vms::rules::ValidationResult& validationResult) override;

private:
    QComboBox* m_checkTypeComboBox{nullptr};
    QStackedWidget* m_stackedWidget{nullptr};
    QLineEdit* m_lineEdit{nullptr};
    QComboBox* m_lookupListComboBox{nullptr};

    void updateUi() override;
};

} // namespace nx::vms::client::desktop::rules
