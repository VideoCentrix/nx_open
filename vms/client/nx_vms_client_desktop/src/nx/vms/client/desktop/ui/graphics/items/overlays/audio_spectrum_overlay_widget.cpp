// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "audio_spectrum_overlay_widget.h"

#include <QtWidgets/QGraphicsGridLayout>

#include <camera/cam_display.h>
#include <camera/resource_display.h>

#include "audio_spectrum_widget.h"
#include "nx/vms/client/core/skin/color_theme.h"
#include "ui/common/palette.h"

namespace nx::vms::client::desktop {

//-------------------------------------------------------------------------------------------------
// AudioSpectrumOverlayWidget::Private

class AudioSpectrumOverlayWidget::Private
{
    AudioSpectrumOverlayWidget* const q = nullptr;

public:
    Private(QnResourceDisplayPtr display, AudioSpectrumOverlayWidget* q);

public:
    QnResourceDisplayPtr const display;
    AudioSpectrumWidget* const widget;
};

AudioSpectrumOverlayWidget::Private::Private(
    QnResourceDisplayPtr display,
    AudioSpectrumOverlayWidget* q)
    :
    q(q),
    display(display),
    widget(new AudioSpectrumWidget(display, q))
{
}

//-------------------------------------------------------------------------------------------------
// AudioSpectrumOverlayWidget

AudioSpectrumOverlayWidget::AudioSpectrumOverlayWidget(
    QnResourceDisplayPtr display,
    QGraphicsWidget* parent)
    :
    base_type(parent),
    d(new Private(display, this))
{
    NX_ASSERT(display);

    // Don't interfere with event handling.
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    setFocusPolicy(Qt::NoFocus);

    // Init layout.
    // We set margins here so that the audio control isn't placed on top of other item buttons.
    QGraphicsGridLayout* layout = new QGraphicsGridLayout();
    layout->setContentsMargins(96, 96, 96, 96);
    layout->setSpacing(0);
    layout->addItem(d->widget, 1, 1);
    layout->setColumnStretchFactor(0, 1);
    layout->setColumnStretchFactor(2, 1);
    layout->setRowStretchFactor(0, 1);
    layout->setRowStretchFactor(2, 1);
    setLayout(layout);
}

AudioSpectrumOverlayWidget::~AudioSpectrumOverlayWidget()
{
}

AudioSpectrumWidget* AudioSpectrumOverlayWidget::audioSpectrumWidget() const {
    return d->widget;
}

} // namespace nx::vms::client::desktop
