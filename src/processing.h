class processing{
public:
    ~processing() ;
    std::string ExtensionofFile(std::string filename) ;
    void FindAllData(vtkSmartPointer< vtkPolyData > polydata ) ;
    void processing_main(std::string& input_file ,
                         std::string& output_file,
                         std::string& mask_file
                                     ) ;
};
