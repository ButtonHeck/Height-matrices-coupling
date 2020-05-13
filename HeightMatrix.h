#ifndef HEIGHTMATRIX_H
#define HEIGHTMATRIX_H

#include <vector>
#include <HeightMatrixIterator.h>

enum SIDE : int
{
  LEFT, RIGHT, TOP, BOTTOM
};

enum MATRIX_TYPE : int
{
  MASTER, TARGET
};

class HeightMatrix
{
public:
  constexpr static float MAX_HEIGHT = 2.0f;

  //RowIterator declaration
  class RowIterator : public HeightMatrixIterator
  {
  public:
    float& operator*();
    float operator++(int) override;
    float operator--(int) override;

  private:
    friend class HeightMatrix;
    RowIterator(const int row, std::vector<std::vector<float>>& storage);
    std::vector<float>::iterator iter;
  };

  //ConstRowIterator declaration
  class ConstRowIterator : public HeightMatrixIterator
  {
  public:
    const float& operator*() const;
    float operator++(int) override;
    float operator--(int) override;

  private:
    friend class HeightMatrix;
    ConstRowIterator(const int row, const std::vector<std::vector<float>>& storage);
    std::vector<float>::const_iterator iter;
  };

  //ColumnIterator declaration
  class ColumnIterator : public HeightMatrixIterator
  {
  public:
    float& operator*();
    float operator++(int) override;
    float operator--(int) override;

  private:
    friend class HeightMatrix;
    ColumnIterator(const int column, std::vector<std::vector<float>>& storage);
    std::vector<std::vector<float>>::iterator iter;
    size_t column;
  };

  //ConstColumnIterator declaration
  class ConstColumnIterator : public HeightMatrixIterator
  {
  public:
    const float& operator*() const;
    float operator++(int) override;
    float operator--(int) override;

  private:
    friend class HeightMatrix;
    ConstColumnIterator(const int column, const std::vector<std::vector<float>>& storage);
    std::vector<std::vector<float>>::const_iterator iter;
    size_t column;
  };

  HeightMatrix(std::size_t width, std::size_t height, double precision);
  RowIterator rowBegin(const int row);
  ConstRowIterator rowBegin(const int row) const;
  ColumnIterator columnBegin(const int column);
  ConstColumnIterator columnBegin(const int column) const;
  std::size_t getWidth() const;
  std::size_t getHeight() const;
  double getPrecision() const;

private:
  std::vector<std::vector<float>> storage;
  std::size_t width, height;
  double precision;
};

#endif // HEIGHTMATRIX_H
