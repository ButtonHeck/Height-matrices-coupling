#pragma once

#include <QOpenGLFunctions>

class QOpenGLShaderProgram;

/**
 * @brief Coordinate system representation class
 */
class CoordinateSystem
{
public:
    CoordinateSystem( QOpenGLShaderProgram * shaderProgram,
                      QOpenGLFunctions & functions );
    ~CoordinateSystem();
    void draw( const QMatrix4x4 & PROJECTION_MATRIX,
               const QMatrix4x4 & VIEW_MATRIX );

private:
    QOpenGLShaderProgram * shaderProgram;
    QOpenGLFunctions & functions;
    GLuint vbo;
};
