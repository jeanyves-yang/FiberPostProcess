# This is the root ITK CMakeList file:
cmake_minimum_required(VERSION 2.8)

# This project is designed to be built outside the Insight source tree.
project(FiberPostProcess)

# Find SlicerExecutionModel
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

#include(ExternalData)

set(ITK_IO_MODULES_USED
ITKIOImageBase
ITKIONRRD
ITKIOCSV
ITKIOGIPL
ITKIOHDF5
ITKIOIPL
ITKIOImageBase
ITKIOLSM
ITKIOMRC
ITKIOMesh
ITKIOMeta
ITKIONIFTI
ITKIONRRD
ITKIORAW
ITKIOVTK
)
#Find ITK
find_package(ITK 4.7 REQUIRED COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageFunction
  ITKVTK
  ${ITK_IO_MODULES_USED}
)

include(${ITK_USE_FILE})

# Find VTK
find_package(VTK REQUIRED NO MODULE)
set( VTK_VERSION_REQUIRED 6 )
if( VTK_VERSION_MAJOR LESS ${VTK_VERSION_REQUIRED} )
  message( FATAL_ERROR "${PROJECT_NAME} requires VTK ${VTK_VERSION_REQUIRED}. Version Found: ${VTK_VERSION_MAJOR}" )
endif()
include(${VTK_USE_FILE})

include(ExternalData)
set(CXX_FILES
FiberPostProcess.cxx
processing.cxx
utils.cxx
)

set(H_FILES
processing.h
utils.h
csv.h
)

SEMMacroBuildCLI(
  NAME FiberPostProcess
  EXECUTABLE_ONLY
  ADDITIONAL_SRCS ${CXX_FILES} ${H_FILES}
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  LINK_DIRECTORIES ${ITK_LIBRARY_DIRS} ${VTK_LIBRARY_DIRS}
  INCLUDE_DIRECTORIES ${ITK_INCLUDE_DIR} ${VTK_INCLUDE_DIR}
  )
option(BUILD_TESTING "Build the testing tree" ON)

IF(BUILD_TESTING)
  include(CTest)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
