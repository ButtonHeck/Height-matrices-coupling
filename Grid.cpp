#include "Grid.h"

Grid::Grid(QOpenGLShaderProgram *shader, QOpenGLFunctions &functions)
  :
    shader(shader),
    gridVerticesCount(0),
    functions(functions),
    functions_4_3(),
    showGrid(false)
{
  functions.glGenBuffers(1, &vbo);
  functions.glGenBuffers(1, &ebo);

  //GL_PRIMITIVE_RESTART is used for height matrix grid rendering
  functions.glEnable(GL_PRIMITIVE_RESTART);
  functions_4_3.initializeOpenGLFunctions();
  functions_4_3.glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
}

Grid::~Grid()
{
  functions.glDeleteBuffers(1, &vbo);
  functions.glDeleteBuffers(1, &ebo);
}

void Grid::update(const HeightMatrix &matrix, int side)
{
  if (matrix.getWidth() == 0)
    return;
  width = matrix.getWidth() * matrix.getPrecision();
  height = matrix.getHeight() * matrix.getPrecision();

  //make sure to renew storage
  vertices.clear();
  indices.clear();
  indicesOffset = 0;
  gridVerticesCount = 0;
  updateGridVertices(matrix.getPrecision());

  matrixVerticesCount = 0;
  updateMatrixVertices(matrix);

  comparisonSideVerticesCount = 0;
  updateComparisonSideVertices(matrix, side);

  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  functions.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);

  functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  functions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int Grid::getWidth() const
{
  return width;
}

int Grid::getHeight() const
{
  return height;
}

void Grid::setShowFlatGrid(bool isShow)
{
  showGrid = isShow;
}

void Grid::updateGridVertices(int matrixPrecision)
{
  int halfWidth = width / 2;
  int halfHeight = height / 2;

  //create lines parallel to X axis (count is equal to height of the grid plus one extra at z = 0.0)
  for (int z = -halfHeight; z <= halfHeight - matrixPrecision; z++)
    {
      // (-X;z) vertex
      GridVertex negX{ (float)(-halfWidth), (float)z };
      bufferGridVertex(std::move(negX));
      indicesOffset++;

      // (X;z) vertex
      GridVertex posX{ (float)(halfWidth - matrixPrecision), (float)z };
      bufferGridVertex(std::move(posX));
      indicesOffset++;
    }

  //create lines parallel to Z axis (count is equal to width of the grid plus one extra at x = 0.0)
  for (int x = -halfWidth; x <= halfWidth - matrixPrecision; x++)
    {
      // (x;-Z) vertex
      GridVertex negZ{ (float)x, (float)(-halfHeight) };
      bufferGridVertex(std::move(negZ));
      indicesOffset++;

      // (x;Z) vertex
      GridVertex posZ{ (float)x, (float)(halfHeight - matrixPrecision) };
      bufferGridVertex(std::move(posZ));
      indicesOffset++;
    }
}

void Grid::updateMatrixVertices(const HeightMatrix& matrix)
{
  int halfWidth = width / 2;
  int halfHeight = height / 2;
  float precision = (float)matrix.getPrecision();

  //create line strips parallel to X axis (count is equal to height of the grid plus one extra at z = 0.0)
  for (HeightMatrix::ConstColumnIterator column = matrix.columnBegin(0); column.isValid(); column++)
    {
      for (HeightMatrix::ConstRowIterator row = matrix.rowBegin(column.getCurrentIndex()); row.isValid(); row++)
        {
          MatrixVertex v{row.getCurrentIndex() * precision - halfWidth,
                         *row,
                         column.getCurrentIndex() * precision - halfHeight};
          bufferMatrixVertex(std::move(v));
          indices.emplace_back(indicesOffset++);
        }
      indices.emplace_back(PRIMITIVE_RESTART_INDEX);
    }

  //create line strips parallel to Z axis (count is equal to width of the grid plus one extra at x = 0.0)
  for (HeightMatrix::ConstRowIterator row = matrix.rowBegin(0); row.isValid(); row++)
    {
      for (HeightMatrix::ConstColumnIterator column = matrix.columnBegin(row.getCurrentIndex()); column.isValid(); column++)
        {
          MatrixVertex v{row.getCurrentIndex() * precision - halfWidth,
                         *column,
                         column.getCurrentIndex() * precision - halfHeight};
          bufferMatrixVertex(std::move(v));
          indices.emplace_back(indicesOffset++);
        }
      indices.emplace_back(PRIMITIVE_RESTART_INDEX);
    }
}

void Grid::updateComparisonSideVertices(const HeightMatrix &matrix, int side)
{
  int halfWidth = width / 2;
  int halfHeight = height / 2;
  float precision = (float)matrix.getPrecision();

  switch (side)
    {
    case LEFT:
      for (HeightMatrix::ConstColumnIterator column = matrix.columnBegin(0); column.isValid(); column++)
        {
          MatrixVertex v{(float)(-halfWidth),
                         *column,
                         column.getCurrentIndex() * precision - halfHeight};
          bufferComparisonSideVertex(std::move(v));
        }
      break;
    case RIGHT:
      for (HeightMatrix::ConstColumnIterator column = matrix.columnBegin(matrix.getWidth() - 1); column.isValid(); column++)
        {
          MatrixVertex v{(float)halfWidth - precision,
                         *column,
                         column.getCurrentIndex() * precision - halfHeight};
          bufferComparisonSideVertex(std::move(v));
        }
      break;
    case TOP:
      for (HeightMatrix::ConstRowIterator row = matrix.rowBegin(0); row.isValid(); row++)
        {
          MatrixVertex v{row.getCurrentIndex() * precision - halfWidth,
                         *row,
                         (float)(-halfHeight)};
          bufferComparisonSideVertex(std::move(v));
        }
      break;
    case BOTTOM:
      for (HeightMatrix::ConstRowIterator row = matrix.rowBegin(matrix.getHeight() - 1); row.isValid(); row++)
        {
          MatrixVertex v{row.getCurrentIndex() * precision - halfWidth,
                         *row,
                         (float)halfHeight - precision};
          bufferComparisonSideVertex(std::move(v));
        }
      break;
    }
}

void Grid::bufferGridVertex(Grid::GridVertex &&gridVertex)
{
  vertices.emplace_back(gridVertex.x);
  vertices.emplace_back(0.0f);
  vertices.emplace_back(gridVertex.z);
  gridVerticesCount++;
}

void Grid::bufferMatrixVertex(Grid::MatrixVertex &&heightMatrixVertex)
{
  vertices.emplace_back(heightMatrixVertex.x);
  vertices.emplace_back(heightMatrixVertex.y);
  vertices.emplace_back(heightMatrixVertex.z);
  matrixVerticesCount++;
}

void Grid::bufferComparisonSideVertex(Grid::MatrixVertex &&sideVertex)
{
  vertices.emplace_back(sideVertex.x);
  vertices.emplace_back(sideVertex.y);
  vertices.emplace_back(sideVertex.z);
  comparisonSideVerticesCount++;
}

void Grid::draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix)
{
  //update transformation matrices
  shader->bind();
  shader->setUniformValue(shader->uniformLocation("u_projection"), projectionMatrix);
  shader->setUniformValue(shader->uniformLocation("u_view"), viewMatrix);

  //prepare VBO
  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glEnableVertexAttribArray(0);

  //render flat grid if necessary
  if (showGrid)
    {
      shader->setUniformValue(shader->uniformLocation("u_color"), QVector4D(0.4f, 0.4f, 0.4f, 1.0f));
      functions.glDrawArrays(GL_LINES, 0, gridVerticesCount);
    }

  //render height matrix grid using EBO with primitive restart mode
  functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  shader->setUniformValue(shader->uniformLocation("u_color"), QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
  functions.glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);

  //render matrix current comparison line strip
  functions.glLineWidth(2.0f);
  shader->setUniformValue(shader->uniformLocation("u_color"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f));
  functions.glDrawArrays(GL_LINE_STRIP, gridVerticesCount + matrixVerticesCount, comparisonSideVerticesCount);
  functions.glLineWidth(1.0f);

  //unbind buffers and release shader program
  functions.glDisableVertexAttribArray(0);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
  functions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  shader->release();
}
