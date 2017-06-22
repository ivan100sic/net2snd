#include "sound_playback_device.hpp"
#include "sound_recording_device.hpp"
#include "bytes.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include <string>
using namespace std;

const int CLOCK_SIG_FREQ = 64;
const unsigned int BIT_WIDTH = 400;

vector<uint32_t> prepare_clock() {
	vector<uint32_t> buffer(8 * BIT_WIDTH);

	size_t buff_i = 0;
	
	for (int u=0; u<8; u++) {
		for (size_t j=0; j<BIT_WIDTH; j++) {
			double x = sin(TAU * j / CLOCK_SIG_FREQ);
			double pos = j * 1.0 / BIT_WIDTH;
			if (0.42 < pos && pos < 0.92) {
				x = 0;
			}
			double scale = 3270;
			buffer[j + buff_i] = i16_to_u32(scale * x);
		}
		buff_i += BIT_WIDTH;
	}
	
	return buffer;
}

vector<uint32_t> bytes_to_sound(string s) {
	vector<uint32_t> buffer(s.size() * BIT_WIDTH);

	size_t buff_i = 0;
	
	for (char x : s) {
		uint8_t b = x;
		vector<int> freqs = {CLOCK_SIG_FREQ, 38, 31, 24, 20, 16, 13, 10, 8, 6};
		vector<double> pows;
		for (int x : freqs) pows.push_back(x);
		pows[1] *= 4;
		for (size_t j=0; j<BIT_WIDTH; j++) {
			double x = pows[0] * sin(TAU * j / freqs[0]);
			double pos = j * 1.0 / BIT_WIDTH;
			if (0.42 < pos && pos < 0.92) {
				x = 0;
			}
			for (int i=0; i<8; i++) {
				if (b & (1 << i)) {
					x += sin(TAU * j / freqs[1+i]) * pows[1+i];
				}
			}
			x += sin(TAU * j / freqs[9]) * pows[9];
			double scale = 1 * 32766 / accumulate(pows.begin(), pows.end(), 0.0);
			buffer[j + buff_i] = i16_to_u32(scale * x);
		}
		buff_i += BIT_WIDTH;
	}
	
	return buffer;
}

int transmit() {
	// cout << "Enter message to transmit: ";
	string s;
	// getline(cin, s);
	auto a = {0x0, 0x1, 0x2, 0x4, 0x8, 0x0, 0x10, 0x20, 0x40, 0x80, 0xff, 0xff};
	for (auto x : a) {
		s.push_back(x);
	}
	s += "\nIvan Stosic";

	s = "This article is about the harmonic oscillator in classical mechanics.\n";

	// s.clear();
	// for (int i=0; i<8; i++) s.push_back(1 << i);

	s = "\xff\xff\xff\xff";

	auto buff = bytes_to_sound(s);
	auto pc = prepare_clock();

	auto spb = SoundPlaybackDevice::get_default();
	if (!spb) return 1;

	spb.write(vector<uint32_t>(1024, 0));
	spb.write(vector<uint32_t>(1024, 0));
	spb.write(pc);

	while (1) {
		spb.write(buff);
	}
	spb.flush();
	return 0;
}

int main() {
	return transmit();
}