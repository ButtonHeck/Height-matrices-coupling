#pragma once

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <memory>

#include "HeightMatrix.h"
#include "Grid.h"
#include "CoordinateSystem.h"

/**
 * @brief View widget of the matrix
 */
class MatrixWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    MatrixWidget( QWidget * parent = 0 );
    void updateMatrixData( const HeightMatrix & MATRIX,
                           COMPARISON_SIDE side );

public slots:
    void setShowFlatGrid( bool showGrid );

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL( int w, int h ) override;
    virtual void setClearColor();
    virtual QVector3D getEyePosition();

    QOpenGLFunctions functions;
    QOpenGLShaderProgram gridShaderProgram;
    QOpenGLShaderProgram csShaderProgram;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<CoordinateSystem> coordinateSystem;
};
