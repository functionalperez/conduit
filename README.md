# Conduit Coroutine Library for C++

conduit is an educational/experimental library for coroutines as they were introduced in C++20. While I hope that this library will prove useful to the community. 

## Building and running `example`

In order to build `example.cpp`, execute the following commands:
```bash
# Download
git clone https://github.com/jantonioperez/conduit.git

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
git clone https://github.com/jantonioperez/conduit.git

# Build
cd conduit
g++-10 -Iinclude -fcoroutines -std=c++20 example.cpp -o run_example

# Run
./run_example
```
## Installing

Conduit can be used in one of two ways. You could add this repository as submodule of your project via `git submodule`, or you can install it system-wide. 

On Ubuntu, installation would be as follows:

1. Download [conduit.deb](https://github.com/functionalperez/packages/raw/main/conduit/conduit.deb) (This is a download link)
2. Go to your downloads folder (`cd ~/Downloads`)
3. Run `sudo apt install ./conduit.deb` (or just double-click conduit.deb in your file manager)

This will install Conduit in `/usr/local/include`, and you'll be able to use Conduit like you would any other system library:

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

Conduit can be uninstalled at any time by running `sudo apt remove conduit`.

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
