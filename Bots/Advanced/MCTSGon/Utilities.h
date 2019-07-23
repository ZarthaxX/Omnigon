#pragma once

#include <chrono>

namespace MCTSGon {
#define Now() std::chrono::high_resolution_clock::now()

	static struct Stopwatch {
		std::chrono::high_resolution_clock::time_point c_time, c_timeout;
		void setTimeout(int us) { c_timeout = c_time + std::chrono::microseconds(us); }
		void Start(int limit) { c_time = Now(); setTimeout(limit); }
		inline bool Timeout() { return Now() > c_timeout; }
		long long EllapsedMicroseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(Now() - c_time).count(); }
		long long EllapsedMilliseconds() { return std::chrono::duration_cast<std::chrono::milliseconds>(Now() - c_time).count(); }
	} stopwatch;//} Stopwatch


	static unsigned int g_seed;
	inline void fast_srand(int seed) {
		//Seed the generator
		g_seed = seed;
	}
	inline int fastrand() {
		//fastrand routine returns one integer, similar output value range as C lib.
		g_seed = (214013 * g_seed + 2531011);
		return (g_seed >> 16) & 0x7FFF;
	}

	inline int rnd(int b) {
		return fastrand() % b;
	}

	inline int rnd(int a, int b) {
		return a + rnd(b - a + 1);
	}

	inline int fastRandInt(int maxSize) {
		return fastrand() % maxSize;
	}
	inline int fastRandInt(int a, int b) {
		return(a + fastRandInt(b - a));
	}
	inline double fastRandDouble() {
		return static_cast<double>(fastrand()) / 0x7FFF;
	}
	inline double fastRandDouble(double a, double b) {
		return a + (static_cast<double>(fastrand()) / 0x7FFF)*(b - a);
	}
}