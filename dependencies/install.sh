
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

cd dependencies

echo -n "Which Lua version should be downloaded? (5.2.3) "
read version

if [ -z "$version" ]; then
    version="5.2.3"
fi

echo Fetching Lua $version...

unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then
    curl "http://www.lua.org/ftp/lua-$version.tar.gz" -o "lua-$version.tar.gz"
else
    wget -q "http://www.lua.org/ftp/lua-$version.tar.gz"
fi

gzip -d "lua-$version.tar.gz"
tar -xf "lua-$version.tar"
rm "lua-$version.tar"

if [[ "$unamestr" == 'Darwin' ]]; then
    echo Nothing more to install.
else
    echo Installing .deb packages...
    sudo apt-get -y -qq install libx11-dev
    sudo apt-get -y -qq install libgl1-mesa-dev
    sudo apt-get -y -qq install libxrandr-dev
    sudo apt-get -y -qq install libasound2-dev
fi
