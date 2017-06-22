#include "sound_playback_device.hpp"
#include "sound_recording_device.hpp"
#include "bytes.hpp"
#include "oscillator.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
using namespace std;

int test_recording() {

	// 1 / (zeta * om0) = bit_width
	// 1 / zeta = bit_width * om0
	// zeta = 1 / (bit_width * om0)
	// zeta = freq / (TAU * bit_width)

	const int BIT_WIDTH = 1000;

	double DAMP = 0.01;
	double TUNE = sqrt(1 - 2 * DAMP * DAMP);

	Oscillator clocc_sig(TUNE * TAU / 64, DAMP);
	Oscillator clocc(TUNE * TAU / BIT_WIDTH, DAMP);
	Oscillator bits[9];

	double ENERGY = 0.2;

	vector<int> freqs = {38, 31, 24, 20, 16, 13, 10, 8, 6};
	vector<double> ENERGIES = {0.25, 1, 1, 1, 1, 1, 1, 1, 1};
	for (int i=0; i<9; i++) {
		bits[i] = Oscillator(TAU / freqs[i], freqs[i] / TAU / BIT_WIDTH);
		bits[i].autotune();
	}

	auto callback = [&]	(vector<sample>&& v) {

		for (sample x : v) {
			double f = u32_to_i16(x) / 32768.;
			clocc_sig.step(f);
			for (int i=0; i<9; i++) bits[i].step(f);

			double old_x = clocc.x;
			clocc.step(clocc_sig.get_energy() / 100.0);
			double new_x = clocc.x;

			cerr << "e8 " << bits[8].get_energy() << '\n';

			if (new_x > 0 && old_x <= 0) {

				// cerr << bits[8].get_energy() << ' ' << clocc.get_energy() << '\n';

				if (bits[8].get_energy() > ENERGY) {
					char x = 0;
					cerr << "e " << clocc.get_energy() << '\n';
					for (int i=0; i<8; i++) {
						cerr << "e " << i << ' ' << bits[i].get_energy() << '\n';
						if (bits[i].get_energy() > ENERGIES[i]) {
							x += 1 << i;
						}
					}
					
					/*if (x == '&') cout << "&amp;";
					else if (x >= 32 && x <= 127) cout << x;
					else cout << '&' << (int)x << ';';
					*/
					cout << x;
					cout.flush();
				}

				for (int i=0; i<9; i++) bits[i].x = bits[i].v = 0;
			}
		}
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