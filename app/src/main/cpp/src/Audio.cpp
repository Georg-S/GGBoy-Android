#include "Audio.hpp"

static constexpr int CHANNEL_COUNT = 2;
static constexpr int FRAME_COUNT = 256;
static constexpr int volume = 15;

Audio::Audio(ggb::SampleBuffer *sampleBuffer)
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
            ;// TODO handle error reporting
    }

    if (value & !m_audioPlaying)
    {
        if (AAudioStream_requestStart(m_stream) == AAUDIO_OK)
            m_audioPlaying = true;
        else
            ;// TODO handle error reporting
    }
}

bool Audio::audioPlaying() const
{
    return m_audioPlaying;
}

bool Audio::initializeAudio(ggb::SampleBuffer *sampleBuffer)
{
    m_data.sampleBuffer = sampleBuffer;

    AAudioStreamBuilder *builder = nullptr;
    auto result = AAudio_createStreamBuilder(&builder);
    if (result != AAUDIO_OK)
    {
        // TODO handle error
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
        // TODO handle error
        AAudioStreamBuilder_delete(builder);
        return false;
    }

    if (AAudioStream_requestStart(m_stream) != AAUDIO_OK)
    {
        // TODO handle error
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
	auto audioData = static_cast<AudioData*>(userData);
	auto audioStream = reinterpret_cast<ggb::AUDIO_FORMAT*>(outStream);

	for (size_t sid = 0; sid < numFrames; ++sid)
	{
		// As a default use the last read value, this prevents audio pops
		audioData->lastReadFrame = audioData->sampleBuffer->pop(audioData->lastReadFrame);

		audioStream[2 * sid + 0] = audioData->lastReadFrame.leftSample * volume; /* L */
		audioStream[2 * sid + 1] = audioData->lastReadFrame.rightSample * volume; /* R */
	}

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}
