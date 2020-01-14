# CMakeList.txt : CMake project for pnrfr_cmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_BUILD_TYPE x64-Release)

#IF(${CMAKE_BUILD_TYPE} STREQUAL "x64-Release")
#    SET (CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/x64) 
#    SET (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/out/x64) 
#    SET (CUSTOM_INST_DIR ${CMAKE_SOURCE_DIR}/../inst/libs/x64) 
#elseif(${CMAKE_BUILD_TYPE} STREQUAL "x86-Release")
#    SET (CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/i386) 
#    SET (CUSTOM_INST_DIR ${CMAKE_SOURCE_DIR}/../inst/libs/i386) 
#ENDIF()

include_directories(include)

# Add source to this project's executable.
file(GLOB SOURCES "*.cpp" "def/dllwrapper.def")
add_library(pnrfr SHARED ${SOURCES})
install(TARGETS pnrfr DESTINATION ${CMAKE_SOURCE_DIR}\\..\\inst\\libs\\x64)


#INSTALL(TARGETS pnrfr DESTINATION ${CUSTOM_INST_DIR})