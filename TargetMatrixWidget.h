#pragma once

#include "MatrixWidget.h"

/**
 * @brief View widget for target matrix
 */
class TargetMatrixWidget : public MatrixWidget
{
public:
    TargetMatrixWidget( QWidget * parent = 0 );

private:
    void setClearColor() override;
    QVector3D getEyePosition() override;
};
