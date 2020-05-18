#pragma once

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>

#include "HeightMatrix.h"

class QOpenGLShaderProgram;

/**
 * @brief Represents grid mesh of a matrix and optionally flat grid layer
 */
class Grid
{
public:
    Grid( QOpenGLShaderProgram * shaderProgram,
          QOpenGLFunctions & functions );
    ~Grid();
    void update( const HeightMatrix & MATRIX,
                 SIDE side );
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
                                       SIDE side );
private:
    std::vector<float> vertices;
    std::vector<GLuint> indices;
    QOpenGLShaderProgram * shaderProgram;
    int width;
    int height;
    GLuint flatGridVerticesCount;
    GLuint matrixGridVerticesCount;
    GLuint indicesOffset;
    GLuint comparisonSideVerticesCount;
    QOpenGLFunctions & functions;
    //need this to use primitive restart with index buffers objects
    QOpenGLFunctions_4_3_Core functions_4_3;
    GLuint vbo;
    GLuint ebo;
    bool flatGridVisible;
};
