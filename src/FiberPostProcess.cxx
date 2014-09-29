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

  std::vector< std::string > fileNameList ;
  bool flagBinaryMask = false ;
  bool flagAttribute = false ;
  bool flagThreshold = false ;
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "-m" ) || !strcmp( argv[ i ] , "--attributeFile" ) )
      {
          flagBinaryMask = true ;
      }
  }
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "-t" ) || !strcmp( argv[ i ] , "--threshold" ) )
      {
          flagThreshold = true ;
      }
  }
  for( int i = 0 ; i < argc ; i++ )
  {
      if( !strcmp( argv[ i ] , "-a" ) || !strcmp( argv[ i ] , "--attribute" ) )
      {
          if( attribute == "binary" )
          {
              flagAttribute = false ;
          }
          else if( attribute == "nonbinary" )
          {
              flagAttribute = true ;
          }
      }
  }
  processing* FiberProcessing = new processing( outputAttributeFiberFileName ) ;
  if( flagBinaryMask == 0 && flagAttribute == 0 ) // no mask, no attribute file
  {
      FiberProcessing->processing_main( inputFiberFileName , outputFiberFileName  ) ;
  }
  if( flagBinaryMask == 1  && flagAttribute == 0 ) // mask, binary
  {
      if( flagThreshold == 1 ) // threshold present (option 3)
      {
          FiberProcessing->processing_main( inputFiberFileName , outputFiberFileName , attributeFileName , threshold , flagAttribute ) ;
      }
      else
      {
          FiberProcessing->processing_main( inputFiberFileName , outputFiberFileName , attributeFileName , flagAttribute) ;
      }
  }
  if( flagBinaryMask == 1 && flagAttribute == 1 ) // mask, non binary
  {
      if( flagThreshold == 1 ) // threshold present (option 3)
      {
          FiberProcessing->processing_main( inputFiberFileName , outputFiberFileName , attributeFileName , threshold , flagAttribute) ;
      }
      else
      {
          FiberProcessing->processing_main( inputFiberFileName , outputFiberFileName , attributeFileName , flagAttribute ) ;
      }
  }
  return EXIT_SUCCESS ;
}
