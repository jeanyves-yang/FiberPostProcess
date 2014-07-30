#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class processing{
public:
    ~processing() ;
    std::string ExtensionofFile(std::string filename) ;
    template< class T>
    void FindAllData(T polydata ) ;
    /*vtkSmartPointer< vtkPolyData >*/void readFiberFile(vtkSmartPointer<vtkPolyData> PolyData, std::string fiberFile , std::string maskFile ) ;
    void processing_main(std::string& input_file ,
                         std::string& output_file,
                         std::string& mask_file
                                     ) ;
    void test( std::string& mask ) ;
    void convertVTKtoITKspace(vtkSmartPointer<vtkPolyData> PolyData , std::string maskFileName) ;
};
