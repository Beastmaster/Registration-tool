message("seek to SubWidgetBold")
cmake_minimum_required(VERSION 2.8)
 
PROJECT(MinWindow)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#find_package(Qt4 REQUIRED)
#include(${QT_USE_FILE})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)


#include dependent head files
include_directories(
../registration
)
#include source dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(Plain_CXX
ImageConvert.cpp
DicomParse.cpp
RegistrationProcess.cpp
)
set(Plain_Head
ImageConvert.h
DicomParse.h
RegistrationProcess.h
../registration/Register.h
../registration/Register.hpp
)

set(QT_UI_FILES
MainWindow.ui
)

#qt .h files
set(QT_HEAD_FILES
MainWindow.h
MarchingCube_construct.h
ResliceView.h
ImageViewer2View.h
ResliceView.h
)

#qt .cxx files
SET(QT_CXX_FILES
MainWindow.cpp
MarchingCube_construct.cpp
ResliceView.cpp
ImageViewer2View.cpp
ResliceView.cpp
)

QT5_WRAP_UI(UISrcs ${QT_UI_FILES})
QT5_WRAP_CPP(MOCSrcs ${QT_HEAD_FILES})
add_library(MainWindow ${QT_CXX_FILES} ${UISrcs} ${MOCSrcs} ${QT_HEAD_FILES} ${Plain_CXX} ${Plain_Head})

#target_link_libraries(MainWindow widget_bold itk_register qcustomplot fmriEngine itkStripTsImageFilter Dialog_Advance)

if(VTK_LIBRARIES)
  if(${VTK_VERSION} VERSION_LESS "6")
    target_link_libraries(MainWindow ${VTK_LIBRARIES} ${ITK_LIBRARIES} QVTK ${ITK_VERSION_MAJOR} )
  else()
    target_link_libraries(MainWindow ${VTK_LIBRARIES} ${ITK_LIBRARIES} ${ITK_VERSION_MAJOR})
  endif()
else()
  target_link_libraries(MainWindow vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
endif()