# Fix clang
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get -qq update
sudo apt-get -qq install build-essential
sudo apt-get -qq install llvm
sudo apt-get -qq install libstdc++-4.8-dev
sudo apt-get -qq install luarocks

# Install coveralls
sudo pip install cpp-coveralls

# I give up...
wget 'http://sourceforge.net/projects/luabinaries/files/5.2.3/Linux%20Libraries/lua-5.2.3_Linux32_64_lib.tar.gz/download' -O lua.tar.gz
tar xf lua.tar.gz
rm -f liblua52.so

clang++ -o ytest -std=c++1y -I./include -L`pwd`/ tests/tests.cpp -llua52 -ldl && ./scripts/buildmoonscripts.sh && ./ytest
