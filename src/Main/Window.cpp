/*
 * Window.cpp
 *
 * Window using GLFW.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#include "Window.hpp"

namespace spacelite::Main {

unsigned short Window::numGlfwUsers = 0;

Window::Window(const Struct::WinProp& properties)
		: ptr(nullptr), width(properties.width), height(properties.height), resized(false) {
	// initialize GLFW (if still necessary)
	if(!glfwInit())
		throw Exception("Could not initialize GLFW: " + Wrapper::GlfwError().str());

	// increase number of GLFW users
	++Window::numGlfwUsers;

	// disable OpenGL context creation (using Vulkan instead)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// create window
	this->ptr = glfwCreateWindow(this->width, this->height, properties.title.c_str(), nullptr, nullptr);

	glfwSetWindowUserPointer(this->ptr, this);
	glfwSetFramebufferSizeCallback(this->ptr, this->resizeCallback);

	if(!(this->ptr))
		throw Exception("Could not create window: " + Wrapper::GlfwError().str());

	glfwMakeContextCurrent(this->ptr);
}

Window::~Window() {
	// destroy window
	if(this->ptr)
		glfwDestroyWindow(this->ptr);

	// terminate GLFW if this is the last user
	if(Window::numGlfwUsers == 1)
		glfwTerminate();

	// decrease number of GLFW users
	if(Window::numGlfwUsers)
		--Window::numGlfwUsers;
}

// get string with API information
std::string Window::getInfo() {
	return std::string("GLFW v") + glfwGetVersionString();
}

// get whether window should be closed
bool Window::isClosed() const {
	return glfwWindowShouldClose(this->ptr);
}

// get the pointer to the window
GLFWwindow * Window::getPointer() {
	return this->ptr;
}

// get the width of the window
unsigned int Window::getWidth() const {
	return this->width;
}

// get the height of the window
unsigned int Window::getHeight() const {
	return this->height;
}

// get whether window has been resized (and reset resize state)
bool Window::isResized() const {
	return this->resized;
}

// run tick: swap front and back buffers, poll for window events
void Window::tick() {
	glfwSwapBuffers(this->ptr);

	glfwPollEvents();
}

// update the dimension of the window
void Window::updateSize() {
	int w = 0;
	int h = 0;

	glfwGetFramebufferSize(this->ptr, &w, &h);

	this->width = static_cast<unsigned int>(w);
	this->height = static_cast<unsigned int>(h);
}

// reset resize state
void Window::resetResize() {
	this->resized = false;
}

// move constructor
Window::Window(Window&& other) noexcept
		:	ptr(other.ptr), width(other.width), height(other.height), resized(other.resized) {
	other.ptr = nullptr;
}

// move assignment
Window& Window::operator=(Window&& other) noexcept {
	this->ptr = other.ptr;
	this->width = other.width;
	this->height = other.height;
	this->resized = other.resized;

	other.ptr = nullptr;

	return *this;
}

// callback for window resize
void Window::resizeCallback(GLFWwindow* window, int w, int h) {
	auto thisPtr = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	thisPtr->resized = true;

	thisPtr->width = static_cast<unsigned int>(w);
	thisPtr->height = static_cast<unsigned int>(h);
}

} /* spacelite::Main */
