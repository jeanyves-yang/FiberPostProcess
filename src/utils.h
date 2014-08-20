#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <vector>

std::string ExtensionOfFile( std::string fileName ) ;

std::string ChangeEndOfFileName (std::string fileName, std::string change ) ;

template< class T >
std::string Convert( T number ) ;

std::vector< std::vector < std::string  > > ConvertArray( std::vector< std::vector< float > > array ) ;

int FindMaxNbofCols( std::vector< std::vector< std::string > > vecData ) ;

template< class T >
std::string Convert( T number )
{
    std::ostringstream buff ;
    buff << number ;
    return buff.str() ;
}

#endif // UTILS_H
