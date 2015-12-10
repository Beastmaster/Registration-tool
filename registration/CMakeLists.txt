message("seek to Register")
cmake_minimum_required(VERSION 2.8)
 
PROJECT(itk_register)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})



include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

#add_library(itk_register Register.hpp)

