cmake_minimum_required(VERSION 3.16)
project(CFD_Parallelized_MPI)

set(CMAKE_CXX_STANDARD 14)

include_directories(includes)
include_directories(tests)
include_directories(tools/VTK_Tools)
include_directories(tools/VTK_Tools/tests)

add_executable(CFD_Parallelized_MPI
        includes/boundary_val.hpp
        includes/cell.hpp
        includes/datastructures.hpp
        includes/enums.hpp
        includes/grid.hpp
        includes/helper.hpp
        includes/init.hpp
        includes/parallel.hpp
        includes/sor.hpp
        includes/uvp.hpp
        includes/visual.hpp
        src/boundary_val.cpp
        src/cell.cpp
        src/grid.cpp
        src/helper.cpp
        src/init.cpp
        src/main.cpp
        src/parallel.cpp
        src/sor.cpp
        src/uvp.cpp
        src/visual.cpp
        tests/catch.hpp
        tests/catch_config.hpp
        tests/sample_test.cpp
        tools/VTK_Tools/tests/catch.hpp
        tools/VTK_Tools/tests/main.cpp
        tools/VTK_Tools/tests/merge.cpp
        tools/VTK_Tools/Compare.cpp
        tools/VTK_Tools/cxxopts.hpp
        tools/VTK_Tools/FileIterator.cpp
        tools/VTK_Tools/FileIterator.hpp
        tools/VTK_Tools/helper.hpp
        tools/VTK_Tools/Merge.cpp
        tools/VTK_Tools/Reader.cpp
        tools/VTK_Tools/Reader.hpp)
