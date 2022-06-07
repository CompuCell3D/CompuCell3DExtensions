project(CC3DMaBoSS)
cmake_minimum_required(VERSION 3.18)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if(NOT CC3D_EXT_NAME)
    message(FATAL_ERROR "Extension name not specified")
endif()

set(EXT_NAME mabosscc3d)
set(EXT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MABOSS_MAXNODES)
    set(MABOSS_MAXNODES 64)
endif()

find_package(Python COMPONENTS Interpreter Development REQUIRED)

if(MINGW)
    set(EXT_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/bin")
else()
    set(EXT_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()
set(EXT_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include/${CC3D_EXT_NAME}/${EXT_NAME}")
set(EXT_PYMODULE_DIR "$ENV{SP_DIR}/${CC3D_EXT_NAME}")

add_subdirectory(cpp)
add_subdirectory(python)
