rmdir /Q /S bin
mkdir bin
rmdir /Q /S lib
mkdir lib
rmdir /Q /S build
mkdir build
rmdir /Q /S doc
mkdir doc

del /Q generated\*.*
del /Q generated\cpp\*
del /Q generated\lua\*
del /Q generated\ruby\*
del /Q generated\python\*
