#pragma once

#include <QOpenGLFunctions_4_3_Core>
#include <memory>

class QOpenGLShaderProgram;

/**
 * @brief Coordinate system representation class
 */
class CoordinateSystem
{
public:
    CoordinateSystem( QOpenGLShaderProgram & shaderProgram,
                      QOpenGLFunctions_4_3_Core & functions );
    ~CoordinateSystem();
    void draw( const QMatrix4x4 & PROJECTION_MATRIX,
               const QMatrix4x4 & VIEW_MATRIX );

private:
    QOpenGLShaderProgram & shaderProgram;
    QOpenGLFunctions_4_3_Core & functions;
    GLuint vao;
    GLuint vbo;
};
