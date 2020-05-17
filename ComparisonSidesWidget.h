#ifndef COMPARISONSIDESWIDGET_H
#define COMPARISONSIDESWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>
#include "HeightMatrix.h"

class ComparisonSidesWidget : public QOpenGLWidget
{
public:
    ComparisonSidesWidget(QWidget *parent = 0);
    virtual ~ComparisonSidesWidget();
    void updateProfileBuffer(const HeightMatrix &matrix, SIDE side, MATRIX_TYPE type);

private:
    struct ProfileVertex
    {
        float x, y;
    };

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mergeMasterAndTargetProfilesVertices();
    void bufferProfileVertex(std::vector<float> &profilesVertices, ProfileVertex&& vertex, GLuint& verticesCount);
    void bufferToVBO();

    void createComparisonLineData(const HeightMatrix &matrix,
                                  SIDE side,
                                  std::vector<float>& profilesVertices,
                                  GLuint& verticesCount,
                                  int& projectionDistance);

    QOpenGLFunctions functions;
    QOpenGLShaderProgram shader;
    GLuint vbo;
    bool vboDataValid;
    std::vector<float> profilesVertices;

    std::vector<float> masterProfileVertices;
    GLuint masterProfileVerticesCount;
    int projectionHorizontalDistanceMaster;

    std::vector<float> targetProfileVertices;
    GLuint targetProfileVerticesCount;
    int projectionHorizontalDistanceTarget;
};

#endif // COMPARISONSIDESWIDGET_H
