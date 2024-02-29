# CS250 Class Project

For the class project you will make a graphics demo program that showcases all the topics discussed in the course.

The demo will showcase all the topics and should have an intuitive way to pick which topic to view.

Each topic should have applicable sliders and/or modifiers to play around with the parameters which affect the topic.

The topics will be presented and implemented with OpenGL.

This will be a cross platform targeting Windows & Web platforms as a minimum and possibly the Ubuntu & Mac platforms.

Your demo must show off the following topics

1. Procedural Geometric Modeling
	- Plane, Cube, Sphere, Torus, Cylinder, and Cone
2. Fog
	- Linear and Exponential
3. Toon / Cell Shading
4. Shadow Mapping
	- From a Directional light with position
5. Geometry and Tessellation Shaders
6. Compute Shaders
7. Value noise, Gradient noise / Perlin noise
	- Generating textures and meshes
8. Hermite Curves / Catmull-Rom Splines

Checkout the [Rubric](docs/Rubric.md) for grading details.

## How to Build and Run

First Setup your [Development Environment](docs/DevEnvironment.md)

### Command Line Build
**Release**
```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/executables/Release/graphics_fun
```

**Debug**
```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/executables/Debug/graphics_fun
```

#### Emscripten
```sh
source /path/to/emsdk/emsdk_env.sh
emcmake cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
python3 -m http.server -d ./build/executables/
# Open Web Browser and go to http://localhost:8000/Release/graphics_fun.html
```


