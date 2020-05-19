#include "ArrangementWidget.h"

ArrangementWidget::ArrangementWidget( QWidget * parent )
    : QOpenGLWidget(parent)
    , functions()
    , vboDataValid(false)
    , projectionHorizontalDistance(0)
{}

ArrangementWidget::~ArrangementWidget()
{
    functions.glDeleteBuffers( 1, &vbo );
}

/**
 * @brief updates target matrix line for a given side, and updates arrangement view
 * @param MASTER_MATRIX master matrix
 * @param targetMatrix target matrix
 * @param masterSide side of the master matrix to couple with
 * @param targetSide side of the target matrix to couple
 */
void ArrangementWidget::updateProfilesData( const HeightMatrix & MASTER_MATRIX,
                                            HeightMatrix & targetMatrix,
                                            COMPARISON_SIDE masterSide,
                                            COMPARISON_SIDE targetSide )
{
    //first update original target line segment data
    updateOriginalProfile( targetMatrix, targetSide );

    //then arrange target line segment with adjacent segment of master line
    updateArrangedProfile( MASTER_MATRIX, targetMatrix, masterSide );

    //make sure both source and arranged profiles are the same size
    adjustOriginalAndArrangedProfiles();

    //renew target matrix comparison line
    updateTargetMatrix( targetMatrix, targetSide );

    //add both source and processed lines data to one storage used by VBO during rendering
    mergeOriginalAndArrangedVertices();

    //set validation flag to false to signal that VBO data should be updated before rendering
    vboDataValid = false;
}

/**
 * @brief initializes OpenGL function pointers, compiles shaders and program
 */
void ArrangementWidget::initializeGL()
{
    //initialize OpenGL function pointers and pre-rendering initialization
    functions.initializeOpenGLFunctions();
    functions.glGenBuffers( 1, &vbo );
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glEnableVertexAttribArray(0);
    functions.glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    functions.glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    functions.glEnable(GL_PROGRAM_POINT_SIZE);

    //compile vertex shader
    QOpenGLShader vertexShader( QOpenGLShader::Vertex );
    vertexShader.compileSourceFile( ":/Shaders/grid/vGrid.glsl" );
    //compile fragment shader
    QOpenGLShader fragmentShader( QOpenGLShader::Fragment );
    fragmentShader.compileSourceFile( ":/Shaders/grid/fGrid.glsl" );
    //link shaders within a program
    shaderProgram.addShader( &vertexShader );
    shaderProgram.addShader( &fragmentShader );
    if ( !shaderProgram.link() )
    {
        qWarning( "Unable to link grid shader program" );
        return;
    }

    //initialize view matrix
    shaderProgram.bind();
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt( QVector3D( 0.0f, 0.0f, 1.0f ), QVector3D( 0.0f, 0.0f, 0.0f ), QVector3D( 0.0f, 1.0f, 0.0f ) );
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_view"), viewMatrix );
}

/**
 * @brief draw function
 */
void ArrangementWidget::paintGL()
{
    functions.glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if ( !shaderProgram.bind() )
    {
        qWarning( "Failed to bind shader program" );
        return;
    }

    //check whether vbo data is outdated
    if ( !vboDataValid )
    {
        updateVBO();
        vboDataValid = true;
    }

    //update projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.ortho( 0.0f, projectionHorizontalDistance, 0.0f, HeightMatrix::MAX_HEIGHT, 0.1f, 2.0f );
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_projection"), projectionMatrix );

    //render source comparison line (before arrangement is applied) - blue line
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_color"), QVector4D( 0.0f, 0.0f, 1.0f, 1.0f ) );
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    GLsizei numOriginalVertices = originalProfileVertices.size() / 2;
    functions.glDrawArrays( GL_LINE_STRIP, 0, numOriginalVertices );
    functions.glDrawArrays( GL_POINTS, 0, numOriginalVertices );

    //render comparison line with arrangement applied - purple line
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_color"), QVector4D( 1.0f, 0.0f, 1.0f, 1.0f ) );
    GLsizei numArrangedVertices = arrangedProfileVertices.size() / 2;
    functions.glDrawArrays( GL_LINE_STRIP, numOriginalVertices, numArrangedVertices );
    functions.glDrawArrays( GL_POINTS, numOriginalVertices, numArrangedVertices );

    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

/**
 * @brief adjusts viewport according to current size of the window
 * @param w width of the viewport
 * @param h height of the viewport
 */
void ArrangementWidget::resizeGL( int w,
                                  int h )
{
    functions.glViewport( 0, 0, w, h );
}

/**
 * @brief updates source profile data before coupling has been applied
 * @param MATRIX matrix to take data from
 * @param side side of the matrix
 */
void ArrangementWidget::updateOriginalProfile( const HeightMatrix & MATRIX,
                                             COMPARISON_SIDE side )
{
    originalProfileVertices.clear();

    if ( side == COMPARISON_SIDE::LEFT || side == COMPARISON_SIDE::RIGHT )
    {
        //iteration by first or last column of the matrix
        HeightMatrix::ConstColumnIterator column = (side == COMPARISON_SIDE::LEFT) ? MATRIX.columnBegin(0) : MATRIX.columnBegin( MATRIX.getWidth() - 1 );
        originalProfileVertices.reserve( MATRIX.getHeight() * 2 );
        for ( ; column.isValid(); column++ )
        {
            originalProfileVertices.emplace_back( column.getCurrentIndex() );
            originalProfileVertices.emplace_back( *column );
        }
        projectionHorizontalDistance = MATRIX.getHeight();
    }
    else
    {
        //iteration by first or last row of the matrix
        HeightMatrix::ConstRowIterator row = (side == COMPARISON_SIDE::TOP) ? MATRIX.rowBegin(0) : MATRIX.rowBegin( MATRIX.getHeight() - 1 );
        originalProfileVertices.reserve( MATRIX.getWidth() * 2 );
        for ( ; row.isValid(); row++ )
        {
            originalProfileVertices.emplace_back( row.getCurrentIndex() );
            originalProfileVertices.emplace_back( *row );
        }
        projectionHorizontalDistance = MATRIX.getWidth();
    }
}

/**
 * @brief updates profile of the target line after coupling with corresponding master's line
 * @param MASTER_MATRIX master matrix
 * @param TARGET_MATRIX target matrix
 * @param masterSide side of the master matrix to couple with
 * @note this function does nothing to target matrix itself, instead it fills arranged line storage with master matrix vertices
 */
void ArrangementWidget::updateArrangedProfile( const HeightMatrix & MASTER_MATRIX,
                                               const HeightMatrix & TARGET_MATRIX,
                                               COMPARISON_SIDE masterSide )
{
    arrangedProfileVertices.clear();
    float masterMatrixPrecision = (float)MASTER_MATRIX.getPrecision();
    float targetMatrixPrecision = (float)TARGET_MATRIX.getPrecision();
    unsigned int interpolationSteps = (int)( masterMatrixPrecision / targetMatrixPrecision );

    //comparing LEFT side of master matrix with RIGHT side of target matrix or vice versa
    if ( masterSide == COMPARISON_SIDE::LEFT || masterSide == COMPARISON_SIDE::RIGHT )
    {
        HeightMatrix::ConstColumnIterator masterColumn = (masterSide == COMPARISON_SIDE::LEFT) ? MASTER_MATRIX.columnBegin(0) : MASTER_MATRIX.columnBegin( MASTER_MATRIX.getWidth() - 1 );
        size_t masterHeight = MASTER_MATRIX.getHeight();

        //fill storage for target matrix arranged side
        for ( size_t masterLineIndex = 0; masterLineIndex < masterHeight - 1; masterLineIndex++ )
        {
            float currentHeight = *masterColumn;
            masterColumn++;
            float heightAtNextIndex = *masterColumn;
            createInterpolants( currentHeight, heightAtNextIndex, interpolationSteps, masterLineIndex * interpolationSteps );
        }

        //add master side last vertex explicitly
        arrangedProfileVertices.emplace_back( ( masterHeight - 1 ) * interpolationSteps );
        arrangedProfileVertices.emplace_back( *masterColumn );
    }

    //comparing TOP side of master matrix with BOTTOM side of target matrix or vice versa
    else
    {
        HeightMatrix::ConstRowIterator masterRow = (masterSide == COMPARISON_SIDE::TOP) ? MASTER_MATRIX.rowBegin(0) : MASTER_MATRIX.rowBegin( MASTER_MATRIX.getHeight() - 1 );
        size_t masterWidth = MASTER_MATRIX.getWidth();

        //fill storage for target matrix arranged side
        for ( size_t masterLineIndex = 0; masterLineIndex < masterWidth - 1; masterLineIndex++ )
        {
            float currentHeight = *masterRow;
            masterRow++;
            float heightAtNextIndex = *masterRow;
            createInterpolants( currentHeight, heightAtNextIndex, interpolationSteps, masterLineIndex * interpolationSteps );
        }

        //add master side last vertex explicitly
        arrangedProfileVertices.emplace_back( ( masterWidth - 1 ) * interpolationSteps );
        arrangedProfileVertices.emplace_back( *masterRow );
    }
}

/**
 * @brief merges both original and arranged vertices data into one storage
 */
void ArrangementWidget::mergeOriginalAndArrangedVertices()
{
    profilesVertices.clear();
    profilesVertices.reserve( originalProfileVertices.size() + arrangedProfileVertices.size() );
    profilesVertices.insert( profilesVertices.end(), originalProfileVertices.begin(), originalProfileVertices.end() );
    profilesVertices.insert( profilesVertices.end(), arrangedProfileVertices.begin(), arrangedProfileVertices.end() );
}

/**
 * @brief buffers data from profiles to vertex buffer object
 */
void ArrangementWidget::updateVBO()
{
    functions.glBindBuffer( GL_ARRAY_BUFFER, vbo );
    functions.glBufferData( GL_ARRAY_BUFFER, profilesVertices.size() * sizeof(float), profilesVertices.data(), GL_STATIC_DRAW );
    functions.glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

/**
 * @brief adjust arranged line profile storage to fit with original
 */
void ArrangementWidget::adjustOriginalAndArrangedProfiles()
{
    //arranged line is not less than original -> cutting down to the target matrix line length
    if ( arrangedProfileVertices.size() >= originalProfileVertices.size() )
    {
        arrangedProfileVertices.resize( originalProfileVertices.size() );
    }
    //arranged line is a part of the original -> expand with vertices from the original line
    else
    {
        arrangedProfileVertices.insert( arrangedProfileVertices.end(),
                                        originalProfileVertices.begin() + arrangedProfileVertices.size(),
                                        originalProfileVertices.end() );
    }
}

/**
 * @brief create interpolated vertices to match the target's matrix precision and stores them in arranged profile buffer
 * @param value1 first value
 * @param value2 second value
 * @param steps number of steps between two values
 * @param xOffset position offset of a new interpolant value on X axis
 */
void ArrangementWidget::createInterpolants( float value1,
                                            float value2,
                                            unsigned int steps,
                                            size_t xOffset )
{
    float stepDistance = 1.0f / steps;
    for ( size_t step = 0; step < steps; step++ )
    {
        float interpolation = stepDistance * step;
        float targetHeight = ( 1.0f - interpolation ) * value1
                             +
                             interpolation * value2;
        arrangedProfileVertices.emplace_back( xOffset + step ); //x
        arrangedProfileVertices.emplace_back( targetHeight );   //y
    }
}

/**
 * @brief update target matrix line for a given side with values stored in arranged profile storage
 * @param matrix matrix to update
 * @param side side to update
 */
void ArrangementWidget::updateTargetMatrix( HeightMatrix & matrix,
                                            COMPARISON_SIDE side )
{
    if ( side == COMPARISON_SIDE::LEFT || side == COMPARISON_SIDE::RIGHT )
    {
        HeightMatrix::ColumnIterator column = (side == COMPARISON_SIDE::LEFT) ? matrix.columnBegin(0) : matrix.columnBegin( matrix.getWidth() - 1 );
        //all odd indices for each pair in profile vector represents point height, thus +1 is applied, X coord is left old
        for ( ; column.isValid(); column++ )
        {
            *column = arrangedProfileVertices[ column.getCurrentIndex() * 2 + 1 ];
        }
    }
    else
    {
        HeightMatrix::RowIterator row = (side == COMPARISON_SIDE::TOP) ? matrix.rowBegin(0) : matrix.rowBegin( matrix.getHeight() - 1 );
        //all odd indices for each pair in profile vector represents point height, thus +1 is applied, X coord is left old
        for ( ; row.isValid(); row++ )
        {
            *row = arrangedProfileVertices[ row.getCurrentIndex() * 2 + 1 ];
        }
    }
}
