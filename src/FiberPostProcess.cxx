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
#include "utils.h"

int main( int argc , char* argv[] )
{
  PARSE_ARGS ;
  bool attributeFlag = false ;
  bool thresholdFlag = false ;
  if( attributeFileName.empty() )
  {
      attributeFlag = true ;
  }
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "-t" ) || !strcmp( argv[ i ] , "--threshold" ) )
      {
          thresholdFlag = true ;
      }
  }
  processing FiberProcessing ;
  FiberProcessing.SetInputFileName( inputFiberFileName ) ;
  FiberProcessing.SetOutputFileName( outputFiberFileName ) ;
  FiberProcessing.SetVisualisation( visualisationFlag ) ;
  FiberProcessing.SetAttributeFileName( attributeFileName ) ;
  FiberProcessing.SetThreshold( threshold ) ;
  FiberProcessing.SetThresholdFlag( thresholdFlag ) ;
  FiberProcessing.SetAttributeFlag( attributeFlag ) ;
  FiberProcessing.SetMaskFlag( mask ) ;
  FiberProcessing.SetCropFlag( crop ) ;
  FiberProcessing.SetThresholdMode ( thresholdMode ) ;
  FiberProcessing.run() ;
  return EXIT_SUCCESS ;
}
