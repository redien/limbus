
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then
    WGET="curl -s "
    WGET_OUT="-o"
else
    WGET="wget -q "
    WGET_OUT="-O"
fi

function extract_tar_gz {
    gzip -d "$1.tar.gz"
    tar -xf "$1.tar"
    rm "$1.tar"
}

function extract_zip {
    unzip -q "$1.zip"
    rm "$1.zip"
}

# Make sure we don't install this in the root directory by mistake
cd dependencies 2> /dev/null



# The specific version of Lua used can be specified
echo -n "Which Lua version should be downloaded? (5.2.3) "
read version

if [ -z "$lua_version" ]; then
    lua_version="5.2.3"
fi

echo Fetching Lua $lua_version...
$WGET "http://www.lua.org/ftp/lua-$lua_version.tar.gz" $WGET_OUT "lua-$lua_version.tar.gz"

echo Extracting Lua $lua_version...
extract_tar_gz "lua-$lua_version"



# This downloads the specific cbind revision used
cbind_revision="bd7e3d747395f855ea8f491c55b03a000c33e32a"

echo "Fetching cbind <$cbind_revision>..."
$WGET "https://codeload.github.com/redien/cbind/zip/$cbind_revision" $WGET_OUT "cbind.zip"
echo "Extracting cbind <$cbind_revision>..."
extract_zip "cbind"
rm -Rf cbind
mv cbind-$cbind_revision cbind



# This installs some system library dependencies
if [[ "$unamestr" == 'Darwin' ]]; then
    echo Nothing more to install.
else
    echo Installing .deb packages...
    sudo apt-get -y -qq install libx11-dev
    sudo apt-get -y -qq install libgl1-mesa-dev
    sudo apt-get -y -qq install libxrandr-dev
    sudo apt-get -y -qq install libasound2-dev
fi
