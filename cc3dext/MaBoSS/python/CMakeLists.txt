FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(UseSWIG)

# Don't link with python on osx
if(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    set(PYTHON_LIBRARY_TO_LINK )
    set(undefined_link_flags "-undefined dynamic_lookup")
else()
    if(MINGW)
        set(PYTHON_LIBRARY_TO_LINK "${Python3_INCLUDE_DIRS}/../python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}$ENV{SHLIB_EXT}")
    else()
        set(PYTHON_LIBRARY_TO_LINK ${Python3_LIBRARIES})
    endif()
    set(undefined_link_flags " ")

endif()

SET(MaBoSSCC3D_LIBS
    cc3deMaBoSS 
    ${PYTHON_LIBRARY_TO_LINK}
)

set_source_files_properties(MaBoSSCC3D.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_LIBRARY(MaBoSSCC3DPy LANGUAGE python SOURCES MaBoSSCC3D.i)
target_include_directories(MaBoSSCC3DPy PUBLIC
    ${Python3_INCLUDE_DIRS} 
    $<BUILD_INTERFACE:${EXT_ROOT_DIR}/cpp>
    $<BUILD_INTERFACE:${EXT_ROOT_DIR}/cpp/engine/src>
    $<INSTALL_INTERFACE:${EXT_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${EXT_INSTALL_INCLUDEDIR}/engine/src>
)

if(MINGW)
    target_include_directories(MaBoSSCC3DPy PUBLIC 
        $<BUILD_INTERFACE:${EXT_ROOT_DIR}/cpp/engine/extern>
        $<INSTALL_INTERFACE:${EXT_INSTALL_INCLUDEDIR}/engine/extern>
    )
endif()

set_target_properties(MaBoSSCC3DPy PROPERTIES 
    SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE 
    LINK_FLAGS ${undefined_link_flags}
)

if(MINGW)
    # set dll name to pyd for compatibility with Python standard
    set_target_properties(MaBoSSCC3DPy PROPERTIES SUFFIX ".pyd")
endif()

SWIG_LINK_LIBRARIES(MaBoSSCC3DPy ${MaBoSSCC3D_LIBS})


if(APPLE)
   # Use rpath for CC3DMaBoSS.so
   set_target_properties(MaBoSSCC3DPy PROPERTIES MACOSX_RPATH TRUE)
   # append directories in the linker search path and outside the project to the INSTALL_RPATH
   set_target_properties(MaBoSSCC3DPy PROPERTIES CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   # this is where libraries on which CC3DMaBoSS.so depends on are installed relative to where CC3DMaBoSS.py is installed
   # set_target_properties(MaBoSSCC3DPy PROPERTIES INSTALL_RPATH "@loader_path/lib")
   set_target_properties(MaBoSSCC3DPy PROPERTIES INSTALL_RPATH "@loader_path/../../..")
endif()

install(TARGETS MaBoSSCC3DPy DESTINATION ${EXT_PYMODULE_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MaBoSSCC3DPy.py DESTINATION ${EXT_PYMODULE_DIR})
