#pragma once
#include <Emulator.hpp>
#include <aaudio/AAudio.h>

class Audio 
{
public:
	explicit Audio(ggb::SampleBuffer* sampleBuffer);
	~Audio();
	void setAudioPlaying(bool value);
	bool audioPlaying() const;
	
private:
	struct AudioData 
	{
		ggb::SampleBuffer* sampleBuffer = nullptr;
		ggb::Frame lastReadFrame = {};
	};

	bool initializeAudio(ggb::SampleBuffer* sampleBuffer);
	static aaudio_data_callback_result_t emulatorAudioCallback(AAudioStream* stream, void* userData,
                                                               void* audioData, int32_t numFrames);
	AudioData m_data = {};
	bool m_audioPlaying = false;
    AAudioStream* m_stream = nullptr;
};