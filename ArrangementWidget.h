#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <vector>

#include "HeightMatrix.h"

/**
 * @brief View widget of the master-target arrangement for the chosen side
 */
class ArrangementWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit ArrangementWidget( QWidget * parent = 0 );
    ~ArrangementWidget();
    void updateProfilesData( const HeightMatrix & MASTER_MATRIX,
                             HeightMatrix & targetMatrix,
                             COMPARISON_SIDE masterSide,
                             COMPARISON_SIDE targetSide );
private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL( int w,
                   int h ) override;
    void updateOriginalProfile( const HeightMatrix & MATRIX,
                                COMPARISON_SIDE side );
    void updateArrangedProfile( const HeightMatrix & MASTER_MATRIX,
                                const HeightMatrix & TARGET_MATRIX,
                                COMPARISON_SIDE masterSide );
    void mergeOriginalAndArrangedVertices();
    void updateVBO();
    void adjustOriginalAndArrangedProfiles();
    void createInterpolants( float value1,
                             float value2,
                             unsigned int steps,
                             size_t xOffset );
    void updateTargetMatrix( HeightMatrix & matrix,
                             COMPARISON_SIDE side );

private:
    QOpenGLShaderProgram shaderProgram;
    GLuint vbo;
    bool vboDataValid;
    std::vector<float> profilesVertices;
    std::vector<float> originalProfileVertices;
    std::vector<float> arrangedProfileVertices;
    size_t projectionHorizontalDistance;
};
