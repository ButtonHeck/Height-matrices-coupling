#pragma once

#include <vector>
#include <QOpenGLFunctions_4_3_Core>
#include <memory>

#include "HeightMatrix.h"

class QOpenGLShaderProgram;

/**
 * @brief Represents grid mesh of a matrix and optionally flat grid layer
 */
class Grid
{
public:
    Grid( QOpenGLShaderProgram & shaderProgram,
          QOpenGLFunctions_4_3_Core & functions );
    ~Grid();
    void update( const HeightMatrix & MATRIX,
                 COMPARISON_SIDE side,
                 bool comparisonOnly = false );
    int getWidth() const;
    int getHeight() const;
    void setShowFlatGrid( bool isShow );
    void draw( const QMatrix4x4 & PROJECTION_MATRIX,
               const QMatrix4x4 & VIEW_MATRIX );

private:
    const GLuint PRIMITIVE_RESTART_INDEX = 0xFFFF;
    struct FlatGridVertex
    {
        float x, z;
    };
    struct MatrixGridVertex
    {
        float x, y, z;
    };

    void updateFlatGridVertices( int matrixPrecision );
    void updateMatrixGridVertices( const HeightMatrix & MATRIX );
    void updateComparisonSideVertices( const HeightMatrix & MATRIX,
                                       COMPARISON_SIDE side );
private:
    std::vector<float> vertices;
    std::vector<GLuint> indices;
    QOpenGLShaderProgram & shaderProgram;
    int width;
    int height;
    GLuint flatGridVerticesCount;
    GLuint matrixGridVerticesCount;
    GLuint indicesOffsetFromFlatGrid;
    GLuint comparisonSideVerticesCount;
    QOpenGLFunctions_4_3_Core & functions;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    bool flatGridVisible;
};
