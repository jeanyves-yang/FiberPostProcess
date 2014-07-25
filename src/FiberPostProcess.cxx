#include <vtkSmoothPolyDataFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include "processing.h"
#include "FiberPostProcessCLP.h"






int main( int argc , char* argv[] )
{
  PARSE_ARGS ;
  processing* FiberProcessing = new processing ;
  FiberProcessing->processing_main( input_fiber_file , output_fiber_file, fiber_mask_file ) ;
  return EXIT_SUCCESS ;
}

