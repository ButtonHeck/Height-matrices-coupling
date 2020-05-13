#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem(QOpenGLShaderProgram *shader, QOpenGLFunctions &functions)
  :
    shader(shader),
    functions(functions)
{
  float csBuffer[18] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  functions.glGenBuffers(1, &vbo);
  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glBufferData(GL_ARRAY_BUFFER, sizeof(csBuffer), csBuffer, GL_STATIC_DRAW);
  functions.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
  functions.glEnableVertexAttribArray(0);
  functions.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  functions.glEnableVertexAttribArray(1);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CoordinateSystem::~CoordinateSystem()
{
  functions.glDeleteBuffers(1, &vbo);
}

void CoordinateSystem::draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix)
{
  shader->bind();
  shader->setUniformValue(shader->uniformLocation("u_projection"), projectionMatrix);
  shader->setUniformValue(shader->uniformLocation("u_view"), viewMatrix);
  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glDrawArrays(GL_POINTS, 0, 3);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
  shader->release();
}
