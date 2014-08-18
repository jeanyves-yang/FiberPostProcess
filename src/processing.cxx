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

#include <fstream>
#include <iomanip>
#include <sstream>

#include "csv.h"


processing::processing( int visuAttribute )
{
    visualize = visuAttribute ;
}

std::string ExtensionOfFile( std::string fileName )
{
    std::string extension ;
    extension = fileName.substr( fileName.find_last_of( "." ) + 1 ,
                                 fileName.size() - fileName.find_last_of( "." ) + 1 ) ;
    return extension ;
}

std::string ChangeEndOfFileName (std::string fileName, std::string change )
{
    std::string extension = ExtensionOfFile( fileName ) ;
    fileName.replace( fileName.end() - extension.length() - 1 , fileName.end() , change ) ;
    return fileName;
}

template< class T >
std::string Convert ( T number )
{
    std::ostringstream buff ;
    buff << number ;
    return buff.str() ;
}



std::vector< std::vector < std::string  > > ConvertArray( std::vector< std::vector< float > > array )
{
    std::vector< std::vector < std::string  > > buffer ;
    for( int i = 0 ; i < array.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < array[i].size() ; j++ )
        {
            buff.push_back( Convert( array[ i ][ j ] ) ) ;
        }
        buffer.push_back( buff ) ;
    }
    return buffer ;
}

void processing::WriteLogFile( processing::fileNameStruct fileName , std::vector< std::vector< float> > vecPointData , float threshold ,
                               vtkSmartPointer< vtkPolyData > cleanedFiberFile )
{
    /*ofstream logFile ;
    logFile.open( "log.csv") ;
    logfile << "Fiber File Input: " << fileName.input << std::endl ;
    logfile << "Mask Input: " << fileName.mask << std::endl ;
    logfile << "Fiber File with improper fibers removed: "  << fileName.cleaned << std::endl ;
    logfile << "Fiber File Output (contains all the data): "  << fileName.output << std::endl ;
    logfile << "Fiber File visualizable: " << fileName.visu << std::endl ;
    logfile << "Number of fibers before processing: " << vecPointData.size() << std::endl ;
    logfile << "Threshold = " << threshold << std::endl ;
    logfile << "Number of fibers after processing: " << cleanedFiberFile->GetNumberOfLines() << std::endl ;
    logfile << std::endl << std::endl << "DETAILS" << std::endl ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        logfile <<
        for( int j = 0 ; j < vecPointData[i].size() ; j++ )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( "InsideMask" ) ;
            pointData->InsertNextValue( vecPointData[ i ][ j ] ) ;
        }
    }*/
    std::vector< std::vector < std::string > > data = ConvertArray( vecPointData ) ;
    csv csvFile( "log.csv" ) ;
    csvFile.initData( data ) ;
    /*csvFile.setfield( 0 , 0 , "Fiber File Input: " ) ;
    csvFile.setfield( 0 , 1 , fileName.input ) ;
    csvFile.setfield( 1 , 0 , "Mask Input: " ) ;
    //csvFile.setfield( 1 , 1 , fileName.mask ) ;
    /*csvFile.setfield( 2 , 0 , "Fiber File with improper fibers removed: " ) ;
    csvFile.setfield( 2 , 1 , "fileName.cleaned " ) ;
    csvFile.setfield( 3 , 0 , "Fiber File Output (contains all the data): " ) ;
    csvFile.setfield( 3 , 1 , fileName.output ) ;
    csvFile.setfield( 4 , 0 , "Fiber File visualizable: " ) ;
    csvFile.setfield( 4 , 1 , fileName.visu ) ;
    csvFile.setfield( 5 , 0 , "Number of fibers before processing: " ) ;
    //csvFile.setfield( 5 , 1 , Convert( vecPointData.size() ) ) ;
    csvFile.setfield( 6 , 0 , "Threshold = " ) ;
    //csvFile.setfield( 6 , 1 , Convert( threshold ) ) ;
    for( int i = 0 ; i < data.size() ; i++ )
    {
        for( int j = 0 ; j < data[i].size() ; j++ )
        {
            //mycsv.setfield( )
        }
    }*/

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
    int i = 1 ;
    while( polyData->GetLines()->GetNextCell( idList ) )
    {
        std::cout<< "Line " << i << ": " << idList->GetNumberOfIds() << " points" <<std::endl ;
        i++ ;
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
            fib.push_back( pixel ) ;
        }
        pointData.push_back( fib ) ;
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
    processing::fileNameStruct fileName ;
    fileName.input = inputFileName ;
    fileName.output = outputFileName ;
    fileName.mask = maskFileName ;
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
        fileName.visu =  ChangeEndOfFileName( outputFileName , "-visu.vtk" ) ;
        fileName.cleaned = ChangeEndOfFileName( outputFileName , "-cleaned.vtk" ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        fileName.visu = ChangeEndOfFileName( outputFileName , "-visu.vtp" ) ;
        fileName.cleaned = ChangeEndOfFileName( outputFileName , "-cleaned.vtp" ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    WriteFiberFile( visuFiber , fileName.visu ) ;
    WriteFiberFile( fiberPolyData , fileName.output ) ;
    WriteFiberFile( cleanedFiberPolyData , fileName.cleaned ) ;
    WriteLogFile( fileName , vecPointData , threshold , cleanedFiberPolyData ) ;
    return 0 ;
}
