![Conduit Logo](/.github/images/logo.png)

# Conduit Coroutine Library for C++

conduit is an educational/experimental library for coroutines as they were introduced in C++20. While I hope that this library will prove useful to the community. 

## Table of contents
* [Using Conduit](#using-conduit)
    * [Add to an existing project via git submodule](#Add-to-an-existing-project-via-git-submodule)
        * [Running tests](#running-tests)
    * [Installing system-wide](#installing-system-wide)
        * [Installing with cmake](#installing-with-cmake)
        * [Installing on Ubuntu](#installing-on-Ubuntu)
        * [Manual installation for arbitrary linux systems](#manual-installation-for-arbitrary-linux-systems)
        * [Using the installed library](#using-the-installed-library)

## Using Conduit

### Add to an existing project via git submodule

Conduit can be added to an existing git repository by running
```bash
git submodule add https://github.com/functionalperez/conduit.git
```
This will add conduit as a submodule of your project, and whenever anyone clones your project via `git clone --recursive <Your Project>`, conduit will be automatically downloaded as well.

If you're taking this route and your project uses cmake, just add this line to your `CMakeLists.txt` folder and the compiler will find conduit:
```cmake
add_subdirectory(conduit)
```
#### Running tests

In order to ensure that your system is set up correctly, it's recommended to compile and run `main.cpp`, which contains tests for conduit.
```bash
# Download
git clone https://github.com/functionalperez/conduit.git

# Build
cd conduit
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++
cmake --build .

# Run
./run_tests
```

Alternatively, if you don't want to use CMake, just run the following commands in the root directory of this project:
```bash
g++-10 -Iinclude -fcoroutines -std=c++20 main.cpp -o run_example

./run_example
```
### Installing system-wide

You can install Conduit on a system-wide basis, and this will allow you to use conduit in C++ projects without adding any additional compiler flags and without adding anything to your build file (although you'll still need to compile your project for C++20).

#### Installing with cmake

In order to install conduit via cmake, run the following commands in the root directory of this project:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++
cmake --install .

# Optionally, you can remove the repository once you've installed it:
cd ../..
rm -rf conduit
```

#### Installing on Ubuntu

On Ubuntu, installation would be as follows:

1. Download conduit.deb from the assets section of [the most recent release](https://github.com/functionalperez/conduit/releases)
2. Go to your downloads folder (`cd ~/Downloads`)
3. Run `sudo apt install ./conduit.deb` (or just double-click the file to open it in the installer)

This will install Conduit in `/usr/local/include`, and Conduit can be uninstalled at any time by running `sudo apt remove conduit`.

#### Manual installation for arbitrary linux systems

Conduit can also be installed manually by running:
```bash
git clone https://github.com/functionalperez/conduit.git
sudo cp -r conduit/include/conduit /usr/local/include

# Optionally, remove the repo after installation as it's no longer needed
rm -rf conduit
```
To uninstall manually, simply run
```bash
sudo rm -r /usr/local/include/conduit
```

#### Using the installed library

Once you've installed Conduit, you can use it as you would any other installed library:
```cpp
// main.cpp
#include <conduit/coroutine.hpp>
#include <conduit/generator.hpp>

#include <string>
#include <iostream>

using namespace conduit;

generator<char> hello_message() {
    for(char c : "Hello, world!") {
        co_yield c;
    }
}

int main() {
    for(char c : hello_message()) {
        std::cout << c;
    }
    std::cout << std::endl;
}
```
```bash
clang++ main.cpp -std=c++20 -o main
```
