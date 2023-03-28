# CosGraphics

[![State-of-the-art Shitcode](https://img.shields.io/static/v1?label=State-of-the-art&message=Shitcode&color=7B5804)](https://github.com/trekhleb/state-of-the-art-shitcode)

This is a simple 2D graphics library that can be used in simple 2D rendering 

## Build

Currently this project has only been tested on Ubuntu22.

Install dependencies

```
sudo apt install build-essential libglfw3-dev libgl1-mesa-dev
```

build
```
cmake --build build
```

build files is in `./export/bin/` and `./export/lib/`

Currently the shaders are not built within the library, but needs to be loaded while running the program. So please make sure that the shaders are at the same path with your program.

---