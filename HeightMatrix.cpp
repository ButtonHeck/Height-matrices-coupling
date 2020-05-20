#include "HeightMatrix.h"

/**
 * @brief Utility function to convert an int value to SIDE enum value with bounds check
 * @param side int representation of a side
 * @return SIDE value according to its integer representation if one matches, or SIDE::LEFT otherwise
 */
COMPARISON_SIDE HeightMatrix::sideFrom( int side )
{
    return side <= 3 ? COMPARISON_SIDE(side) : COMPARISON_SIDE::LEFT;
}

HeightMatrix::HeightMatrix( size_t width,
                            size_t height,
                            double precision,
                            MATRIX_TYPE type )
    : width(width)
    , height(height)
    , precision(precision)
    , type(type)
{
    //allocate data for storage
    storage.resize(height);
    for ( std::vector<float> & row : storage )
    {
        row.resize(width);
    }
}


//----HeightMatrix getters---------

size_t HeightMatrix::getWidth() const
{
    return width;
}

size_t HeightMatrix::getHeight() const
{
    return height;
}

double HeightMatrix::getPrecision() const
{
    return precision;
}

HeightMatrix::MATRIX_TYPE HeightMatrix::getType() const
{
    return type;
}

HeightMatrix::RowIterator HeightMatrix::rowBegin( const size_t ROW )
{
    return RowIterator( ROW, storage );
}

HeightMatrix::ConstRowIterator HeightMatrix::rowBegin( const size_t ROW ) const
{
    return ConstRowIterator( ROW, storage );
}

HeightMatrix::ColumnIterator HeightMatrix::columnBegin( const size_t COLUMN )
{
    return ColumnIterator( COLUMN, storage );
}

HeightMatrix::ConstColumnIterator HeightMatrix::columnBegin( const size_t COLUMN ) const
{
    return ConstColumnIterator( COLUMN, storage );
}


//----Iterator definitions-----

HeightMatrix::Iterator::Iterator( size_t endIndex )
    : currentIndex(0)
    , endIndex(endIndex)
{}

bool HeightMatrix::Iterator::isValid()
{
    return currentIndex < endIndex;
}

size_t HeightMatrix::Iterator::getCurrentIndex() const
{
    return currentIndex;
}


//----RowIterator definitions----

HeightMatrix::RowIterator::RowIterator(const size_t ROW,
                                        std::vector< std::vector<float> > & storage )
    : Iterator( storage.at(ROW).size() )
    , iter( storage.at(ROW).begin() )
{}

float & HeightMatrix::RowIterator::operator*()
{
    return *iter;
}

float HeightMatrix::RowIterator::operator++(int)
{
    float prev = *iter;
    iter++;
    currentIndex++;
    return prev;
}

float HeightMatrix::RowIterator::operator--(int)
{
    float prev = *iter;
    iter--;
    currentIndex--;
    return prev;
}


//----ConstRowIterator definitions----

HeightMatrix::ConstRowIterator::ConstRowIterator(const size_t ROW,
                                                  const std::vector< std::vector<float> > & STORAGE )
    : Iterator( STORAGE.at(ROW).size() )
    , iter( STORAGE.at(ROW).cbegin() )
{}

const float & HeightMatrix::ConstRowIterator::operator*() const
{
    return *iter;
}

float HeightMatrix::ConstRowIterator::operator++(int)
{
    float prev = *iter;
    iter++;
    currentIndex++;
    return prev;
}

float HeightMatrix::ConstRowIterator::operator--(int)
{
    float prev = *iter;
    iter--;
    currentIndex--;
    return prev;
}


//----ColumnIterator definitions----

HeightMatrix::ColumnIterator::ColumnIterator(const size_t COLUMN,
                                              std::vector< std::vector<float> > & storage )
    : Iterator( storage.size() )
    , iter( storage.begin() )
    , column(COLUMN)
{}

float & HeightMatrix::ColumnIterator::operator*()
{
    return (*iter).at(column);
}

float HeightMatrix::ColumnIterator::operator++(int)
{
    float prev = (*iter).at(column);
    iter++;
    currentIndex++;
    return prev;
}

float HeightMatrix::ColumnIterator::operator--(int)
{
    float prev = (*iter).at(column);
    iter--;
    currentIndex--;
    return prev;
}


//----ConstColumnIterator definitions----

HeightMatrix::ConstColumnIterator::ConstColumnIterator(const size_t COLUMN,
                                                        const std::vector< std::vector<float> > & STORAGE )
    : Iterator( STORAGE.size() )
    , iter( STORAGE.cbegin() )
    , column(COLUMN)
{}

const float & HeightMatrix::ConstColumnIterator::operator*() const
{
    return (*iter).at(column);
}

float HeightMatrix::ConstColumnIterator::operator++(int)
{
    float prev = (*iter).at(column);
    iter++;
    currentIndex++;
    return prev;
}

float HeightMatrix::ConstColumnIterator::operator--(int)
{
    float prev = (*iter).at(column);
    iter--;
    currentIndex--;
    return prev;
}
