#ifndef MYSOUNDLIB_OSCILLATOR
#define MYSOUNDLIB_OSCILLATOR

#include <cmath>
#include <complex>
using namespace std;

struct Oscillator {

	typedef complex<double> cdouble;

	double om0, zeta;
	double x, v, t;

	cdouble A, B;

	Oscillator(double om0, double zeta) : om0(om0), zeta(zeta),
		x(0), v(0), t(0)
	{
		double krn = sqrt(om0*om0 - zeta*zeta*om0*om0);
		A = cdouble(-om0*zeta, -krn);
		B = cdouble(-om0*zeta, +krn);
	}
	
	Oscillator() : om0(0) {}

	void step(double f, double dt = 1) {
		double g = f/(om0*om0);			
		cdouble c1 = (B*(g-x) + v) / (A - B);
		cdouble c2 = x - c1 - g;

		x = (c1 * exp(A*dt) + c2 * exp(B*dt) + g).real();
		v = (c1 * A * exp(A*dt) + c2 * B * exp(B*dt)).real();
	}

	double get_energy() const {
		return x*x*om0*om0 + v*v;
	}

	void autotune() {
		om0 *= sqrt(1 - 2 * zeta * zeta);
	}

};

#endif // MYSOUNDLIB_OSCILLATOR