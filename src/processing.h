#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class processing{
    int visualize ;
public:
    processing( int ) ;

    std::string ExtensionofFile(std::string filename) ;
    std::string changeEndofFileName ( std::string fileName, std::string change ) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    template< class T>
    vtkSmartPointer< vtkPolyData > readFiberFile( T reader , std::string fiberFile ) ;
    void writeFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName ) ;
    vtkSmartPointer< vtkPolyData > ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData , std::string maskFileName ) ;
    vtkSmartPointer< vtkPolyData > CleanFiber( vtkSmartPointer< vtkPolyData > PolyData , float threshold ) ;
    vtkSmartPointer< vtkPolyData > AddPointData( vtkSmartPointer< vtkPolyData > PolyData ) ;
    vtkSmartPointer< vtkPolyData > CreateVisuFiber( vtkSmartPointer< vtkPolyData > PolyData ) ;
    int processing_main(std::string& inputFileName ,
                        std::string& outputFileName ,
                        std::string& maskFileName ,
                        float threshold
                        ) ;
};
