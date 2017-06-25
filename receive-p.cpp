#include "common-p.hpp"
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

	double DAMP = 0.001;

	Oscillator clocc_sig_on(TAU / CLOCK_SIG_FREQ_ON, DAMP);
	Oscillator clocc_sig_off(TAU / CLOCK_SIG_FREQ_OFF, DAMP);
	Oscillator clocc(TAU / BIT_WIDTH, 0.1);

	clocc.autotune();
	clocc_sig_on.autotune();
	clocc_sig_off.autotune();

	Oscillator bits[16];

	Oscillator transmit_on(TAU / TRANSMIT_ON, DAMP);
	Oscillator transmit_off(TAU / TRANSMIT_OFF, DAMP);

	transmit_on.autotune();
	transmit_off.autotune();

	for (int i=0; i<16; i++) {
		bits[i] = Oscillator(TAU / freqs[i], DAMP);
		bits[i].autotune();
	}

	int clock_event_no = 0;

	string data;

	auto callback = [&]	(vector<sample>&& v) {

		for (sample x : v) {
			double f = u32_to_i16(x) / 32768.;
			clocc_sig_on.step(f);
			clocc_sig_off.step(f);
			for (int i=0; i<16; i++) bits[i].step(f);

			transmit_on.step(f);
			transmit_off.step(f);

			double old_x = clocc.x;
			double sign = clocc_sig_on.get_energy() - clocc_sig_off.get_energy();
			clocc.step(sign);
			double new_x = clocc.x;

			if (new_x > 0 && old_x <= 0) {
				cerr << "Clock event no " << clock_event_no++ << '\n';
				cerr << "Transmit on/off energies:\n";
				cerr << transmit_on.get_energy() << ' ' << transmit_off.get_energy() << '\n';

				if (transmit_on.get_energy() > 2 * transmit_off.get_energy()) {
					unsigned char x = 0;
					for (int i=0; i<8; i++) {
						int bit_off = 2*i;
						int bit_on = 2*i+1;
						if (bits[bit_on].get_energy() > bits[bit_off].get_energy()) {
							x += 1 << i;
						}
					}
					cout << (data += x) << '\n';
					cout.flush();
				}
				for (int i=0; i<16; i++) bits[i].x = bits[i].v = 0;
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