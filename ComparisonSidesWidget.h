#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>

#include "HeightMatrix.h"

/**
 * @brief View widget of the master and target matrices original profiles for the chosen side
 */
class ComparisonSidesWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit ComparisonSidesWidget( QWidget * parent = 0 );
    virtual ~ComparisonSidesWidget();
    void updateProfileBuffer( const HeightMatrix & MATRIX,
                              COMPARISON_SIDE side );
private:
    struct ProfileVertex
    {
        float x, y;
    };

    void initializeGL() override;
    void paintGL() override;
    void resizeGL( int w, int h ) override;

    void mergeMasterAndTargetProfilesVertices();
    void bufferProfileVertex( std::vector<float> & profilesVertices,
                              ProfileVertex && vertex);
    void updateVBO();
    void createComparisonLineData( const HeightMatrix & MATRIX,
                                   COMPARISON_SIDE side,
                                   std::vector<float> & profilesVertices,
                                   int & projectionDistance );
private:
    QOpenGLShaderProgram shaderProgram;
    GLuint vbo;
    bool vboDataValid;
    std::vector<float> profilesVertices;

    std::vector<float> masterProfileVertices;
    int projectionHorizontalDistanceMaster;
    std::vector<float> targetProfileVertices;
    int projectionHorizontalDistanceTarget;
};
