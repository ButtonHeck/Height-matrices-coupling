#include "HeightMatrixIterator.h"

HeightMatrixIterator::HeightMatrixIterator(size_t endIndex)
  :
    currentIndex(0),
    endIndex(endIndex)
{}

bool HeightMatrixIterator::isValid()
{
  return currentIndex < endIndex;
}

int HeightMatrixIterator::getCurrentIndex() const
{
  return currentIndex;
}
