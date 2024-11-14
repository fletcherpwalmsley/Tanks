#!/usr/bin/env bash

# Get Box2d
if [ ! -d "box2d" ]; then
    # Clone the box2d repository from GitHub
    echo "box2d does not exist. Cloning from GitHub..."
    git clone https://github.com/erincatto/box2d.git
fi
cd box2d

# Apply modifications to the box2d Cmake so that it will install
# correctly into the `portlibs` dir in devkitpro
git apply ../box2d_wii.patch

# Slightly modified build.sh script from box2d
rm -rf build
mkdir build
cd build
/opt/devkitpro/portlibs/wii/bin/powerpc-eabi-cmake ..
cmake --build . -j 6
cmake --install .
cd ../..
rm -rf box2d
