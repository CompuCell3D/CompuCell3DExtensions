set(CMAKE_HAVE_LIBC_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(PTHREADS_LIB Threads::Threads)

add_subdirectory(engine)

include_directories(engine/src)

set(CC3DMaBoSS_SRCS
    CC3DMaBoSS.cpp
)
set(CC3DMaBoSS_HDRS
    CC3DMaBoSS.h
    CC3DMaBoSSDLLSpecifier.h
)

# #################
# # Build library #
# #################

set(target_name cc3deMaBoSS)

add_library(${target_name} SHARED ${CC3DMaBoSS_SRCS})
target_include_directories(${target_name} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(${target_name} PRIVATE -DMAXNODES=${MABOSS_MAXNODES})
target_link_libraries(${target_name} cc3deMaBoSSeng)

###########
# Install #
###########

install(TARGETS ${target_name} DESTINATION ${EXT_INSTALL_LIBDIR})
install(
    FILES 
    ${CC3DMaBoSS_HDRS} 
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE 
    DESTINATION 
    ${EXT_INSTALL_INCLUDEDIR}
)
