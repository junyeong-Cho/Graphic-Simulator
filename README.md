# CS250 Assignments

For the class assignments you will make a series of graphics demo programs that showcases all the topics discussed in the course.

Each topic will have applicable sliders and/or modifiers to play around with the parameters which affect the topic.

The topics will be presented and implemented with OpenGL.

This will be a cross platform targeting Windows & Web platforms as a minimum and possibly the Ubuntu & Mac platforms.

Your topics include but are not limited to:

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


## How to Build and Run

First Setup your [Development Environment](docs/DevEnvironment.md)

### Command Line Build

#### General Steps

**Release**
```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/executables/Release/graphics_fun
```

**Debug**
```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build/build --config Debug
./build/executables/Debug/graphics_fun
```

**Emscripten**
```sh
source /path/to/emsdk/emsdk_env.sh
emcmake cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
python3 -m http.server -d ./build/executables/
# Open Web Browser and go to http://localhost:8000/Release/graphics_fun.html
```

#### Recommended for Copy/Paste Commands
```sh
cmake -B build/release -S . -DCMAKE_BUILD_TYPE=Release
cmake -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug
source ~/emsdk/emsdk_env.sh
emcmake cmake -B build/webrelease -S . -DCMAKE_BUILD_TYPE=Release
emcmake cmake -B build/webdebug -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build/release --config Release
cmake --build build//debug --config Debug
cmake --build build/webrelease --config Release
cmake --build build/webdebug --config Debug
```

