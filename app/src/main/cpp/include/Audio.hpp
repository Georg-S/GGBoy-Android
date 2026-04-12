#pragma once
#include <Emulator.hpp>
#include <aaudio/AAudio.h>
#include <atomic>

#include "EmulatorMessage.hpp"

class Audio 
{
public:
	explicit Audio(ggb::SampleBuffer* sampleBuffer, EmulatorMessageHandler* messageHandler);
	~Audio();
	void setAudioPlaying(bool value);
	bool audioPlaying() const;
    void setVolume(int volume);
	
private:
	struct AudioData 
	{
		ggb::SampleBuffer* sampleBuffer = nullptr;
		ggb::Frame lastReadFrame = {};
        std::atomic<int> m_volume = 15;
	};

	bool initializeAudio(ggb::SampleBuffer* sampleBuffer);
	static aaudio_data_callback_result_t emulatorAudioCallback(AAudioStream* stream, void* userData,
                                                               void* audioData, int32_t numFrames);
	AudioData m_data = {};
	bool m_audioPlaying = false;
    AAudioStream* m_stream = nullptr;
    EmulatorMessageHandler* m_messageHandler = nullptr;
};