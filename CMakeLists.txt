cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME Reg_Tool)
PROJECT(${PROJECT_NAME})
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#find_package(Qt5 REQUIRED)
#include(${QT_USE_FILE})


# find and use QT5 lib
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)


include_directories(
registration
MainWindow
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(registration)
add_subdirectory(MainWindow)


add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} MainWindow)

#itk_register
#itkStripTsImageFilter
if(VTK_LIBRARIES)
  if(${VTK_VERSION} VERSION_LESS "6")
    target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES} ${ITK_LIBRARIES} QVTK ${ITK_VERSION_MAJOR} )
  else()
    target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${ITK_VERSION_MAJOR})
  endif()
else()
  target_link_libraries(${PROJECT_NAME} vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
endif()