## Download and unpack boost library
Download and uppack boost library under ./include/boost directory.
https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz

```shell
mkdir -p ./include
cd ./include
wget https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz
tar xzf boost_1_88_0.tar.gz
ln -sfT boost_1_88_0 boost
```

## How to build and run a code sample
``` shell
mkdir ./build
cd build
cmake ..
# Build the target using a prefixed number.
# For example, if you want to build 50_gtest_template.cpp, you would use the target "50".
make 50
./run
```