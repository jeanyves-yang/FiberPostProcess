#include "../csv.h"

int main()
{
    csv csvBaseline ;
    csvBaseline.read( "Data/initHeader.csv" ) ;
    csv csvTest ;
    std::vector< std::vector< std::string > > header ;
    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back("Fiber File ") ;
    buff.push_back( logFileName ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask Input: " ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    csvTest.initHeader( header ) ;
    if(csvTest == csvBaseline )
    {
        return 1 ;
    }

    return 0 ;
}
