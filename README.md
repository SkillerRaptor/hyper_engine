<img src="./Development/Images/Branding.png" align="left" width="128px"/>

# HyperEngine
[![Build Windows](https://img.shields.io/github/workflow/status/SkillerRaptor/HyperEngine/build-windows?style=flat&label=Build%20Windows&logo=github)](https://github.com/SkillerRaptor/HyperEngine/blob/master/.github/workflows/build-windows.yml)
[![Build Linux](https://img.shields.io/github/workflow/status/SkillerRaptor/HyperEngine/build-linux?style=flat&label=Build%20Linux&logo=github)](https://github.com/SkillerRaptor/HyperEngine/blob/master/.github/workflows/build-linux.yml)
[![Support Server](https://img.shields.io/discord/674880770137128970.svg?&style=flat&label=Discord&logo=discord)](https://discord.gg/tYu9yYY)
[![License](https://img.shields.io/badge/license-MIT-yellow?style=flat)](https://github.com/SkillerRaptor/HyperEngine/blob/master/LICENSE)

<br />

HyperEngine is an modern multi-platform 2D & 3D game engine written in C++.
The engine is focused on testing new designs in form of rendering or performance.

This repository contains the source code of the HyperEngine.
Anyone is welcome to contribute or use the source of the HyperEngine.

## Contents
- [Getting Started](#getting-started)
- [Screenshots](#screenshots)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [License](#license)

## Getting Started

### Prerequisites
In order to build the engine, you will need the following things installed:
- CMake
- Conan
- Python
- Vulkan SDK

### Installation
1. Download CMake
```shell
sudo apt install cmake
```

2. Download Python3
```shell
sudo apt install python3
```

3. Install Conan
```shell
pip3 install conan
```

4. Install the Vulkan SDK (Optional)

### Building
1. Download the source code by using Git and cloning the repository, or downloading it as a zip.
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

5. Run the engine to check if everything was build successfully.

## Screenshots

## Dependencies
- [Catch2](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt) Copyright (c) Boost Software License 1.0, Catch2 present
- [DiscordGameSDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide) Copyright (c) MIT License, Discord Inc. present
- [fmt](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst) Copyright (c) MIT License, Victor Zverovich 2012-present
- [glad](https://github.com/Dav1dde/glad/blob/master/LICENSE) Copyright (c) MIT License, David Herberth 2013-2021
- [glfw](https://github.com/glfw/glfw/blob/master/LICENSE.md) Copyright (c) zlib License, Marcus Geelnard 2002-2006 & Camilla Löwy 2006-2019
- [glslang](https://github.com/KhronosGroup/glslang/blob/master/LICENSE.txt) Copyright (c) MIT License, The Khronos Group Inc. 2020
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers/blob/master/LICENSE.txt) Copyright (c) Apache License 2.0, The Khronos Group Inc. 2019-2021
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/blob/master/LICENSE.txt) Copyright (c) MIT, Advanced Micro Devices 2017-2021
- [volk](https://github.com/zeux/volk/blob/master/LICENSE.md) Copyright (c) MIT, Arseny Kapoulkine 2018-2019

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
Please make sure to update tests as appropriate.

## License
HyperEngine is distributed under the [MIT](https://github.com/SkillerRaptor/HyperEngine/blob/master/LICENSE) license.
