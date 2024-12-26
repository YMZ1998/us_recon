cmake_minimum_required (VERSION 3.18) 
cmake_policy(SET CMP0079 NEW)

#project
project(us_recon) 

# 设置 Debug 模式下的编译和链接器选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")

# 设置 Release 模式下的编译和链接器选项，启用调试符号
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")

#set project output directory
SET(PROJECT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../output")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")
set(LIBRARY_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")
set(RUNTIME_OUTPUT_DIRECTORY "${PROJECT_OUTPUT_DIR}")
message(STATUS "PROJECT_OUTPUT_DIR: " ${PROJECT_OUTPUT_DIR})

#build setting: just unicode version
add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
add_definitions(-D_UNICODE -DUNICODE)
SET(CMAKE_DEBUG_POSTFIX d)
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")	
	if (WIN32)
        set(WINXXBITS Win64)
        set(util_postfix _64)
        message(STATUS "Target is 64 bits")
    endif(WIN32)
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	if (WIN32)
        set(WINXXBITS Win32)
        set(util_postfix _32)
        message(STATUS "Target is 32 bits")
    endif(WIN32)
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

#openmp
include (FindOpenMP)
if (OPENMP_FOUND)
      message("build with openmp")
      set (HAVE_OPENMP TRUE)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

#cmake folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

#cuda
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES 70 75 80)
endif(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)

add_definitions(-D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH)

include("${PROJECT_SOURCE_DIR}/macro.cmake")

SET(UTILS_PATH "${PROJECT_SOURCE_DIR}/utils")

#utils
if(NOT UTILS_PATH)
  find_path(UTILS_PATH NAMES utils_path.cmake)
endif()
if(NOT UTILS_PATH)
  message(FATAL_ERROR "no utils found")
endif()
include(${UTILS_PATH}/utils_path.cmake)

subdirs(core)
#projects
option(build_test "Build test" OFF)
if(build_test)
    subdirs(test)
endif(build_test)
