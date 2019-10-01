cmake_minimum_required(VERSION 3.7)

# set project name
set(PROJECT_NAME "vulkan-triangle")
project(${PROJECT_NAME})

# show source directory
message("Source: \"${CMAKE_CURRENT_SOURCE_DIR}/\"")

# find Multi-threading library
find_package(Threads REQUIRED)

if(${Threads_FOUND})
	message("Multi-threading library found: \"${CMAKE_THREAD_LIBS_INIT}\"")
else()
	message(WARNING "Multi-threading library not found.")
endif()

# find Vulkan
find_package(Vulkan REQUIRED)

if(${Vulkan_FOUND})
	message("Found Vulkan API library: \"${Vulkan_LIBRARIES}\"")
else()
	message(WARNING "Vulkan API not found.")
endif()

# find GLFW
find_package(glfw3 REQUIRED)

if(${glfw3_FOUND})
	message("Found GLFW v${glfw3_VERSION}.")
else()
	message(WARNING "GLFW >= v3.0 not found.")
endif()

# find GLM
FIND_PACKAGE(GLM QUIET)
FIND_PACKAGE(glm QUIET)

if(${glm_FOUND})
	set(GLM_FOUND ${glm_FOUND})
	set(GLM_VERSION ${glm_VERSION})
endif()

if(${GLM_FOUND})
	message("Found GLM v${GLM_VERSION}.")
else()
	message(WARNING "GLM not found.")
endif()

# specify source files
add_executable(	${PROJECT_NAME}
				"main.cpp"
				"Engine/Graphics.cpp"
				"Main/App.cpp"
				"Main/Window.cpp"
)

# C++ 17 (including corresponding standard library) is required
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# set include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS})

# set libraries
message("Setting libraries: ${Vulkan_LIBRARIES} glfw.")

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)

# set library options
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_VULKAN)

# set compiler options
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)

	if(${CMAKE_BUILD_TYPE} AND ${CMAKE_BUILD_TYPE} STREQUAL Debug)
		message("Using MSVC without optimiziation and with debugging information.")
		target_compile_options(${PROJECT_NAME} PRIVATE /Od)
		target_compile_options(${PROJECT_NAME} PRIVATE /Zi)
	else()
		message("Using MSVC with maximum optimiziation and without debugging information.")
		target_compile_options(${PROJECT_NAME} PRIVATE /Ox)
		target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
	endif()
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

    if(NOT CMAKE_BUILD_TYPE)
    	SET(CMAKE_BUILT_TYPE Release ... FORCE)
    endif()

	if(${CMAKE_BUILD_TYPE} MATCHES Debug)
		message("Using \"${CMAKE_CXX_COMPILER}\" with minimum optimization and debugging information.")
		target_compile_options(${PROJECT_NAME} PRIVATE -g3)
		target_compile_options(${PROJECT_NAME} PRIVATE -O0)
	else()
		message("Using \"${CMAKE_CXX_COMPILER}\" with maximum optimization and without debugging information.")
		target_compile_options(${PROJECT_NAME} PRIVATE -g0)
		target_compile_options(${PROJECT_NAME} PRIVATE -O3)
		target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
	endif()
endif()

message("Run \"make\" from build directory to build.")
