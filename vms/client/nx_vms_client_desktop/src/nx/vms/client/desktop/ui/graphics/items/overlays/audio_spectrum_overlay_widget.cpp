// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "audio_spectrum_overlay_widget.h"

#include <QtCore/QElapsedTimer>

#include <utils/media/voice_spectrum_analyzer.h>
#include <camera/cam_display.h>
#include <camera/resource_display.h>
#include <nx/vms/client/core/skin/color_theme.h>
#include <ui/graphics/items/resource/voice_spectrum_painter.h>
#include <ui/graphics/items/resource/media_resource_widget.h>

#include "nx/vms/client/core/utils/geometry.h"

namespace nx::vms::client::desktop {

//-------------------------------------------------------------------------------------------------
// AudioSpectrumOverlayWidget::Private

class AudioSpectrumOverlayWidget::Private
{
    AudioSpectrumOverlayWidget* const q = nullptr;

public:
    Private(QnResourceDisplayPtr display, AudioSpectrumOverlayWidget* q);

public:
    QnResourceDisplayPtr display;
    VoiceSpectrumPainter painter;
    QElapsedTimer timer;
};

AudioSpectrumOverlayWidget::Private::Private(
    QnResourceDisplayPtr display,
    AudioSpectrumOverlayWidget* q)
    :
    q(q),
    display(display)
{
    painter.options.color = nx::vms::client::core::colorTheme()->color("camera.visualizer");

    timer.start();
}

//-------------------------------------------------------------------------------------------------
// AreaSelectOverlayWidget

AudioSpectrumOverlayWidget::AudioSpectrumOverlayWidget(
    QnResourceDisplayPtr display,
    QGraphicsWidget* parent)
    :
    base_type(parent),
    d(new Private(display, this))
{
    NX_ASSERT(display);
}

AudioSpectrumOverlayWidget::~AudioSpectrumOverlayWidget()
{
}

void AudioSpectrumOverlayWidget::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    d->painter.update(d->timer.elapsed(), d->display->camDisplay()->audioSpectrum().data);
    d->painter.options.visualizerLineOffset = rect().width() / 80;

    QRectF targetRect = rect().adjusted(rect().width() / 4, rect().height() / 4, -rect().width() / 4, -rect().height() / 4);
    d->painter.paint(painter, targetRect);
}

} // namespace nx::vms::client::desktop
