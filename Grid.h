#ifndef GRID_H
#define GRID_H

#include <vector>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include "HeightMatrix.h"

class Grid
{
public:
  Grid(QOpenGLShaderProgram *shader, QOpenGLFunctions& functions);
  ~Grid();
  void update(const HeightMatrix &matrix, SIDE side);
  int getWidth() const;
  int getHeight() const;
  void setShowFlatGrid(bool isShow);
  void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix);

private:
  const GLuint PRIMITIVE_RESTART_INDEX = 0xFFFF;
  struct GridVertex
  {
    float x, z;
  };
  struct MatrixVertex
  {
    float x, y, z;
  };

  void updateGridVertices(int matrixPrecision);
  void updateMatrixVertices(const HeightMatrix &matrix);
  void updateComparisonSideVertices(const HeightMatrix &matrix, SIDE side);
  void bufferGridVertex(GridVertex&& gridVertex);
  void bufferMatrixVertex(MatrixVertex&& heightMatrixVertex);
  void bufferComparisonSideVertex(MatrixVertex&& sideVertex);

  std::vector<float> vertices;
  std::vector<GLuint> indices;
  QOpenGLShaderProgram *shader;
  int width, height;
  GLuint gridVerticesCount, matrixVerticesCount, indicesOffset, comparisonSideVerticesCount;
  QOpenGLFunctions &functions;
  //need this to use primitive restart with index buffers objects
  QOpenGLFunctions_4_3_Core functions_4_3;
  GLuint vbo, ebo;
  bool showGrid;
};

#endif // GRID_H
