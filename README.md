# Important note !!!!!

In order for the code to be run, 

1. Create a folder with a name `build`

2. Change directory and run the following commands

```shell
cmake ..
make
```


3. .dat file should be moved to build folder

4. Program should be run with appropriate number of processes that is `iproc * jproc` (it is 6 for the test case)

EXAMPLE:

```shell
mpirun -np 6 ./sim
```

5. Created vtk files have an order such that `Solution.omg_iomg_j.stepnum.vtk`

# CFD Lab code skeleton

Code Skeleton for the CFD Lab taught at TUM Informatics

This repository contains:

* an example input file (`cavity100.dat`), with `100` stemming from the `itermax`
* the headers
* the files with the respective method stubs

Please [fork this repository and add us as collaborators](https://gitlab.lrz.de/tum-i05/public/cfdlabcodeskeleton/-/wikis/home).

Find more information in the [documentation](https://tum-i05.pages.gitlab.lrz.de/public/cfdlabcodeskeleton/).

## Software Requirements

* VTK 7 or higher
* GCC 9 (optional) 

### GCC version

You can get you current version of GCC by running:

```shell
g++ -v
```

### Defining your GCC version

If you have GCC 9 or newer, you can set in the `CMakeLists.txt` file:

```cmake
set(gpp9 True)
```

If you have a version lower than 9, then you don't have to modify the `CMakeLists.txt` file.

This will affect how we are using the C++ filesystem library, which is available already in GCC 7 as an experimental feature.

### Setup of VTK and GCC 9 (Ubuntu **20.04**)

```
apt-get update &&
apt-get upgrade -y &&
apt-get install -y build-essential cmake libvtk7-dev libfmt-dev
```

### Setup of VTK and GCC 9 (Ubuntu **18.04**)

If you want, you can upgrade your compiler version to have access to more recent C++ features.
This is, however, optional.

```
apt-get update &&
apt-get install -y software-properties-common &&
add-apt-repository -y ppa:ubuntu-toolchain-r/test &&
apt-get upgrade -y &&
apt-get install -y build-essential cmake libvtk7-dev libfmt-dev gcc-9 g++-9
apt-get install -y gcc-9 g++-9
```

## Using CMake

CMake is a C++ build system generator, which simplifies the building process compared e.g. to a system-specific Makefile. The CMake configuration is defined in the `CMakeList.txt` file.

In order to build your code with CMake, you can follow this (quite common) procedure:

1. Create a build directory: `mkdir build`
2. Get inside it: `cd build`
3. Configure and generate the build system: `cmake ..` (Note the two dots, this means that the `CmakeLists.txt` File is in the folder above)
4. Build your code: `make` (build the executable)

### Troubleshooting: VTK not found

You might run into a problem where the VTK library is not found. To fix this, you can try the following steps:

1. Find the installation path of your VTK library 
2. Define this path as an environment variable, as e.g. `export VTK_DIR=".../lib/cmake/vtk-8.2"`
3. Start in a clean build folder
4. Run `cmake ..` again

### Set a different GCC version

If you have multiple compiler versions installed you can set the GCC version which should be used by `cmake` like this:

```shell
export CXX=`which g++-7`
```

Make sure to use a backtick (\`) to get the `which` command executed. Afterwards, you can run `cmake ..`.

## Testing 

The folder `tests` contains example files for simple unit testing with the [Catch2](https://github.com/catchorg/Catch2) unit testing framework. The tests can be compiled using `cmake` as well.

After building, run:

```
ctest --verbose
```

With the `verbose` option you can get more details for failing tests.

## Documentation 

The generate the documentation, run:

```
pip3 install --user mkdocs mkdocs-material
```

and then serve it to be able to access it through your browser:

```
mkdocs serve
```
