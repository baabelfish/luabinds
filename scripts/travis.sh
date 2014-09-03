# Fix clang
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get -qq update
sudo apt-get -qq install build-essential
sudo apt-get -qq install llvm
sudo apt-get -qq install libstdc++-4.8-dev

# Install coveralls
sudo pip install cpp-coveralls

cd tests
clang++ -Wall -o ytests *.cpp -std=gnu++11 -lpthread && ./ytests
# && coveralls --exclude lib --exclude tests
