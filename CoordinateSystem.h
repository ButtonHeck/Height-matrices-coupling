#pragma once

#include <QOpenGLFunctions>
#include <memory>

class QOpenGLShaderProgram;

/**
 * @brief Coordinate system representation class
 */
class CoordinateSystem
{
public:
    CoordinateSystem( QOpenGLShaderProgram * shaderProgram,
                      std::shared_ptr<QOpenGLFunctions> functions );
    ~CoordinateSystem();
    void draw( const QMatrix4x4 & PROJECTION_MATRIX,
               const QMatrix4x4 & VIEW_MATRIX );

private:
    QOpenGLShaderProgram * shaderProgram;
    std::shared_ptr<QOpenGLFunctions> functions;
    GLuint vbo;
};
