# Simple graphics

Simple graphics engine using **SDL2** and **OpenGL**. Allows you to load 3D models into a demo scene.

Currently has two ways to load models:

- Using my custom loader for `obj` and `mtl` files;
- Using `Assimp` to load `fbx` (other formats will be added in future).

Here's a screenshot of a demo scene, that contains 2 models: city model that is loaded from `obj` and `mtl` files and AK-47 that is loaded from `fbx` file and albedo (diffuse) map.

![Demo scene](./demo_screenshot.png)

The `Assimp` library is quite slow, especially when loading `obj` files. So I wrote my own loader that parses `obj` and `mtl` files using `Boost::Spirit::X3` parsing library. For the demo scene, this changed the city model loading time from `~800ms` to `~13ms` in `RELEASE` mode. In future, I plan to add custom `fbx` loader too, so I can get rid of `Assimp` entirely.

## Dependencies

I use `conan` package manager to pull all the necessary dependecies. This project's dependencies can be seen in [conanfile.txt](./conanfile.txt).

## Compilation info

To compile the project, first you'll need to install `conan` to manage the dependencies:

```
pip install conan
```

If you don't have `pip` installed, refer to [this](https://docs.conan.io/en/latest/installation.html) `conan` installation guide.

Next, we're creating two profiles for Debug and Release:

```
cd build
conan install .. -s build_type=Debug -if Debug
conan install .. -s build_type=Release -if Release
```

After that, the easiest way to build the application is by using VS Code [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension.

## Building manually

If you're using other editor or don't want to use the CMake Tools extension, you'll have to build the project manually.
First, use CMake to generate the appropriate build files (replace **Release** with **Debug** if needed):

```
cd Release
cmake ../.. -DCMAKE_BUILD_TYPE=RELEASE
```

Using generated files, you can compile the project. On OSX/Linux use:

```
cmake --build .
```

On Windows, you have to specify the build type:

```
cmake --build . --config RELEASE
```

Now, enjoy your freshly minted binaries inside the **bin** folder!

## Launching the app

Internal code uses relative paths for loading models and shaders, so make sure that your working directory is the project root. Here's an example of how you can run the binaries:

```
./build/Release/bin/simple-graphics
```

## Cleaning up build files

If you want to clean up the build files and binaries, you can use `git` from the project root directory:

```
git clean -dfX build
```
