/*
 * App.hpp
 *
 * Application.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef APP_HPP_
#define APP_HPP_

#include "Exception.hpp"
#include "Window.hpp"

#include "../Engine/Graphics.hpp"
#include "../Struct/AppInfo.hpp"
#include "../Wrapper/CLocale.hpp"

#include <cstdlib>	// EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>	// std::cout, std::endl

namespace spacelite::Main {
	class App {
		/* STATIC CONSTANTS */
		static const Struct::AppInfo appInfo;
		static const unsigned int defaultWinWidth;
		static const unsigned int defaultWinHeight;
		/* END STATIC CONSTANTS */

		/* SINGLETON */
	public:
		static App& getInstance(int argc, char * argv[]) {
			static App instance(argc, argv);

			return instance;
		}

	private:
		App(int argc, char * argv[]);

	public:
		App(const App&) = delete;

		void operator=(const App&) = delete;
		/* END SINGLETON */

		// run application
		int run();

	private:
		// using C locale for output to stdout
		const Wrapper::CLocale cLocale;

	protected:
		// state
		bool isRunning;

		// main window
		Window mainWindow;

		// engine
		Engine::Graphics& engineGraphics;

	private:
		// perform a tick
		bool tick();
	};

} /* spacelite::Main */

#endif /* APP_HPP_ */
