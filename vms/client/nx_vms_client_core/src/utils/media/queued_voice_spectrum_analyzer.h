// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <memory>
#include <queue>
#include <utility>

#include <QtCore/QObject>

#include <nx/media/audio/format.h>
#include <nx/utils/thread/mutex.h>

#include "voice_spectrum_analyzer.h"

class NX_VMS_CLIENT_CORE_API QnQueuedVoiceSpectrumAnalyzer:
    public QObject
{
    Q_OBJECT

public:
    QnQueuedVoiceSpectrumAnalyzer();
    virtual ~QnQueuedVoiceSpectrumAnalyzer();

    void initialize(int srcSampleRate, int channels);

    void pushData(qint64 timestampUsec, const nx::media::audio::Format& format, const void* sampleData,
                  int sampleBytes, qint64 maxQueueSizeUsec);

    QnSpectrumData readSpectrumData(qint64 timestampUsec);

    void reset();

private:
    std::unique_ptr<QnVoiceSpectrumAnalyzer> m_baseAnalyzer;
    std::queue<std::pair<qint64, QnSpectrumData>> m_queue; // (timestamp, data) pairs.
    mutable nx::Mutex m_mutex;
};
