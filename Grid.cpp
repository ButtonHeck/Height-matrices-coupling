#include "Grid.h"

#include <QOpenGLShaderProgram>

Grid::Grid( QOpenGLShaderProgram * shaderProgram,
            QOpenGLFunctions & functions )
    : shaderProgram(shaderProgram)
    , width(0)
    , height(0)
    , flatGridVerticesCount(0)
    , matrixGridVerticesCount(0)
    , indicesOffset(0)
    , comparisonSideVerticesCount(0)
    , functions(functions)
    , functions_4_3()
    , flatGridVisible(false)
{
    functions.glGenBuffers( 1, &vbo );
    functions.glGenBuffers( 1, &ebo );
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    functions.glEnableVertexAttribArray(0);

    //GL_PRIMITIVE_RESTART is used for height matrix grid rendering
    functions.glEnable(GL_PRIMITIVE_RESTART);
    functions_4_3.initializeOpenGLFunctions();
    functions_4_3.glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
}

Grid::~Grid()
{
    functions.glDeleteBuffers( 1, &vbo );
    functions.glDeleteBuffers( 1, &ebo );
}

/**
 * @brief updates grids data and related buffers
 * @param MATRIX matrix
 * @param side side of the matrix
 */
void Grid::update( const HeightMatrix & MATRIX,
                   COMPARISON_SIDE side )
{
    if ( MATRIX.getWidth() == 0 )
    {
        return;
    }
    //update dimensions
    const double MATRIX_PRECISION = MATRIX.getPrecision();
    width = MATRIX.getWidth() * MATRIX_PRECISION;
    height = MATRIX.getHeight() * MATRIX_PRECISION;

    //make sure to renew storages
    vertices.clear();
    indices.clear();
    indicesOffset = 0;

    flatGridVerticesCount = 0;
    updateFlatGridVertices( MATRIX_PRECISION );

    matrixGridVerticesCount = 0;
    updateMatrixGridVertices(MATRIX);

    comparisonSideVerticesCount = 0;
    updateComparisonSideVertices( MATRIX, side );

    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW );
    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );

    functions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    functions.glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW );
    functions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

/**
 * @brief updates flat grid vertices storage
 * @param matrixPrecision precision of the matrix
 */
void Grid::updateFlatGridVertices( int matrixPrecision )
{
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    auto bufferFlatGridVertex = [this]( FlatGridVertex && gridVertex ) {
        vertices.emplace_back( gridVertex.x );
        vertices.emplace_back( 0.0f );
        vertices.emplace_back( gridVertex.z );
        flatGridVerticesCount++;
    };

    //create lines parallel to X axis (count is equal to height of the grid plus one extra at z = 0.0)
    for ( int z = -halfHeight; z <= halfHeight - matrixPrecision; z++ )
    {
        // (-X;z) vertex
        FlatGridVertex negX{ (float)(-halfWidth), (float)z };
        bufferFlatGridVertex( std::move(negX) );
        indicesOffset++;

        // (X;z) vertex
        FlatGridVertex posX{ (float)(halfWidth - matrixPrecision), (float)z };
        bufferFlatGridVertex( std::move(posX) );
        indicesOffset++;
    }

    //create lines parallel to Z axis (count is equal to width of the grid plus one extra at x = 0.0)
    for ( int x = -halfWidth; x <= halfWidth - matrixPrecision; x++ )
    {
        // (x;-Z) vertex
        FlatGridVertex negZ{ (float)x, (float)(-halfHeight) };
        bufferFlatGridVertex( std::move(negZ) );
        indicesOffset++;

        // (x;Z) vertex
        FlatGridVertex posZ{ (float)x, (float)(halfHeight - matrixPrecision) };
        bufferFlatGridVertex( std::move(posZ) );
        indicesOffset++;
    }
}

/**
 * @brief updates grid vertices storage of the matrix
 * @param MATRIX matrix
 */
void Grid::updateMatrixGridVertices( const HeightMatrix & MATRIX )
{
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    float precision = (float)MATRIX.getPrecision();
    auto bufferMatrixGridVertex = [this]( MatrixGridVertex && heightMatrixVertex ) {
        vertices.emplace_back( heightMatrixVertex.x );
        vertices.emplace_back( heightMatrixVertex.y );
        vertices.emplace_back( heightMatrixVertex.z );
        matrixGridVerticesCount++;
    };

    //create line strips parallel to X axis (count is equal to height of the grid plus one extra at z = 0.0)
    for ( HeightMatrix::ConstColumnIterator column = MATRIX.columnBegin(0); column.isValid(); column++ )
    {
        for ( HeightMatrix::ConstRowIterator row = MATRIX.rowBegin( column.getCurrentIndex() ); row.isValid(); row++ )
        {
            MatrixGridVertex v{ row.getCurrentIndex() * precision - halfWidth,
                                *row,
                                column.getCurrentIndex() * precision - halfHeight };
            bufferMatrixGridVertex( std::move(v) );
            indices.emplace_back( indicesOffset++ );
        }
        indices.emplace_back(PRIMITIVE_RESTART_INDEX);
    }

    //create line strips parallel to Z axis (count is equal to width of the grid plus one extra at x = 0.0)
    for ( HeightMatrix::ConstRowIterator row = MATRIX.rowBegin(0); row.isValid(); row++ )
    {
        for ( HeightMatrix::ConstColumnIterator column = MATRIX.columnBegin( row.getCurrentIndex() ); column.isValid(); column++ )
        {
            MatrixGridVertex v{ row.getCurrentIndex() * precision - halfWidth,
                                *column,
                                column.getCurrentIndex() * precision - halfHeight };
            bufferMatrixGridVertex( std::move(v) );
            indices.emplace_back( indicesOffset++ );
        }
        indices.emplace_back(PRIMITIVE_RESTART_INDEX);
    }
}

/**
 * @brief updates comparison side vertices storage
 * @param MATRIX matrix
 * @param side side of the matrix
 */
void Grid::updateComparisonSideVertices( const HeightMatrix & MATRIX,
                                         COMPARISON_SIDE side )
{
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    float precision = (float)MATRIX.getPrecision();
    auto bufferComparisonSideVertex = [this]( MatrixGridVertex && sideVertex ) {
        vertices.emplace_back( sideVertex.x );
        vertices.emplace_back( sideVertex.y );
        vertices.emplace_back( sideVertex.z );
        comparisonSideVerticesCount++;
    };

    switch (side)
    {
    case COMPARISON_SIDE::LEFT:
        for ( HeightMatrix::ConstColumnIterator column = MATRIX.columnBegin(0); column.isValid(); column++ )
        {
            MatrixGridVertex v{ (float)(-halfWidth),
                                *column,
                                column.getCurrentIndex() * precision - halfHeight };
            bufferComparisonSideVertex( std::move(v) );
        }
        break;
    case COMPARISON_SIDE::RIGHT:
        for ( HeightMatrix::ConstColumnIterator column = MATRIX.columnBegin( MATRIX.getWidth() - 1 ); column.isValid(); column++ )
        {
            MatrixGridVertex v{ (float)halfWidth - precision,
                                *column,
                                column.getCurrentIndex() * precision - halfHeight };
            bufferComparisonSideVertex( std::move(v) );
        }
        break;
    case COMPARISON_SIDE::TOP:
        for ( HeightMatrix::ConstRowIterator row = MATRIX.rowBegin(0); row.isValid(); row++ )
        {
            MatrixGridVertex v{ row.getCurrentIndex() * precision - halfWidth,
                                *row,
                                (float)(-halfHeight) };
            bufferComparisonSideVertex( std::move(v) );
        }
        break;
    case COMPARISON_SIDE::BOTTOM:
        for ( HeightMatrix::ConstRowIterator row = MATRIX.rowBegin( MATRIX.getHeight() - 1 ); row.isValid(); row++ )
        {
            MatrixGridVertex v{ row.getCurrentIndex() * precision - halfWidth,
                                *row,
                                (float)halfHeight - precision };
            bufferComparisonSideVertex( std::move(v) );
        }
        break;
    }
}

/**
 * @brief draw function
 * @param PROJECTION_MATRIX projection matrix
 * @param VIEW_MATRIX view matrix
 */
void Grid::draw( const QMatrix4x4 & PROJECTION_MATRIX,
                 const QMatrix4x4 & VIEW_MATRIX )
{
    //update transformation matrices
    shaderProgram->bind();
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_projection"), PROJECTION_MATRIX );
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_view"), VIEW_MATRIX );

    //bind buffers
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

    //render flat grid if necessary
    if (flatGridVisible)
    {
        shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_color"), QVector4D( 0.4f, 0.4f, 0.4f, 1.0f ) );
        functions.glDrawArrays( GL_LINES, 0, flatGridVerticesCount );
    }

    //render height matrix grid using EBO with primitive restart mode
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_color"), QVector4D( 1.0f, 1.0f, 1.0f, 1.0f ) );
    functions.glDrawElements( GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0 );

    //render matrix current comparison line strip
    functions.glLineWidth(2.0f);
    shaderProgram->setUniformValue( shaderProgram->uniformLocation("u_color"), QVector4D( 1.0f, 1.0f, 0.0f, 1.0f ) );
    functions.glDrawArrays( GL_LINE_STRIP, flatGridVerticesCount + matrixGridVerticesCount, comparisonSideVerticesCount );
    functions.glLineWidth(1.0f);

    //unbind buffers and release shader program
    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );
    functions.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    shaderProgram->release();
}


//-------getters and setters-------------

int Grid::getWidth() const
{
    return width;
}

int Grid::getHeight() const
{
    return height;
}

void Grid::setShowFlatGrid( bool isShow )
{
    flatGridVisible = isShow;
}
