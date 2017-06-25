#include "sound_playback_device.hpp"
#include "sound_recording_device.hpp"
#include "bytes.hpp"

#include <vector>

const int CLOCK_SIG_FREQ_ON = 60;
const int CLOCK_SIG_FREQ_OFF = 64;

const int TRANSMIT_ON = 48;
const int TRANSMIT_OFF = 46;
const unsigned int BIT_WIDTH = 1500;

std::vector<double> freqs = {
	39, 38,
	35, 34,
	31, 30,
	27, 26,
	23, 22,
	19, 18,
	15, 14,
	11, 10
};