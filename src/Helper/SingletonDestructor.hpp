/*
 * SingletonDestructor.hpp
 *
 * Destroys a (quasi-)singleton before termination of the whole program.
 *
 *  Created on: Oct 2, 2019
 *      Author: ans
 */

#ifndef HELPER_SINGLETONDESTRUCTOR_HPP_
#define HELPER_SINGLETONDESTRUCTOR_HPP_

#include <cassert>	// assert

namespace spacelite::Helper {

	/*
	 * TEMPLATE
	 */
	template<class S>
	class SingletonDestructor {
	public:
		// constructor: set singleton pointer to nullptr
		SingletonDestructor() : ptr(nullptr) {}

		// destructor: destroy singleton (if specified)
		~SingletonDestructor() {
			if(this->ptr)
				delete this->ptr;
		}

		// set singleton (by pointer)
		void setPointer(S * pointer) {
			assert(!(this->ptr));

			this->ptr = pointer;
		}

	private:
		S * ptr;
	};

} /* spacelite::Helper */

#endif /* HELPER_SINGLETONDESTRUCTOR_HPP_ */
