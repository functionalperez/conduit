# Conduit Coroutine Library for C++

conduit is an educational/experimental library for coroutines as they were introduced in C++20. While I hope that this library will prove useful to the community. 

## Building and running `example`

In order to build `example.cpp`, execute the following commands:
```bash
# Download
git clone https://github.com/functionalperez/conduit.git

# Build
cd conduit
mkdir build
cd build
cmake .. && cmake --build .

# Run
./run_example
```

If you modify `example.cpp`, just rebuild by running `cmake --build .`.

Alternatively, if you don't want to use CMake, just run:
```bash
# Download
git clone https://github.com/functionalperez/conduit.git

# Build
cd conduit
g++-10 -Iinclude -fcoroutines -std=c++20 example.cpp -o run_example

# Run
./run_example
```
## Add to an existing project via git submodule

Conduit can be added to an existing git repository by running
```bash
git submodule add https://github.com/functionalperez/conduit.git
```
This will add conduit as a submodule of your project, and whenever anyone clones your project via `git clone --recursive <Your Project>`, conduit will be automatically downloaded as well.

If you're taking this route and your project uses cmake, just add this line to your `CMakeLists.txt` folder and the compiler will find conduit:
```cmake
add_subdirectory(conduit)
```

## Install System-wide for use in any project

You can install Conduit on a system-wide basis, and this will allow you to use conduit in C++ projects without adding any additional compiler flags and without adding anything to your build file (although you'll still need to compile your project for C++20).

### Installing with cmake

You can run the following commands to install conduit using cmake:
```bash
git clone htttps://github.com/functionalperez/conduit.git

cd conduit
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++
cmake --install .

# Optionally, you can remove the repository once you've installed it:
cd ../..
rm -rf conduit
```

### Installing on Ubuntu

On Ubuntu, installation would be as follows:

1. Download [conduit.deb](https://github.com/functionalperez/packages/raw/main/conduit/conduit.deb) (This is a download link)
2. Go to your downloads folder (`cd ~/Downloads`)
3. Run `sudo apt install ./conduit.deb` (or just double-click the file to open it in the installer)

This will install Conduit in `/usr/local/include`, and Conduit can be uninstalled at any time by running `sudo apt remove conduit`.

### Manual installation on linux systems

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

--- 

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