# jubilant-funicular
[![Build Status](https://travis-ci.org/NivenT/jubilant-funicular.svg?branch=master)](https://travis-ci.org/NivenT/jubilant-funicular)

# Description
This is a library to aid in the creation of (2D) games using OpenGL and SDL2. It was originally created while following tutorials on the [MakingGamesWithBen](https://www.youtube.com/channel/UCL5m1_llmeiAdZMo_ZanIvg) youtube channel, but it has since grown into its own library, and so has many differences from [the one](https://github.com/Barnold1953/GraphicsTutorials) written in that series. This library is here for me to learn a little about game development.

I am using this library to write [this game](https://github.com/NivenT/Planet).
If you are interesting in using this for your own project, then looking in the `examples` folder may be helpful.

Ideally this entire library would work equally well on Linux, OSX, and Windows. In practice, the code is only tested on Linux, so while I try to keep things OS-agnostic, I cannot promise this is the case. Open an issue if it fails to build on your computer.

Documentation can be found [here](https://nivent.github.io/jubilant-funicular/html/index.html)

# Requirements
In a perfect world there would be no dependencies and all the necessary code would be written by me; however, in our world you should make sure you have

* [OpenGL](https://www.opengl.org/)
  * [Glew](http://glew.sourceforge.net/)
  * [GLUT](http://freeglut.sourceforge.net/)
* [SDL2](https://www.libsdl.org/download-2.0.php)
  * SDL2-mixer (Optional)
  * SDL2-ttf
* [DevIL](http://openil.sourceforge.net/) (Optional)

All of these can be obtained using `apt-get` on Ubuntu. If you want to get them all at once, run

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev \
    libglew-dev freeglut3-dev libxmu-dev libxi-dev libdevil-dev
```
You'll likely already have some of these (e.g. libxmu-dev and libxi-dev)

On Mac, you can get them using `brew`.

If you don't want to use the ones labelled Optional, check out the `option`s in [CMakeLists.txt](https://github.com/NivenT/jubilant-funicular/blob/master/CMakeLists.txt#L45). 

## Building Library (Along with Examples and Tests)
If you want to build this library in isolation, the do the following
```bash
git clone https://github.com/NivenT/jubilant-funicular
cd jubilant-funicular
mkdir build
cd build
cmake ../
make
```

After that, to run all the tests you just need to call `make test`. The tests are built inside `tests/tests_bin`, so if you want to run one individually, then do something like the following (from inside the build folder)
```bash
cd tests/tests_bin
cd utils
./path_tests
```

The examples have to be run manually (i.e. there's no `make example`). They are build inside `examples_bin`, so you can run one by doing something like (from inside the build folder)
```bash
cd examples_bin
cd basic_square
./basic_square
```

## Note
There are a few dependencies (none of the ones listed above) that come with the project. In an attempt to limit the repo's size, I've included (parts of) them directly instead of as submodules. These can be found in the [deps](https://github.com/NivenT/jubilant-funicular/tree/master/deps) folder and include

* [glm](https://glm.g-truc.net/0.9.8/index.html)
* [Dear Imgui](https://github.com/ocornut/imgui) (Optional)
    * [Imgui_tabs](https://github.com/ebachard/imgui_tabs)
* [CImg](https://github.com/dtschump/CImg) (Optional)
