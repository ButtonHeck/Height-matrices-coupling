#include "HeightMatrix.h"

SIDE HeightMatrix::sideFrom( int side )
{
    return side < 3 ? SIDE(side) : SIDE::LEFT;
}

HeightMatrix::HeightMatrix(std::size_t width, std::size_t height, double precision)
    :
      width(width),
      height(height),
      precision(precision)
{
    //allocate data for storage
    storage.resize(height);
    for (std::vector<float>& row : storage)
        row.resize(width);
}

std::size_t HeightMatrix::getWidth() const
{
    return width;
}

std::size_t HeightMatrix::getHeight() const
{
    return height;
}

double HeightMatrix::getPrecision() const
{
    return precision;
}

HeightMatrix::RowIterator HeightMatrix::rowBegin(const int row)
{
    return RowIterator(row, storage);
}

HeightMatrix::ConstRowIterator HeightMatrix::rowBegin(const int row) const
{
    return ConstRowIterator(row, storage);
}

HeightMatrix::ColumnIterator HeightMatrix::columnBegin(const int column)
{
    return ColumnIterator(column, storage);
}

HeightMatrix::ConstColumnIterator HeightMatrix::columnBegin(const int column) const
{
    return ConstColumnIterator(column, storage);
}


//RowIterator definitions

HeightMatrix::RowIterator::RowIterator(const int row, std::vector<std::vector<float>>& storage)
    :
      HeightMatrixIterator(storage.at(row).size()),
      iter(storage.at(row).begin())
{}

float &HeightMatrix::RowIterator::operator*()
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


//ConstRowIterator definitions

HeightMatrix::ConstRowIterator::ConstRowIterator(const int row, const std::vector<std::vector<float> > &storage)
    :
      HeightMatrixIterator(storage.at(row).size()),
      iter(storage.at(row).cbegin())
{}

const float &HeightMatrix::ConstRowIterator::operator*() const
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


//ColumnIterator definitions

HeightMatrix::ColumnIterator::ColumnIterator(const int column, std::vector<std::vector<float> > &storage)
    :
      HeightMatrixIterator(storage.size()),
      iter(storage.begin()),
      column(column)
{}

float &HeightMatrix::ColumnIterator::operator*()
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


//ConstColumnIterator definitions

HeightMatrix::ConstColumnIterator::ConstColumnIterator(const int column, const std::vector<std::vector<float> > &storage)
    :
      HeightMatrixIterator(storage.size()),
      iter(storage.cbegin()),
      column(column)
{}

const float &HeightMatrix::ConstColumnIterator::operator*() const
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
