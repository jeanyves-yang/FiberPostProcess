#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class processing{
public:
    ~processing() ;
    std::string ExtensionofFile(std::string filename) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    template< class T>
    vtkSmartPointer< vtkPolyData > readFiberFile( T reader , std::string fiberFile ) ;
    void writeFiberFile( vtkSmartPointer< vtkPolyData > PolyData , std::string outputFileName ) ;
    vtkSmartPointer< vtkPolyData > ApplyMaskToFiber( vtkSmartPointer< vtkPolyData > PolyData , std::string maskFileName ) ;
    vtkSmartPointer< vtkPolyData > CleanFiber( vtkSmartPointer< vtkPolyData > PolyData , float threshold ) ;
    void processing_main(std::string& inputFileName ,
                         std::string& outputFileName,
                         std::string& maskFileName
                                     ) ;
};
