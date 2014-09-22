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
                               vtkSmartPointer< vtkPolyData > cleanedFiberFile , std::vector< float > cumul , std::vector< float > average )
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
        data[ i ].insert( data[ i ].begin() , Convert( cumul[ i ] ) ) ;
    }
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        data[ i ].insert( data[ i ].begin() , Convert( average[ i ] ) ) ;
    }
    csvFile.initHeader( headerData ) ;
    csvFile.initData( data ) ;
    std::vector< std::string > rowsId , colsId ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        rowsId.push_back( "FIBER " + Convert( i ) ) ;
    }
    int max = FindMaxNbOfCols( data ) ;
    colsId.push_back( "" ) ;
    colsId.push_back( "AVERAGE VALUE" ) ;
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

int processing::CheckNaN( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData )
{
    std::vector< std::vector< float > > pointData ;
    vtkPoints* Points = polyData->GetPoints() ;
    vtkCellArray* Lines = polyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    vtkIdType NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = polyData->GetNumberOfCells() ;
    for( int fiberId = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; fiberId++ )
    {
        for( int pointId = 0 ; pointId < NumberOfPoints ; pointId ++ )
        {
            if( std::isnan( vecPointData[ fiberId ][ pointId ] ) != 0 )
            {
                std::cout<<"vecPointData[" << fiberId <<"][" << pointId <<"] = " << vecPointData[ fiberId ][ pointId ]<<std::endl ;
                return 1 ;
            }
        }
    }
}

std::vector< std::vector< float > > processing::ApplyMaskToFiber(vtkSmartPointer< vtkPolyData > polyData , std::string maskFileName )
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
    std::vector< std::vector< float > > pointData ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        std::vector < float > fib ;
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

vtkSmartPointer< vtkPolyData > processing::CropFiber( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData )
{
    vtkSmartPointer< vtkPolyData > cropFiber = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    std::vector< std::vector< float > > pointData ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkPoints* Points = polyData->GetPoints() ;
    vtkCellArray* Lines = polyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    vtkIdType NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = polyData->GetNumberOfCells() ;

    for( int fiberId = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; fiberId++ )
    {
        std::vector< float > pointDataPerFiber ;
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        int pointId = 0 ;
        while( vecPointData[ fiberId ][ pointId ] == 0 )
        {
            //deletePoint( polyData->GetPoints() , pointId ) ;
            pointId++ ;
        }
        if( pointId >= NumberOfPoints )
        {
            NumberOfPoints = 0 ;
        }
        else
        {
            NumberOfPoints = NumberOfPoints - pointId ;
        }
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        /*pointId = 0 ;
                    while( vecPointData[ fiberId ][ vecPointData[ fiberId ].size()-pointId ] == 0 )
                    {
                        deletePoint( polyData->GetPoints() , vecPointData[ fiberId ].size() - pointId ) ;
                        pointId++ ;
                    }*/
        int location = 0 ;
        int compteur = 0 ;
        while( pointId < vecPointData[ fiberId ].size() )
        {
            pointDataPerFiber.push_back( vecPointData[ fiberId ][ pointId ] ) ;
            NewPoints->InsertNextPoint( Points->GetPoint( Ids[ pointId ] ) ) ;
            NewLine->GetPointIds()->SetId( location , NewId ) ;
            NewId ++ ;
            pointId ++ ;
            location++ ;
            compteur++;
        }
        pointData.push_back( pointDataPerFiber ) ;
        NewLines->InsertNextCell( NewLine ) ;
    }
    cropFiber->SetPoints( NewPoints ) ;
    cropFiber->SetLines( NewLines ) ;
    cropFiber->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( 0 ) ) ;
    cropFiber->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( 1 ) ) ;
    cropFiber->GetPointData()->AddArray( CreatePointData( pointData , "InsideMask" ) ) ;
    return cropFiber ;
}

void processing::deletePoint( vtkSmartPointer< vtkPoints > fiberPoints , int pointId )
{
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New() ;
    for(vtkIdType i = 0 ; i < fiberPoints->GetNumberOfPoints() ; i++ )
    {
        if( i != pointId )
        {
            double p[3];
            fiberPoints->GetPoint( i , p ) ;
            newPoints->InsertNextPoint( p ) ;
        }
    }
    fiberPoints->ShallowCopy( newPoints ) ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreatePointData( std::vector< std::vector< float> > vecPointData , const char* fieldName )
{
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecPointData.size() ; i++ )
    {
        for( int j = 0 ; j < vecPointData[i].size() ; j++ )
        {
            pointData->SetNumberOfComponents( 1 ) ;
            pointData->SetName( fieldName ) ;
            pointData->InsertNextValue( vecPointData[ i ][ j ] ) ;
        }
    }
    return pointData ;
}

vtkSmartPointer< vtkDoubleArray > processing::CreateCellData(std::vector< float > vecCellData , const char *fieldName )
{
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < vecCellData.size() ; i++ )
    {
        cellData->SetNumberOfComponents( 1 ) ;
        cellData->SetName( fieldName ) ;
        cellData->InsertNextValue( vecCellData[ i ] ) ;
    }
    return cellData ;
}

std::vector< float > processing::CumulValuePerFiber( std::vector< std::vector< float> > pointData )
{
    std::vector< float > cumul ;
    for(int fiberId = 0 ; fiberId < pointData.size() ; fiberId++ )
    {
        float tmp = 0;
        for( int pointId = 0 ; pointId < pointData[ fiberId ].size() ; pointId++ )
        {
            tmp = tmp + pointData[ fiberId ][ pointId ] ;
        }
        cumul.push_back( tmp ) ;
    }
    return cumul ;
}

std::vector< float > processing::AverageValuePerFiber( std::vector< std::vector< float> > pointData )
{
    std::vector< float > cumul ;
    for(int fiberId = 0 ; fiberId < pointData.size() ; fiberId++ )
    {
        float tmp = 0;
        for( int pointId = 0 ; pointId < pointData[ fiberId ].size() ; pointId++ )
        {
            tmp = tmp + pointData[ fiberId ][ pointId ] ;
        }
        cumul.push_back( tmp/pointData[ fiberId ].size() ) ;
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

vtkSmartPointer< vtkPolyData > processing::CreateVisuFiber(vtkSmartPointer< vtkPolyData > polyData )
{
    vtkSmartPointer< vtkPolyData > NewPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> NewLines = vtkSmartPointer<vtkCellArray>::New() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    vtkPoints* Points = polyData->GetPoints() ;
    vtkCellArray* Lines = polyData->GetLines() ;
    vtkIdType* Ids ;
    vtkIdType NumberOfPoints ;
    int NewId = 0 ;
    Lines->InitTraversal() ;
    const int nfib = polyData->GetNumberOfCells() ;
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
    polyData->GetPointData()->GetAbstractArray( 1 )->SetName( "InsideMask" ) ;
    NewPolyData->GetPointData()->AddArray( polyData->GetPointData()->GetAbstractArray( 1 ) ) ;
    return NewPolyData ;

}

std::vector< std::vector< std::string > > processing::GetCellData( vtkSmartPointer< vtkPolyData > polyData , char * fieldName )
{
    int arrayId ;
    for( int i = 0 ; i < polyData->GetCellData()->GetNumberOfArrays() ; i++ )
    {
        if( polyData->GetCellData()->GetArrayName( i ) == fieldName )
        {
            arrayId = i ;
            break ;
        }
    }
}

std::vector< std::vector< std::string > > processing::GetPointData( vtkSmartPointer< vtkPolyData > polyData , char * fieldName )
{
    int arrayId ;
    for( int i = 0 ; i < polyData->GetPointData()->GetNumberOfArrays() ; i++ )
    {
        if( polyData->GetPointData()->GetArrayName( i ) == fieldName )
        {
            arrayId = i ;
            break ;
        }
    }
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
    std::vector< float > average = AverageValuePerFiber( vecPointData ) ;
    vtkSmartPointer< vtkDoubleArray > cellData , cellData2 ;
    cellData =  CreateCellData( cumul , "CumulativeValue" ) ;
    cellData2 = CreateCellData( average , "AverageValue") ;
    fiberPolyData->GetCellData()->AddArray( cellData ) ;
    fiberPolyData->GetCellData()->AddArray( cellData2 ) ;
    vtkSmartPointer< vtkDoubleArray > pointData =  CreatePointData( vecPointData , "InsideMask" ) ;
    fiberPolyData->GetPointData()->AddArray( pointData ) ;
    WriteFiberFile( fiberPolyData , fileName.output ) ;
    vtkSmartPointer< vtkPolyData > cropFiberPolyData = CropFiber( fiberPolyData , vecPointData ) ;
    cleanedFiberPolyData = CleanFiber( fiberPolyData , threshold ) ;
    cleanedFiberPolyData = AddPointData( cleanedFiberPolyData ) ;
    vtkSmartPointer< vtkPolyData > visuFiber = vtkSmartPointer< vtkPolyData >::New() ;
    if( visualize )
    {
        visuFiber = CreateVisuFiber( fiberPolyData ) ;
    }
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
    //vecPointData[0][0] = 0.0 / 0.0 ;
    if( CheckNaN( fiberPolyData , vecPointData ) == 1 )
    {
        std::cout<<"NaN error"<<std::endl;
        return 1 ;
    }
    WriteFiberFile( visuFiber , fileName.visu ) ;
    WriteFiberFile( cleanedFiberPolyData , fileName.cleaned ) ;
    WriteFiberFile( cropFiberPolyData , "croppedFiber.vtk") ;
    WriteLogFile( fileName , vecPointData , threshold , cleanedFiberPolyData , cumul , average ) ;
    return 0 ;
}
