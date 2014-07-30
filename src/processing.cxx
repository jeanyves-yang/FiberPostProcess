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

processing::~processing()
{}


std::string processing::ExtensionofFile( std::string filename )
{
    std::string extension ;
    extension = filename.substr( filename.find_last_of(".")+1,filename.size()-filename.find_last_of( "." ) + 1 ) ;
    return extension ;
}

template< class T> //vtkpolydata
void processing::FindAllData( T polydata )
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
    while( polydata->GetLines()->GetNextCell(idList) )
    {
        std::cout<< "Line " << i << ": " << idList->GetNumberOfIds() << " points" <<std::endl ;
        i++;
        for( vtkIdType pointId = 0 ; pointId < idList->GetNumberOfIds() ; pointId++ )
        {
            /*std::cout <<idList->GetId( pointId ) << " " ;*/

        }
    }
}

/*vtkSmartPointer< vtkPolyData >*/ void processing::readFiberFile( vtkSmartPointer < vtkPolyData > PolyData , std::string fiberFile , std::string maskFile )
{
    typedef itk::Image< unsigned char , 3 > ImageType ;
    typedef itk::ImageFileReader< ImageType > ReaderType ;
    ReaderType::Pointer reader = ReaderType::New() ;
    reader->SetFileName( maskFile ) ;
    ImageType::Pointer fiberImage = ImageType::New() ;
    fiberImage = reader->GetOutput() ;
    ImageType::IndexType index ;
    itk::Point< double , 3 > p ;
    vtkSmartPointer < vtkPolyData > fiberPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    std::string extension = ExtensionofFile( fiberFile ) ;
    /*if( extension.compare( mask_file ) == 0 )
    {
        std::cout << "The mask does not have the same extension as the input file." << std::endl ;
        return ;
    }*/
    /* Check the file extension */
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > reader = vtkPolyDataReader::New() ;
        reader->SetFileName( fiberFile.c_str() ) ;
        PolyData=reader->GetOutput() ;
        reader->Update() ;
        std::cout << "VTK File read" << std::endl ;
        //return fiberPolyData ;
    }
    else
    {
        if( extension.rfind("vtp") != std::string::npos )
        {
            vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New() ;
            reader->SetFileName( fiberFile.c_str() ) ;
            PolyData = reader->GetOutput() ;
            reader->Update() ;
            std::cout << "VTP File read" << std::endl ;
            //return fiberPolyData ;
        }
        else
        {
            std::cout << "File could not be read" << std::endl ;
            //return fiberPolyData ;
        }
    }
    /*    TEST                */
    const int nfib = PolyData->GetNumberOfCells() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        //double [][] pointsCoord ;
        vtkSmartPointer< vtkCell > fiber = PolyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        std::cout << "fiberPoints->GetNumberOfPoints()" << std::endl ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            double* coordinates = fiberPoints->GetPoint( j ) ;
            std::cout << coordinates[0] << " " << coordinates[1] << " " << coordinates[2] << std::endl;
            /* Flip the coordinates */
            coordinates[0] = - coordinates[0] ;
            coordinates[1] = - coordinates[1] ;
            p[0] = coordinates[0] ;
            p[1] = coordinates[1] ;
            p[2] = coordinates[2] ;

            fiberImage->TransformPhysicalPointToIndex( p , index ) ;

        }
    }

}


    /*FindAllData( PolyData ) ;

   /**/

    /*if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataWriter > fiberwriter = vtkPolyDataWriter::New() ;
        fiberwriter->SetFileName( output_file.c_str() ) ;
        fiberwriter->SetInputData(PolyData);
        fiberwriter->Update() ;
        std::cout << "VTK File written" << std::endl ;
    }
    else
    {
        if( extension.rfind("vtp") != std::string::npos )
        {
            vtkSmartPointer< vtkXMLPolyDataWriter > fiberwriter = vtkXMLPolyDataWriter::New() ;
            fiberwriter->SetFileName( output_file.c_str() ) ;
            fiberwriter->SetInputData( PolyData ) ;
            fiberwriter->Update() ;
            std::cout << "VTP File written" << std::endl ;
        }
        else
        {
            std::cout << "File could not be written" << std::endl ;
        }
    }*/


void processing::convertVTKtoITKspace( vtkSmartPointer < vtkPolyData > PolyData , std::string maskFileName )
{
    /* Gets the coordinates of each points per fiber */
    const int nfib = PolyData->GetNumberOfCells() ;
    for( int i = 0 ; i < nfib ; ++i )
    {
        //double [][] pointsCoord ;
        vtkSmartPointer< vtkCell > fiber = PolyData->GetCell( i ) ;
        vtkSmartPointer< vtkPoints > fiberPoints = fiber->GetPoints() ;
        std::cout << "fiberPoints->GetNumberOfPoints()" << std::endl ;
        for( int j = 0 ; j < fiberPoints->GetNumberOfPoints() ; ++j )
        {
            double* coordinates = fiberPoints->GetPoint( j ) ;
            std::cout << coordinates[0] << " " << coordinates[1] << " " << coordinates[2] << std::endl;
            /* Flip the coordinates */
            coordinates[0] = - coordinates[0] ;
            coordinates[1] = - coordinates[1] ;
        }
    }
}

void processing::test( std::string& mask )
{
    /*vtkSmartPointer< vtkImageReader2 > test = vtkSmartPointer< vtkImageReader2 >::New() ;
    test->SetFileName( mask.c_str() );
    std::cout<< test->GetFileName() << std::endl ;
    test->Update() ;
    visualize( test ) ;*/
    typedef itk::Image<unsigned char, 3> ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType ;
    ReaderType::Pointer readerMask = ReaderType::New() ;
    readerMask->SetFileName( mask ) ;
    readerMask->Update() ;
    ImageType::Pointer image = readerMask->GetOutput() ;
    typedef itk::ImageToVTKImageFilter< ImageType > itkVtkConverter ;
    itkVtkConverter::Pointer conv = itkVtkConverter::New() ;
    conv->SetInput( image ) ;
    conv->Update() ;
    vtkSmartPointer<  vtkImageData > image2 = vtkSmartPointer< vtkImageData >::New() ;
    image2->ShallowCopy( conv->GetOutput() ) ;
    return ;
}

void processing::processing_main(std::string& input_file ,
                                 std::string& output_file ,
                                 std::string& mask_file
                                 )
{
    vtkSmartPointer < vtkPolyData > fiberPolyData = vtkSmartPointer< vtkPolyData >::New() ;
    /*fiberPolyData =*/ readFiberFile( fiberPolyData, input_file, mask_file ) ;
    //convertVTKtoITKspace( fiberPolyData , mask_file );
    test( mask_file ) ;
}
