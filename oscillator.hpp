#ifndef MYSOUNDLIB_OSCILLATOR
#define MYSOUNDLIB_OSCILLATOR

#include <cmath>

struct Oscillator {
	double om0, zeta;
	double x, v, t;

	Oscillator(double om0, double zeta) : om0(om0), zeta(zeta),
		x(0), v(0), t(0) {}
	
	Oscillator() : om0(0) {}

	void step(double f, double dt = 1) {
		int res = 128;
		dt /= res;

		while (res--) {
			double a = f - om0*om0*x - 2*zeta*om0*v;
			double vn = v + a * dt;
			double dx = dt * (v + vn) / 2;

			x += dx;
			v = vn;
			t += dt;
		}
	}

	double get_energy() const {
		return x*x*om0*om0 + v*v;
	}

	void autotune() {
		om0 *= sqrt(1 - 2 * zeta * zeta);
	}

};

#endif // MYSOUNDLIB_OSCILLATOR