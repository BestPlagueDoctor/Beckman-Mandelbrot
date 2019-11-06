#include <iostream>
#include <math.h>
#include <complex>


int main() {
	float rec;
	std::cin >> rec;
	float imc;
	std::cin >> imc;
	std::complex<float> cvar = (rec, imc);
	//std::complex<double> cvar = (5.0, 3.0);
	std::cout << cvar;
	std::cout << std::abs(cvar);
	std::complex<float> zvar;
	int iter;
	for (int i = 0; i < 101; i++) {
		zvar = ((zvar * zvar) + cvar);
		iter  = i;
		if (std::abs(zvar) >= 2.0) {
			break;
		}
		std::cout << i << "  " << zvar << "\n";
	}
	std::cout << iter << "\n";
	if (iter == 100.0) {std::cout << "Within Mandlebrot" << "\n";}
	else {std::cout << "Not within Mandlebrot" << "\n";}
	std::cout << cvar;
	return 0;
}
