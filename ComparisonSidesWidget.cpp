#include "ComparisonSidesWidget.h"

ComparisonSidesWidget::ComparisonSidesWidget( QWidget * parent )
    : QOpenGLWidget(parent)
    , vboDataValid(false)
    , projectionHorizontalDistanceMaster(0)
    , projectionHorizontalDistanceTarget(0)
{}

ComparisonSidesWidget::~ComparisonSidesWidget()
{
    glDeleteBuffers( 1, &vbo );
}

/**
 * @brief initializes OpenGL function pointers, shader program and some pre-rendering stuff
 */
void ComparisonSidesWidget::initializeGL()
{
    //initialize OpenGL functions and pre-rendering setup
    initializeOpenGLFunctions();
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray(0);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_PROGRAM_POINT_SIZE);

    //create shaders
    QOpenGLShader vertexShader( QOpenGLShader::Vertex );
    vertexShader.compileSourceFile(":/Shaders/grid/vGrid.glsl");
    QOpenGLShader fragmentShader( QOpenGLShader::Fragment );
    fragmentShader.compileSourceFile(":/Shaders/grid/fGrid.glsl");
    //create shader program
    shaderProgram.addShader( &vertexShader );
    shaderProgram.addShader( &fragmentShader );
    if ( !shaderProgram.link() )
    {
        qWarning("Unable to link comparison widget program");
    }

    //initialize view matrix
    if ( !shaderProgram.bind() )
    {
        qWarning( "Error during comparison widget program binding" );
        return;
    }
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt( QVector3D( 0.0f, 0.0f, 1.0f ), QVector3D( 0.0f, 0.0f, 0.0f ), QVector3D( 0.0f, 1.0f, 0.0f ) );
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_view"), viewMatrix );
}

/**
 * @brief draw function
 */
void ComparisonSidesWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if ( !shaderProgram.bind() )
    {
        qWarning( "Error during comparison widget program binding" );
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
    float projectionRightPlane = std::max( projectionHorizontalDistanceMaster, projectionHorizontalDistanceTarget );
    projectionMatrix.ortho( 0.0f, projectionRightPlane, 0.0f, HeightMatrix::MAX_HEIGHT, 0.1f, 2.0f );
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_projection"), projectionMatrix );

    //render maser matrix profile - red line
    GLsizei numMasterVertices = masterProfileVertices.size() / 2;
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_color"), QVector4D( 1.0f, 0.0f, 0.0f, 1.0f ) );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glDrawArrays( GL_LINE_STRIP, 0, numMasterVertices );
    glDrawArrays( GL_POINTS, 0, numMasterVertices );

    //render target matrix profile - blue line
    GLsizei numTargetVertices = targetProfileVertices.size() / 2;
    shaderProgram.setUniformValue( shaderProgram.uniformLocation("u_color"), QVector4D( 0.0f, 0.0f, 1.0f, 1.0f ) );
    glDrawArrays( GL_LINE_STRIP, numMasterVertices, numTargetVertices );
    glDrawArrays( GL_POINTS, numMasterVertices, numTargetVertices );
}

/**
 * @brief adjusts viewport according to current size of the window
 * @param w width of the viewport
 * @param h height of the viewport
 */
void ComparisonSidesWidget::resizeGL( int w, int h )
{
    glViewport( 0, 0, w, h );
}

/**
 * @brief updates profile buffer dependent on a given matrix
 * @param MATRIX matrix
 * @param side side of the given matrix
 */
void ComparisonSidesWidget::updateProfileBuffer( const HeightMatrix & MATRIX,
                                                 COMPARISON_SIDE side )
{
    if ( MATRIX.getWidth() == 0 )
    {
        return;
    }
    std::vector<float> & vertices = (MATRIX.getType() == HeightMatrix::MASTER) ? masterProfileVertices : targetProfileVertices;
    vertices.clear();
    int & projectionHorizontalDistance = (MATRIX.getType() == HeightMatrix::MASTER) ? projectionHorizontalDistanceMaster : projectionHorizontalDistanceTarget;
    createComparisonLineData( MATRIX, side, vertices, projectionHorizontalDistance );

    //add both master and target profile lines data to one storage used by VBO during rendering
    mergeMasterAndTargetProfilesVertices();

    //set validation flag to false to signal that VBO data should be updated before rendering
    vboDataValid = false;
}

/**
 * @brief merges both master and target profile vertices into one common storage
 */
void ComparisonSidesWidget::mergeMasterAndTargetProfilesVertices()
{
    profilesVertices.clear();
    profilesVertices.reserve( masterProfileVertices.size() + targetProfileVertices.size() );
    profilesVertices.insert( profilesVertices.end(), masterProfileVertices.begin(), masterProfileVertices.end() );
    profilesVertices.insert( profilesVertices.end(), targetProfileVertices.begin(), targetProfileVertices.end() );
}

/**
 * @brief updates vertex buffer with data stored in profiles storage
 */
void ComparisonSidesWidget::updateVBO()
{
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, profilesVertices.size() * sizeof(float), profilesVertices.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

/**
 * @brief stores profile vertices of a given side from a given matrix to a given storage
 * @param MATRIX matrix
 * @param side side of the matrix
 * @param vertices storage to fill
 * @param projectionDistance distance used in projection matrix dependent on the profile length
 */
void ComparisonSidesWidget::createComparisonLineData( const HeightMatrix & MATRIX,
                                                      COMPARISON_SIDE side,
                                                      std::vector<float> & vertices,
                                                      int & projectionDistance )
{
    float precision = (float)MATRIX.getPrecision();
    if ( side == COMPARISON_SIDE::LEFT || side == COMPARISON_SIDE::RIGHT )
    {
        HeightMatrix::ConstColumnIterator column = (side == COMPARISON_SIDE::LEFT) ? MATRIX.columnBegin(0) : MATRIX.columnBegin( MATRIX.getWidth() - 1 );
        for ( ; column.isValid(); column++ )
        {
            bufferProfileVertex( vertices, ProfileVertex{ column.getCurrentIndex() * precision, *column } );
        }
        projectionDistance = MATRIX.getHeight() * precision;
    }
    else
    {
        HeightMatrix::ConstRowIterator row = (side == COMPARISON_SIDE::TOP) ? MATRIX.rowBegin(0) : MATRIX.rowBegin( MATRIX.getHeight() - 1 );
        for ( ; row.isValid(); row++ )
        {
            bufferProfileVertex( vertices, ProfileVertex{ row.getCurrentIndex() * precision, *row } );
        }
        projectionDistance = MATRIX.getWidth() * precision;
    }
}

/**
 * @brief helper function to buffer vertex into a given storage
 * @param vertices buffer of vertices
 * @param vertex a profile line vertex
 */
void ComparisonSidesWidget::bufferProfileVertex( std::vector<float> & vertices,
                                                 ProfileVertex && vertex )
{
    vertices.emplace_back( vertex.x );
    vertices.emplace_back( vertex.y );
}
