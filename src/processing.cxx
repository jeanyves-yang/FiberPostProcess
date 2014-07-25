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

processing::~processing()
{}


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

void processing::processing_main(std::string& input_file ,
                                 std::string& output_file ,
                                 std::string& mask_file
                                 )
{
    vtkSmartPointer < vtkPolyData > PolyData;
    vtkSmartPointer < vtkPolyData > PolyDataMask;
    std::string extension = ExtensionofFile( input_file ) ;
    std::string extension_mask = ExtensionofFile( mask_file ) ;
    if( extension.compare( mask_file ) == 0 )
    {
        std::cout << "The mask does not have the same extension as the input file." << std::endl ;
        return ;
    }
    if( extension.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > reader = vtkPolyDataReader::New() ;
        reader->SetFileName( input_file.c_str() ) ;
        PolyData=reader->GetOutput() ;
        reader->Update() ;
        std::cout << "VTK File read" << std::endl ;
    }
    else
    {
        if( extension.rfind("vtp") != std::string::npos )
        {
            vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New() ;
            reader->SetFileName( input_file.c_str() ) ;
            PolyData = reader->GetOutput() ;
            reader->Update() ;
            std::cout << "VTP File read" << std::endl ;
        }
        else
        {
            std::cout << "File could not be read" << std::endl ;
        }
    }

    if( extension_mask.rfind("vtk") != std::string::npos )
    {
        vtkSmartPointer< vtkPolyDataReader > readerMask = vtkPolyDataReader::New() ;
        readerMask->SetFileName( mask_file.c_str() ) ;
        PolyDataMask=readerMask->GetOutput() ;
        readerMask->Update() ;
        std::cout << "VTK MaskFile read" << std::endl ;
    }
    else
    {
        if( extension_mask.rfind("vtp") != std::string::npos )
        {
            vtkSmartPointer<vtkXMLPolyDataReader> readerMask = vtkXMLPolyDataReader::New() ;
            readerMask->SetFileName( input_file.c_str() ) ;
            PolyDataMask = readerMask->GetOutput() ;
            readerMask->Update() ;
            std::cout << "VTP MaskFile read" << std::endl ;
        }
        else
        {
            std::cout << "Mask could not be read" << std::endl ;
        }
    }


    if( extension.rfind("vtk") != std::string::npos )
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
    }
    FindAllData( PolyData ) ;


}
