#include "MatrixWidget.h"
#include <QMouseEvent>
#include <QMatrix4x4>

MatrixWidget::MatrixWidget( QWidget * parent )
    : QOpenGLWidget(parent)
    , functions()
    , eyePosition( 20, 20, 20 )
{}

/**
 * @brief delegates update call to the widget's underlying grid object
 * @param MATRIX matrix
 * @param side side of the matrix
 */
void MatrixWidget::updateMatrixData( const HeightMatrix & MATRIX,
                                     COMPARISON_SIDE side )
{
    grid->update( MATRIX, side );
}

/**
 * @brief delegates flat grid visibility setter call to grid object
 * @param showGrid bool flag
 */
void MatrixWidget::setShowFlatGrid( bool showGrid )
{
    makeCurrent();
    grid->setShowFlatGrid(showGrid);
    update();
}

void MatrixWidget::mouseMoveEvent( QMouseEvent * event )
{
    constexpr QVector3D Y_AXIS_VECTOR( 0.0, 1.0, 0.0 );
    constexpr float ZOOM_IN_RATIO = 1.05f;
    constexpr float ZOOM_OUT_RATIO = 1 / ZOOM_IN_RATIO;

    if ( event->buttons() & Qt::LeftButton )
    {
        QMatrix4x4 rotMatrixXZ;
        rotMatrixXZ.rotate( lastMousePosition.x() - event->x(), Y_AXIS_VECTOR );

        QMatrix4x4 rotMatrixForViewAxis;
        rotMatrixForViewAxis.rotate( 90, Y_AXIS_VECTOR );
        QVector3D viewVerticalAxis = rotMatrixForViewAxis.map(eyePosition);
        viewVerticalAxis.setY( 0.0f );
        viewVerticalAxis.normalize();

        QMatrix4x4 rotMatrixVertical;
        rotMatrixVertical.rotate( ( lastMousePosition.y() - event->y() ) * 0.5, viewVerticalAxis );

        QMatrix4x4 rotationMatrix = rotMatrixXZ * rotMatrixVertical;
        eyePosition = rotationMatrix.map(eyePosition);
        lastMousePosition = event->localPos();
        update();
    }
    else if ( event->buttons() & Qt::RightButton )
    {
        if ( event->y() > lastMousePosition.y() )
        {
            eyePosition *= ZOOM_IN_RATIO;
        }
        else if ( event->y() < lastMousePosition.y() )
        {
            eyePosition *= ZOOM_OUT_RATIO;
        }
        lastMousePosition = event->localPos();
        update();
    }
}

void MatrixWidget::mousePressEvent( QMouseEvent * event )
{
    if ( ( event->buttons() & Qt::LeftButton ) && !mousePosSaved )
    {
        lastMousePosition = event->localPos();
        mousePosSaved = true;
    }
}

void MatrixWidget::mouseReleaseEvent( QMouseEvent * event )
{
    Q_UNUSED(event);
    if ( mousePosSaved )
    {
        mousePosSaved = false;
    }
}

/**
 * @brief initializes OpenGL functions, shaders and initializes grid and CS objects
 */
void MatrixWidget::initializeGL()
{
    //initialize OpenGL function pointers and pre-rendering setup
    functions.initializeOpenGLFunctions();
    setClearColor();

    //create shaders for grid mesh
    QOpenGLShader vertexGridShader( QOpenGLShader::Vertex );
    vertexGridShader.compileSourceFile( ":/Shaders/grid/vGrid.glsl" );
    QOpenGLShader fragmentGridShader( QOpenGLShader::Fragment );
    fragmentGridShader.compileSourceFile( ":/Shaders/grid/fGrid.glsl" );
    //shader program
    gridShaderProgram.addShader( &vertexGridShader );
    gridShaderProgram.addShader( &fragmentGridShader );
    if ( !gridShaderProgram.link() )
    {
        qWarning("Unable to link grid shader program");
    }

    //create shaders for coordinate system
    QOpenGLShader vertexCsShader( QOpenGLShader::Vertex );
    vertexCsShader.compileSourceFile( ":/Shaders/coordinateSystem/vCS.glsl" );
    QOpenGLShader geometryCsShader( QOpenGLShader::Geometry );
    geometryCsShader.compileSourceFile( ":/Shaders/coordinateSystem/gCS.glsl" );
    QOpenGLShader fragmentCsShader( QOpenGLShader::Fragment );
    fragmentCsShader.compileSourceFile( ":/Shaders/coordinateSystem/fCS.glsl" );
    //shader program
    csShaderProgram.addShader( &vertexCsShader );
    csShaderProgram.addShader( &geometryCsShader );
    csShaderProgram.addShader( &fragmentCsShader );
    if ( !csShaderProgram.link() )
    {
        qWarning("Unable to link coordinate system shader program");
    }

    //initialize grid and coordinate system objects
    grid = std::make_unique<Grid>( gridShaderProgram, functions );
    coordinateSystem = std::make_unique<CoordinateSystem>( csShaderProgram, functions );
}

/**
 * @brief draw function, constructs matrices delegates draw calls to grid and CS objects
 */
void MatrixWidget::paintGL()
{
    functions.glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //update projection matrix
    QMatrix4x4 projectionMatrix;
    const float FOV = 40.0f;
    const float FAR_DISTANCE = 300.0f;
    projectionMatrix.perspective( FOV, (float)width() / (float)height(), 0.1f, FAR_DISTANCE );

    //update view matrix
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt( eyePosition, QVector3D( 0.0f, 0.0f, 0.0f ), QVector3D( 0.0f, 1.0f, 0.0f ) );

    //grid rendering
    grid->draw( projectionMatrix, viewMatrix );

    //coordinate system rendering
    coordinateSystem->draw( projectionMatrix, viewMatrix );
}

/**
 * @brief resizes viewport according to the window size
 * @param w widht of the viewport
 * @param h height of the viewport
 */
void MatrixWidget::resizeGL( int w, int h )
{
    functions.glViewport( 0, 0, w, h );
}

/**
 * @brief sets initial clear color value
 * @note custom clear color helps distinguish this widget from derived
 */
void MatrixWidget::setClearColor()
{
    functions.glClearColor( 0.1f, 0.0f, 0.0f, 1.0f );
}
