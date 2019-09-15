/*
 * main.cpp
 *
 * Entry point of the application - one line only.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#include "Main/App.hpp"

int main(int argc, char * argv[]) { return spacelite::Main::App::getInstance(argc, argv).run(); }
