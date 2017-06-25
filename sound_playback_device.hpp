#ifndef MYSOUNDLIB_SOUND_PLAYBACK_DEVICE
#define MYSOUNDLIB_SOUND_PLAYBACK_DEVICE

// Creates an object which encapsulates a snd_pcm_t handle
#include <alsa/asoundlib.h>
#include <vector>
#include <iostream>
using namespace std;

struct SoundPlaybackDevice {
	snd_pcm_t* handle;

	static SoundPlaybackDevice get_default() {
		// Options
		unsigned int sample_rate = 44100;
		int DEFAULT_BUFFER_SIZE = 16384;
		snd_pcm_hw_params_t* params;

		SoundPlaybackDevice spd;

		int err = snd_pcm_open(&spd.handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
		if (err < 0) {
			spd.handle = nullptr;
			return spd;
		}

		snd_pcm_hw_params_malloc(&params);
		snd_pcm_hw_params_any(spd.handle, params);
		snd_pcm_hw_params_set_access(spd.handle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(spd.handle, params,
			SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate_near(spd.handle, params,
			&sample_rate, nullptr);
		snd_pcm_hw_params_set_channels(spd.handle, params, 2);
		snd_pcm_hw_params_set_buffer_size(spd.handle, params,
			DEFAULT_BUFFER_SIZE);

		snd_pcm_hw_params(spd.handle, params);
		snd_pcm_hw_params_free(params);

		snd_pcm_prepare(spd.handle);
		snd_pcm_reset(spd.handle);

		return spd;
	}

	operator bool () const {
		return handle != nullptr;
	}

	void write(const vector<unsigned int>& buff) const {
		if (handle) {
			int err = snd_pcm_writei(handle, buff.data(), buff.size());
			if (err < 0) {
				cerr << "Recovering " << err << '\n';
				snd_pcm_recover(handle, err, 1);
			}
		}
	}

	void flush() const {
		snd_pcm_drain(handle);
	}

	~SoundPlaybackDevice() {
		if (handle) {
			snd_pcm_close(handle);
		}
	}
};

#endif // MYSOUNDLIB_SOUND_PLAYBACK_DEVICE