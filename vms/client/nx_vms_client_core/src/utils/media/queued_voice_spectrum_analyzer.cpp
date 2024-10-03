// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "queued_voice_spectrum_analyzer.h"

template<class T>
static void clear(std::queue<T> &queue)
{
    while (!queue.empty())
        queue.pop();
}

QnQueuedVoiceSpectrumAnalyzer::QnQueuedVoiceSpectrumAnalyzer():
    m_baseAnalyzer(std::make_unique<QnVoiceSpectrumAnalyzer>())
{
}

QnQueuedVoiceSpectrumAnalyzer::~QnQueuedVoiceSpectrumAnalyzer() = default;

void QnQueuedVoiceSpectrumAnalyzer::initialize(int srcSampleRate, int channels)
{
    m_baseAnalyzer->initialize(srcSampleRate, channels);
}

void QnQueuedVoiceSpectrumAnalyzer::pushData(qint64 timestampUsec, const nx::media::audio::Format& format,
                                             const void* sampleData, int sampleBytes,
                                             qint64 maxQueueSizeUsec)
{
    if (!m_baseAnalyzer->processData(format, sampleData, sampleBytes))
        return; // Wasn't updated.
    QnSpectrumData data = m_baseAnalyzer->getSpectrumData();

    NX_MUTEX_LOCKER lock(&m_mutex);

    // Moved back in time => discard queue.
    if (!m_queue.empty() && m_queue.back().first > timestampUsec)
        clear(m_queue);

    // Prevent overflows.
    while (!m_queue.empty() && m_queue.back().first - m_queue.front().first > maxQueueSizeUsec)
        m_queue.pop();

    m_queue.push({timestampUsec, std::move(data)});
}

QnSpectrumData QnQueuedVoiceSpectrumAnalyzer::readSpectrumData(qint64 timestampUsec)
{
    NX_MUTEX_LOCKER lock(&m_mutex);

    while (m_queue.size() > 1 && m_queue.front().first < timestampUsec)
        m_queue.pop();

    return m_queue.empty() ? QnSpectrumData() : m_queue.front().second;
}

void QnQueuedVoiceSpectrumAnalyzer::reset()
{
    m_baseAnalyzer->reset();

    NX_MUTEX_LOCKER lock(&m_mutex);
    clear(m_queue);
}
