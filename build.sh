#!/usr/bin/env bash

project_root=..
mkdir -p build ; cd build

flags=-Wall

cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/generic/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/platform/sdl/**/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/platform/unix/**/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/util/*.c

ar rcs liblimbus.a *.o
