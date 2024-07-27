// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QScopedPointer>

#include <qt_graphics_items/graphics_widget.h>

class QnMediaResourceWidget;

namespace nx::vms::client::desktop {

class AudioSpectrumOverlayWidget: public GraphicsWidget
{
    Q_OBJECT
    using base_type = GraphicsWidget;

public:
    AudioSpectrumOverlayWidget(QnMediaResourceWidget* resourceWidget, QGraphicsWidget* parent);
    virtual ~AudioSpectrumOverlayWidget() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    class Private;
    const QScopedPointer<Private> d;
};

} // namespace nx::vms::client::desktop
