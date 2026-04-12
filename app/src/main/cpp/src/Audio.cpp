#include "Audio.hpp"

static constexpr int CHANNEL_COUNT = 2;
static constexpr int FRAME_COUNT = 256;

Audio::Audio(ggb::SampleBuffer *sampleBuffer, EmulatorMessageHandler *messageHandler)
        : m_messageHandler(messageHandler)
{
    initializeAudio(sampleBuffer);
}

Audio::~Audio()
{
    if (m_stream)
        AAudioStream_close(m_stream);
}

void Audio::setAudioPlaying(bool value)
{
    if (!value && m_audioPlaying)
    {
        if (AAudioStream_requestPause(m_stream) == AAUDIO_OK)
            m_audioPlaying = false;
        else
            m_messageHandler->addMessage(EmulatorMessage::Error, "Error stopping audio");
    }

    if (value & !m_audioPlaying)
    {
        if (AAudioStream_requestStart(m_stream) == AAUDIO_OK)
            m_audioPlaying = true;
        else
            m_messageHandler->addMessage(EmulatorMessage::Error, "Error starting audio");
    }
}

bool Audio::audioPlaying() const
{
    return m_audioPlaying;
}

void Audio::setVolume(int volume)
{
    m_data.m_volume = std::clamp(volume, 0, 100);
}

bool Audio::initializeAudio(ggb::SampleBuffer *sampleBuffer)
{
    m_data.sampleBuffer = sampleBuffer;

    AAudioStreamBuilder *builder = nullptr;
    auto result = AAudio_createStreamBuilder(&builder);
    if (result != AAUDIO_OK)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error initializing audio");
        return false;
    }

    AAudioStreamBuilder_setSampleRate(builder, ggb::STANDARD_SAMPLE_RATE);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setChannelCount(builder, CHANNEL_COUNT);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, FRAME_COUNT);
    AAudioStreamBuilder_setDataCallback(builder, Audio::emulatorAudioCallback, &m_data);
//    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);

    if (AAudioStreamBuilder_openStream(builder, &m_stream) != AAUDIO_OK)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error opening audio stream");
        AAudioStreamBuilder_delete(builder);
        return false;
    }

    if (AAudioStream_requestStart(m_stream) != AAUDIO_OK)
    {
        m_messageHandler->addMessage(EmulatorMessage::Error, "Error starting audio");
        AAudioStreamBuilder_delete(builder);
        return false;
    }
    m_audioPlaying = true;

    AAudioStreamBuilder_delete(builder);

    return true;
}

aaudio_data_callback_result_t Audio::emulatorAudioCallback(AAudioStream *stream, void *userData,
                                                           void *outStream, int32_t numFrames)
{
    static_assert(std::atomic<int>::is_always_lock_free, "Atomic int must be lock-free");
    auto audioData = static_cast<AudioData *>(userData);
    auto audioStream = reinterpret_cast<ggb::AUDIO_FORMAT *>(outStream);

    for (size_t sid = 0; sid < numFrames; ++sid)
    {
        // As a default use the last read value, this prevents audio pops
        audioData->lastReadFrame = audioData->sampleBuffer->pop(audioData->lastReadFrame);

        audioStream[2 * sid + 0] = audioData->lastReadFrame.leftSample * audioData->m_volume; /* L */
        audioStream[2 * sid + 1] = audioData->lastReadFrame.rightSample * audioData->m_volume;
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}
