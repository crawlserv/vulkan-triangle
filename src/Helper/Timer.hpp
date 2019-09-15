/*
 * Timer.hpp
 *
 * Simple timer for stopping milliseconds.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef HELPER_TIMER_HPP_
#define HELPER_TIMER_HPP_

#include <chrono>
#include <iostream>

namespace spacelite::Helper {

	/*
	 * DECLARATION
	 */
	class Timer {
	public:
		Timer();

		unsigned long long since() const;
		void reset();

	private:
		std::chrono::steady_clock::time_point start;
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: automatic start of the timer
	inline Timer::Timer() : start(std::chrono::steady_clock::now()) {}

	// get the time (in ms) since creation or last reset of the timer
	inline unsigned long long Timer::since() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now()
				- this->start
		).count();
	}

	// reset the timer
	inline void Timer::reset() {
		this->start = std::chrono::steady_clock::now();
	}
}


#endif /* HELPER_TIMER_HPP_ */
