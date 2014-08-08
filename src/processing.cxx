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

std::string processing::ExtensionofFile( std::string filename )
{
    std::string extension ;
    extension = filename.substr( filename.find_last_of(".")+1,filename.size()-filename.find_last_of( "." ) + 1 ) ;
    return extension ;
}


void processing::FindAllData( vtkSmartPointer< vtkPolyData > polydata )
{
    polydata->GetLines()->InitTraversal() ;
    std::cout << "Normals: " << polydata->GetPointData()->GetNormals() << std::endl ;
    vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays() ;
    std::cout << "Lines: " << polydata->GetNumberOfLines() << std::endl ;
    vtkSmartPointer< vtkIdList > idList = vtkSmartPointer< vtkIdList >::New() ;
    std::cout << "Cells: " << polydata->GetNumberOfCells() << std::endl ;
    std::cout << "Points: " << polydata->GetNumberOfPoints() << std::endl ;
    std::cout << "Polys: " << polydata->GetNumberOfPolys() << std::endl ;
    std::cout << "Strips: " << polydata->GetNumberOfStrips() << std::endl ;
    std::cout << "Verts: " << polydata->GetNumberOfVerts() << std::endl ;
    int i=1;
    while( polydata->GetLines()->GetNextCell( idList ) )
    {
        std::cout<< "Line " << i << ": " << idList->GetNumberOfIds() << " points" <<std::endl ;
        i++;
        for( vtkIdType pointId = 0 ; pointId < idList->GetNumberOfIds() ; pointId++ )
        {
            /*std::cout <<idList->GetId( pointId ) << " " ;*/
        }
    }
}
template <class T>
vtkSmartPointer< vtkPolyData > processing::readFiberFile( T reader , std::string fiberFile )
{
    vtkSmartPointer< vtkPolyData > PolyData ;
    reader->SetFileName( fiberFile.c_str() ) ;
    PolyData=reader->GetOutput() ;
    reader->Update() ;
    return PolyData ;
}

void processing::writeFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName )
{
    vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New() ;
    writer->SetInputData( PolyData ) ;
    //writer->SetDataModeToBinary() ;
    writer->SetFileName( outputFileName.c_str() ) ;
    writer->Update() ;
}

vtkSmartPointer< vtkPolyData > processing::ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData , std::string maskFileName )
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
    const int nfib = PolyData->GetNumberOfCells() ;
    std::cout<< nfib <<std::endl;
    // Inserting an array corresponding to the % of the fiber which is not inside the mask
    vtkSmartPointer< vtkDoubleArray > fiberData = vtkSmartPointer< vtkDoubleArray >::New() ;
    vtkSmartPointer< vtkDoubleArray > pointData = vtkSmartPointer< vtkDoubleArray >::New() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        float badPixel = 0.0f ;
        std::cout << " FIBER NUMBER " << i << std::endl ;
        vtkSmartPointer< vtkCell > fiber = PolyData->GetCell( i ) ;
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
            if(pixel == 255)
            {

                badPixel = badPixel +1.0f ;
                pointData->SetNumberOfComponents( 1 ) ;
                pointData->SetName( "InsideMask" ) ;
                pointData->InsertNextValue( 255 ) ;
            }
            else
            {
                pointData->InsertNextValue( 0 ) ;
            }
        }
        std::cout << "Pixels out of mask = " << badPixel << std::endl ;
        fiberData->SetNumberOfComponents( nfib ) ;
        fiberData->SetName( "UnwantedFiber" ) ;
        fiberData->InsertNextValue( badPixel/fiberPoints->GetNumberOfPoints()*100 ) ;

        // Reading the data inserted
        std::cout<< "% of the fiber outside of the mask = " << badPixel/fiberPoints->GetNumberOfPoints()*100 << "%" <<std::endl ;
    }
    PolyData->GetPointData()->AddArray( pointData ) ;
    PolyData->GetCellData()->AddArray( fiberData ) ;
    return PolyData ;
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
            pointData->SetNumberOfComponents( PolyData->GetNumberOfPoints() ) ;
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
    fiberReadArray = vtkDoubleArray::SafeDownCast( PolyData->GetCellData()->GetArray("UnwantedFiber") ) ;
    Lines->InitTraversal() ;
    for( int i = 0 ; Lines->GetNextCell( NumberOfPoints , Ids ) ; i++ )
    {
        std::cout<<"coucou";
        vtkSmartPointer< vtkPolyLine > NewLine = vtkSmartPointer< vtkPolyLine >::New() ;
        NewLine->GetPointIds()->SetNumberOfIds( NumberOfPoints ) ;
        std::cout << " FIBER NUMBER " << i << std::endl ;
        std::cout << fiberReadArray->GetValue( i ) <<"%"<<std::endl ;
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
            std::cout << "Deleted" <<std::endl ;
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
    std::string extension = ExtensionofFile( inputFileName ) ;
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > fiberPolyDataReader = vtkSmartPointer< vtkPolyDataReader >::New() ;
        fiberPolyData = readFiberFile( fiberPolyDataReader, inputFileName ) ;
    }
    else if( extension.rfind("vtp") != std::string::npos )
    {
        vtkSmartPointer<vtkXMLPolyDataReader> fiberPolyDataReader = vtkSmartPointer< vtkXMLPolyDataReader >::New() ;
        fiberPolyData = readFiberFile( fiberPolyDataReader, inputFileName ) ;
    }
    else
    {
        std::cout << "File could not be read" << std::endl ;
        return 1 ;
    }
    FindAllData( fiberPolyData ) ;
    fiberPolyData = ApplyMaskToFiber( fiberPolyData , maskFileName ) ;
    cleanedFiberPolyData = CleanFiber(fiberPolyData , threshold ) ;
    cleanedFiberPolyData = AddPointData( cleanedFiberPolyData ) ;
    vtkSmartPointer< vtkPolyData > visuFiber = vtkSmartPointer< vtkPolyData >::New() ;
    if( visualize )
    {
        visuFiber = CreateVisuFiber( fiberPolyData ) ;
    }
    writeFiberFile( visuFiber , "visuFiber.vtk") ;
    writeFiberFile( fiberPolyData , outputFileName ) ;
    writeFiberFile( cleanedFiberPolyData , "cleanedFiber.vtk" ) ;
    return 0 ;
}
