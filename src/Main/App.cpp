/*
 * App.cpp
 *
 * Application.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#include "App.hpp"

namespace spacelite::Main {

/* STATIC CONSTANTS */
const Struct::AppInfo App::appInfo("vulkan-triangle", 1, 0, 0);
const unsigned int App::defaultWinWidth = 800;
const unsigned int App::defaultWinHeight = 600;
/* END STATIC CONSTANTS */

// constructor: initialize all components
App::App(int argc __attribute__ ((unused)), char * argv[] __attribute__ ((unused)))
		:	locale("en_US.UTF8"),
			isRunning(true),
			mainWindow(Struct::WinProp(App::appInfo.name, App::defaultWinWidth, App::defaultWinHeight)),
			engineGraphics(Engine::Graphics::getInstance(App::appInfo, mainWindow, &(this->graphicsDestructor))) {}

// run the main loop
int App::run() {
	try {
		while(this->tick()) {}
	}
	catch(const Main::Exception& exception) {
		std::cout << exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	std::cout << "Bye bye." << std::endl;

	return EXIT_SUCCESS;
}

// perform a tick and return whether to continue
bool App::tick() {
	if(this->mainWindow.isClosed()) {
		// main window has been closed: end application
		this->isRunning = false;

		return false;
	}

	// update engine
	this->engineGraphics.tick();

	// update main window
	this->mainWindow.tick();

	// return whether to continue
	return this->isRunning;
}

} /* spacelite::Main */
