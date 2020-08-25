#!/bin/sh

version=`cat VERSION`

mkdir -p build_package/conduit
cd build_package/conduit
mkdir -p DEBIAN
echo "Package: conduit
Version: $version
Maintainer: A. Perez <perez.cs@pm.me>
Description: Conduit Coroutine Library for C++
Homepage: https://github.com/functionalperez/conduit
Architecture: all
Depends: g++-10, clang-10" > DEBIAN/control

mkdir -p usr/local/include
cp -r ../../include/conduit usr/local/include

cd ..
dpkg -b ./conduit ./conduit_${version}-0_all.deb
