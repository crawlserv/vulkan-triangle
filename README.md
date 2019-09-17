# vulkan-triangle
An implementation of [the vertex buffer example](https://github.com/Overv/VulkanTutorial/blob/master/code/18_vertex_buffer.cpp) from [vulkan-tutorial.com](https://vulkan-tutorial.com/) in my own coding style and with many more classes.

## Building vulkan-triangle on Linux

You can clone the complete source code into a sub-folder named `vulkan-triangle` using [`git`](https://git-scm.com/):

```
git clone https://github.com/crawlserv/vulkan-triangle
```

The following additional components are required to build crawlserv++ on your system:

* [`cmake`](https://cmake.org/), version 3.7 or higher
* [`GNU Make`](https://www.gnu.org/software/make/) or a compatible Makefile parser
* [`gcc`](https://gcc.gnu.org/), version 6 or higher, or [`clang`](https://clang.llvm.org/), version 5 or higher – or any other modern C++ 17 compiler
* a multi-threading library supported by `cmake` like `pthreads` (e.g. `libpthread-stubs0-dev` on Ubuntu)
* the [`GLFW`](https://www.glfw.org/) library, version 3.0 or higher (tested with version 3.3)
* the [`Vulkan API`](https://www.khronos.org/vulkan/), version 1.1 or higher
* the [`GLM`](https://glm.g-truc.net/) library (tested with version 0.9.9)

After installing these components and cloning or downloading the source code, use the terminal to go into the `vulkan-triangle` directory with downloaded files and run the following commands:

```
mkdir build 
cd build
cmake ../src
```

If `cmake` was successful and shows `Build files have been written to: [...]`, proceed with:

```
make
```

It should finish with `[100%] Built target vulkan-triangle`.

The program should have been built inside the newly created `build` directory.

Leave this directory with `cd ..`.

## Compiling the shaders

For compiling the shaders, you need the [`glslch`](https://github.com/google/shaderc/tree/master/glslc) program which is also included in the [`Vulkan SDK`](https://www.lunarg.com/vulkan-sdk/). On Linux, make the provided `.sh` file executable and run it:

```
chmod +x compile_shd.sh
./compile_shd.sh
```

Now you can run the program by using `./build/vulkan-triangle`.
