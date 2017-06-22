#ifndef MYSOUNDLIB_SOUND_RECORDING_DEVICE
#define MYSOUNDLIB_SOUND_RECORDING_DEVICE

// Creates an object which encapsulates a snd_pcm_t handle
#include <alsa/asoundlib.h>
#include <vector>
#include <thread>
#include <iostream>
using namespace std;

typedef uint32_t sample;

template<class T>
struct SoundRecordingDevice {
	snd_pcm_t* handle;
	T callback;
	thread thrd;

	explicit SoundRecordingDevice(T callback) : callback(callback) {}

	static SoundRecordingDevice get_default(T callback) {
		// Options
		unsigned int sample_rate = 44100;
		snd_pcm_hw_params_t* params;

		SoundRecordingDevice srd(callback);

		// default uredjaj je "hw:0,0"
		snd_pcm_open(&srd.handle, "hw:0,0", SND_PCM_STREAM_CAPTURE, 0); // blocking pozivi
		snd_pcm_hw_params_malloc(&params);
		snd_pcm_hw_params_any(srd.handle, params);
		snd_pcm_hw_params_set_access(srd.handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(srd.handle, params, SND_PCM_FORMAT_S16_LE);
		
		snd_pcm_hw_params_set_rate_near(srd.handle, params, &sample_rate, nullptr);

		snd_pcm_hw_params_set_channels(srd.handle, params, 2);
		snd_pcm_hw_params(srd.handle, params);
		snd_pcm_hw_params_free(params);

		return srd;
	}

	operator bool () const {
		return handle != nullptr;
	}

	static void run (SoundRecordingDevice* obj, T callback) {
		int DEFAULT_BUFFER_SIZE = 128;
		vector<uint32_t> buf;
		while (1) {
			buf.resize(DEFAULT_BUFFER_SIZE);
			snd_pcm_readi(obj->handle, buf.data(), DEFAULT_BUFFER_SIZE);
			callback(move(buf));
		}
	}

	void start() {
		if (handle) {
			thrd = thread(run, this, callback);
		}
	}

	void join() {
		if (thrd.joinable()) {
			thrd.join();
		}
	}
};

#endif // MYSOUNDLIB_SOUND_RECORDING_DEVICE