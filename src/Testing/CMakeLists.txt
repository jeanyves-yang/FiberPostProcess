# project FiberPostProcess

set(TEMP_DIR ${FiberPostProcess_BINARY_DIR}/Testing/Temporary )
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Data )

set(BaselineNormalRun ${SOURCE_DIR}/CingulumNormalRun-cleaned.vtk)
set(BaselineEmptyMask ${SOURCE_DIR}/CingulumEmptyMask-cleaned.vtk)
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
  ${OutputCleaned}
  ModuleEntryPoint
  -i ${Input}
  -o ${Output}
  -m ${Mask}
  -t 4
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
  -t 4
  -v 1
)

# Test with a mask equal to the input image
add_test(NAME FiberPostProcessIdentity COMMAND $<TARGET_FILE:FiberProcessingTest>
  --compare
  ${Input}
  ${OutputCleaned}
  ModuleEntryPoint
  -i ${Input}
  -o ${Output}
  -m ${IdentityMask}
  -t 4
  -v 1
)
