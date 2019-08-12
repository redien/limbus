#!/usr/bin/env bash

project_root=..
mkdir -p build ; cd build

if [ -z "$SDL_PATH" ]; then
    SDL_PATH=/usr/local/Cellar/sdl2/*
fi
SDL_INCLUDE_PATH=$SDL_PATH/include/SDL2
SDL_LIB_PATH=$SDL_PATH/lib

flags=-Wall

cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/generic/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/platform/sdl/**/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/platform/unix/**/*.c
cc -c $flags -I $project_root/include -I $SDL_INCLUDE_PATH $project_root/source/util/*.c

cc -shared -fpic -L $SDL_LIB_PATH -lSDL2 *.o -o liblimbus.dylib
