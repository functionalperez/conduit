# conduit

conduit is an educational/experimental library for coroutines as they were introduced in C++20. While I hope that this library will prove useful to the community. 

## Building and running `example`

In order to build `example.cpp`, execute the following commands:
```
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
```
# Download
git clone https://github.com/jantonioperez/conduit.git

# Build
cd conduit
g++-10 -Iinclude -fcoroutines -std=c++20 example.cpp -o run_example

# Run
./run_example
```
