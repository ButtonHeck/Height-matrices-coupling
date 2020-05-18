#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <memory>
#include <QMatrix4x4>
#include "HeightMatrix.h"
#include "Grid.h"
#include "CoordinateSystem.h"

class MatrixWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    MatrixWidget(QWidget *parent = 0);
    void updateMatrixData(const HeightMatrix &matrix, SIDE side);

public slots:
    void setShowGrid(bool showGrid);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    virtual void setClearColor();
    virtual QVector3D getEyePosition();

    const float FOV = 40.0f;
    const float FAR_DISTANCE = 300.0f;

    QOpenGLFunctions functions;
    QOpenGLShaderProgram gridShader, csShader;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<CoordinateSystem> coordinateSystem;
};

#endif // MATRIXWIDGET_H
