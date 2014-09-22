# project FiberPostProcess

# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
  "http://slicer.kitware.com/midas3/api/rest?method=midas.bitstream.download&checksum=%(hash)"
  )
set(TEMP_DIR ${FiberPostProcess_BINARY_DIR}/Testing/Temporary )
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Data )
set(BaselineEmptyMask ${SOURCE_DIR}/CingulumEmptyMask.vtk)
set(Output ${TEMP_DIR}/output.vtk)
set(OutputCleaned ${TEMP_DIR}/output-cleaned.vtk)
set(VoidMask ${SOURCE_DIR}/inputnull.nrrd)


# Test of a normal run of the program
add_executable(FiberProcessingTest vtkTest.cxx)
target_link_libraries(FiberProcessingTest FiberPostProcessLib)
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessTest1 COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/CingulumNormalRun.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/Cingulum.vtk}
  -o ${Output}
  -m DATA{Data/inputT2-test3.nrrd}
  -t 0.25
  -v 1
)

# Test with an empty mask
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessEmptyMask COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  ${BaselineEmptyMask}
  ${OutputCleaned}
  ModuleEntryPoint
  -i DATA{Data/Cingulum.vtk}
  -o ${Output}
  -m ${VoidMask}
  -t 0.25
  -v 1
)

# Test with a mask equal to the input image
ExternalData_Add_Test(FiberPostProcessData NAME FiberPostProcessIdentity COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  DATA{Data/Cingulum.vtk}
  ${Output}
  ModuleEntryPoint
  -i DATA{Data/Cingulum.vtk}
  -o ${Output}
  -m DATA{Data/dwi.nrrd}
  -t 0.25
  -v 1
)

# CSV Tests

add_executable(CSVTestInitHeader csvTestInitHeader.cxx)
add_test(NAME FiberPostProcessCSVInitHeader COMMAND $<TARGET_FILE:CSVTestInitHeader>)

add_executable(CSVTestInitData csvTestInitData.cxx)
add_test(NAME FiberPostProcessCSVInitData COMMAND $<TARGET_FILE:CSVTestInitData>)

add_executable(CSVTestInitRowsId csvTestInitRowsId.cxx)
add_test(NAME FiberPostProcessCSVInitRowsId COMMAND $<TARGET_FILE:CSVTestInitRowsId>)

add_executable(CSVTestInitColsId csvTestInitColsId.cxx)
add_test(NAME FiberPostProcessCSVInitColsId COMMAND $<TARGET_FILE:CSVTestInitColsId>)

add_executable(CSVTestInitFull csvTestInitFull.cxx)
add_test(NAME FiberPostProcessCSVInitFull COMMAND $<TARGET_FILE:CSVTestInitFull>)

# NaN Cases Tests
add_executable(NaNFunction TestIsNanFunction.cxx)
add_test(NAME FiberPostProcessNaNFunction COMMAND $<TARGET_FILE:NaNFunction>)

# Add a build target to populate the real data.
ExternalData_Add_Target(FiberPostProcessData)
