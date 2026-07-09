cmake_minimum_required(VERSION 3.20)
project(column_generation_cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PATHWYSE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# PathWyse: use the already-built shared library and its public headers,
find_library(PATHWYSE_CORE_LIBRARY
        NAMES pathwyse_core
        PATHS "${PATHWYSE_ROOT}/bin"
        NO_DEFAULT_PATH)

if(NOT PATHWYSE_CORE_LIBRARY)
    message(FATAL_ERROR "libpathwyse_core.so not found under ${PATHWYSE_ROOT}/bin - run ./install.sh first")
endif()

# Gurobi (used by the RMP)
#
# On some setups, the prebuilt libgurobi_c++.a shipped with Gurobi was built
# against an older libstdc++ ABI and fails to link with "undefined reference
# to GRBModel::..." errors when using a newer compiler. If you hit that,
# reconfigure with -DGUROBI_CXX_BUILD_FROM_SOURCE=ON. 
option(GUROBI_CXX_BUILD_FROM_SOURCE "Rebuild the Gurobi C++ wrapper from source instead of linking the prebuilt libgurobi_c++.a" OFF)

find_path(GUROBI_INCLUDE_DIR gurobi_c++.h
        HINTS "$ENV{GUROBI_HOME}/include")

file(GLOB _gurobi_versioned_libs "$ENV{GUROBI_HOME}/lib/libgurobi[0-9]*.so")
foreach(_lib ${_gurobi_versioned_libs})
    get_filename_component(_lib_name ${_lib} NAME_WE)
    if(_lib_name MATCHES "^libgurobi([0-9]+)$")
        set(GUROBI_VERSIONED_NAME "gurobi${CMAKE_MATCH_1}")
    endif()
endforeach()

find_library(GUROBI_C_LIBRARY
        NAMES ${GUROBI_VERSIONED_NAME}
        HINTS "$ENV{GUROBI_HOME}/lib")

if(NOT GUROBI_INCLUDE_DIR OR NOT GUROBI_C_LIBRARY)
    message(FATAL_ERROR "Gurobi not found - set the GUROBI_HOME environment variable")
endif()

if(GUROBI_CXX_BUILD_FROM_SOURCE)
    find_path(GUROBI_CXX_SRC_DIR Model.cpp
            HINTS "$ENV{GUROBI_HOME}/src/cpp")

    if(NOT GUROBI_CXX_SRC_DIR)
        message(FATAL_ERROR "GUROBI_CXX_BUILD_FROM_SOURCE is ON but $ENV{GUROBI_HOME}/src/cpp was not found")
    endif()

    file(GLOB GUROBI_CXX_SOURCES "${GUROBI_CXX_SRC_DIR}/*.cpp")
    add_library(gurobi_cxx_local STATIC ${GUROBI_CXX_SOURCES})
    target_include_directories(gurobi_cxx_local PUBLIC ${GUROBI_INCLUDE_DIR})
    set(GUROBI_CXX_TARGET gurobi_cxx_local)
else()
    find_library(GUROBI_CXX_LIBRARY
            NAMES gurobi_c++
            HINTS "$ENV{GUROBI_HOME}/lib")

    if(NOT GUROBI_CXX_LIBRARY)
        message(FATAL_ERROR "libgurobi_c++ not found - set the GUROBI_HOME environment variable")
    endif()

    set(GUROBI_CXX_TARGET ${GUROBI_CXX_LIBRARY})
endif()

add_executable(column_generation_cpp
        console.h
        duals.h
        master.h
        master.cpp
        pricer.h
        pricer.cpp
        main.cpp)

target_include_directories(column_generation_cpp PRIVATE
        "${PATHWYSE_ROOT}/src/core"
        ${GUROBI_INCLUDE_DIR})

target_link_libraries(column_generation_cpp PRIVATE
        ${PATHWYSE_CORE_LIBRARY}
        ${GUROBI_CXX_TARGET}
        ${GUROBI_C_LIBRARY})
