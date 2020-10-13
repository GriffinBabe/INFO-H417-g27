# INFO-H417 Database Architectures - Group 27

This is the repository for the project of the INFO-H417 course group 27, year 2020-2021 at the "Ecole Polytechnique de Bruxelles"  faculty.

Authors:
- Darius Couchard
- Stefano Donne
- Cédrick De Pauw

## Installation Process

Required libraries and tools (can be installed with apt-get on ubuntu):

- build-essential (need to support c++17)
- cmake
- libboost-all-dev
- clang-format-10

If your are using CLion, just open the project in the root folder. CMake will be run automatically by the IDE and create a `cmake-build-debug` or `cmake-build-release` folder. 

You can manually create a folder (for example `build/` but can be anything as long as you put it on the `.gitignore` file), the call CMake manually

```
cd <build_folder>
cmake .. // will create the build files (makefile for linux, .sln for Visual Studio, ...)
make // compiles the file with gcc
```

You also need to manually download the database and put it in the `database/` folder.

## Clang-Format

I don't want to impose clang format, but it's a very handy tool to format the spacing of your code. You can configure it with CLion and add a shortcut to it, or simply call it from the terminal.
The `.clang-format` file is the formatter configuration file.

```
clang-format -i *.cpp // will format all cpp files in the folder and subfolders
clang-format -i *.hpp // obvious
```