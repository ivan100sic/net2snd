#include "sound_playback_device.hpp"
#include "sound_recording_device.hpp"
#include "bytes.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include <string>
using namespace std;

vector<uint32_t> to_sound() {
	
	int pulses = 6;
	int spacing = 100;

	vector<uint32_t> buffer(2 * pulses * spacing, 0);
	for (int i=0; i<pulses; i++) {
		buffer[i*spacing] = 0x7fff7fff;
	}
	
	return buffer;
}

int transmit() {

	auto buff = to_sound();

	auto spb = SoundPlaybackDevice::get_default();
	if (!spb) return 1;

	spb.write(vector<uint32_t>(1024, 0));
	spb.write(vector<uint32_t>(1024, 0));
	spb.write(vector<uint32_t>(1024, 0));

	while (1) {
		spb.write(buff);
	}
	spb.flush();
	return 0;
}

int main() {
	return transmit();
}