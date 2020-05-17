#include "CoordinateSystem.h"

#include <QOpenGLShaderProgram>

CoordinateSystem::CoordinateSystem( QOpenGLShaderProgram * shaderProgram,
                                    QOpenGLFunctions & functions )
    : shaderProgram(shaderProgram)
    , functions(functions)
{
    constexpr float COORDINATE_SYSTEM_COLORS[9] = { 1.0f, 0.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f };
    //generate and fill vertex buffer object
    functions.glGenBuffers( 1, &vbo );
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glBufferData( GL_ARRAY_BUFFER, sizeof(COORDINATE_SYSTEM_COLORS), COORDINATE_SYSTEM_COLORS, GL_STATIC_DRAW );
    functions.glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    functions.glEnableVertexAttribArray(1);
    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );

    //for alpha blending
    functions.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

CoordinateSystem::~CoordinateSystem()
{
    functions.glDeleteBuffers( 1, &vbo );
}

/**
 * @brief update necessary uniform matrices and draws coordinate system
 * @param PROJECTION_MATRIX projection matrix
 * @param VIEW_MATRIX view matrix
 */
void CoordinateSystem::draw( const QMatrix4x4 & PROJECTION_MATRIX,
                             const QMatrix4x4 & VIEW_MATRIX )
{
    if ( !shaderProgram->bind() )
    {
        qWarning( "Coordinate system shader failed to bind" );
        return;
    }
    //update matrices
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_projection"), PROJECTION_MATRIX );
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_view"), VIEW_MATRIX );

    //drawing coordinate system with blending enabled
    functions.glEnable(GL_BLEND);
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glDrawArrays( GL_POINTS, 0, 3 );
    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );
    functions.glDisable(GL_BLEND);
}
