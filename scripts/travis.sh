# Fix clang
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get -qq update
sudo apt-get -qq install build-essential
sudo apt-get -qq install llvm
sudo apt-get -qq install libstdc++-4.8-dev

# Install coveralls
sudo pip install cpp-coveralls

# Lua
mkdir dependencies
cd dependencies
git clone https://github.com/LuaDist/lua.git --quiet
cd lua
cmake .
make
sudo make install

# Debug
find /usr/include|grep lua

cmake .
make
./ytest
