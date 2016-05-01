Building
========

## General

If you want to modify the shader parsers of the *graphics* library frontends you will need to modify the `*.l` and `*.y` files and compile them back to the `*.cpp` files with [Flex](http://flex.sourceforge.net/) and [Bison](https://www.gnu.org/software/bison/) respectively.

If you want to modify the shaders used by the *ui* library you will need to modify the `*.glsl` files and compile them to `*.spv` binaries using [Glslang](https://www.khronos.org/opengles/sdk/tools/Reference-Compiler/). Choose the proper shader type based on the filename suffix.


## Platforms

Below are the instructions to compile Nucleus for different platforms. If you want to cross-compile Nucleus, which is particularly relevant for mobile targets, details for each host compiling platform are provided as well.

### Windows

* __Compiling on Windows__: Install [Visual Studio 2015](http://www.visualstudio.com/). Open `nucleus.sln` and build the project `nucleus-windows`. Alternatively, if you have the [Windows 10 SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk) installed you can compile the Universal Windows Platform wrapper by building the project `nucleus-uwp`.

### Windows Phone

* __Compiling on Windows__: Install [Visual Studio 2015](http://www.visualstudio.com/) along with the [Windows 10 SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk). Open `nucleus.sln` and build the project `nucleus-uwp`.

### Linux

* __Compiling on Linux__: Follow the `.travis.yml` file.

### Android

* __Compiling on Windows__: Install [Visual Studio 2015](http://www.visualstudio.com/) along with the [Android SDK](http://developer.android.com/sdk/index.html) and [Android NDK](https://developer.android.com/tools/sdk/ndk/index.html#download). Open `nucleus.sln` and build the project `nucleus-android`.

* __Compiling on Linux__: *TODO*.

* __Compiling on OS X__: *TODO*.

### iOS

* __Compiling on Windows__: Install [Visual Studio 2015](http://www.visualstudio.com/). Install [OS X](http://www.apple.com/osx/) in a virtual machine, e.g. via [VirtualBox](https://www.virtualbox.org/), and ensure that both your host and virtual machine share a network. Install [Xcode](https://developer.apple.com/xcode/) on the virtual machine. [...]. On your host machine, open `nucleus.sln` and build the project `nucleus-ios`. Observe how nothing works. Cry.

* __Compiling on OS X__: Buy a Mac. Cry.
