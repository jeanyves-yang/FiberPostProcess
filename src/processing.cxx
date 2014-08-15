#include <vtkSmoothPolyDataFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include "processing.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include "itkImageToVTKImageFilter.h"


#include <vtkImageWriter.h>
#include <itkPoint.h>
#include <itkLinearInterpolateImageFunction.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

#include <vtkPointData.h>

#include <vtkPolyLine.h>



processing::processing( int visuAttribute )
{
    visualize = visuAttribute ;
}

std::string processing::ExtensionOfFile( std::string fileName )
{
    std::string extension ;
    extension = fileName.substr( fileName.find_last_of( "." ) + 1 ,
                                 fileName.size() - fileName.find_last_of( "." ) + 1 ) ;
    return extension ;
}

std::string processing::ChangeEndOfFileName (std::string fileName, std::string change )
{
  std::string extension = ExtensionOfFile( fileName ) ;
  fileName.replace( fileName.end() - extension.length() - 1 , fileName.end() , change ) ;
  return fileName;
}



void processing::FindAllData( vtkSmartPointer< vtkPolyData > polyData )
{
    polyData->GetLines()->InitTraversal() ;
    std::cout << "Normals: " << polyData->GetPointData()->GetNormals() << std::endl ;
    vtkIdType numberOfPointArrays = polyData->GetPointData()->GetNumberOfArrays() ;
    std::cout << "Lines: " << polyData->GetNumberOfLines() << std::endl ;
    vtkSmartPointer< vtkIdList > idList = vtkSmartPointer< vtkIdList >::New() ;
    std::cout << "Cells: " << polyData->GetNumberOfCells() << std::endl ;
    std::cout << "Points: " << polyData->GetNumberOfPoints() << std::endl ;
    std::cout << "Polys: " << polyData->GetNumberOfPolys() << std::endl ;
    std::cout << "Strips: " << polyData->GetNumberOfStrips() << std::endl ;
    std::cout << "Verts: " << polyData->GetNumberOfVerts() << std::endl ;
    int i=1;
    while( polyData->GetLines()->GetNextCell( idList ) )
    {
        std::cout<< "Line " << i << ": " << idList->GetNumberOfIds() << " points" <<std::endl ;
        i++;
        for( vtkIdType pointId = 0 ; pointId < idList->GetNumberOfIds() ; pointId++ )
        {
            /*std::cout <<idList->GetId( pointId ) << " " ;*/
        }
    }
}

int processing::FindMaxNbOfPoints( vtkSmartPointer< vtkPolyData > polyData )
{
    polyData->GetLines()->InitTraversal() ;
    vtkSmartPointer< vtkIdList > idList = vtkSmartPointer< vtkIdList >::New() ;
    int max = 0 ;
    while( polyData->GetLines()->GetNextCell( idList ) )
    {
        if( idList->GetNumberOfIds() > max )
        {
            max = idList->GetNumberOfIds() ;
        }
    }
    return max ;
}
template <class T>
vtkSmartPointer< vtkPolyData > processing::ReadFiberFile( T reader , std::string fiberFile )
{
    vtkSmartPointer< vtkPolyData > PolyData ;
    reader->SetFileName( fiberFile.c_str() ) ;
    PolyData=reader->GetOutput() ;
    reader->Update() ;
    return PolyData ;
}

void processing::WriteFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName )
{
    std::string extension = ExtensionOfFile( outputFileName ) ;
    if( extension.rfind( "vtk" ) != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New() ;
        writer->SetInputData( PolyData ) ;
        writer->SetFileName( outputFileName.c_str() ) ;
        writer->Update() ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New() ;
        writer->SetInputData( PolyData ) ;
        writer->SetDataModeToBinary() ;
        writer->SetFileName( outputFileName.c_str() ) ;
        writer->Update() ;
    }
}

std::vector<std::vector< float > > processing::ApplyMaskToFiber(vtkSmartPointer< vtkPolyData > polyData , std::string maskFileName )
{
    typedef itk::Image< int , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    ReaderType::Pointer readerMask = ReaderType::New() ;
    readerMask->SetFileName( maskFileName ) ;
    readerMask->Update() ;
    ImageType::Pointer maskImage = ImageType::New() ;
    maskImage = readerMask->GetOutput() ;
    typedef itk::Index< 3 > IndexType ;
    ImageType::IndexType index ;
    typedef itk::LinearInterpolateImageFunction< ImageType , double > InterpolationType ;
    InterpolationType::Pointer interp = InterpolationType::New() ;
    interp->SetInputImage( maskImage ) ;
    itk::Point< double , 3 > p ;
    const int nfib = polyData->GetNumberOfCells() ;
    std::cout<< nfib <<std::endl;
    std::vector< std::vector< float > > pointData ;
    // Inserting an array corresponding to the % of the fiber which is not inside the mask
    //vtkSmartPointer< vtkDoubleArray > fiberData = vtkSmartPointer< vtkDoubleArray >::New() ;
    //vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        std::vector < float > fib ;
        std::cout << " FIBER NUMBER " << i << std::endl ;
        vtkSmartPointer< vtkCell > fiber = polyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {

            double* coordinates = fiberPoints->GetPoint( j ) ;
            /* Flip the coordinates */
            coordinates[0] = - coordinates[0] ;
            coordinates[1] = - coordinates[1] ;
            p[0] = coordinates[0] ;
            p[1] = coordinates[1] ;
            p[2] = coordinates[2] ;
            maskImage->TransformPhysicalPointToIndex( p , index ) ;
            //Checks if the index is inside the image boundaries
            if( !interp->IsInsideBuffer( index ) )
            {
                std::cout << "Index out of bounds" << std::endl ;
            }
            else
            {
            }
            ImageType::PixelType pixel = maskImage->GetPixel( index ) ;
           // pointData[i][j] = pixel ;
           fib.push_back( pixel ) ;
        }
        pointData.push_back( fib ) ;
        //std::cout << "Pixels out of mask = " << badPixel << std::endl ;
        //fiberData->SetNumberOfComponents( nfib ) ;
        //fiberData->SetName( "UnwantedFiber" ) ;
        //fiberData->InsertNextValue( badPixel/fiberPoints->GetNumberOfPoints()*100 ) ;
    }
    return pointData ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreatePointData( std::vector< std::vector< float> > vecPointData )
{
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        for( int j = 0 ; j < vecPointData[i].size() ; j++ )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( "InsideMask" ) ;
            pointData->InsertNextValue( vecPointData[ i ][ j ] ) ;
        }
    }
    return pointData ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreateCellData( std::vector< float > vecCellData )
{
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecCellData.size() ; i++ )
    {
            cellData->SetNumberOfComponents( 1 ) ;
            cellData->SetName( "CumulativeValue" ) ;
            cellData->InsertNextValue( vecCellData[ i ] ) ;
    }
    return cellData ;
}

std::vector< float > processing::CumulValuePerFiber( std::vector< std::vector< float> > pointData )
{
    std::vector< float > cumul ;
    for(int i = 0 ; i < pointData.size() ; i++ )
    {
        float tmp = 0;
        for( int j = 0 ; j < pointData[ i ].size() ; j++ )
        {
            tmp = tmp + pointData[ i ][ j ] ;
        }
        cumul.push_back( tmp ) ;
    }
    return cumul ;
}

vtkSmartPointer< vtkPolyData > processing::AddPointData( vtkSmartPointer< vtkPolyData > PolyData )
{
    const int nfib = PolyData->GetNumberOfCells() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        vtkSmartPointer< vtkCell > fiber = PolyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( "InsideMask" ) ;
            pointData->InsertNextValue( 0 ) ;
        }

    }
    PolyData->GetPointData()->AddArray( pointData ) ;
    return PolyData;
}

vtkSmartPointer< vtkPolyData > processing::CleanFiber( vtkSmartPointer< vtkPolyData > PolyData , float threshold )
{
    vtkSmartPointer< vtkPolyData > NewPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    //vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkPoints* Points = PolyData->GetPoints() ;
    vtkCellArray* Lines = PolyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    int NewId = 0 ;
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( PolyData->GetCellData()->GetArray("CumulativeValue") ) ;
    Lines->InitTraversal() ;
    for( int i = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; i++ )
    {
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        if( fiberReadArray->GetValue( i ) <= threshold )
        {
            for( int j = 0 ; j < NumberOfPoints ; j ++ )
            {
                NewPoints->InsertNextPoint( Points->GetPoint( Ids[j] ) ) ;
                NewLine->GetPointIds()->SetId( j , NewId ) ;
                NewId++ ;
            }
            NewLines->InsertNextCell( NewLine ) ;
       }
        else
        {
        }
    }
    NewPolyData->SetPoints( NewPoints ) ;
    NewPolyData->SetLines( NewLines ) ;
    return NewPolyData ;
}

vtkSmartPointer< vtkPolyData > processing::CreateVisuFiber( vtkSmartPointer< vtkPolyData > PolyData )
{
    vtkSmartPointer< vtkPolyData > NewPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    vtkPoints* Points = PolyData->GetPoints() ;
    vtkCellArray* Lines = PolyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    int NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = PolyData->GetNumberOfCells() ;
    for( int i = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; i++ )
    {
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        for( int j = 0 ; j < NumberOfPoints ; j ++ )
        {
            NewPoints->InsertNextPoint( Points->GetPoint( Ids[j] ) ) ;
            NewLine->GetPointIds()->SetId( j , NewId ) ;
            NewId++ ;
        }
        NewLines->InsertNextCell( NewLine ) ;
    }
    NewPolyData->SetPoints( NewPoints ) ;
    NewPolyData->SetLines( NewLines ) ;
    PolyData->GetPointData()->GetAbstractArray( 1 )->SetName( "InsideMask" ) ;
    NewPolyData->GetPointData()->AddArray( PolyData->GetPointData()->GetAbstractArray( 1 ) ) ;
    return NewPolyData ;

}

int processing::processing_main(std::string& inputFileName ,
                                std::string& outputFileName ,
                                std::string& maskFileName ,
                                float threshold )
{
    typedef itk::Image< int , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    vtkSmartPointer< vtkPolyData > fiberPolyData ;
    vtkSmartPointer< vtkPolyData > cleanedFiberPolyData ;
    std::string extension = ExtensionOfFile( inputFileName ) ;
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > fiberPolyDataReader = vtkSmartPointer< vtkPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, inputFileName ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataReader> fiberPolyDataReader = vtkSmartPointer< vtkXMLPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, inputFileName ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    //FindAllData( fiberPolyData ) ;
    int nfib = fiberPolyData->GetNumberOfLines() ;
    int N = FindMaxNbOfPoints( fiberPolyData ) ;
    std::vector< std::vector< float > > vecPointData ;
    vecPointData = ApplyMaskToFiber( fiberPolyData , maskFileName ) ;
    std::vector< float > cumul = CumulValuePerFiber( vecPointData ) ;
    vtkSmartPointer< vtkDoubleArray > pointData =  CreatePointData( vecPointData ) ;
    fiberPolyData->GetPointData()->AddArray( pointData ) ;
    vtkSmartPointer< vtkDoubleArray > cellData ;
    cellData =  CreateCellData( cumul ) ;
    fiberPolyData->GetCellData()->AddArray( cellData ) ;
    cleanedFiberPolyData = CleanFiber( fiberPolyData , threshold ) ;
    cleanedFiberPolyData = AddPointData( cleanedFiberPolyData ) ;
    vtkSmartPointer< vtkPolyData > visuFiber = vtkSmartPointer< vtkPolyData >::New() ;
    if( visualize )
    {
        visuFiber = CreateVisuFiber( fiberPolyData ) ;
    }
    std::string visuFiberFileName , cleanedFiberFileName ;
    if( extension.rfind( "vtk" ) != std::string::npos )
    {
        visuFiberFileName = ChangeEndOfFileName( outputFileName , "-visu.vtk" ) ;
        cleanedFiberFileName = ChangeEndOfFileName( outputFileName , "-cleaned.vtk" ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        visuFiberFileName = ChangeEndOfFileName( outputFileName , "-visu.vtp" ) ;
        cleanedFiberFileName = ChangeEndOfFileName( outputFileName , "-cleaned.vtp" ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    WriteFiberFile( visuFiber , visuFiberFileName) ;
    WriteFiberFile( fiberPolyData , outputFileName ) ;
    WriteFiberFile( cleanedFiberPolyData , cleanedFiberFileName ) ;
    return 0 ;
}
