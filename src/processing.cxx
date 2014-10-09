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

#include <vtkMath.h>
#include <vtkCellLocator.h>

/* allocate memory for an nrow x ncol matrix */
template< class TReal>
TReal **create_matrix( long nrow, long ncol )
{
    typedef TReal* TRealPointer ;
    TReal **m = new TRealPointer[ nrow ] ;
    TReal* block = ( TReal* ) calloc( nrow*ncol , sizeof( TReal ) ) ;
    m[ 0 ] = block ;
    for( int row = 1 ; row < nrow ; ++row )
    {
        m[ row ] = &block[ row * ncol ] ;
    }
    return m ;
}

/* free a TReal matric allocated with matrix () */
template< class TReal >
void free_matrix( TReal **m )
{
    free( m[0] ) ;
    delete[] m ;
}

processing::processing()
{
}

void processing::SetInputFileName( std::string inputFiberFileName )
{
    InputFileName = inputFiberFileName ;
}
void processing::SetOutputFileName( std::string outputFiberFileName )
{
    OutputFileName = outputFiberFileName ;
}
void processing::SetVisualisation( int visualisationFlag )
{
    FlagVisualize = visualisationFlag ;
}
void processing::SetAttributeFileName( std::string attributeFileName )
{
    AttributeFileName = attributeFileName ;
}

void processing::processing::SetThresholdFlag( int thresholdFlag )
{
    FlagThreshold = thresholdFlag ;
}

void processing::SetAttributeFlag( int attributeFlag )
{
    FlagAttribute = attributeFlag ;
}

void processing::SetCleanFlag( int cleanFlag )
{
    FlagClean = cleanFlag ;
}

void processing::WriteLogFile(processing::fileNameStruct fileName , std::vector< std::vector< float> > vecPointData ,
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
    buff.push_back( "Fiber File containing all the data: " ) ;
    buff.push_back( fileName.log  ) ;
    headerData.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Fiber File Output (croped if needed, fibers removed if needed): " ) ;
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
    buff.push_back( Convert( Threshold ) ) ;
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

vtkSmartPointer< vtkPolyData > processing::CheckNaN( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData )
{
    std::vector< std::vector< float > > pointData ;
    vtkPoints* points = polyData->GetPoints() ;
    vtkCellArray* lines = polyData->GetLines() ;
    vtkIdType* ids ;
    vtkIdType numberOfPoints ;
    vtkIdType newId = 0 ;
    lines->InitTraversal() ;
    vtkSmartPointer< vtkPolyData > newPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New() ;
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New() ;
    for( int fiberId = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; fiberId++ )
    {
        int NanFiber = 0 ;
        vtkSmartPointer< vtkPolyLine > newLine = vtkSmartPointer< vtkPolyLine >::New() ;
        newLine->GetPointIds()->SetNumberOfIds( numberOfPoints ) ;
        for( int pointId = 0 ; pointId < numberOfPoints ; pointId ++ )
        {
            if( std::isnan( vecPointData[ fiberId ][ pointId ] ) != 0 )
            {
                NanFiber = 1 ;
            }
            else
            {
                newPoints->InsertNextPoint( points->GetPoint( ids[ pointId ] ) ) ;
                newLine->GetPointIds()->SetId( pointId , newId ) ;
                newId++ ;
            }
        }
        if(NanFiber == 0)
        {
            newLines->InsertNextCell( newLine ) ;
        }
    }
    newPolyData->SetPoints( newPoints ) ;
    newPolyData->SetLines( newLines ) ;
    return newPolyData ;
}

vtkSmartPointer< vtkPolyData > processing::CheckNaN( vtkSmartPointer< vtkPolyData > polyData ) // compute the fa on the tensors
{
   /* //vtkSmartPointer< vtkDoubleArray > processing::CreateCellData(std::vector< float > vecCellData , const char *fieldName )

    for( int i = 0 ; i < vecCellData.size() ; i++ )
    {
        cellData->SetNumberOfComponents( 1 ) ;
        cellData->SetName( fieldName ) ;
        cellData->InsertNextValue( vecCellData[ i ] ) ;*/
     vtkSmartPointer< vtkPolyData > cleanedPolyData = polyData ;
     vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
     cellData->SetNumberOfComponents( 1 ) ;
     cellData->SetName( "NaNCases") ;
     int nanTensorFlag = 0 ;
     std::vector< std::vector< std::string > > dataField ;
     int nbArrays = polyData->GetPointData()->GetNumberOfArrays() ;
     std::vector< int > NanFiberId ;
     for( int i = 0 ; i < nbArrays ; i++ )
     {
         if( strcmp( polyData->GetPointData()->GetArray( i )->GetName() , "tensors" ) == 0 )
         {
             int nbComponents, nbTuples ;
             nbComponents = polyData->GetPointData()->GetArray( i )->GetNumberOfComponents() ;
             nbTuples = polyData->GetPointData()->GetArray( i )->GetNumberOfTuples() ;
             for( int j = 0 ; j < nbTuples ; j++ )
             {

                 NanFiberId.push_back( -1 ) ;
                 double eigenValues[ 3 ] ;
                 double **eigenVectors = create_matrix< double > ( 3, 3 ) ;
                 double **tensors = create_matrix< double >( 3, 3 ) ;
                 tensors[ 0 ][ 0 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 0 ) ;
                 tensors[ 0 ][ 1 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 1 ) ;
                 tensors[ 0 ][ 2 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 2 ) ;

                 tensors[ 1 ][ 0 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 3 ) ;
                 tensors[ 1 ][ 1 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 4 ) ;
                 tensors[ 1 ][ 2 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 5 ) ;

                 tensors[ 2 ][ 0 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 6 ) ;
                 tensors[ 2 ][ 1 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 7 ) ;
                 tensors[ 2 ][ 2 ] = polyData->GetPointData()->GetArray( i )->GetComponent( j , 8 ) ;
                 vtkMath::Jacobi( tensors , eigenValues , eigenVectors ) ;
                 for( int  k = 0 ; k < 3 ; k ++ )
                 {
                     if( eigenValues [ k ] <= 0 )
                     {
                         NanFiberId.push_back( j ) ;
                     }

                 }
                 free_matrix ( tensors ) ;
             }
         }
         vtkSmartPointer< vtkDataArray > dataArray = polyData->GetPointData()->GetArray( i ) ;
         std::vector< std::vector< float > > dataVector = convertDataToVector( dataArray ) ;
         CheckNaN( cleanedPolyData , dataVector ) ;
     }
     vtkCellArray* lines = polyData->GetLines() ;
     vtkIdType numberOfPoints ;
     vtkIdType* ids ;
     lines->InitTraversal() ;
     vtkSmartPointer< vtkCellLocator > locateFiber = vtkSmartPointer< vtkCellLocator >::New() ;
     for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
     {
         if( std::find(NanFiberId.begin(), NanFiberId.end(), i ) != NanFiberId.end() )
         {
             cellData->InsertNextValue( locateFiber->FindCell( polyData->GetPoint( std::find(NanFiberId.begin(), NanFiberId.end(), i ) - NanFiberId.begin() ) ) ) ;
         }
         else
         {
             cellData->InsertNextValue( -1 ) ;
         }
     }
     cleanedPolyData->GetCellData()->AddArray( cellData ) ;
     return cleanedPolyData ;
}

std::vector< std::vector< float > > processing::convertDataToVector(vtkSmartPointer<vtkDataArray> array )
{
    std::vector< std::vector< float > > dataVector ;
    int nbComponents = array->GetNumberOfComponents() ;
    int nbTuples = array->GetNumberOfTuples() ;
    for( int i = 0 ; i < nbTuples ; i++ )
    {
        std::vector< float > tmp ;
        for( int j = 0 ; j < nbComponents ; j++ )
        {
            tmp.push_back( array->GetComponent( i , j ) ) ;
        }
        dataVector.push_back( tmp ) ;
    }
    return dataVector ;
}

std::vector< std::vector< float > > processing::ApplyMaskToFiber(vtkSmartPointer< vtkPolyData > polyData )
{
    typedef itk::Image< float , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    ReaderType::Pointer readerMask = ReaderType::New() ;
    readerMask->SetFileName( AttributeFileName ) ;
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
    float epsilon = 0.000001f ;
    vtkSmartPointer< vtkDoubleArray > newTensors = vtkSmartPointer< vtkDoubleArray >::New() ;
    newTensors->SetName( "tensors" ) ;
    vtkSmartPointer< vtkDataArray > tensors = polyData->GetPointData()->GetArray( "tensors" ) ;
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
        int endOfFiber = 0 ;
        std::vector< float > pointDataPerFiber ;
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        int pointId = 0 ;
        while( vecPointData[ fiberId ][ pointId ] == 0 )
        {
            pointId++ ;
        }
        if( pointId >= NumberOfPoints )
        {
            continue ;
        }
        else
        {
            NumberOfPoints = NumberOfPoints - pointId ;
        }
        while( vecPointData[ fiberId ][ vecPointData[ fiberId ].size() - endOfFiber -1 ] < epsilon && endOfFiber < vecPointData[ fiberId ].size() ) //TODO: add to the XML
        {
            endOfFiber++ ;
        }
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints - endOfFiber ) ;
        int location = 0 ;
        int compteur = 0 ;
        while( pointId < vecPointData[ fiberId ].size() - endOfFiber )
        {
            newTensors->InsertNextTupleValue( tensors->GetTuple9( pointId ) ) ;
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
    cropFiber->GetPointData()->AddArray( newTensors ) ;
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
    vtkSmartPointer< vtkDoubleArray > cellData = vtkSmartPointer< vtkDoubleArray >::New() ;
    cellData = vtkDoubleArray::SafeDownCast( polyData->GetCellData()->GetArray( "NanCases" ) ) ;
    lines->InitTraversal() ;
    for( int i = 0 ; lines->GetNextCell( numberOfPoints , ids ) ; i++ )
    {
        vtkSmartPointer< vtkPolyLine > newLine = vtkSmartPointer< vtkPolyLine >::New() ;
        newLine->GetPointIds()->SetNumberOfIds( numberOfPoints ) ;
        if( fiberReadArray->GetValue( i ) <= threshold && fiberReadArray->GetValue( i ) == 0 )
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

std::vector< std::string > processing::ThresholdPolyData(vtkSmartPointer< vtkPolyData > polyData )
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

        if( fiberReadArray->GetValue( i ) <= Threshold )
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
            NewPoints->InsertNextPoint( Points->GetPoint( Ids[ j ] ) ) ;
            NewLine->GetPointIds()->SetId( j , NewId ) ;
            NewId++ ;
        }
        NewLines->InsertNextCell( NewLine ) ;
    }
    NewPolyData->SetPoints( NewPoints ) ;
    NewPolyData->SetLines( NewLines ) ;
    NewPolyData->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( "CumulativeValue") ) ;
    NewPolyData->GetCellData()->AddArray( polyData->GetCellData()->GetAbstractArray( "AverageValue") ) ;
    NewPolyData->GetPointData()->AddArray( polyData->GetPointData()->GetAbstractArray( "InsideMask" ) ) ;
    return NewPolyData ;

}

int processing::run()
{
    processing::fileNameStruct fileName ;
    fileName.input = InputFileName ;
    fileName.output = OutputFileName ;
    fileName.mask = AttributeFileName ;
    typedef itk::Image< int , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    vtkSmartPointer< vtkPolyData > fiberPolyData ;
    vtkSmartPointer< vtkPolyData > cleanedFiberPolyData ;
    std::string extension = ExtensionOfFile( InputFileName ) ;
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > fiberPolyDataReader = vtkSmartPointer< vtkPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, InputFileName ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataReader> fiberPolyDataReader = vtkSmartPointer< vtkXMLPolyDataReader >::New() ;
        fiberPolyData = ReadFiberFile( fiberPolyDataReader, InputFileName ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    vtkSmartPointer< vtkAbstractArray > tensors = fiberPolyData->GetPointData()->GetAbstractArray( "tensors" ) ;
    std::vector< std::vector< float > > vecPointData ;
    std::vector< float > cumul , average ;
    vtkSmartPointer< vtkDoubleArray > pointData ;
    if( FlagAttribute == 0 )
    {
        vecPointData = ApplyMaskToFiber( fiberPolyData ) ;
        cumul = CumulValuePerFiber( vecPointData ) ;
        average = AverageValuePerFiber( vecPointData ) ;
        vtkSmartPointer< vtkDoubleArray > cellData , cellData2 ;
        cellData =  CreateCellData( cumul , "CumulativeValue" ) ;
        cellData2 = CreateCellData( average , "AverageValue" ) ;
        fiberPolyData->GetCellData()->AddArray( cellData ) ;
        fiberPolyData->GetCellData()->AddArray( cellData2 ) ;
        pointData =  CreatePointData( vecPointData , "InsideMask" ) ;
        fiberPolyData->GetPointData()->AddArray( pointData ) ;
    }
    vtkSmartPointer< vtkPolyData > visuFiber = vtkSmartPointer< vtkPolyData >::New() ;
    if( FlagVisualize )
    {
        visuFiber = CreateVisuFiber( fiberPolyData ) ;
    }
    if( extension.rfind( "vtk" ) != std::string::npos )
    {
        fileName.visu =  ChangeEndOfFileName( OutputFileName , "-visu.vtk" ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        fileName.visu = ChangeEndOfFileName( OutputFileName , "-visu.vtp" ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    cleanedFiberPolyData = CheckNaN( fiberPolyData ) ;
    if( !FlagClean )
    {
        cleanedFiberPolyData->GetPointData()->AddArray( tensors ) ;
    }
    else
    {
        if( FlagThreshold ==true )
        {
            cleanedFiberPolyData = CleanFiber( fiberPolyData , Threshold ) ;
        }
    }
    cleanedFiberPolyData = CropFiber( cleanedFiberPolyData , vecPointData ) ;
    WriteFiberFile( visuFiber , fileName.visu ) ;
    WriteFiberFile( cleanedFiberPolyData , fileName.output ) ;
    //if( FlagAttribute == 0 )
    //{
     //   WriteLogFile( fileName , vecPointData , cleanedFiberPolyData , cumul , average ) ;
   // }
    return 0 ;
}
