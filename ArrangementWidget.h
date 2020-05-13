#ifndef ARRANGEMENTWIDGET_H
#define ARRANGEMENTWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>
#include "HeightMatrix.h"

class ArrangementWidget : public QOpenGLWidget
{
public:
  ArrangementWidget(QWidget *parent = 0);
  ~ArrangementWidget();
  void updateProfilesData(const HeightMatrix &masterMatrix, HeightMatrix& targetMatrix, int masterSide, int targetSide);

private:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
  void updateSourceProfile(const HeightMatrix &matrix, int side);
  void updateArrangedProfile(const HeightMatrix &masterMatrix, HeightMatrix &targetMatrix, int masterSide);
  void mergeSourceAndArrangedVertices();
  void bufferToVBO();
  void adjustSourceAndArrangedProfiles();
  void createInterpolants(float value1, float value2, unsigned int steps, size_t x);
  void updateTargetMatrix(HeightMatrix &matrix, int side);

  QOpenGLFunctions functions;
  QOpenGLShaderProgram shader;
  GLuint vbo;
  bool vboDataValid;
  std::vector<float> profilesVertices;
  std::vector<float> sourceProfileVertices;
  std::vector<float> arrangedProfileVertices;
  GLuint profileVerticesCount;
  int projectionHorizontalDistance;
};

#endif // ARRANGEMENTWIDGET_H
