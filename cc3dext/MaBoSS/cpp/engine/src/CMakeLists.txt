macro(MABOSS_GEN source_file target_file label)
    # Read the source file, make replacements and output target file
    file(READ ${source_file} source_string)
    string(REPLACE "yy" ${label} s "${source_string}")
    string(REPLACE "register" "" target_string "${s}")
    file(WRITE ${target_file} "${target_string}")
endmacro(MABOSS_GEN)

SET(MaBoSS_SRCS 
    BooleanGrammar.cc 
    BooleanNetwork.cc 
    BuiltinFunctions.cc 
    Cumulator.cc 
    EnsembleEngine.cc 
    FinalStateDisplayer.cc 
    FinalStateSimulationEngine.cc 
    FixedPointDisplayer.cc 
    Function.cc 
    LogicalExprGen.cc 
    MaBEstEngine.cc 
    MBDynBitset.cc 
    MetaEngine.cc 
    ProbaDist.cc 
    ProbTrajDisplayer.cc 
    RunConfig.cc 
    RunConfigGrammar.cc 
    StatDistDisplayer.cc 
    StochasticSimulationEngine.cc 
    user_func.cc 
    Utils.cc 
    maboss-config.cpp 
)

SET(MaBoSS_HDRS
    BooleanGrammar.h 
    BooleanNetwork.h 
    Client.h 
    Cumulator.h 
    DataStreamer.h 
    EnsembleEngine.h 
    FinalStateDisplayer.h 
    FinalStateSimulationEngine.h 
    FixedPointDisplayer.h 
    Function.h 
    MaBEstEngine.h 
    MBDynBitset.h 
    MetaEngine.h 
    ProbaDist.h 
    Probe.h 
    ProbTrajDisplayer.h 
    RandomGenerator.h 
    RPC.h 
    RunConfig.h 
    Server.h 
    StatDistDisplayer.h 
    StochasticSimulationEngine.h 
    Utils.h 
    maboss-config.h 
)

set(MaBoSS_AUXS 
    BooleanGrammar.l
    BooleanGrammar.y
    RunConfigGrammar.l
    RunConfigGrammar.y
)

set(MaBoSS_LIBS_IMPORTED ${PTHREADS_LIB})

if(MINGW)
    list(APPEND MaBoSS_SRCS ../extern/sys/times.cpp)

    find_library(LIBDL_IMPORTED NAMES dl libdl REQUIRED)
    add_library(LIBDL_IMPORTED SHARED IMPORTED)
    list(APPEND MaBoSS_LIBS_IMPORTED ${LIBDL_IMPORTED})
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

####################
# Generate scripts #
####################

# BooleanGrammar.cc
execute_process(
    COMMAND ${FLEX_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/BooleanGrammar.l"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_BINARY_DIR}/lex.yy.c" "${CMAKE_BINARY_DIR}/lex.CTBNDL.c")
MABOSS_GEN("${CMAKE_BINARY_DIR}/lex.CTBNDL.c" "${CMAKE_CURRENT_SOURCE_DIR}/lex.CTBNDL.cc" "CTBNDL")

execute_process(
    COMMAND ${BISON_EXECUTABLE} -v "${CMAKE_CURRENT_SOURCE_DIR}/BooleanGrammar.y"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
MABOSS_GEN("${CMAKE_BINARY_DIR}/BooleanGrammar.tab.c" "${CMAKE_CURRENT_SOURCE_DIR}/BooleanGrammar.cc" "CTBNDL")

file(COPY "${CMAKE_BINARY_DIR}/BooleanGrammar.tab.c" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

# RunConfigGrammar.cc
execute_process(
    COMMAND ${FLEX_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/RunConfigGrammar.l"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_BINARY_DIR}/lex.yy.c" "${CMAKE_BINARY_DIR}/lex.RC.c")
MABOSS_GEN("${CMAKE_BINARY_DIR}/lex.RC.c" "${CMAKE_CURRENT_SOURCE_DIR}/lex.RC.cc" "RC")

execute_process(
    COMMAND ${BISON_EXECUTABLE} -v "${CMAKE_CURRENT_SOURCE_DIR}/RunConfigGrammar.y"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
MABOSS_GEN("${CMAKE_BINARY_DIR}/RunConfigGrammar.tab.c" "${CMAKE_CURRENT_SOURCE_DIR}/RunConfigGrammar.cc" "RC")

file(COPY "${CMAKE_BINARY_DIR}/RunConfigGrammar.tab.c" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

#################
# Build library #
#################
set(target_name cc3deMaBoSSeng)

add_library(${target_name} SHARED ${MaBoSS_SRCS})
target_link_libraries(${target_name} ${MaBoSS_LIBS_IMPORTED})
if(MINGW)
    target_include_directories(${target_name} PUBLIC ../extern)
endif()
target_compile_options(${target_name} PRIVATE -DMAXNODES=${MABOSS_MAXNODES})

###########
# Install #
###########

install(TARGETS ${target_name} DESTINATION ${EXT_INSTALL_LIBDIR})
install(FILES ${MaBoSS_HDRS} DESTINATION ${EXT_INSTALL_INCLUDEDIR}/engine/src)
if(MINGW)
    install(FILES ../extern/sys/times.h DESTINATION ${EXT_INSTALL_INCLUDEDIR}/extern/sys)
endif()
