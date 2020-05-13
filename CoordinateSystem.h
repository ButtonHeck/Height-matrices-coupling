#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class CoordinateSystem
{
public:
  CoordinateSystem(QOpenGLShaderProgram *shader, QOpenGLFunctions &functions);
  ~CoordinateSystem();
  void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix);

private:
  QOpenGLShaderProgram *shader;
  QOpenGLFunctions &functions;
  GLuint vbo;
};

#endif // COORDINATESYSTEM_H
