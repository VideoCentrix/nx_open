#include "voice_spectrum_painter.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include <ui/workaround/sharp_pixmap_painting.h>

#include <utils/media/voice_spectrum_analyzer.h>

QnVoiceSpectrumPainter::QnVoiceSpectrumPainter() = default;
QnVoiceSpectrumPainter::~QnVoiceSpectrumPainter() = default;

void QnVoiceSpectrumPainter::update(qint64 timeMs, const Data& data)
{
    const auto timeStepMs = timeMs - m_oldTimeMs;
    m_oldTimeMs = timeMs;

    if (data.isEmpty())
    {
        m_data = generateEmptyData(timeMs, QnVoiceSpectrumAnalyzer::bandsCount());
    }
    else
    {
        Data normalized = data;
        normalizeData(normalized);

        m_data = animateData(m_data, normalized, timeStepMs);
    }
}

void QnVoiceSpectrumPainter::paint(QPainter* painter, const QRectF& rect)
{
    if (m_data.isEmpty())
        return;

    const qreal lineWidth = qRound(qMax(m_options.visualizerLineWidth,
        (rect.width() / m_data.size()) - m_options.visualizerLineOffset));

    const qreal midY = rect.center().y();
    const qreal maxHeight = rect.height() * m_options.maxVisualizerHeightCoeff;

    QPainterPath path;
    for (int i = 0; i < m_data.size(); ++i)
    {
        const qreal lineHeight = qRound(qMax(maxHeight * m_data[i], m_options.visualizerLineOffset * 2));
        path.addRect(qRound(rect.left() + i * (lineWidth + m_options.visualizerLineOffset)),
            qRound(midY - (lineHeight / 2)), lineWidth, lineHeight);
    }

    paintSharp(painter,
        [&](QPainter* painter) { painter->fillPath(path, m_options.visualizerColor); });
}

void QnVoiceSpectrumPainter::reset() {
    m_data = Data();
}

void QnVoiceSpectrumPainter::normalizeData(Data& source) {
    if (source.isEmpty())
        return;

    const auto max = std::max_element(source.cbegin(), source.cend());

    // Do not normalize if silence.
    if (*max < m_options.normalizerSilenceValue)
        return;

    // Do not normalize if there is bigger value, so normalizing will always only increase values.
    if (*max > m_options.normalizerIncreaseValue)
        return;

    const auto k = m_options.normalizerIncreaseValue / *max;
    for (auto& e: source)
        e *= k;
}

QnVoiceSpectrumPainter::Data QnVoiceSpectrumPainter::animateData(const Data& prev, const Data& next, qint64 timeStepMs)
{
    //NX_ASSERT(next.size() == QnVoiceSpectrumAnalyzer::bandsCount());

    if (prev.size() != next.size())
        return next;

    const qreal maxUpChange = qBound(0.0, m_options.visualizerAnimationUpSpeed * timeStepMs / 1000, 1.0);
    const qreal maxDownChange = qBound(0.0, m_options.visualizerAnimationDownSpeed * timeStepMs / 1000, 1.0);

    Data result(prev.size());
    for (int i = 0; i < prev.size(); ++i)
    {
        const auto current = prev[i];
        const auto target = next[i];
        auto change = target - current;
        if (change > 0)
            change = qMin(change, maxUpChange);
        else
            change = qMax(change, -maxDownChange);
        result[i] = qBound(0.0, current + change, 1.0);
    }

    return result;
}

QnVoiceSpectrumPainter::Data QnVoiceSpectrumPainter::generateEmptyData(qint64 elapsedMs, int bandsCount)
{
    // Making slider move forth and back...
    const int size = bandsCount;
    const int maxIdx = size * 2 - 1;

    Data result(bandsCount, 0.0);
    int idx = qRound(16.0 * elapsedMs / 1000) % maxIdx;
    if (idx >= size)
        idx = maxIdx - idx;

    const bool isValidIndex = idx >= 0 && idx < result.size();
    NX_ASSERT(isValidIndex, "Invalid timeStep value");
    if (isValidIndex)
        result[idx] = 0.2;

    return result;
}
