
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cd dependencies

echo Fetching Lua 5.1.4...
wget -q http://www.lua.org/ftp/lua-5.1.4.tar.gz
gzip -d lua-5.1.4.tar.gz
tar -xf lua-5.1.4.tar
rm lua-5.1.4.tar

echo Installing .deb packages...
sudo apt-get -y -qq install libx11-dev
sudo apt-get -y -qq install libgl1-mesa-dev
sudo apt-get -y -qq install libxrandr-dev
sudo apt-get -y -qq install libasound2-dev

