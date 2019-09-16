/*
 * TimerHighRes.hpp
 *
 * Simple timer for stopping microseconds.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef HELPER_TIMERHIGHRES_HPP_
#define HELPER_TIMERHIGHRES_HPP_

#include <chrono>
#include <iostream>

namespace spacelite::Helper {

	/*
	 * DECLARATION
	 */
	class TimerHighRes {
	public:
		TimerHighRes();

		unsigned long long since() const;
		void reset();

	private:
		std::chrono::high_resolution_clock::time_point start;
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: automatic start of the timer
	inline TimerHighRes::TimerHighRes() : start(std::chrono::high_resolution_clock::now()) {}

	// get the time (in μs) since creation or last reset of the timer
	inline unsigned long long TimerHighRes::since() const {
		return std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now()
				- this->start
		).count();
	}

	// reset the timer
	inline void TimerHighRes::reset() {
		this->start = std::chrono::high_resolution_clock::now();
	}
}


#endif /* HELPER_TIMER_HPP_ */
