// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "audio_spectrum_widget.h"

#include <QtCore/QElapsedTimer>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsLinearLayout>

#include <utils/media/voice_spectrum_analyzer.h>
#include <camera/cam_display.h>
#include <camera/resource_display.h>
#include <nx/vms/client/core/skin/color_theme.h>
#include <nx/vms/client/core/skin/skin.h>
#include <nx/vms/client/core/utils/geometry.h>
#include <ui/graphics/items/resource/voice_spectrum_painter.h>
#include <ui/graphics/items/generic/image_button_widget.h>

namespace nx::vms::client::desktop {

constexpr QSize kButtonSize(48 * 3, 48 * 3);

constexpr QMargins kButtonInternalMargins(48, 48, 48, 48);

constexpr int kButtonSpectrumSpacing = 2;

constexpr QSize kSpectrumSize(kButtonSize.width() * 3, kButtonSize.height());

constexpr QSize kHalfSpectrumSize(kSpectrumSize.width() / 2, kSpectrumSize.height());

constexpr QMargins kSpectrumInternalMargins(48, 24, 48, 24);

//-------------------------------------------------------------------------------------------------
// AudioSpectrumOverlayWidget::Private

class AudioSpectrumWidget::Private
{
    AudioSpectrumWidget* const q = nullptr;

public:
    Private(QnResourceDisplayPtr display, AudioSpectrumWidget* q);

public:
    QnResourceDisplayPtr display;
    QnImageButtonWidget* const button;
    GraphicsWidget* const leftSpacer;
    GraphicsWidget* const rightSpacer;
    QColor backgroundDefault;
    QColor backgroundHovered;
    QColor backgroundPressed;
    VoiceSpectrumPainter painter;
    QElapsedTimer timer;
};

AudioSpectrumWidget::Private::Private(
    QnResourceDisplayPtr display,
    AudioSpectrumWidget* q)
    :
    q(q),
    display(display),
    button(new QnImageButtonWidget(q)),
    leftSpacer(new GraphicsWidget(q)),
    rightSpacer(new GraphicsWidget(q)),
    backgroundDefault(core::colorTheme()->color("camera.audioOnly.background.default")),
    backgroundHovered(core::colorTheme()->color("camera.audioOnly.background.hovered")),
    backgroundPressed(core::colorTheme()->color("camera.audioOnly.background.pressed"))
{
}

//-------------------------------------------------------------------------------------------------
// AreaSelectOverlayWidget

AudioSpectrumWidget::AudioSpectrumWidget(
    QnResourceDisplayPtr display,
    QGraphicsWidget* parent)
    :
    base_type(parent),
    d(new Private(display, this))
{
    NX_ASSERT(display);

    // Init layout.
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    layout->addItem(d->leftSpacer);
    layout->addItem(d->button);
    layout->addItem(d->rightSpacer);
    layout->setSpacing(kButtonSpectrumSpacing);
    setLayout(layout);

    // Init button & spacer.
    d->button->setIcon(qnSkin->icon("item/audio_on.svg", "item/audio_off.svg"));
    d->button->setCheckable(true);
    d->button->setFixedSize(kButtonSize);
    d->button->setImageMargins(kButtonInternalMargins);

    // Set up a layout update function.
    auto updateLayout = [this, layout]
    {
        if (isMuted())
        {
            d->leftSpacer->setMinimumSize(kHalfSpectrumSize);
            d->leftSpacer->setMaximumSize(kHalfSpectrumSize);
            d->rightSpacer->setMinimumSize(kHalfSpectrumSize);
            d->rightSpacer->setMaximumSize(kHalfSpectrumSize);
        }
        else
        {
            d->leftSpacer->setMinimumSize(QSize());
            d->leftSpacer->setMaximumSize(QSize());
            d->rightSpacer->setMinimumSize(kSpectrumSize);
            d->rightSpacer->setMaximumSize(kSpectrumSize);
        }
    };
    updateLayout();

    // Set up event handling. Only the button accepts mouse clicks.
    setAcceptedMouseButtons(Qt::NoButton);
    d->leftSpacer->setAcceptedMouseButtons(Qt::NoButton);
    d->rightSpacer->setAcceptedMouseButtons(Qt::NoButton);

    // Set up visualizer.
    d->painter.options.color = core::colorTheme()->color("camera.audioOnly.visualizer");
    d->timer.start();

    // Set up signal handlers.
    connect(d->button, &QnImageButtonWidget::toggled, this, updateLayout);
    connect(d->button, &QnImageButtonWidget::toggled, this, &AudioSpectrumWidget::mutedChanged);
}

AudioSpectrumWidget::~AudioSpectrumWidget()
{
}

bool AudioSpectrumWidget::isMuted() const {
    return d->button->isChecked();
}

void AudioSpectrumWidget::setMuted(bool muted) {
    d->button->setChecked(muted);
}

void AudioSpectrumWidget::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    QColor buttonColor = d->button->isPressed() ? d->backgroundPressed :
                         d->button->isHovered() ? d->backgroundHovered : d->backgroundDefault;

    if (isMuted())
    {
        // Muted - draw a circle and that's it.
        QPainterPath path;
        path.addEllipse(d->button->geometry());
        painter->fillPath(path, buttonColor);
    }
    else
    {
        // Prepare for arc drawing.
        QRectF leftRect = d->button->geometry();
        QRectF rightRect = core::Geometry::aligned(leftRect.size(), d->rightSpacer->geometry(),
                                                   Qt::AlignRight);

        // Draw button background.
        QRectF buttonRect = d->button->geometry();
        QPainterPath path;
        path.moveTo((leftRect.topLeft() + leftRect.topRight()) / 2);
        path.lineTo(buttonRect.topRight());
        path.lineTo(buttonRect.bottomRight());
        path.lineTo((leftRect.bottomLeft() + leftRect.bottomLeft()) / 2);
        path.arcTo(leftRect, 270, -180);
        painter->fillPath(path, buttonColor);

        // Draw spectrum background.
        QRectF spacerRect = d->rightSpacer->geometry();
        path.clear();
        path.moveTo((rightRect.bottomLeft() + rightRect.bottomRight()) / 2);
        path.lineTo(spacerRect.bottomLeft());
        path.lineTo(spacerRect.topLeft());
        path.lineTo((rightRect.topLeft() + rightRect.topRight()) / 2);
        path.arcTo(rightRect, 90, -180);
        painter->fillPath(path, d->backgroundDefault);

        // Draw spectrum.
        QRectF spectrumRect = core::Geometry::eroded(spacerRect, kSpectrumInternalMargins);
        d->painter.update(d->timer.elapsed(), d->display->camDisplay()->audioSpectrum().data);
        d->painter.options.visualizerLineOffset = spectrumRect.width() / 30;
        d->painter.paint(painter, spectrumRect);
    }
}

} // namespace nx::vms::client::desktop
