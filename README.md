![](github-assets/chisel_engine.jpg)

# Chisel Engine
A small voxel engine written in C++17, created entirely for messing around with graphics programming and other educational purposes.

# Features
There aren't any notable feature yet as this engine has been around for only ~4 months. But, at least it has:

- World sub-division system.
- Basic procedural terrain generation.
- Block-breaking mechanism implemented using the DDA algorithm.

# Goals
Of course, I want to add more content to this engine so it's less boring. Here's a list of stuff I want to add next:

- [ ] Texturing for each block.
- [ ] MSAA so the graphics looks less edgy.
- [ ] Frustum culling.
- [ ] SSAO.

There may be more but I will focus on what's important for now.

# Platform

| Platform              | Compiler | Build system |
|-----------------------|----------|--------------|
| Linux PopOS 22.04 LTS | g++      | make         |

# Building

Install `glew`, `SDL2`, and `glm` via `apt`:

```sh
$ sudo apt install libglew-dev libsdl2-dev libglm-dev
```

To build the engine:
```sh
$ cd src/
$ make # make CFG=debug if you want to compile with g++ debug flags
```

To run:
```sh
$ make run
```