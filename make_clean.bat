
:: Limbus - A cross-platform multimedia library.
:: Written in 2014 by Jesper Oskarsson jesosk@gmail.com
::
:: To the extent possible under law, the author(s) have dedicated all copyright
:: and related and neighboring rights to this software to the public domain worldwide.
:: This software is distributed without any warranty.
::
:: You should have received a copy of the CC0 Public Domain Dedication along with this software.
:: If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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
