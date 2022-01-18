#!/bin/sh
mkdir -p build
g++ -g3 -std=c++20 -Isrc/ -Isrc/kl -Isrc/klb src/klb/*cpp src/kl/*cpp src/ff/*cpp -o build/klb.bootstrap -lssl && echo "build/klb.bootstrap is now ready to be used"

