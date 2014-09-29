#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <sstream>
#include "utils.h"

class processing{
    int visualize ;
public:
    struct fileNameStruct{
        std::string input ;
        std::string mask ;
        std::string visu ;
        std::string output ;
        std::string cleaned ;
    };

    processing( int ) ;

    void WriteLogFile(fileNameStruct fileName , std::vector< std::vector< float> > data ,
                      float threshold , vtkSmartPointer< vtkPolyData > cleanedFiberFile , std::vector<float> cumul , std::vector<float> average) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    int FindMaxNbOfPoints( vtkSmartPointer< vtkPolyData > polyData ) ;
    template< class T>
    vtkSmartPointer< vtkPolyData > ReadFiberFile( T reader , std::string fiberFile ) ;
    void WriteFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName ) ;
    vtkSmartPointer< vtkPolyData > CheckNaN( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData ) ;
    vtkSmartPointer< vtkPolyData > CheckNaN( vtkSmartPointer< vtkPolyData > polyData ) ;
    std::vector< std::vector< float > > convertDataToVector( vtkSmartPointer< vtkDataArray > array ) ;
    std::vector< std::vector< float > > ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData , std::string attributeFileName  ) ;
    vtkSmartPointer< vtkPolyData > CropFiber( vtkSmartPointer< vtkPolyData > polyData , std::vector< std::vector< float > > vecPointData ) ;
    void deletePoint( vtkSmartPointer< vtkPoints > fiberPoints , int pointId ) ;
    std::vector< std::string > ThresholdPolyData( vtkSmartPointer< vtkPolyData > polyData , float threshold ) ;
    vtkSmartPointer< vtkDoubleArray > CreatePointData( std::vector< std::vector< float> > vecPointData , const char* fieldName ) ;
    vtkSmartPointer< vtkDoubleArray > CreateCellData( std::vector< float > vecCellData , const char* fieldName ) ;
    std::vector< float > CumulValuePerFiber( std::vector< std::vector< float> > pointData ) ;
    std::vector< float > AverageValuePerFiber( std::vector< std::vector< float> > pointData ) ;
    vtkSmartPointer< vtkPolyData > CleanFiber( vtkSmartPointer< vtkPolyData > polyData , float threshold ) ;
    vtkSmartPointer< vtkPolyData > AddPointData( vtkSmartPointer< vtkPolyData > polyData ) ;
    vtkSmartPointer< vtkPolyData > CreateVisuFiber( vtkSmartPointer< vtkPolyData > polyData ) ;

    std::vector< std::vector< std::string > > GetCellData( vtkSmartPointer< vtkPolyData > polyData , char * fieldName ) ;
    std::vector< std::vector< std::string > > GetPointData( vtkSmartPointer< vtkPolyData > polyData , char * fieldName ) ;
    int processing_main(std::string& inputFileName ,
                        std::string& outputFileName ,
                        std::string& attributeFileName ,
                        float threshold ,
                        bool flagAttribute
                        ) ;
    int processing_main(std::string& inputFileName ,
                        std::string& outputFileName
                        ) ;

    int processing_main( std::string& inputFileName ,
                         std::string& outputFileName ,
                         std::string& attributeFileName,
                         bool flagAttribute
                         ) ;
};
