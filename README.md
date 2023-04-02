# CosGraphics

[![State-of-the-art Shitcode](https://img.shields.io/static/v1?label=State-of-the-art&message=Shitcode&color=7B5804)](https://github.com/trekhleb/state-of-the-art-shitcode)

This is a simple 2D graphics library that can be used in simple 2D rendering 

## Build

Clone repository

```
git clone https://github.com/OppositeNor/cos-graphics.git
```

For Linux Debian you have to install dependencies.


```
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev
```

For Windows you have to download [MinGW](https://www.mingw-w64.org) and [CMake](https://cmake.org/) to compile this program.

build
```
cmake --build build
```

build files is in `./export/bin/` and `./export/lib/`

Currently the shaders are not built within the library, and needs to be loaded while running the program. So please make sure that the shaders are at the same path with your program.

---