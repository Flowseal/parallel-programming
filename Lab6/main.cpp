#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <fstream>

double calculate_pi_sync() {
	double pi = 0.0;

	for (long long i = 0; i < 1'000'000; ++i) {
		pi += (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
	}
	return pi * 4.0;
}

double calculate_pi_parallel() {
	double pi = 0.0;

#pragma omp parallel for
	for (long long i = 0; i < 1'000'000; ++i) {
		pi += (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
	}
	return pi * 4.0;
}

double calculate_pi_parallel_atomic() {
	double pi = 0.0;

#pragma omp parallel for
	for (long long i = 0; i < 1'000'000; ++i) {
		double term = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
#pragma omp atomic
		pi += term;
	}
	return pi * 4.0;
}

double calculate_pi_reduction() {
	double pi = 0.0;

#pragma omp parallel for reduction(+:pi)
	for (long long i = 0; i < 1'000'000; ++i) {
		pi += (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
	}
	return pi * 4.0;
}

void start_pi_calc(double (*func)()) {
	double start_time = omp_get_wtime();
	double pi = func();
	double end_time = omp_get_wtime();
	std::cout << std::setprecision(12) << std::fixed;
	std::cout << "PI: " << pi << "\n";
	std::cout << "Time: " << (end_time - start_time) << " seconds\n";
}

int main() {
	std::cout << "SYNC" << std::endl << "============" << std::endl;
	start_pi_calc(calculate_pi_sync);
	std::cout << "============" << std::endl;

	std::cout << "PARALLEL" << std::endl << "============" << std::endl;
	start_pi_calc(calculate_pi_parallel);
	std::cout << "============" << std::endl;

	std::cout << "PARALLEL ATOMIC" << std::endl << "============" << std::endl;
	start_pi_calc(calculate_pi_parallel_atomic);
	std::cout << "============" << std::endl;

	std::cout << "REDUCTION" << std::endl << "============" << std::endl;
	start_pi_calc(calculate_pi_reduction);
	std::cout << "============" << std::endl;
	return 0;
}
