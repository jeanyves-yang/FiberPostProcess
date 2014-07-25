# project FiberPostProcess

set(TEMP_DIR ${FiberPostProcess_BINARY_DIR}/Testing/Temporary )
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${Data} )

# Test checking that the program cannot run without arguments
add_test(NAME OpenFileTest COMMAND $<TARGET_FILE:FiberPostProcess>)
set_tests_properties( OpenFileTest PROPERTIES WILL_FAIL true )

set(Baseline baseline.vtk)
set(Input ${SOURCE_DIR}/Cingulum.vtk)
set(Output ${TEMP_DIR}/output.vtk)
add_executable(vtkTest vtkTest.cxx)
add_test(NAME FiberPostProcess COMMAND $<TARGET_FILE:FiberPostProcess>
  --compare
  ${Baseline}
  ${Output}
  ModuleEntryPoint
  ${Input}
  ${Output}
)
