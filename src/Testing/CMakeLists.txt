# project FiberPostProcess

set(TEMP_DIR ${FiberPostProcess_BINARY_DIR}/Testing/Temporary )
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Data )

set(BaselineNormalRun ${SOURCE_DIR}/CingulumNormalRun.vtk)
set(BaselineEmptyMask ${SOURCE_DIR}/CingulumEmptyMask.vtk)
set(Input ${SOURCE_DIR}/Cingulum.vtk)
set(Output ${TEMP_DIR}/output.vtk)
set(OutputCleaned ${TEMP_DIR}/output-cleaned.vtk)
set(Mask ${SOURCE_DIR}/inputT2-test3.nrrd)
set(VoidMask ${SOURCE_DIR}/inputnull.nrrd)
set(IdentityMask ${SOURCE_DIR}/dwi.nrrd)

# Test of a normal run of the program
add_executable(FiberProcessingTest vtkTest.cxx)
target_link_libraries(FiberProcessingTest FiberPostProcessLib)
add_test(NAME FiberPostProcess COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  ${BaselineNormalRun}
  ${Output}
  ModuleEntryPoint
  -i ${Input}
  -o ${Output}
  -m ${Mask}
  -t 0.25
  -v 1
)

# Test with an empty mask
add_test(NAME FiberPostProcessEmptyMask COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  ${BaselineEmptyMask}
  ${OutputCleaned}
  ModuleEntryPoint
  -i ${Input}
  -o ${Output}
  -m ${VoidMask}
  -t 0.25
  -v 1
)

# Test with a mask equal to the input image
add_test(NAME FiberPostProcessIdentity COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  ${Input}
  ${Output}
  ModuleEntryPoint
  -i ${Input}
  -o ${Output}
  -m ${IdentityMask}
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
