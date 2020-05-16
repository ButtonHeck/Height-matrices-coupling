#include "ComparisonSidesWidget.h"

ComparisonSidesWidget::ComparisonSidesWidget(QWidget *parent)
  :
    QOpenGLWidget(parent),
    functions(),
    projectionHorizontalDistanceMaster(0),
    projectionHorizontalDistanceTarget(0)
{}

ComparisonSidesWidget::~ComparisonSidesWidget()
{
  functions.glDeleteBuffers(1, &vbo);
}

void ComparisonSidesWidget::initializeGL()
{
  functions.initializeOpenGLFunctions();
  functions.glGenBuffers(1, &vbo);
  functions.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  QOpenGLShader vGridShader(QOpenGLShader::Vertex);
  vGridShader.compileSourceFile(":/Shaders/grid/vGrid.glsl");
  QOpenGLShader fGridShader(QOpenGLShader::Fragment);
  fGridShader.compileSourceFile(":/Shaders/grid/fGrid.glsl");
  shader.addShader(&vGridShader);
  shader.addShader(&fGridShader);
  if (!shader.link())
    qWarning("Unable to link grid shader program");
}

void ComparisonSidesWidget::paintGL()
{
    functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (!shader.bind())
    return;

  //check whether vbo data is outdated
  if (!vboDataValid)
    {
      bufferToVBO();
      vboDataValid = true;
    }

  //update projection matrix
  QMatrix4x4 projectionMatrix;
  projectionMatrix.ortho(0.0f,
                         std::max(projectionHorizontalDistanceMaster, projectionHorizontalDistanceTarget),
                         0.0f,
                         HeightMatrix::MAX_HEIGHT,
                         0.1f,
                         10.0f);
  shader.setUniformValue(shader.uniformLocation("u_projection"), projectionMatrix);

  //update view matrix
  QMatrix4x4 viewMatrix;
  viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
  shader.setUniformValue(shader.uniformLocation("u_view"), viewMatrix);

  //prepare vbo
  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glEnableVertexAttribArray(0);

  //render maser matrix profile
  shader.setUniformValue(shader.uniformLocation("u_color"), QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
  functions.glDrawArrays(GL_LINE_STRIP, 0, masterProfileVerticesCount);
  functions.glEnable(GL_PROGRAM_POINT_SIZE);
  functions.glDrawArrays(GL_POINTS, 0, masterProfileVerticesCount);

  //render target matrix profile
  shader.setUniformValue(shader.uniformLocation("u_color"), QVector4D(0.0f, 0.0f, 1.0f, 1.0f));
  functions.glDrawArrays(GL_LINE_STRIP, masterProfileVerticesCount, targetProfileVerticesCount);
  functions.glDrawArrays(GL_POINTS, masterProfileVerticesCount, targetProfileVerticesCount);

  functions.glDisableVertexAttribArray(0);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComparisonSidesWidget::resizeGL(int w, int h)
{
    functions.glViewport(0, 0, w, h);
}

void ComparisonSidesWidget::updateProfileBuffer(const HeightMatrix &matrix, int side, MATRIX_TYPE type)
{
  if (matrix.getWidth() == 0)
    return;
  std::vector<float>& vertices = (type == MATRIX_TYPE::MASTER) ? masterProfileVertices : targetProfileVertices;
  vertices.clear();
  GLuint& verticesCount = (type == MATRIX_TYPE::MASTER) ? masterProfileVerticesCount : targetProfileVerticesCount;
  verticesCount = 0;
  int& projectionHorizontalDistance = (type == MATRIX_TYPE::MASTER) ? projectionHorizontalDistanceMaster : projectionHorizontalDistanceTarget;
  createComparisonLineData(matrix, side, vertices, verticesCount, projectionHorizontalDistance);

  //add both master and target profile lines data to one storage used by VBO during rendering
  mergeMasterAndTargetProfilesVertices();

  //set validation flag to false to signal that VBO data should be updated before rendering
  vboDataValid = false;
}

void ComparisonSidesWidget::mergeMasterAndTargetProfilesVertices()
{
  profilesVertices.clear();
  profilesVertices.reserve(masterProfileVertices.size() + targetProfileVertices.size());
  profilesVertices.insert(profilesVertices.end(), masterProfileVertices.begin(), masterProfileVertices.end());
  profilesVertices.insert(profilesVertices.end(), targetProfileVertices.begin(), targetProfileVertices.end());
}

void ComparisonSidesWidget::bufferToVBO()
{
  functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  functions.glBufferData(GL_ARRAY_BUFFER, profilesVertices.size() * sizeof(float), profilesVertices.data(), GL_STATIC_DRAW);
  functions.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComparisonSidesWidget::createComparisonLineData(const HeightMatrix& matrix,
                                                     int side,
                                                     std::vector<float>& vertices,
                                                     GLuint& verticesCount,
                                                     int& projectionDistance)
{
  float precision = (float)matrix.getPrecision();
  if (side == LEFT || side == RIGHT)
    {
      HeightMatrix::ConstColumnIterator column = (side == LEFT) ? matrix.columnBegin(0) : matrix.columnBegin(matrix.getWidth() - 1);
      for (; column.isValid(); column++)
        bufferProfileVertex(vertices, ProfileVertex{column.getCurrentIndex() * precision, *column}, verticesCount);
      projectionDistance = matrix.getHeight() * precision;
    }
  else
    {
      HeightMatrix::ConstRowIterator row = (side == TOP) ? matrix.rowBegin(0) : matrix.rowBegin(matrix.getHeight() - 1);
      for (; row.isValid(); row++)
        bufferProfileVertex(vertices, ProfileVertex{row.getCurrentIndex() * precision, *row}, verticesCount);
      projectionDistance = matrix.getWidth() * precision;
    }
}

void ComparisonSidesWidget::bufferProfileVertex(std::vector<float>& vertices, ProfileVertex &&vertex, GLuint& verticesCount)
{
  vertices.emplace_back(vertex.x);
  vertices.emplace_back(vertex.y);
  verticesCount++;
}
