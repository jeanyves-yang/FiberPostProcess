#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class processing{
    int visualize ;
public:
    processing( int ) ;
    std::string ExtensionOfFile(std::string filename) ;
    std::string ChangeEndOfFileName ( std::string fileName, std::string change ) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    int FindMaxNbOfPoints( vtkSmartPointer< vtkPolyData > polyData ) ;
    template< class T>
    vtkSmartPointer< vtkPolyData > ReadFiberFile( T reader , std::string fiberFile ) ;
    void WriteFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName ) ;
    std::vector< std::vector< float > > ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData , std::string maskFileName  ) ;
    vtkSmartPointer< vtkDoubleArray > CreatePointData( std::vector< std::vector< float> > vecPointData ) ;
    vtkSmartPointer< vtkDoubleArray > CreateCellData( std::vector< float > vecCellData ) ;
    std::vector< float > CumulValuePerFiber( std::vector< std::vector< float> > pointData ) ;
    vtkSmartPointer< vtkPolyData > CleanFiber( vtkSmartPointer< vtkPolyData > PolyData , float threshold ) ;
    vtkSmartPointer< vtkPolyData > AddPointData( vtkSmartPointer< vtkPolyData > PolyData ) ;
    vtkSmartPointer< vtkPolyData > CreateVisuFiber( vtkSmartPointer< vtkPolyData > PolyData ) ;
    int processing_main(std::string& inputFileName ,
                        std::string& outputFileName ,
                        std::string& maskFileName ,
                        float threshold
                        ) ;
};
