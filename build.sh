#!/usr/bin/env bash

project_root=..
mkdir -p build ; cd build

if [ "`uname`" == "Darwin" ]; then
    if [ -z "$SDL_PATH" ]; then
        SDL_PATH="/usr/local/Cellar/sdl2/*"
    fi
    PLATFORM_COMPILE_FLAGS="-I $SDL_PATH/include/SDL2"
    PLATFORM_LINK_FLAGS="-L $SDL_PATH/lib -lSDL2"
    PLATFORM_SOURCE_FILES="$project_root/source/platform/sdl/**/*.c"
    PLATFORM_ARTIFACT_NAME="liblimbus.dylib"
fi

cc -c -Wall \
      -I $project_root/include \
      $PLATFORM_COMPILE_FLAGS \
      $project_root/source/generic/*.c \
      $project_root/source/util/*.c \
      $project_root/source/platform/unix/**/*.c \
      $PLATFORM_SOURCE_FILES

cc -shared -fpic \
   $PLATFORM_LINK_FLAGS \
   *.o \
   -o $PLATFORM_ARTIFACT_NAME
