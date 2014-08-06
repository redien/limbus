Limbus
======
A cross-platform multimedia library aimed at supporting a wide range of programming languages. It exposes a set of C APIs used to generate high-level object-oriented bindings for supported languages at compile-time.

Currently binding generators exists for C++, C#, Lua, Python and Ruby

Current status
------
Current release is an alpha release which means that it has not gone through a lot of testing yet. The core APIs such as window/opengl_context/mouse/keyboard should be fairly stable but will most likely get minor changes until the library is stable enough for an actual stable release.

The Lua and C++ bindings are the ones that have been developed and tested the most but the other bindings should be functional.

How to compile
------
Building the library should be straightforward. Use CMake on the directory to generate project files and put them into the 'build' sub-directory. Then compile the project with the compiler you specified.
Static libraries should compile to the 'lib' sub-directory and shared libraries (or DLLs) will compile to the 'bin' directory. Generated bindings will be placed in the 'generated' sub-directory.

Feel free to email me (jesosk@gmail.com) if you're having trouble compiling the library.

Copying
------
Limbus - A cross-platform multimedia library.
Written in 2014 by Jesper Oskarsson jesosk@gmail.com

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software.
If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
