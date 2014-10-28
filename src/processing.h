#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <sstream>
#include "utils.h"

class processing
{
public:
    struct fileNameStruct
    {
        std::string input ;
        std::string mask ;
        std::string visu ;
        std::string output ;
        std::string log ;
    };

    processing() ;
    void SetInputFileName( std::string inputFiberFileName ) ;
    void SetOutputFileName( std::string outputFiberFileName ) ;
    void SetVisualisation( int visualisationFlag ) ;
    void SetAttributeFileName( std::string attributeFileName ) ;
    void SetThresholdFlag( int thresholdFlag ) ;
    void SetAttributeFlag( int attributeFlag ) ;
    void SetCleanFlag( int cleanFlag ) ;
//to put in protected !!!
    void WriteLogFile(fileNameStruct fileName , std::vector< std::vector< float> > data ,
                      vtkSmartPointer< vtkPolyData > fiberFile , std::vector<float> cumul , std::vector<float> average ) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    int FindMaxNbOfPoints( vtkSmartPointer< vtkPolyData > polyData ) ;
    template< class T>
    vtkSmartPointer< vtkPolyData > ReadFiberFile( T reader , std::string fiberFile ) ;
    void WriteFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName ) ;
    int GetPointId( int fiberId, int pointId , vtkSmartPointer< vtkPolyData > polyData ) ;
    vtkSmartPointer< vtkPolyData > CheckNaN( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData ) ;//change name
    vtkSmartPointer< vtkPolyData > CheckNaN( vtkSmartPointer< vtkPolyData > polyData ) ;
    std::vector< std::vector< float > > convertDataToVector( vtkSmartPointer< vtkDataArray > array ) ;
    std::vector< std::vector< float > > ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData  ) ;
    vtkSmartPointer< vtkPolyData > CropFiber(vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData ) ;
    void deletePoint( vtkSmartPointer< vtkPoints > fiberPoints , int pointId ) ;
    std::vector< std::string > ThresholdPolyData( vtkSmartPointer< vtkPolyData > polyData ) ;
    vtkSmartPointer< vtkDoubleArray > CreatePointData( std::vector< std::vector< float> > vecPointData , const char* fieldName ) ;
    vtkSmartPointer< vtkDoubleArray > CreateCellData( std::vector< float > vecCellData , const char* fieldName ) ;
    std::vector< float > CumulValuePerFiber( std::vector< std::vector< float> > pointData ) ;
    std::vector< float > AverageValuePerFiber( std::vector< std::vector< float> > pointData ) ;
    vtkSmartPointer< vtkPolyData > CleanFiber( vtkSmartPointer< vtkPolyData > polyData , float threshold ) ;
    vtkSmartPointer< vtkPolyData > AddPointData( vtkSmartPointer< vtkPolyData > polyData ) ;
    vtkSmartPointer< vtkPolyData > CreateVisuFiber( vtkSmartPointer< vtkPolyData > polyData ) ;
    int run() ;

private:
    int FlagVisualize ; // enables the writing of a vtk file visualizable through Slicer or another visualizer (removes the tensors, point data fields are not visualizable if tensors are present in the vtk file)
    std::string InputFileName ;
    std::string OutputFileName ;
    std::string AttributeFileName ; // name of the attribute/mask file
    int FlagAttribute ; // 0 if there is a mask, 1 if there is no mask
    int FlagThreshold ;
    int FlagClean ;
    float Threshold ;
};
