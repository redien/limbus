#!/usr/bin/env bash

project_root=../..
mkdir -p build/examples ; cd build/examples

flags="-Wall"
limbusflags="-I $project_root/include -L $project_root/build/ -llimbus"
sdlflags="-L $SDL_LIB_PATH -lSDL2"
openglflags="-framework OpenGL"

compile_example() {
    cc -o $1 $flags $limbusflags $sdlflags $openglflags $project_root/examples/c/$1.c
}

compile_example opengl_example
compile_example opengl_window
compile_example screen_example
compile_example timing_example
