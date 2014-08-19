#ifndef CSV_H
#define CSV_H

#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

template< class T >
std::string Convert ( T number )
{
    std::ostringstream buff ;
    buff << number ;
    return buff.str() ;
}

std::vector< std::vector < std::string  > > ConvertArray( std::vector< std::vector< float > > array )
{
    std::vector< std::vector < std::string  > > buffer ;
    for( int i = 0 ; i < array.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < array[i].size() ; j++ )
        {
            buff.push_back( Convert( array[ i ][ j ] ) ) ;
        }
        buffer.push_back( buff ) ;
    }
    return buffer ;
}

class csv
{
    friend bool operator==( const csv &, const csv & ) ;
    friend bool operator!=( const csv &, const csv & ) ;

public:
    csv() ;
    csv( char* ) ;
    void read(char * ) ;
    void write(char * ) ;
    void initData( std::vector< std::vector< std::string > > vecData ) ;
    std::string getfield( int , int ) ;
    int FindMaxNbofCols( std::vector< std::vector< std::string > > vecData ) ;
    void setfield( int , int , std::string ) ;
    const csv &operator=( const csv & ) ;
    int rows , cols ;

private:
    std::vector< std::vector< std::string > > data ;
};

csv::csv()
{
    rows = cols = 0 ;
}

csv::csv( char* fname )
{
    read( fname ) ;
}

const csv &csv::operator=( const csv &n )
{
    rows = n.rows ;
    cols = n.cols ;
    data = n.data ;
    return *this ;
}

int csv::FindMaxNbofCols( std::vector< std::vector< std::string > > vecData )
{
    int max = 0 ;
    for( int i = 0 ; i < vecData.size() ; i++ )
    {
        if( vecData[ i ].size() > max )
        {
            max = vecData[ i ].size() ;
        }
    }
    return max ;
}

void csv::initData( std::vector< std::vector< std::string > > vecData )
{
    data.clear() ;
    rows = vecData.size() ;
    cols = FindMaxNbofCols( vecData ) ;
    for( int i = 0 ; i < vecData.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < cols ; j++ )
        {
            if( j < vecData[ i ].size() )
            {
                buff.push_back( Convert( vecData[ i ][ j ] ) ) ;
            }
            else
            {
                buff.push_back( "0" ) ;
            }
        }
        data.push_back( buff ) ;
    }
}

void csv::read( char* fname )
{
    ifstream in( fname ) ;
    std::string element , delimiters = ",\n\r" ;
    char ch;
    rows = cols = 0 ;
    data.clear() ;
    data.push_back( std::vector < std::string >() ) ;
    while( in.read( (char*)&ch , 1 ) )
    {
        if( delimiters.find_first_of( ch ) == delimiters.npos )
        {
            element += ch ;
        }
        else
        {
            if( ch != '\r' )
            {
                data[rows].push_back( element ) ;
                element = "" ;
                if( ch == '\n' )
                {
                    data.push_back( std::vector< std::string >() ) ;
                    rows++ ;
                }
            }
        }
    }
    in.close();
    data[ rows ].push_back( element ) ;
    cols = data[ 0 ].size() ;
    rows = data.size() ;
}

void csv::write( char* fname )
{
    std::ofstream out ;
    out.open( fname , std::ofstream::out |std::ofstream::app ) ;
    for( int x = 0 ; x < rows ; x++ )
    {
        for( int y = 0 ; y < cols ; y++ )
        {
            out << data[ x ][ y ] ;
            if( y != cols - 1 )
            {
                out << "," ;
            }
        }
        out << endl ;
    }
    out.close() ;
}

std::string csv::getfield( int x , int y )
{
    return data[ x ][ y ] ;
}

void csv::setfield( int x , int y , std::string val )
{
    data[ x ][ y ] = val ;
}

bool operator==( const csv &d1 , const csv &d2 )
{
    return d1 == d2 ;
}

bool operator!=( const csv &d1 , const csv &d2 )
{
    return d1 != d2 ;
}
#endif
