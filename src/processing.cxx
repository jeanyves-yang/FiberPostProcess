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
#include "utils.h"


processing::processing( int visuAttribute )
{
    visualize = visuAttribute ;
}

void processing::WriteLogFile( processing::fileNameStruct fileName , std::vector< std::vector< float> > vecPointData , float threshold ,
                               vtkSmartPointer< vtkPolyData > cleanedFiberFile , std::vector< float > cumul ,
                               std::vector< std::string > fiberStatus )
{
    std::vector< std::vector < std::string > > data = ConvertArray( vecPointData ) ;
    csv csvFile ;
    std::vector< std::vector < std::string > > headerData ;
    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back("Fiber File Input: " ) ;
    buff.push_back( fileName.input ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask Input: " ) ;
    buff.push_back( fileName.mask ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Fiber File with improper fibers removed: " ) ;
    buff.push_back( fileName.cleaned  ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Fiber File Output (contains all the data): " ) ;
    buff.push_back( fileName.output ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Fiber File visualizable: " ) ;
    buff.push_back( fileName.visu ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Number of fibers before processing: " ) ;
    buff.push_back( Convert( vecPointData.size() ) ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Number of fibers after processing: " ) ;
    buff.push_back( Convert( cleanedFiberFile->GetNumberOfCells() ) ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Threshold = " ) ;
    buff.push_back( Convert( threshold ) ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        data[ i ].insert(data[ i ].begin() , Convert( cumul[ i ] ) ) ;
    }
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        data[ i ].insert(data[ i ].begin() , fiberStatus[ i ] ) ;
    }
    csvFile.initHeader( headerData ) ;
    csvFile.initData( data ) ;
    std::vector< std::string > rowsId , colsId ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        rowsId.push_back( "FIBER " + Convert( i ) ) ;
    }
    int max = FindMaxNbofCols( data ) ;
    colsId.push_back( "" ) ;
    colsId.push_back( "STATUS( 1 = rejected 0 = passed )" ) ;
    colsId.push_back( "CUMUL VALUE" ) ;
    for( int i = 0 ; i < max ; i++ )
    {
        colsId.push_back( "POINT " + Convert( i ) ) ;
    }
    csvFile.initRowsId( rowsId ) ;
    csvFile.initColsId( colsId ) ;
    csvFile.write( logFileName ) ;
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

void processing::WriteFiberFile( vtkSmartPointer< vtkPolyData > polyData , std::string outputFileName )
{
    std::string extension = ExtensionOfFile( outputFileName ) ;
    if( extension.rfind( "vtk" ) != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New() ;
        writer->SetInputData( polyData ) ;
        writer->SetFileName( outputFileName.c_str() ) ;
        writer->Update() ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New() ;
        writer->SetInputData( polyData ) ;
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
        cumul.push_back( tmp/pointData[ i ].size() ) ;
    }
    return cumul ;
}

vtkSmartPointer< vtkPolyData > processing::AddPointData( vtkSmartPointer< vtkPolyData > polyData )
{
    const int nfib = polyData->GetNumberOfCells() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        vtkSmartPointer< vtkCell > fiber = polyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( "InsideMask" ) ;
            pointData->InsertNextValue( 0 ) ;
        }

    }
    polyData->GetPointData()->AddArray( pointData ) ;
    return polyData;
}

vtkSmartPointer< vtkPolyData > processing::CleanFiber( vtkSmartPointer< vtkPolyData > polyData , float threshold )
{
    vtkSmartPointer< vtkPolyData > newPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkPoints* points = polyData->GetPoints() ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType* ids ;
    vtkIdType numberOfPoints ;
    int newId = 0 ;
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "CumulativeValue" ) ) ;
    lines->InitTraversal() ;
    for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
    {
        vtkSmartPointer< vtkPolyLine > newLine = vtkSmartPointer< vtkPolyLine >::New() ;
        newLine->GetPointIds()->SetNumberOfIds( numberOfPoints ) ;
        if( fiberReadArray->GetValue( i ) <= threshold )
        {
            for( int j = 0 ; j < numberOfPoints ; j ++ )
            {
                newPoints->InsertNextPoint( points->GetPoint( ids[j] ) ) ;
                newLine->GetPointIds()->SetId( j , newId ) ;
                newId++ ;
            }
            newLines->InsertNextCell( newLine ) ;
        }
        else
        {
        }
    }
    newPolyData->SetPoints( newPoints ) ;
    newPolyData->SetLines( newLines ) ;
    return newPolyData ;
}

std::vector< std::string > processing::ThresholdPolyData( vtkSmartPointer< vtkPolyData > polyData , float threshold )
{
    std::vector< std::string > fiberStatus ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType numberOfPoints ;
    vtkIdType* ids ;
    vtkSmartPointer< vtkDoubleArray > fiberReadArray = vtkSmartPointer< vtkDoubleArray >::New() ;
    fiberReadArray = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "CumulativeValue" ) ) ;
    lines->InitTraversal() ;
    for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
    {

        if( fiberReadArray->GetValue( i ) <= threshold )
        {
            fiberStatus.push_back( "0" ) ;
        }
        else
        {
            fiberStatus.push_back( "1" ) ;
        }
    }
    return fiberStatus ;
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
    std::vector< std::string > fiberStatus = ThresholdPolyData( fiberPolyData , threshold ) ;
    WriteFiberFile( visuFiber , fileName.visu ) ;
    WriteFiberFile( fiberPolyData , fileName.output ) ;
    WriteFiberFile( cleanedFiberPolyData , fileName.cleaned ) ;
    WriteLogFile( fileName , vecPointData , threshold , cleanedFiberPolyData , cumul , fiberStatus ) ;
    return 0 ;
}
