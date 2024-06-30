# 使用指南

## 工程简介

这个工程实现了图像的一些结构体，并且主要使用C函数进行实现。

## 编译命令

### Windows

```
cmake -B build -G "MinGW Makefiles"

cmake --build build --config Release --target install
```

[Link](https://github.com/Jia-Baos/Notes/blob/master/CMake/cmake-command.md)

### Linux

```
sudo mkdir build && cd build

sudo cmake ..

sudo make

sudo make install
```

## git添加子模块

```
git submodule add https://github.com/google/benchmark.git 3rdparty/benchmark

git submodule add https://github.com/google/googletest.git 3rdparty/googletest
```

若出现错误信息`fatal: 'benchmark' already exists in the index`，则按下面方式删除记录后再重新添加即可。

```
git rm -r --cached 3rdparty/benchmark

git rm -r --cached 3rdparty/googletest
```
