#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>

#include "HeightMatrix.h"

/**
 * @brief View widget of the master-target arrangement for the chosen side
 */
class ArrangementWidget : public QOpenGLWidget
{
public:
    ArrangementWidget( QWidget * parent = 0 );
    ~ArrangementWidget();
    void updateProfilesData( const HeightMatrix & MASTER_MATRIX,
                             HeightMatrix & targetMatrix,
                             SIDE masterSide,
                             SIDE targetSide );
private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL( int w,
                   int h ) override;
    void updateOriginalProfile( const HeightMatrix & MATRIX,
                                SIDE side );
    void updateArrangedProfile( const HeightMatrix & MASTER_MATRIX,
                                const HeightMatrix & TARGET_MATRIX,
                                SIDE masterSide );
    void mergeOriginalAndArrangedVertices();
    void updateVBO();
    void adjustOriginalAndArrangedProfiles();
    void createInterpolants( float value1,
                             float value2,
                             unsigned int steps,
                             size_t xOffset );
    void updateTargetMatrix( HeightMatrix & matrix,
                             SIDE side );

private:
    QOpenGLFunctions functions;
    QOpenGLShaderProgram shaderProgram;
    GLuint vbo;
    bool vboDataValid;
    std::vector<float> profilesVertices;
    std::vector<float> originalProfileVertices;
    std::vector<float> arrangedProfileVertices;
    int projectionHorizontalDistance;
};
