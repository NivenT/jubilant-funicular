# jubilant-funicular
I think technically this is a game engine. Whatever it is, it's certainly incomplete and in-development...

# Description
This is a library to aid in the creation of (2D) games using OpenGL and SDL2. It was originally created while following tutorials on the [MakingGamesWithBen](https://www.youtube.com/channel/UCL5m1_llmeiAdZMo_ZanIvg) youtube channel, but it has its differences from [the library](https://github.com/Barnold1953/GraphicsTutorials) written in that series. This library is here for me to learn a little about game development; it's not production quality, so I can't recommend using it in your own projects. However, if you want an example of what's buildable with it, you can check out [this game](https://github.com/NivenT/Planet) that I'm writing in part just to exercise this library.


Documentation can be found [here](https://nivent.github.io/jubilant-funicular/html/index.html)

# Requirements
Ideally, there would be no dependencies and all the necessary code would be written by me, but that's not the case, so make sure you have

* [OpenGL](https://www.opengl.org/)
  * [Glew](http://glew.sourceforge.net/)
  * [GLUT](http://freeglut.sourceforge.net/)
* [SDL2](https://www.libsdl.org/download-2.0.php)
  * SDL2-mixer
  * SDL2-ttf
* [DevIL](http://openil.sourceforge.net/)

All of these can be obtained using `apt-get` on Ubuntu. If you want to get them all at once, run

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev \
    libglew-dev freeglut3-dev libxmu-dev libxi-dev libdevil-dev
```
You'll likely already have some of these (e.g. libxmu-dev and libxi-dev)

On Mac, you can get them using `brew`.

## Note
There are a few dependencies (none of the ones listed above) that come with the project. In an attempt to limit the repo's size, I've included (parts of) them directly instead of as submodules. These can be found in the [deps](https://github.com/NivenT/jubilant-funicular/tree/master/deps) folder and include

* [glm](https://glm.g-truc.net/0.9.8/index.html)
* [imgui](https://github.com/ocornut/imgui) (Optional)
* [CImg](https://github.com/dtschump/CImg) (Optional)
