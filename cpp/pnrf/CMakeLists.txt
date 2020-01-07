# CMakeList.txt : CMake project for pnrfr_cmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)

# Add source to this project's executable.
file(GLOB SOURCES "*.cpp")
add_library(pnrf SHARED ${SOURCES})

#add_library (pnrf  "stdafx.cpp" "stdafx.h" "targetver.h" 
#"ioffline.h" "filemeta.h" "fileprocessor.h" "pnrferror.h" "pnrfhelper.h" 
#"dllmain.cpp"
#"meta.cpp" "pnrferror.cpp" "pnrfhelper.cpp"
#"dllwrapper.cpp" "dllwrapper.def" "dllwrapper.h")
