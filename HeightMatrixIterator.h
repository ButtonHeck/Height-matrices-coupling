#ifndef HEIGHTMATRIXITERATOR_H
#define HEIGHTMATRIXITERATOR_H

#include <iterator>

class HeightMatrixIterator
{
public:
  HeightMatrixIterator(size_t endIndex);
  bool isValid();
  int getCurrentIndex() const;
  virtual float operator++(int) = 0;
  virtual float operator--(int) = 0;

protected:
  size_t currentIndex;
  size_t endIndex;
};

#endif // HEIGHTMATRIXITERATOR_H
