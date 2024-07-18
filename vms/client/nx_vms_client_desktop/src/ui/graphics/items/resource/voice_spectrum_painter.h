#pragma once

#include <QtCore/QVector>
#include <QtGui/QColor>

class QPainter;

namespace nx::vms::client::desktop {

struct VoiceSpectrumPainterOptions {
    /** Max visualizer value change per second (on increasing and decreasing values). */
    qreal visualizerAnimationUpSpeed = 5.0;
    qreal visualizerAnimationDownSpeed = 1.0;

    /** Maximum visualizer height. */
    qreal maxVisualizerHeightCoeff = 0.9;

    /** Values lower than that value will be counted as silence. */
    qreal normalizerSilenceValue = 0.1;

    /** Maximum value, to which all values are normalized. */
    qreal normalizerIncreaseValue = 0.9;

    /** Recommended visualizer line width. */
    qreal visualizerLineWidth = 4.0;

    /** Visualizer offset between lines. */
    qreal visualizerLineOffset = 2.0;

    /** Color to use for audio spectrum painting. */
    QColor color;
};

class VoiceSpectrumPainter {
public:
    using Data = QVector<double>;

    VoiceSpectrumPainter();
    virtual ~VoiceSpectrumPainter();

    const VoiceSpectrumPainterOptions& options() const
    {
        return m_options;
    }

    void setOptions(const VoiceSpectrumPainterOptions& options)
    {
        m_options = options;
    }

    void update(qint64 timeMs, const Data& data);

    void paint(QPainter* painter, const QRectF& rect);

    void reset();

private:
    void normalizeData(Data& source);
    Data animateData(const Data& prev, const Data& next, qint64 timeStepMs);
    static Data generateEmptyData(qint64 elapsedMs, int bandsCount);

private:
    VoiceSpectrumPainterOptions m_options;
    Data m_data;
    qint64 m_oldTimeMs = 0;
};

} // namespace nx::vms::client::desktop


