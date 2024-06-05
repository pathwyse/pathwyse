# Install

We provide simple scripts to install PathWyse, along with a step-by-step manual installation. Please choose either method. 

The scripts will compile the library, the standalone version and a cython wrapper and write the binaries under the **bin** folder.

## Linux

The requirements for installing PathWyse under Linux are the following:
- A C++20 Compiler
- CMake
- A Python installation (optional, necessary for Python integration)
- pip installation (optional, necessary for Python integration)

We tested PathWyse under Linux (Kubuntu 22.04), with GCC 11.4.0, CMake 3.29.2, Python 3.10.12 and pip 22.0.2.

### a) Install Script

If the requirements are already met, open a terminal and launch the script install.sh in the root folder of the project.

```
./install.sh
```

It might be necessary to grant the script execution permissions. If so, run the following command before launching the script:

```
chmod +x install.sh
```
After a successful install, you should find 3 files under the bin folder:
- The shared library: libpathwyse_core.so 
- The standalone executable: pathwyse
- A Python wrapper.

### b) Manual installation

Please, open a terminal and move to the root folder of the project. Then:

1) Create a build folder and move into it
```
mkdir build
cd build
```
2) Run CMake to generate build files and build the project
```
cmake ..
make -j 8
```

After these steps, you should find 2 files in the bin folder: libpathwyse.so (the library) and pathwyse (a standalone executable).

(optional) To install the Python wrapper

3) Navigate back to the parent directory and install additional Python requirements
```
cd ..
pip install -r src/wrapper_python/requirements.txt
```
5) Build the Python extension module
```
python3 src/wrapper_python/setup.py  build_ext -j 8 --build-lib="bin/"
```

After step 5, the Python wrapper will be added to the bin folder.


## Windows

The requirements for installing PathWyse under Windows are the following:
- Microsoft Visual C++ (MSVC) compiler 
- CMake
- A Python installation (optional, necessary for Python integration)
- pip installation (optional, necessary for Python integration)

We remark that MSVC is a hard requirement to compile the Cython wrapper, other projects, such as MinGW, can be used to compile the library.
In the following, the script and installation guide are tailored around MSVC.

We tested PathWyse under Windows 11, with MSVC 2022, CMake 3.29.2, Python 3.10.12 and pip 22.0.2.

### a) Install Script

Double click and run the script **install.bat** in the root folder of the project. 

Additional permissions might have to be granted to run the script.

After a successful install, you should find 5 files under the bin folder: 
- Shared library: pathwyse_core.dll
- Additional library files, used for linking: pathwyse_core.lib, pathwyse_core.ext
- Executable: pathwyse.exe
- The Python wrapper

### b) Manual installation
Please, open a terminal and move to the root folder of the project. Then:

1) Create a build folder and move into it
```
mkdir build
cd build
```
2) Run CMake to generate build files and build the project
```
cmake ..
cmake --build . --config Release -j 8
```

After these steps, you should find 4 files in the bin folder: pathwyse.exe, pathwyse_core.dll, pathwyse_core.lib, pathwyse_core.ext

(optional) To install the Python wrapper

3) Navigate back to the parent directory and install additional Python requirements
```
cd ..
pip install -r src\wrapper_python\requirements.txt
```
5) Build the Python extension module
```
python src\wrapper_python\setup.py build_ext -j 8 --build-lib=bin\
```

After step 5, the Python wrapper will be added to the bin folder.

## macOS

At this time, macOS support has not been tested yet; it should be possible to compile the library by following steps similar to the Linux installation guide, if a compiler with C++20 features is installed.

## Common Issues

- If compilation fails due to missing libraries, it is likely that the compiler does not support C++20. Please update your compiler or consider using a different one.