#!/bin/sh
mkdir -p build
g++ -g3 -std=c++20 -Isrc/ -Isrc/kl -Isrc/klb src/klb/*cpp src/kl/*cpp src/ff/*cpp -o build/klb.exe

