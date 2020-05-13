#include "MatrixWidget.h"

MatrixWidget::MatrixWidget(QWidget *parent)
  :
    QOpenGLWidget(parent),
    functions()
{}

void MatrixWidget::updateMatrixData(const HeightMatrix &matrix, int side)
{
  grid->update(matrix, side);
}

void MatrixWidget::setShowGrid(bool showGrid)
{
  grid->setShowFlatGrid(showGrid);
}

void MatrixWidget::initializeGL()
{
  functions.initializeOpenGLFunctions();
  setClearColor();

  QOpenGLShader vGridShader(QOpenGLShader::Vertex);
  vGridShader.compileSourceFile(":/Shaders/grid/vGrid.glsl");
  QOpenGLShader fGridShader(QOpenGLShader::Fragment);
  fGridShader.compileSourceFile(":/Shaders/grid/fGrid.glsl");
  gridShader.addShader(&vGridShader);
  gridShader.addShader(&fGridShader);
  if (!gridShader.link())
    qWarning("Unable to link grid shader program");

  QOpenGLShader vCsShader(QOpenGLShader::Vertex);
  vCsShader.compileSourceFile(":/Shaders/coordinateSystem/vCS.glsl");
  QOpenGLShader gCsShader(QOpenGLShader::Geometry);
  gCsShader.compileSourceFile(":/Shaders/coordinateSystem/gCS.glsl");
  QOpenGLShader fCsShader(QOpenGLShader::Fragment);
  fCsShader.compileSourceFile(":/Shaders/coordinateSystem/fCS.glsl");
  csShader.addShader(&vCsShader);
  csShader.addShader(&gCsShader);
  csShader.addShader(&fCsShader);
  if (!csShader.link())
    qWarning("Unable to link coordinate axis shader program");

  grid = std::make_unique<Grid>(&gridShader, functions);
  coordinateSystem = std::make_unique<CoordinateSystem>(&csShader, functions);
}

void MatrixWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if(!gridShader.bind())
    return;

  //update projection matrix
  QMatrix4x4 projectionMatrix;
  projectionMatrix.perspective(FOV, (float)width() / (float)height(), 0.1f, FAR_DISTANCE);

  //update view matrix
  QMatrix4x4 viewMatrix;
  viewMatrix.lookAt(getEyePosition(), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

  //grid rendering
  grid->draw(projectionMatrix, viewMatrix);

  //coordinate system rendering
  coordinateSystem->draw(projectionMatrix, viewMatrix);
}

void MatrixWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}

void MatrixWidget::setClearColor()
{
  glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
}

QVector3D MatrixWidget::getEyePosition()
{
  return QVector3D(grid->getWidth(), std::min(grid->getWidth(), grid->getHeight()), grid->getHeight());
}
