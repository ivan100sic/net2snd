#include "sound_playback_device.hpp"
#include "sound_recording_device.hpp"
#include "bytes.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include <string>
using namespace std;

vector<sample> sample_buffer() {
	const int n = 100;

	vector<sample> buff(n, 0);
	for (size_t i=0; i<n; i+=2) {
		int ch1 = int(sin(TAU * i / n) * 32767);
		unsigned short int x = ch1;
		unsigned int y = x;
		buff[i] = y + (y << 16);
	}
	return buff;
}

int test_playback() {
	auto spb = SoundPlaybackDevice::get_default();
	if (!spb) return 1;
	while (1) {
		spb.write(sample_buffer());
	}
	return 0;
}

int test_recording() {
	auto callback = [](vector<sample>&& v) {
		double tot = 0;
		for (auto sample : v) {
			tot += abs((short int)(sample & 65535));
		}
		double sndlvl = log(tot / v.size() / 32768) / log(10.) * 10;
		int rep = 60 + 2 * sndlvl;
		while (rep--) cerr << '#';
		cerr << '\n';
		// cerr << sndlvl << " dB\n";
	};

	auto srd = SoundRecordingDevice<decltype(callback)>::get_default(callback);
	if (!srd) {
		cerr << "failed\n";
		return 1;
	}

	srd.start();
	srd.join();
	return 0;
}

int main() {
	return test_recording();
}