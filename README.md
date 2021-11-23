# HyperEngine
[![Build Windows](https://img.shields.io/github/workflow/status/SkillerRaptor/HyperEngine/build-windows?style=flat&label=Build%20Windows&logo=github)](https://github.com/SkillerRaptor/HyperEngine/blob/master/.github/workflows/build-windows.yml)
[![Build Linux](https://img.shields.io/github/workflow/status/SkillerRaptor/HyperEngine/build-linux?style=flat&label=Build%20Linux&logo=github)](https://github.com/SkillerRaptor/HyperEngine/blob/master/.github/workflows/build-linux.yml)
[![License](https://img.shields.io/badge/license-MIT-yellow?style=flat)](https://github.com/SkillerRaptor/HyperEngine/blob/master/LICENSE)

<br />

HyperEngine is a modern multi-platform 2D & 3D game engine written in C++20.
The engine is focused on testing new designs in form of rendering or performance.

This repository contains the source code of the HyperEngine.
Anyone is welcome to contribute or use the source of the HyperEngine.

## Contents
- [Getting Started](#getting-started)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [License](#license)

## Getting Started

### Prerequisites
In order to build the engine, you will need the following things installed:
- CMake
- Vulkan SDK

### Building
1. Download the source code by using git and cloning the repository, or downloading it as a zip.
```shell
git clone ttps://github.com/SkillerRaptor/HyperEngine
cd HyperEngine
```

2. Create a folder in which the CMake project files will be generated in.
```shell
mkdir build
cd build
```

3. Configure the project by using the <code><a href="https://github.com/SkillerRaptor/HyperEngine/blob/master/CMakeLists.txt">CMakeLists.txt</a></code> inside of the root directory.
```shell
cmake .. -D CMAKE_BUILD_TYPE=Debug
```

4. Build the project using CMake
```shell
cmake --build .
```

5. Run the engine to check if everything was built successfully.

## Dependencies
- [glfw](https://github.com/glfw/glfw/blob/master/LICENSE.md) Copyright (c) zlib License, Marcus Geelnard 2002-2006 & Camilla Löwy 2006-2019
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers/blob/master/LICENSE.txt) Copyright (c) Apache License 2.0, The Khronos Group Inc. 2019-2021
- [volk](https://github.com/zeux/volk/blob/master/LICENSE.md) Copyright (c) MIT, Arseny Kapoulkine 2018-2019

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
Please make sure to update tests as appropriate.

## License
HyperEngine is distributed under the [MIT](https://github.com/SkillerRaptor/HyperEngine/blob/master/LICENSE) license.
