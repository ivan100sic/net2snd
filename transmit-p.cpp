#include "common-p.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include <string>
using namespace std;

vector<double> bit_j(8, 0.0);
double transmit_j = 0;
double clock_j = 0;

vector<uint32_t> prepare_clock() {
	size_t warmup_len = 32;
	vector<uint32_t> buffer(warmup_len * BIT_WIDTH);

	size_t buff_i = 0;

	for (size_t u=0; u<warmup_len; u++) {
		for (size_t j=0; j<BIT_WIDTH; j++) {
			double pos = j * 1.0 / BIT_WIDTH;
			double x = 0;
			if (0.42 < pos && pos < 0.92) {
				x = sin(clock_j += TAU / CLOCK_SIG_FREQ_OFF);
			} else {
				x = sin(clock_j += TAU / CLOCK_SIG_FREQ_ON);
			}
			x += sin(transmit_j += TAU / TRANSMIT_OFF);
			double scale = 4000;
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
		
		for (size_t j=0; j<BIT_WIDTH; j++) {
			double x = 0;
			double pos = j * 1.0 / BIT_WIDTH;
			if (0.42 < pos && pos < 0.92) {
				x = sin(clock_j += TAU / CLOCK_SIG_FREQ_OFF);
			} else {
				x = sin(clock_j += TAU / CLOCK_SIG_FREQ_ON);
			}
			for (int i=0; i<8; i++) {
				int fid = 2*i;
				if (b & (1 << i)) {
					x += sin(bit_j[i] += TAU / freqs[fid + 1]);
				} else {
					x += sin(bit_j[i] += TAU / freqs[fid]);
				}
			}
			x += sin(transmit_j += TAU / TRANSMIT_ON);
			double scale = 1200.0;
			buffer[j + buff_i] = i16_to_u32(scale * x);
		}
		buff_i += BIT_WIDTH;
	}

	for (double& x : bit_j) {
		tau(x);
	}
	tau(transmit_j);
	tau(clock_j);

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

	// s = "\xff\xff\xff\xff";

	auto pc = prepare_clock();

	auto spb = SoundPlaybackDevice::get_default();
	if (!spb) return 1;

	spb.write(vector<uint32_t>(1024, 0));
	spb.write(vector<uint32_t>(1024, 0));
	spb.write(pc);

	while (1) {
		auto bts = bytes_to_sound(s);
		spb.write(bts);
	}
	
	spb.flush();
	return 0;
}

int main() {
	return transmit();
}