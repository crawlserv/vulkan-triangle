/*
 * Window.hpp
 *
 * Window using GLFW.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "Exception.hpp"

#include "../Struct/WinProp.hpp"
#include "../Wrapper/GlfwError.hpp"

#include <GLFW/glfw3.h>

#include <string>	// std::string

namespace spacelite::Main {

	class Window {
	public:
		Window(const Struct::WinProp& properties);
		virtual ~Window();

		// getters
		static std::string getInfo();
		bool isClosed() const;
		GLFWwindow * getPointer();
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		bool isResized() const;

		// perform tick
		void tick();

		// update state
		void updateSize();
		void resetResize();

		// not copyable, only moveable
		Window(const Window&) = delete;
		Window(Window&& other) noexcept;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&& other) noexcept;

	protected:
		GLFWwindow * ptr;

	private:
		// instance counter
		static unsigned short numGlfwUsers;

		// window properties
		unsigned int width;
		unsigned int height;
		bool resized;

		// callback
		static void resizeCallback(GLFWwindow * window, int width, int height);

		// exception sub-class
		MAIN_EXCEPTION_CLASS();
	};

} /* spacelite::Main */

#endif /* WINDOW_H_ */
